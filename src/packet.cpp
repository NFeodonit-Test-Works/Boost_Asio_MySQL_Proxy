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

#include "packet.hpp"

#include <iostream>
#include <stdexcept>

namespace proxy
{
// ======== MySqlCommand ========

// static
bool MySqlCommand::is_valid(Command t_command)
{
  switch(t_command) {
    case Command::COM_QUIT:
    case Command::COM_INIT_DB:
    case Command::COM_QUERY:
    case Command::COM_FIELD_LIST:
    case Command::COM_REFRESH:
    case Command::COM_STATISTICS:
    case Command::COM_PROCESS_INFO:
    case Command::COM_PROCESS_KILL:
    case Command::COM_DEBUG:
    case Command::COM_PING:
    case Command::COM_CHANGE_USER:
    case Command::COM_RESET_CONNECTION:
    //case MySqlCommands::COM_SET_OPTION :
    case Command::COM_STMT_RESET:
    case Command::COM_STMT_PREPARE:
    case Command::COM_STMT_EXECUTE:
    //case MySqlCommands::COM_STMT_FETCH :
    case Command::COM_STMT_CLOSE:
    case Command::COM_STMT_SEND_LONG_DATA: {
      return true;
    }
  }
  return false;
}

// static
bool MySqlCommand::has_sql_field(Command t_command)
{
  switch(t_command) {
    case Command::COM_INIT_DB:
    case Command::COM_QUERY:
    case Command::COM_STMT_PREPARE: {
      return true;
    }
  }
  return false;
}

// static
const char* MySqlCommand::name(
    Command t_command, std::uint64_t t_payload_length)
{
  switch(t_command) {
    case Command::COM_QUIT: {
      return "COM_QUIT";
    }
    case Command::COM_INIT_DB: {
      return "COM_INIT_DB";
    }
    case Command::COM_QUERY: {
      return "COM_QUERY";
    }
    case Command::COM_FIELD_LIST: {
      return "COM_FIELD_LIST";
    }
    case Command::COM_REFRESH: {
      return "COM_REFRESH";
    }
    case Command::COM_STATISTICS: {
      return "COM_STATISTICS";
    }
    case Command::COM_PROCESS_INFO: {
      return "COM_PROCESS_INFO";
    }
    case Command::COM_PROCESS_KILL: {
      return "COM_PROCESS_KILL";
    }
    case Command::COM_DEBUG: {
      return "COM_DEBUG";
    }
    case Command::COM_PING: {
      return "COM_PING";
    }
    case Command::COM_CHANGE_USER: {
      return "COM_CHANGE_USER";
    }
    case Command::COM_RESET_CONNECTION: {
      return "COM_RESET_CONNECTION";
    }
    //case MySqlCommands::COM_SET_OPTION :
    case Command::COM_STMT_RESET: {
      switch(t_payload_length) {
        case 3:
          return "COM_SET_OPTION";
        case 5:
          return "COM_STMT_RESET";
      }
      break;
    }
    case Command::COM_STMT_PREPARE: {
      return "COM_STMT_PREPARE";
    }
    case Command::COM_STMT_EXECUTE: {
      return "COM_STMT_EXECUTE";
    }
    //case MySqlCommands::COM_STMT_FETCH :
    case Command::COM_STMT_CLOSE: {
      switch(t_payload_length) {
        case 9:
          return "COM_STMT_FETCH";
        case 5:
          return "COM_STMT_CLOSE";
      }
      break;
    }
    case Command::COM_STMT_SEND_LONG_DATA: {
      return "COM_STMT_SEND_LONG_DATA";
    }
  }
  return "";
}


// ======== MySqlResponse ========

// static
MySqlResponse::Response MySqlResponse::get_response(
    unsigned char t_payload_0, std::uint64_t t_payload_length)
{
  // See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_basic_response_packets.html
  auto response = static_cast<Response>(t_payload_0);
  switch(response) {
    case Response::OK_PACKET: {
      // packet_length == payload_length_ + 4
      if(t_payload_length > 3) {  // length of packet > 7
        return Response::OK_PACKET;
      }
      break;
    }
    case Response::EOF_PACKET: {
      // packet_length == payload_length_ + 4
      if(t_payload_length < 5) {  // length of packet < 9
        return Response::EOF_PACKET;
      }
      break;
    }
    case Response::ERR_PACKET: {
      return Response::ERR_PACKET;
    }
    case Response::OTHER_PACKET: {
      return Response::OTHER_PACKET;
    }
  }
  return Response::OTHER_PACKET;
}


// ======== MySqlPacket ========

// See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_basic_packets.html
void MySqlPacket::collect(
    unsigned char t_received_byte, MySqlConnectionState& t_connection_state)
{
  switch(m_packet_state) {
    case PacketState::PAYLOAD_LENGTH_0: {
      m_payload_is_received = false;
      m_received_bytes = 0;
      m_payload_length = t_received_byte;
      m_packet_state = PacketState::PAYLOAD_LENGTH_1;
      break;
    }

    case PacketState::PAYLOAD_LENGTH_1: {
      m_payload_length += static_cast<std::uint64_t>(t_received_byte << 8u);
      m_packet_state = PacketState::PAYLOAD_LENGTH_2;
      break;
    }

    case PacketState::PAYLOAD_LENGTH_2: {
      m_payload_length += static_cast<std::uint64_t>(t_received_byte << 16u);
      m_packet_state = PacketState::SEQUENCE_ID;
      break;
    }

    case PacketState::SEQUENCE_ID: {
      m_sequence_id = t_received_byte;
      m_payload_not_ended = (MAX_PAYLOAD_LENGTH == m_payload_length);

#ifdef PROXY_PACKET_DEBUG
      if(m_payload_first_part) {
        m_payload.clear();
        m_payload.shrink_to_fit();
        m_payload.reserve(m_payload_length);
      } else {
        m_payload.reserve(m_payload.capacity() + m_payload_length);
      }
#endif  // ifdef PROXY_PACKET_DEBUG

      if(0 == m_payload_length) {
        m_payload_is_received = true;
        m_payload_first_part = true;
        m_packet_state = PacketState::PAYLOAD_LENGTH_0;
      } else {
        m_packet_state = PacketState::PAYLOAD;
      }
      break;
    }

    case PacketState::PAYLOAD: {
#ifdef PROXY_PACKET_DEBUG
      m_payload.push_back(t_received_byte);
#endif  // ifdef PROXY_PACKET_DEBUG

      // Analyse the 1st byte of the packet payload.
      if(0 == m_received_bytes) {
        switch(t_connection_state) {
          case MySqlConnectionState::CONNECTION_PHASE: {
            connection_phase_parse(t_received_byte, t_connection_state);
            break;
          }
          case MySqlConnectionState::COMMAND_PHASE: {
            command_phase_parse(t_received_byte);
            break;
          }
        }
      }

      // Collect the payload data.
      if(0 < m_received_bytes || !m_payload_first_part) {
        collect_data(t_received_byte);
      }

      ++m_received_bytes;

      if(m_received_bytes == m_payload_length) {
        if(!m_payload_not_ended) {
          m_payload_is_received = true;
          data_is_received();
        }

        m_payload_first_part = !m_payload_not_ended;
        m_packet_state = PacketState::PAYLOAD_LENGTH_0;
      }
      break;
    }
  }
}


// ======== FromClientPacket ========

void FromClientPacket::connection_phase_parse(
    unsigned char /*t_payload_0*/, MySqlConnectionState& /*t_connection_state*/)
{
  // Here we do nothing.
}

void FromClientPacket::command_phase_parse(unsigned char t_payload_0)
{
  if(m_payload_first_part) {
    m_command = static_cast<MySqlCommand::Command>(t_payload_0);
    if(MySqlCommand::is_valid(m_command)) {
      if(0 != m_sequence_id) {
        // See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_basic_packets.html
        // The sequence-id is incremented with each packet and may wrap around.
        // It starts at 0 and is reset to 0 when a new command begins
        // in the Command Phase.
        throw std::runtime_error("BAD packet");
      }
    }
    m_sql_string.clear();
    m_sql_string.shrink_to_fit();
  }

  if(MySqlCommand::has_sql_field(m_command)) {
    if(m_payload_first_part) {
      m_sql_string.reserve(m_payload_length - 1);
      m_sql_data_receiving = true;
    } else {
      m_sql_string.reserve(m_sql_string.capacity() + m_payload_length - 1);
    }
  }
}

void FromClientPacket::collect_data(unsigned char t_received_byte)
{
  if(m_sql_data_receiving) {
    m_sql_string.push_back(static_cast<char>(t_received_byte));
  }
}

void FromClientPacket::data_is_received()
{
  m_sql_data_receiving = false;
}


// ======== FromServerPacket ========

void FromServerPacket::connection_phase_parse(
    unsigned char t_payload_0, MySqlConnectionState& t_connection_state)
{
  // See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_connection_lifecycle.html
  if(m_payload_first_part) {
    const MySqlResponse::Response response =
        MySqlResponse::get_response(t_payload_0, m_payload_length);
    switch(response) {
      case MySqlResponse::Response::OK_PACKET: {
        t_connection_state = MySqlConnectionState::COMMAND_PHASE;
        break;
      }
      case MySqlResponse::Response::EOF_PACKET: {
        // Here we do nothing.
        break;
      }
      case MySqlResponse::Response::ERR_PACKET: {
        std::cout << "Server response: ERR_Packet\n";
        break;
      }
      case MySqlResponse::Response::OTHER_PACKET: {
        // Here we do nothing.
        break;
      }
    }
  }
}

void FromServerPacket::command_phase_parse(unsigned char /*t_payload_0*/)
{
  // Here we do nothing.
}

void FromServerPacket::collect_data(unsigned char /*t_received_byte*/)
{
  // Here we do nothing.
}

void FromServerPacket::data_is_received()
{
  // Here we do nothing.
}

}  // namespace proxy
