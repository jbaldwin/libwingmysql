#pragma once

#include <string>

namespace wing
{

class Connection
{
public:
    Connection(
        std::string host,
        uint16_t port,
        std::string user,
        std::string password,
        std::string database,
        uint64_t client_flags
    );

    auto GetHost() const -> const std::string&;
    auto GetPort() const -> uint16_t;
    auto GetUser() const -> const std::string&;
    auto GetPassword() const -> const std::string&;
    auto GetDatabase() const -> const std::string&;
    auto GetClientFlags() const -> uint64_t;

private:
    std::string m_host;
    uint16_t m_port;
    std::string m_user;
    std::string m_password;
    std::string m_database;
    uint64_t m_client_flags;
};

} // wing
