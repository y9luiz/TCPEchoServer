#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class TCPEchoServer {
public:
  class Impl;

  /// @brief Starts a TCP echo server using a given address and port.
  /// @param address, ip address for the server
  /// @param port, port to bind
  TCPEchoServer(std::string_view address, int port);

  virtual ~TCPEchoServer();

private:
  void startAcceptConnections();

  std::unique_ptr<Impl> m_impl;
};