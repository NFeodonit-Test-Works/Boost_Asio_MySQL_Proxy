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

#include "connection_manager.hpp"

#include <iostream>

namespace proxy
{
void ConnectionManager::start(const ConnectionPtr& t_connection)
{
  m_connections.insert(t_connection);
  t_connection->start();
  std::cout << "Opened connections: " << m_connections.size() << "\n";
}

void ConnectionManager::stop(const ConnectionPtr& t_connection)
{
  t_connection->stop();
  m_connections.erase(t_connection);
  std::cout << "Opened connections: " << m_connections.size() << "\n";
}

void ConnectionManager::stop_all()
{
  for(const auto& connecton : m_connections) {
    connecton->stop();
  }
  m_connections.clear();
  std::cout << " All connections are closed.\n";
}

}  // namespace proxy
