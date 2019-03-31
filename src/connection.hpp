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

#ifndef PROXY_CONNECTION_HPP
#define PROXY_CONNECTION_HPP

#include <array>
#include <cstddef>
#include <functional>
#include <memory>

#include <boost/asio.hpp>

namespace proxy
{
class Connection;

using ConnectionPtr = std::shared_ptr<Connection>;

/// Represents a single proxy connection between the client and MySQL server.
class Connection : public std::enable_shared_from_this<Connection>
{
public:
  Connection() = delete;
  Connection(const Connection&) = delete;
  Connection(Connection&&) = delete;
  Connection& operator=(const Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

  ~Connection() = default;

  /// Functor for the actions for the connection stop.
  using StopTransferFunc = std::function<void(ConnectionPtr t_connection)>;

  /// Construct a connection with the given client socket and server endpoint.
  explicit Connection(boost::asio::ip::tcp::socket t_client_socket,
      const boost::asio::ip::tcp::endpoint& t_server_endpoint,
      StopTransferFunc&& t_stop_handler_func);

  /// Start the first asynchronous operation for the connection.
  void start();

  /// Stop all asynchronous operations associated with the connection.
  void stop();

private:
  /// Perform an asynchronous connection operation.
  void do_connect();

  /// Perform an asynchronous receive operation.
  void do_receive();

  /// The handler used to process the transfer operation.
  void do_transfer(boost::asio::ip::tcp::socket& t_read_from,
      boost::asio::ip::tcp::socket& t_send_to,
      const boost::asio::mutable_buffer& t_read_buffer,
      std::size_t t_bytes_transferred,
      bool t_from_client_to_server);

  /// Socket for the connection from the client.
  boost::asio::ip::tcp::socket m_client_socket;

  /// Endpoint for the server socket.
  const boost::asio::ip::tcp::endpoint& m_server_endpoint;

  /// Socket for the connection to the server.
  boost::asio::ip::tcp::socket m_server_socket;

  static const std::size_t BUFFER_LENGTH = 8192;

  /// Buffer for the incoming data from the client.
  std::array<char, BUFFER_LENGTH> m_client_buffer;

  /// Buffer for the incoming data from the server.
  std::array<char, BUFFER_LENGTH> m_server_buffer;

  /// Set the actions for the connection stop.
  StopTransferFunc m_stop_transfer_func;
};  // class connection

}  // namespace proxy

#endif  // PROXY_CONNECTION_HPP
