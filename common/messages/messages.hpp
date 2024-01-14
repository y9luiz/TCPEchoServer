#pragma once

#include <vector>
#include <cstdint>
#include <string>

enum class MessageType : uint8_t
{
    LoginRequest,
    LoginResponse,
    EchoRequest,
    EchoResponse,
    Invalid
};

class Message
{
protected:
    struct Header
    {
        uint16_t size;
        MessageType type;
        uint8_t sequence;

        void encode(std::vector<char> &output);
    };

    Header m_header{};
    Message() = default;
    Message(const std::vector<char> &bytesToDecode);

public:
    virtual ~Message();

    inline int sequence() const
    {
        return m_header.sequence;
    }

    virtual std::vector<char> encode() = 0;
};

class LoginRequestMessage : public Message
{
public:
    LoginRequestMessage(const std::vector<char> &bytesToDecode);
    LoginRequestMessage(const std::string &username, const std::string &password, int sequence);

    ~LoginRequestMessage() override;

    inline std::string username() { return m_username; }
    inline std::string password() { return m_password; }

    std::vector<char> encode() override;

private:
    std::string m_username;
    std::string m_password;
};

class LoginResponseMessage : public Message
{
public:
    enum class ResponseCode : uint16_t
    {
        FAILED,
        OK
    };

    LoginResponseMessage(const std::vector<char> &bytesToDecode);
    LoginResponseMessage(ResponseCode responseCode, int sequence);

    ~LoginResponseMessage() override = default;

    inline ResponseCode responseCode() { return m_responseCode; }

    std::vector<char> encode() override;

private:
    ResponseCode m_responseCode;
};

class EchoRequest : public Message
{
public:
    EchoRequest(const std::vector<char> &bytesToDecode);
    EchoRequest(uint16_t messageSize, std::string cypherMessage, int sequence);

    inline std::string cypherMessage() { return m_cypherMessage; }

    std::vector<char> encode() override;

private:
    uint16_t m_messageSize;
    std::string m_cypherMessage;
};

class EchoResponse : public Message
{
public:
    EchoResponse(const std::vector<char> &bytesToDecode);
    EchoResponse(uint16_t messageSize, std::string plainMessage, int sequence);

    inline std::string plainMessage() const { return m_plainMessage; }

    std::vector<char> encode() override;

private:
    uint16_t m_messageSize;
    std::string m_plainMessage;
};