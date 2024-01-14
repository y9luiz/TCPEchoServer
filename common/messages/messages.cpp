#include "messages.hpp"

#include <stdexcept>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

namespace
{
    constexpr auto LoginRequestExpectedLen = 68;
    constexpr auto LoginResponseExpectedLen = 6;
}

void Message::Header::encode(vector<char> &output)
{
    // Message size (including header)
    output[0] = static_cast<char>(size & 0xFF);
    output[1] = static_cast<char>(size >> 8 & 0xFF);

    // Message type
    output[2] = static_cast<uint8_t>(type);

    // Sequence
    output[3] = sequence;
}

Message::Message(const vector<char> &bytesToDecode)
{
    if (bytesToDecode.size() < sizeof(Header))
    {
        throw invalid_argument("Byte sequence smaller than message header.");
    }

    m_header.size = bytesToDecode[0] + (bytesToDecode[1] << 8);
    m_header.type = static_cast<MessageType>(bytesToDecode[2]);
    m_header.sequence = bytesToDecode[3];
}

Message::~Message() = default;

LoginRequestMessage::LoginRequestMessage(const std::vector<char> &bytesToDecode) : Message(bytesToDecode)
{
    if (bytesToDecode.size() < LoginRequestExpectedLen)
    {
        throw invalid_argument("Byte sequence smaller than required login request message size.");
    }

    constexpr int UsernameStart = 4;
    constexpr int UsernameSize = 32;

    auto zerosUsernameIt = std::find(bytesToDecode.begin() + UsernameStart,
                                     bytesToDecode.begin() + UsernameStart + UsernameSize, 0);

    m_username.assign(bytesToDecode.cbegin() + UsernameStart, zerosUsernameIt);

    std::cout << m_username.length() << "\n";
    constexpr int PasswordStart = UsernameStart + UsernameSize;
    constexpr int PasswordSize = 32;

    auto zerosPasswordIt = std::find(bytesToDecode.begin() + PasswordStart,
                                     bytesToDecode.begin() + PasswordStart + PasswordSize, 0);

    m_password.assign(bytesToDecode.cbegin() + PasswordStart,
                      zerosPasswordIt);
}

LoginRequestMessage::LoginRequestMessage(const std::string &username, const std::string &password, int sequence)
    : m_username(username), m_password(password)
{
    m_header.type = MessageType::LoginRequest;
    m_header.size = 68;
    m_header.sequence = sequence;
}

LoginRequestMessage::~LoginRequestMessage() = default;

vector<char> LoginRequestMessage::encode()
{
    vector<char> output(m_header.size, 0);

    m_header.encode(output);

    output.insert(output.begin() + sizeof(Header), m_username.begin(), m_username.end());

    constexpr auto UsernameLen = 32;

    output.insert(output.begin() + sizeof(Header) + UsernameLen, m_password.begin(), m_password.end());

    return output;
}

LoginResponseMessage::LoginResponseMessage(const vector<char> &bytesToDecode)
    : Message(bytesToDecode)
{
    if (bytesToDecode.size() < LoginResponseExpectedLen)
    {
        throw invalid_argument("Byte sequence smaller than required login response message size.");
    }

    m_responseCode = static_cast<ResponseCode>(bytesToDecode[4] + (bytesToDecode[5] << 8));
}

LoginResponseMessage::LoginResponseMessage(ResponseCode responseCode, int sequence)
    : m_responseCode(responseCode)
{
    m_header.type = MessageType::LoginResponse;
    m_header.size = sizeof(Header) + sizeof(m_responseCode);
    m_header.sequence = sequence;
}

vector<char> LoginResponseMessage::encode()
{
    vector<char> output(m_header.size, 0);

    m_header.encode(output);

    // Status Code
    output[4] = static_cast<char>(static_cast<uint16_t>(m_responseCode) & 0xFF);
    output[5] = static_cast<char>(static_cast<uint16_t>(m_responseCode) >> 8 & 0xFF);

    return output;
}

EchoRequest::EchoRequest(const std::vector<char> &bytesToDecode)
    : Message(bytesToDecode)
{
    int cypherMessageSize = bytesToDecode[4] + (bytesToDecode[5] << 8);
    m_cypherMessage.reserve(cypherMessageSize);

    auto start = bytesToDecode.begin() + sizeof(Header) + sizeof(m_messageSize);
    auto end = bytesToDecode.begin() + sizeof(Header) + sizeof(m_messageSize) + cypherMessageSize;

    copy(start, end, back_inserter(m_cypherMessage));
}

EchoRequest::EchoRequest(uint16_t messageSize, string cypherMessage, int sequence) : m_cypherMessage(cypherMessage), m_messageSize(messageSize)
{
    m_cypherMessage.resize(m_messageSize);

    m_header.size = sizeof(Header) + sizeof(m_messageSize) + m_cypherMessage.size();
    m_header.type = MessageType::EchoRequest;
    m_header.sequence = sequence;
}

vector<char> EchoRequest::encode()
{
    vector<char> output(m_header.size, 0);

    m_header.encode(output);

    // Message size
    output[4] = static_cast<char>(static_cast<uint16_t>(m_messageSize) & 0xFF);
    output[5] = static_cast<char>(static_cast<uint16_t>(m_messageSize) >> 8 & 0xFF);

    // The message itself
    copy(m_cypherMessage.begin(), m_cypherMessage.end(), output.begin() + sizeof(Header) + sizeof(m_messageSize));

    return output;
}

EchoResponse::EchoResponse(const std::vector<char> &bytesToDecode)
    : Message(bytesToDecode)
{
    uint16_t plainMessageSize = bytesToDecode[4] + (bytesToDecode[5] << 8);

    m_plainMessage.reserve(plainMessageSize);
    auto start = bytesToDecode.begin() + sizeof(Header) + sizeof(plainMessageSize);
    auto end = bytesToDecode.begin() + sizeof(Header) + sizeof(plainMessageSize) + plainMessageSize;
    copy(start, end, back_inserter(m_plainMessage));
}

EchoResponse::EchoResponse(uint16_t messageSize, std::string plainMessage, int sequence)
    : m_plainMessage(plainMessage), m_messageSize(messageSize)
{
    m_plainMessage.resize(m_messageSize);

    m_header.size = sizeof(Header) + sizeof(m_messageSize) + m_plainMessage.size();
    m_header.type = MessageType::EchoRequest;
    m_header.sequence = sequence;
}

vector<char> EchoResponse::encode()
{
    vector<char> output(m_header.size, 0);

    m_header.encode(output);

    // Message size
    output[4] = static_cast<char>(static_cast<uint16_t>(m_messageSize) & 0xFF);
    output[5] = static_cast<char>(static_cast<uint16_t>(m_messageSize) >> 8 & 0xFF);

    // The message itself
    copy(m_plainMessage.begin(), m_plainMessage.end(), output.begin() + sizeof(Header) + sizeof(m_messageSize));

    return output;
}
