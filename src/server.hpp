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

#ifndef PROXY_SERVER_HPP
#define PROXY_SERVER_HPP

#include <string>

#include <boost/asio.hpp>

#include "connection.hpp"
#include "connection_manager.hpp"
#include "packet_logger.hpp"

namespace proxy
{
/// The top-level class of the proxy server for the MySQL connections.
class Server
{
public:
  Server() = delete;
  Server(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&) = delete;
  Server& operator=(Server&&) = delete;

  ~Server() = default;

  /// Construct the server to listen on the specified client TCP address and port,
  /// to connect to the specified server TCP address and port,
  /// and to write SQL requests to the specified log file.
  explicit Server(const std::string& t_client_address,
      const std::string& t_client_port,
      const std::string& t_server_address,
      const std::string& t_server_port,
      const std::string& t_log_file_path);

  /// Run the server's io_context loop.
  void run();

private:
  /// Perform an asynchronous accept operation.
  void do_accept();

  /// Wait for a request to stop the server.
  void do_await_stop();

  /// The io_context used to perform asynchronous operations.
  boost::asio::io_context m_io_context;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set m_signals;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor m_acceptor;

  /// Endpoint for the server socket.
  boost::asio::ip::tcp::endpoint m_server_endpoint;

  /// The connection manager which owns all live connections.
  ConnectionManager m_connection_manager;

  /// Packet logger which writes the SQL requests to the log file.
  PacketLogger m_packet_logger;
};

}  // namespace proxy

#endif  // PROXY_SERVER_HPP
