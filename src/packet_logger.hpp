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

#ifndef PROXY_PACKET_LOGGER_HPP
#define PROXY_PACKET_LOGGER_HPP

#include <cstddef>
#include <fstream>

#include <boost/asio.hpp>

#include "packet.hpp"

namespace proxy
{
/// Represents the file logger for the MySQL packets.
class PacketLogger
{
public:
  PacketLogger(const PacketLogger&) = delete;
  PacketLogger(PacketLogger&&) = delete;
  PacketLogger& operator=(const PacketLogger&) = delete;
  PacketLogger& operator=(PacketLogger&&) = delete;

  ~PacketLogger() = default;

  explicit PacketLogger(const std::string& t_log_file_path);

  /// Writes the packet to the log file.
  void packet_logger(const MySqlPacket* t_packet, bool t_from_client_to_server);

  /// Synchronizes with the underlying storage device.
  void flush();

private:
  /// Write the log to this file.
  std::ofstream m_log_file;
};

inline void PacketLogger::flush()
{
  m_log_file.flush();
}

}  // namespace proxy

#endif  // PROXY_PACKET_LOGGER_HPP
