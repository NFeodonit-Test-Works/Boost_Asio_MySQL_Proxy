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

#include "server.hpp"

#include <csignal>
#include <cstddef>
#include <utility>

namespace proxy
{
Server::Server(const std::string& t_client_address,
    const std::string& t_client_port,
    const std::string& t_server_address,
    const std::string& t_server_port,
    const std::string& t_log_file_path)
    : m_io_context(1)
    , m_signals(m_io_context)
    , m_acceptor(m_io_context)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  m_signals.add(SIGINT);
  m_signals.add(SIGTERM);
#if defined(SIGQUIT)
  m_signals.add(SIGQUIT);
#endif  // if defined(SIGQUIT)

  do_await_stop();

  // Start listening on the client socket.
  boost::asio::ip::tcp::resolver resolver(m_io_context);
  boost::asio::ip::tcp::endpoint client_ep =
      *resolver.resolve(t_client_address, t_client_port).begin();

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  m_acceptor.open(client_ep.protocol());
  m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  m_acceptor.bind(client_ep);
  m_acceptor.listen();

  do_accept();
}

void Server::run()
{
  // The io_context::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  m_io_context.run();
}

void Server::do_accept()
{
  m_acceptor.async_accept([this](boost::system::error_code l_error,
                              boost::asio::ip::tcp::socket l_client_socket) {
    // Check whether the server was stopped by a signal before this
    // completion handler had a chance to run.
    if(!m_acceptor.is_open()) {
      return;
    }

    if(!l_error) {
      m_connection_manager.start(std::make_shared<Connection>(
          std::move(l_client_socket), m_connection_manager));
    }

    do_accept();
  });
}

void Server::do_await_stop()
{
  m_signals.async_wait(
      [this](boost::system::error_code /*l_error*/, int /*l_signo*/) {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_context::run()
        // call will exit.
        m_acceptor.close();
        m_connection_manager.stop_all();
      });
}

}  // namespace proxy
