#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <messages.hpp>
#include <cypher_utils.hpp>

using boost::asio::ip::tcp;

bool parseUserInput(int argc, char *argv[], std::string &username, std::string &password, std::string &message)
{

    if (argc < 4)
    {
        return false;
    }

    username = argv[1];
    password = argv[2];

    bool isFirstWord = true;
    for (int i = 3; i < argc; i++)
    {
        if (isFirstWord)
        {
            message += argv[i];
            isFirstWord = false;
        }
        else
        {
            message += " ";
            message += argv[i];
        }
    }

    return true;
}

int main(int argc, char *argv[])
{

    std::string username, password, originalMessage;
    if (!parseUserInput(argc, argv, username, password, originalMessage))
    {
        std::cout << "usage: ./client <username> <password> <message>\n";
        return 1;
    }

    boost::asio::io_context io_context;

    tcp::socket socket(io_context);
    socket.connect(tcp::endpoint(tcp::v4(), 5000));

    // I've leaved this hardcoded (in the client only). It actually only matter if you
    // going to use multiplexing socket
    int sequence = 13;
    LoginRequestMessage loginRequestMessage(username, password, sequence);
    auto data = loginRequestMessage.encode();

    socket.send(boost::asio::buffer(data));

    boost::array<char, 1024> buffer;
    boost::system::error_code error;
    size_t len = socket.read_some(boost::asio::buffer(buffer), error);

    std::vector<char> vec(buffer.begin(), buffer.begin() + len);

    LoginResponseMessage loginResponseMessage(vec);

    std::cout << "Login sStatus: "
              << ((static_cast<int>(loginResponseMessage.responseCode()) == 1) ? "OK" : "Failed") << "\n";

    auto cypherMessage = createCypherMessage(originalMessage, username, password, sequence);

    EchoRequest echoRequest(cypherMessage.size(), cypherMessage, sequence);

    std::cout << "sending echo request\n";
    socket.send(boost::asio::buffer(echoRequest.encode()));

    len = socket.read_some(boost::asio::buffer(buffer), error);

    EchoResponse echoResponse({buffer.begin(), buffer.begin() + len});

    std::cout << "got an echo response: " << echoResponse.plainMessage() << "\n";

    std::cout << (echoResponse.plainMessage() == originalMessage) ? "The message is equals to the original\n" : "wrong message, bad server\n";

    return 0;
}