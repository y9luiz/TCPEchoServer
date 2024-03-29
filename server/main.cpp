#include <iostream>
#include <tcp_echo_server.hpp>

using namespace std;

int main()
{
  TCPEchoServer echoServer("127.0.0.1", 5000);

  cout << "TCP echo server running at port 5000\n";

  char k;
  while (true)
  {
    cout << "\tPress q, to quit\n";
    cout << "\tPress a, to add user\n";

    cin >> k;

    if (k == 'q')
    {
      echoServer.stop();
      break;
    }

    if (k == 'a')
    {
      string username;
      string password;

      cout << "insert username: ";
      cin >> username;

      cout << "insert password: ";
      cin >> password;

      echoServer.addUser(username, password);
    }
  }

  return 0;
}