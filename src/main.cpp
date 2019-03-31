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

#include <iostream>
#include <string>

#include "server.hpp"

int main(int t_argc, char* t_argv[])
{
  try {
    // Check command line arguments.
    if(t_argc != 6) {
      std::cerr << "Usage: boost-asio-mysql-proxy"
                   " <client ip> <port> <mysql server ip> <port> <log file>\n";
      return 1;
    }

    // Initialise the server.
    proxy::Server proxy_server(
        t_argv[1], t_argv[2], t_argv[3], t_argv[4], t_argv[5]);

    // Run the server until stopped.
    proxy_server.run();
  } catch(std::exception& e) {
    std::cerr << "Server stopped with exception: " << e.what() << "\n";
  }

  return 0;
}
