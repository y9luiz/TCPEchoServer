#pragma once

#include <string>
#include <iostream>
#include <fstream>

uint32_t calculateCheckSum(const std::string &str)
{
    uint32_t number = 0;

    for (const char c : str)
    {
        number += c;
    }

    return number;
}

uint32_t nextKey(uint32_t previousKey)
{
    return (previousKey * 1103515245 + 12345) % 0x7FFFFFF;
}

std::string generateCypherKey(std::string username, std::string password, uint16_t sequence, int expectedLen)
{
    std::string cypherKey(expectedLen, '\0');

    uint32_t usernameCheckSum = calculateCheckSum(username);
    uint32_t passwordCheckSum = calculateCheckSum(password);

    std::cout << "usernameCheckSum " << usernameCheckSum << "\n";
    std::cout << "passwordCheckSum " << passwordCheckSum << "\n";

    uint32_t initialKey = (sequence << 16) | (usernameCheckSum << 8) | (passwordCheckSum);

    std::cout << "initial key " << initialKey << "\n";
    cypherKey[0] = nextKey(initialKey) % 256;
    for (int i = 1; i < expectedLen; ++i)
    {
        cypherKey[i] = nextKey(cypherKey[i - 1]) % 256;

        std::cout << "cypherKey [" << i << "] = " << unsigned(uint8_t(cypherKey[i])) << " \n";
    }

    return cypherKey;
}

std::string createCypherMessage(const std::string &messageToCypher, const std::string &username, const std::string &password, uint16_t sequence)
{
    std::string cypherKey = generateCypherKey(username, password, sequence, messageToCypher.size());
    std::string cypherMessage;
    for (int i = 0; i < messageToCypher.size(); ++i)
    {
        cypherMessage += messageToCypher[i] ^ static_cast<uint8_t>(cypherKey[i]);
    }

    return cypherMessage;
}

std::string decypherMessage(const std::string &cypherMessage, const std::string &username, const std::string &password, uint16_t sequence)
{
    std::string cypherKey = generateCypherKey(username, password, sequence, cypherMessage.size());
    std::string output;
    for (int i = 0; i < cypherMessage.size(); ++i)
    {
        output += cypherMessage[i] ^ static_cast<uint8_t>(cypherKey[i]);
    }

    return output;
}