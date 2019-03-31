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

#ifndef PROXY_PACKET_HPP
#define PROXY_PACKET_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace proxy
{
// ======== MySqlConnectionState ========

// See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_connection_lifecycle.html
/// Connection states between the client and the MySQL server.
enum class MySqlConnectionState
{
  CONNECTION_PHASE,
  COMMAND_PHASE
};


// ======== MySqlCommand ========

/// Commands from the client to the MySQL server.
class MySqlCommand
{
public:
  MySqlCommand(const MySqlCommand&) = delete;
  MySqlCommand(MySqlCommand&&) = delete;
  MySqlCommand& operator=(const MySqlCommand&) = delete;
  MySqlCommand& operator=(MySqlCommand&&) = delete;

  explicit MySqlCommand() = default;
  ~MySqlCommand() = default;

  // See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_command_phase.html
  enum class Command : unsigned char
  {
    UNKNOWN = 0x00,
    COM_QUIT = 0x01,
    COM_INIT_DB = 0x02,  // has SQL string
    COM_QUERY = 0x03,  // has SQL string
    COM_FIELD_LIST = 0x04,
    COM_REFRESH = 0x07,
    COM_STATISTICS = 0x08,
    COM_PROCESS_INFO = 0x0A,
    COM_PROCESS_KILL = 0x0C,
    COM_DEBUG = 0x0D,
    COM_PING = 0x0E,
    COM_CHANGE_USER = 0x11,
    COM_RESET_CONNECTION = 0x1F,
    //COM_SET_OPTION = 0x1A,  // length == 3, equal to COM_STMT_RESET

    COM_STMT_PREPARE = 0x16,  // has SQL string
    COM_STMT_EXECUTE = 0x17,
    //COM_STMT_FETCH = 0x19,  // length == 9, equal to COM_STMT_CLOSE
    COM_STMT_CLOSE = 0x19,  // length == 5
    COM_STMT_RESET = 0x1A,  // length == 5
    COM_STMT_SEND_LONG_DATA = 0x18
  };

  /// Check if the command is valid.
  static bool is_valid(Command t_command);

  /// Check if the command has the SQL field string.
  static bool has_sql_field(Command t_command);

  /// Get the name string of the command.
  static const char* name(Command t_command, std::uint64_t t_payload_length);

private:
};


// ======== MySqlResponse ========

/// Responses from the MySQL server.
class MySqlResponse
{
public:
  MySqlResponse() = delete;
  MySqlResponse(const MySqlResponse&) = delete;
  MySqlResponse(MySqlResponse&&) = delete;
  MySqlResponse& operator=(const MySqlResponse&) = delete;
  MySqlResponse& operator=(MySqlResponse&&) = delete;

  ~MySqlResponse() = default;

  // See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_basic_response_packets.html
  enum class Response : unsigned char
  {
    OK_PACKET = 0x00,  // length of packet > 7
    EOF_PACKET = 0xFE,  // length of packet < 9
    ERR_PACKET = 0xFF,
    OTHER_PACKET = 0x01
  };

  /// Get the response from the given 0-byte of the MySQL packet payload.
  static Response get_response(
      unsigned char t_payload_0, std::uint64_t t_payload_length);

private:
};


// ======== MySqlPacket ========

// See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_command_phase.html
/// Represents the data packets between the client and the MySQL server.
class MySqlPacket
{
  // See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_basic_packets.html
  /// Data between client and server is exchanged in packets of max 16MByte
  /// size. If the payload is larger than or equal to 2^24-1 bytes
  /// the length is set to 2^24-1 (ff ff ff) and a additional packets are sent
  /// with the rest of the payload until the payload of a packet is less
  /// than 2^24-1 bytes.

  static const std::uint64_t MAX_PAYLOAD_LENGTH = 0xffffff;

  enum class PacketState
  {
    PAYLOAD_LENGTH_0,
    PAYLOAD_LENGTH_1,
    PAYLOAD_LENGTH_2,
    SEQUENCE_ID,
    PAYLOAD
  };

public:
  MySqlPacket(const MySqlPacket&) = delete;
  MySqlPacket(MySqlPacket&&) = delete;
  MySqlPacket& operator=(const MySqlPacket&) = delete;
  MySqlPacket& operator=(MySqlPacket&&) = delete;

  explicit MySqlPacket() = default;
  virtual ~MySqlPacket() = default;

  /// Collects the packet from the incoming bytes.
  void collect(
      unsigned char t_received_byte, MySqlConnectionState& t_connection_state);

  /// Check if the packet is fully received.
  bool is_received() const;

#ifdef PROXY_PACKET_DEBUG
  std::uint64_t payload_length() const;
  unsigned char sequence_id() const;
  const std::vector<unsigned char>& payload() const;
#endif  // ifdef PROXY_PACKET_DEBUG

protected:
  virtual void connection_phase_parse(
      unsigned char t_payload_0, MySqlConnectionState& t_connection_state) = 0;
  virtual void command_phase_parse(unsigned char t_payload_0) = 0;
  virtual void collect_data(unsigned char t_received_byte) = 0;
  virtual void data_is_received() = 0;

  bool m_payload_first_part = true;
  bool m_payload_not_ended = false;
  bool m_payload_is_received = false;

  std::uint64_t m_payload_length = 0;
  unsigned char m_sequence_id = 0;
  std::uint64_t m_received_bytes = 0;

private:
  PacketState m_packet_state = PacketState::PAYLOAD_LENGTH_0;

#ifdef PROXY_PACKET_DEBUG
  std::vector<unsigned char> m_payload;
#endif  // ifdef PROXY_PACKET_DEBUG
};

inline bool MySqlPacket::is_received() const
{
  return m_payload_is_received;
}

#ifdef PROXY_PACKET_DEBUG
inline std::uint64_t MySqlPacket::payload_length() const
{
  return m_payload_length;
}

inline unsigned char MySqlPacket::sequence_id() const
{
  return m_sequence_id;
}

inline const std::vector<unsigned char>& MySqlPacket::payload() const
{
  return m_payload;
}
#endif  // ifdef PROXY_PACKET_DEBUG


// ======== FromClientPacket ========

// See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_command_phase.html
/// Represents the data packets from the client to the MySQL server.
class FromClientPacket : public MySqlPacket
{
public:
  FromClientPacket(const FromClientPacket&) = delete;
  FromClientPacket(FromClientPacket&&) = delete;
  FromClientPacket& operator=(const FromClientPacket&) = delete;
  FromClientPacket& operator=(FromClientPacket&&) = delete;

  explicit FromClientPacket() = default;
  ~FromClientPacket() override = default;

  /// Get the string representation of the client's command.
  const char* get_command_string() const;

  /// Get the SQL field string of the command.
  const std::string& get_sql_string() const;

  /// Check if the command has the SQL field string.
  bool has_sql_string() const;

protected:
  void connection_phase_parse(unsigned char t_payload_0,
      MySqlConnectionState& t_connection_state) override;
  void command_phase_parse(unsigned char t_payload_0) override;
  void collect_data(unsigned char t_received_byte) override;
  void data_is_received() override;

private:
  MySqlCommand::Command m_command = MySqlCommand::Command::UNKNOWN;
  bool m_sql_data_receiving = false;
  std::string m_sql_string;
};

inline const char* FromClientPacket::get_command_string() const
{
  return MySqlCommand::name(m_command, m_payload_length);
}

inline const std::string& FromClientPacket::get_sql_string() const
{
  return m_sql_string;
}

inline bool FromClientPacket::has_sql_string() const
{
  return !m_sql_string.empty();
}


// ======== FromServerPacket ========

// See https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_connection_phase.html
/// Represents the data packets from the MySQL server to the client.
/// Currently used only to set the connection state.
class FromServerPacket : public MySqlPacket
{
public:
  FromServerPacket(const FromServerPacket&) = delete;
  FromServerPacket(FromServerPacket&&) = delete;
  FromServerPacket& operator=(const FromServerPacket&) = delete;
  FromServerPacket& operator=(FromServerPacket&&) = delete;

  explicit FromServerPacket() = default;
  ~FromServerPacket() override = default;

protected:
  void connection_phase_parse(unsigned char t_payload_0,
      MySqlConnectionState& t_connection_state) override;
  void command_phase_parse(unsigned char t_payload_0) override;
  void collect_data(unsigned char t_received_byte) override;
  void data_is_received() override;
};

}  // namespace proxy

#endif  // PROXY_PACKET_HPP
