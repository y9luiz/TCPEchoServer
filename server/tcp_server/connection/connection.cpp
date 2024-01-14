#include "connection.hpp"
#include <messages.hpp>
#include <cypher_utils.hpp>

#include <iostream>

using namespace std;
using boost::asio::ip::tcp;

Connection::Connection(boost::asio::io_context &context,
                       std::unordered_map<string, string> &usersMap, mutex &usersMutex)
    : m_socket(context), m_usersMap(usersMap), m_usersMutex(usersMutex)
{
}

Connection::~Connection()
{
    cout << "connection closed\n";
}

void Connection::start()
{
    receiveMessages();
}

void Connection::receiveMessages()
{
    auto self = shared_from_this();
    m_socket.async_receive(boost::asio::buffer(m_recvBuffer),
                           [self](const auto &error, auto dataLen)
                           {
                               cout << "got message\n";

                               vector<char> buffer(self->m_recvBuffer.begin(), self->m_recvBuffer.end());

                               self->handleMessages(buffer);
                           });
}

void Connection::handleMessages(const vector<char> &buffer)
{
    switch (static_cast<MessageType>(buffer[2]))
    {
    case MessageType::LoginRequest:
    {
        cout << "Got login request\n";
        handleLoginRequest(buffer);
        if (isAuthenticated())
        {
            receiveMessages();
        }
        return;
    }
    case MessageType::EchoRequest:
    {
        if (!isAuthenticated())
        {
            cout << "User not authenticated,ignoring echo request\n";
            return;
        }

        cout << "Got echo request\n";
        handleEchoRequest(buffer);
        return;
    }
    }

    cout << "Invalid message type\n";
}

bool Connection::isAuthenticated()
{
    return !m_username.empty() || !m_password.empty();
}

void Connection::handleLoginRequest(const vector<char> &buffer)
{
    try
    {
        LoginRequestMessage message(buffer);

        lock_guard lk(m_usersMutex);

        auto it = m_usersMap.find(message.username());

        if (it != m_usersMap.end() && it->second == message.password())
        {
            cout << "user authenticated\n";
            m_username = message.username();
            m_password = message.password();
            m_sequence = message.sequence();

            LoginResponseMessage response(LoginResponseMessage::ResponseCode::OK, message.sequence());

            m_sendBuffer = response.encode();
        }
        else
        {
            cout << "user: " << message.username() << " not authorized\n";

            LoginResponseMessage response(LoginResponseMessage::ResponseCode::FAILED, message.sequence());
            m_sendBuffer = response.encode();
        }

        m_socket.async_send(boost::asio::buffer(m_sendBuffer), [](const auto &ec, auto bytesTransferred)
                            {
        if(!ec)
          {
            cout << "Status message sent.\n";
            return;
          }

          cout << "Was not possible to send status message.\n";
          cout << ec.message() << "\n"; });
    }
    catch (const invalid_argument &e)
    {
        cout << "Invalid message: " << e.what() << "\n";
    }
}

void Connection::handleEchoRequest(const vector<char> &buffer)
{
    try
    {
        EchoRequest request(buffer);
        auto cypherMessage = request.cypherMessage();
        auto message = decypherMessage(cypherMessage, m_username, m_password, m_sequence);

        EchoResponse response(message.size(), message, m_sequence);

        m_sendBuffer = response.encode();

        m_socket.async_send(boost::asio::buffer(m_sendBuffer), [](const auto &ec, auto bytesTransferred)
                            {
        if(!ec)
          {
            cout << "Status message sent.\n";
            return;
          }

          cout << "Was not possible to send status message.\n";
          cout << ec.message() << "\n"; });
    }
    catch (const invalid_argument &e)
    {
        cout << "Invalid message: " << e.what() << "\n";
    }
}
