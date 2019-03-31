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

#include "packet_logger.hpp"

#include <string>

#ifdef PROXY_PACKET_DEBUG
#include <iostream>
#endif  // ifdef PROXY_PACKET_DEBUG

namespace proxy
{
PacketLogger::PacketLogger(const std::string& t_log_file_path)
{
  m_log_file.open(t_log_file_path);
}

void PacketLogger::packet_logger(
    const MySqlPacket* t_packet, bool t_from_client_to_server)
{
  // If the packet is from the client, write it to the log file.
  if(t_from_client_to_server) {
    auto client_packet = dynamic_cast<const FromClientPacket*>(t_packet);
    if(!client_packet) {
      return;
    }

    // Get the string representation of the client's command.
    std::string command_string = client_packet->get_command_string();

    if(!command_string.empty()) {
#ifdef PROXY_PACKET_DEBUG
      std::cout << command_string;
#endif  // ifdef PROXY_PACKET_DEBUG
      m_log_file << command_string;

      // If the command has the SQL field string, write it to the log file.
      if(client_packet->has_sql_string()) {
        const std::string& sql_str = client_packet->get_sql_string();

#ifdef PROXY_PACKET_DEBUG
        std::cout << ", SQL: " << sql_str;
#endif  // ifdef PROXY_PACKET_DEBUG
        m_log_file << ", SQL: " << sql_str;
      }

#ifdef PROXY_PACKET_DEBUG
      std::cout << "\n";
#endif  // ifdef PROXY_PACKET_DEBUG
      m_log_file << "\n";
    }
  }
}

}  // namespace proxy
