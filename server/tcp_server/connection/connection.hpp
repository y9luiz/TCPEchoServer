
#pragma once

#include <boost/asio.hpp>

#include <memory>

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    /// @brief Creates a tcp connection
    /// @param context, reference for the parent boost context.
    /// @param usersMap, reference for the parent (server) user in memory database/map
    /// @param usersMutex , reference for the mutex that protects `usersMap`
    Connection(boost::asio::io_context &context,
               std::unordered_map<std::string, std::string> &usersMap, std::mutex &usersMutex);

    virtual ~Connection();

    inline boost::asio::ip::tcp::socket &socket() { return m_socket; }

    /// @brief Start the connection flow.
    /// i.e: receive and send messages in the expected order
    void start();

private:
    void receiveMessages();
    void handleMessages(const std::vector<char> &buffer);

    bool isAuthenticated();

    void handleLoginRequest(const std::vector<char> &buffer);

    void handleEchoRequest(const std::vector<char> &buffer);

    std::mutex &m_usersMutex;                                 // Reference for the server map mutex
    std::unordered_map<std::string, std::string> &m_usersMap; // Reference for the server map
    std::string m_username;                                   // authenticated username
    std::string m_password;                                   // authenticated user password
    uint16_t m_sequence;
    boost::asio::ip::tcp::socket m_socket;
    std::array<char, 256> m_recvBuffer;
    std::vector<char> m_sendBuffer;
    std::mutex m_callbacksMutex;
};
