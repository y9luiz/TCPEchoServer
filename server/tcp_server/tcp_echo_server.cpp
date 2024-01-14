#include "tcp_echo_server.hpp"
#include "connection/connection.hpp"

#include <boost/asio.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <array>
#include <thread>

using namespace std;
using boost::asio::ip::tcp;

class TCPEchoServer::Impl
{
public:
  Impl(string_view address, int port)
      : m_address(address), m_port(port),
        m_acceptor(
            m_context,
            tcp::endpoint(boost::asio::ip::address::from_string(m_address),
                          m_port))
  {
    doAcceptConnections();
  }

  virtual ~Impl()
  {
    if (m_contextThread)
    {
      m_contextThread->join();
    }
  }

  void addUser(const std::string &username, const std::string &password)
  {
    lock_guard lk(m_usersMutex);

    m_usersMap[username] = password;
  }

  void stop()
  {
    m_context.stop();
  }

private:
  void doAcceptConnections()
  {
    lock_guard lk(m_usersMutex);

    auto connection =
        make_shared<Connection>(m_context, m_usersMap, m_usersMutex);
    m_acceptor.async_accept(connection->socket(), [connection, this](auto &ec)
                            {
      if(!ec)
      {
        connection->start();

        doAcceptConnections();
      } });

    if (!m_contextThread)
    {
      m_contextThread = make_unique<thread>([this]()
                                            { m_context.run(); });
    }
  }

  string m_address;
  int m_port;
  std::mutex m_usersMutex;
  std::unordered_map<string, string> m_usersMap;
  boost::asio::io_context m_context;
  unique_ptr<thread> m_contextThread;
  boost::asio::ip::tcp::acceptor m_acceptor;
};

TCPEchoServer::TCPEchoServer(string_view address, int port)
    : m_impl(make_unique<TCPEchoServer::Impl>(address, port)) {}

TCPEchoServer::~TCPEchoServer() = default;

void TCPEchoServer::addUser(const std::string &username, const std::string &password)
{
  m_impl->addUser(username, password);
}

void TCPEchoServer::stop()
{
  m_impl->stop();
}