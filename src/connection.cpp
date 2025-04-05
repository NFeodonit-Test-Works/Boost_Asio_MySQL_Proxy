/*****************************************************************************
 * Project:  Boost_Asio_MySQL_Proxy
 * Purpose:  Test project
 * Author:   NikitaFeodonit, nfeodonit@yandex.com
 *****************************************************************************
 *   Copyright (c) 2019 NikitaFeodonit
 *
 *    This file is part of the Boost_Asio_MySQL_Proxy project.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published
 *    by the Free Software Foundation, either version 3 of the License,
 *    or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *    See the GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "connection.hpp"

#include <utility>

#ifdef PROXY_PACKET_DEBUG
#include <iomanip>
#include <iostream>
#endif  // ifdef PROXY_PACKET_DEBUG

namespace proxy
{
Connection::Connection(boost::asio::ip::tcp::socket t_client_socket,
    const boost::asio::ip::tcp::endpoint& t_server_endpoint,
    StopTransferFunc&& t_stop_handler_func,
    PacketLoggerFunc&& t_packet_logger_func)
    : m_client_socket(std::move(t_client_socket))
    , m_server_endpoint(t_server_endpoint)
    , m_server_socket(m_client_socket.get_executor().context())
    , m_client_buffer{}
    , m_server_buffer{}
    , m_stop_transfer_func(std::move(t_stop_handler_func))
    , m_packet_logger_func(std::move(t_packet_logger_func))
{
}

void Connection::start()
{
  do_connect();
}

void Connection::stop()
{
  m_client_socket.close();
  m_server_socket.close();
}

void Connection::do_connect()
{
  // Open the server connection. Connection from the client is already opened.
  m_server_socket.open(m_server_endpoint.protocol());
  m_server_socket.async_connect(m_server_endpoint,
      [this](const boost::system::error_code& l_error) -> void {
        if(!l_error) {
          // The connection was successful.
          // Start listening for the data on the connections.
          do_receive();
        }
      });
}

void Connection::do_receive()
{
  // Start listening for the data on the client connection.
  m_client_socket.async_receive(boost::asio::buffer(m_client_buffer),
      [this](const boost::system::error_code& l_error,
          std::size_t l_bytes_transferred) -> void {
        if(!l_error) {
          // Transfer the data from the client to the server.
          do_transfer(m_client_socket, m_server_socket,
              boost::asio::buffer(m_client_buffer), l_bytes_transferred, true);
        }
      });

  // Also listen for the data on the server connection.
  m_server_socket.async_receive(boost::asio::buffer(m_server_buffer),
      [this](const boost::system::error_code& l_error,
          std::size_t l_bytes_transferred) -> void {
        if(!l_error) {
          // Transfer the data from the server to the client.
          do_transfer(m_server_socket, m_client_socket,
              boost::asio::buffer(m_server_buffer), l_bytes_transferred, false);
        }
      });
}

// This function is called whenever the data is received.
void Connection::do_transfer(boost::asio::ip::tcp::socket& t_read_from,
    boost::asio::ip::tcp::socket& t_send_to,
    const boost::asio::mutable_buffer& t_read_buffer,
    std::size_t t_bytes_transferred,
    bool t_from_client_to_server)
{
  do_packet_logging(
      t_read_buffer, t_bytes_transferred, t_from_client_to_server);

  // Forward the received data on to "the other side".
  t_send_to.send(boost::asio::buffer(t_read_buffer, t_bytes_transferred));

  // Read more data from "this side".
  t_read_from.async_read_some(boost::asio::buffer(t_read_buffer, BUFFER_LENGTH),
      [this, &t_read_from, &t_send_to, t_read_buffer, t_from_client_to_server](
          const boost::system::error_code& l_error,
          std::size_t l_bytes_transferred) -> void {
        if(!l_error) {
          do_transfer(t_read_from, t_send_to, t_read_buffer,
              l_bytes_transferred, t_from_client_to_server);
        } else if(l_error != boost::asio::error::operation_aborted) {
          // Perform the actions for the connection stop.
          if(m_stop_transfer_func) {
            m_stop_transfer_func(shared_from_this());
          }
        }
      });
}

void Connection::do_packet_logging(
    const boost::asio::mutable_buffer& t_read_buffer,
    std::size_t t_bytes_transferred,
    bool t_from_client_to_server)
{
#ifdef PROXY_PACKET_DEBUG
  // Debug printers.
  std::string begin_str = t_from_client_to_server ? "--->>>" : "<<<===";
  std::string end_str = t_from_client_to_server ? "---+++" : "===|||";
  std::cout << std::hex << std::setfill('0');  // needs to be set only once

  // Prints the all buffer bytes.
  std::cout << begin_str << "BUFFER: ";
  for(std::size_t i = 0; i < t_read_buffer.size(); ++i) {
    unsigned char buffer_byte =
        static_cast<unsigned char*>(t_read_buffer.data())[i];
    std::cout << std::setw(2) << +buffer_byte << " ";
  }
  std::cout << end_str << "\n";

  // Prints the buffer context as string.
  std::string data(
      static_cast<char*>(t_read_buffer.data()), t_read_buffer.size());
  std::cout << begin_str << "BUFFER AS STRING: " << data << end_str << "\n";
#endif  // ifdef PROXY_PACKET_DEBUG


  // Collects the corresponding packet from the incoming stream of bytes.
  MySqlPacket* packet = t_from_client_to_server
      ? static_cast<MySqlPacket*>(&m_client_packet)
      : static_cast<MySqlPacket*>(&m_server_packet);

  auto* buffer_data = static_cast<unsigned char*>(t_read_buffer.data());

  for(std::size_t i = 0; i < t_bytes_transferred; ++i) {
    packet->collect(buffer_data[i], m_connection_state);
  }


  if(packet->is_received()) {
#ifdef PROXY_PACKET_DEBUG
    // Prints the all collected packet bytes.
    std::cout << begin_str << "PACKET: "
              << " [[[ length: " << std::setw(2) << packet->payload_length()
              << ", sequence_id: " << std::setw(2) << packet->sequence_id()
              << ", payload: ";
    for(std::size_t k = 0; k < packet->payload().size(); ++k) {
      unsigned char packet_byte =
          static_cast<const unsigned char*>(packet->payload().data())[k];
      std::cout << std::setw(2) << +packet_byte << " ";
    }
    std::cout << " ]]]\n";
#endif  // ifdef PROXY_PACKET_DEBUG


    // Perform the actions for the packet logging.
    if(m_packet_logger_func) {
      m_packet_logger_func(packet, t_from_client_to_server);
    }
  }
}

}  // namespace proxy
