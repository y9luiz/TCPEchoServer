#include "tcp_echo_server.hpp"

#include <boost/asio.hpp>

#include <functional>
#include <iostream>
#include <memory>

using namespace std;
using boost::asio::ip::tcp;

class Connection : enable_shared_from_this<Connection> {
public:
  Connection(boost::asio::io_context &context,
             std::unordered_map<string, string> &usersMap, mutex &usersMutex)
      : m_socket(context), m_usersMap(usersMap), m_usersMutex(usersMutex) {
    createMessageCallbacks();
  }

  tcp::socket &socket() { return m_socket; }

  void start() {

    m_socket.async_receive(boost::asio::buffer(m_recvBuffer),
                           [this](const auto &error, auto dataLen) {
                             char messageType = m_recvBuffer[2];
                             m_messageCallbacksMap[messageType](m_recvBuffer,
                                                                dataLen);
                           });
  }

private:
  void createMessageCallbacks() {
    // m_messageCallbacksMap.insert({0, [this](const auto &data, size_t len) {
    //                                 handleLoginRequest(data, len);
    //                               }});

    // m_messageCallbacksMap.insert({2, [this](const auto &data, size_t len) {
    //                                 handleEchoRequest(data, len);
    //                               }});
  }

  void handleLoginRequest(const vector<char> &buffer, size_t len) {
    std::string username;
    constexpr int UsernameStart = 4;
    constexpr int UsernameSize = 32;

    username.assign(buffer.cbegin() + UsernameStart,
                    buffer.cbegin() + UsernameStart + UsernameSize);
    {
      lock_guard<mutex> lk(m_usersMutex);
      auto mapIt = m_usersMap.find(username);

      if (mapIt != m_usersMap.end() && mapIt->first == username) {
        cout << "User: " << username << " found\n";
        std::string password;
        constexpr int PasswordStart = UsernameStart + UsernameSize;
        constexpr int PasswordSize = 32;

        password.assign(buffer.cbegin() + PasswordStart,
                        buffer.cbegin() + PasswordStart + PasswordSize);

        if (password == mapIt->second) {
          cout << "Password match\n";
          std::string msg = "hello";
          m_socket.async_send(boost::asio::buffer(msg), [](auto, auto) {});
        } else {
          cout << "incorrect password\n";
        }
      } else {
        cout << "User not found";
      }
    }
  }

  void handleEchoRequest(const vector<char> &buffer, size_t len) {}

  using MessageCallback = std::function<void(const vector<char> &, size_t)>;

  std::mutex &m_usersMutex;
  std::unordered_map<string, string> &m_usersMap;
  tcp::socket m_socket;
  vector<char> m_recvBuffer;
  vector<char> m_sendBuffer;
  std::unordered_map<char, MessageCallback> m_messageCallbacksMap;
};

class TCPEchoServer::Impl {
public:
  Impl(string_view address, int port)
      : m_address(address), m_port(port), m_socket(m_context),
        m_acceptor(
            m_context,
            tcp::endpoint(boost::asio::ip::address::from_string(m_address),
                          m_port)) {

    startAcceptConnections();
  }

  virtual ~Impl() = default;

private:
  void startAcceptConnections() {
    auto connection =
        make_shared<Connection>(m_context, m_usersMap, m_usersMutex);
    m_acceptor.async_accept(connection->socket(),
                            [connection, this](const auto errorCode) {
                              cout << "got a new connection\n";
                              connection->start();
                            });
  }

  string m_address;
  int m_port;
  std::mutex m_usersMutex;
  std::unordered_map<string, string> m_usersMap;
  boost::asio::io_context m_context;
  boost::asio::ip::tcp::socket m_socket;
  boost::asio::ip::tcp::acceptor m_acceptor;
};

TCPEchoServer::TCPEchoServer(string_view address, int port)
    : m_impl(make_unique<TCPEchoServer::Impl>(address, port)) {}

TCPEchoServer::~TCPEchoServer() = default;
