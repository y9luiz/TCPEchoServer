#include <iostream>
#include <tcp_echo_server.hpp>
#include <thread>

using namespace std;

int main() {
  cout << "done\n";

  TCPEchoServer echoServer("127.0.0.1", 5000);

  this_thread::sleep_for(std::chrono::seconds(30));
  return 0;
}