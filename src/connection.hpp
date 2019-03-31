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
#include <boost/asio.hpp>
#include <memory>

namespace proxy
{
class ConnectionManager;

/// Represents a single connection from a client.
class Connection : public std::enable_shared_from_this<Connection>
{
public:
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  /// Construct a connection with the given socket.
  explicit Connection(
      boost::asio::ip::tcp::socket socket, ConnectionManager& manager);

  /// Start the first asynchronous operation for the connection.
  void start();

  /// Stop all asynchronous operations associated with the connection.
  void stop();

private:
  /// Socket for the connection.
  boost::asio::ip::tcp::socket m_socket;

  /// The manager for this connection.
  ConnectionManager& m_connection_manager;

  /// Buffer for incoming data.
  std::array<char, 8192> m_buffer;
};  // class connection

typedef std::shared_ptr<Connection> ConnectionPtr;

}  // namespace proxy

#endif  // PROXY_CONNECTION_HPP
