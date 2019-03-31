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

#include "connection_manager.hpp"

namespace proxy
{
Connection::Connection(boost::asio::ip::tcp::socket t_client_socket,
    StopTransferFunc&& t_stop_handler_func)
    : m_client_socket(std::move(t_client_socket))
    , m_stop_transfer_func(std::move(t_stop_handler_func))
{
}

void Connection::start() {}

void Connection::stop()
{
  m_client_socket.close();
}

}  // namespace proxy
