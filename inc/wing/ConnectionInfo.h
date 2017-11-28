#pragma once

#include <string>

namespace wing
{

class ConnectionInfo
{
public:
    /**
     * Creates connection information for which MySQL server to connect to.
     * @param host The MySQL server hostname.
     * @param port The MySQL server port.
     * @param user The user to authenticate with.
     * @param password The password to authenticate with.
     * @param database The database to use upon connecting (optional).
     * @param client_flags MySQL client flags (optional).
     */
    ConnectionInfo(
        std::string host,
        uint16_t port,
        std::string user,
        std::string password,
        std::string database = "",
        uint64_t client_flags = 0
    );

    /**
     * @return The MySQL server hostname.
     */
    auto GetHost() const -> const std::string&;

    /**
     * @return The MySQL server port.
     */
    auto GetPort() const -> uint16_t;

    /**
     * @return The MySQL user to authenticate with.
     */
    auto GetUser() const -> const std::string&;

    /**
     * @return The MySQL password to authenticate with.
     */
    auto GetPassword() const -> const std::string&;

    /**
     * @return The database to use upon connecting.
     */
    auto GetDatabase() const -> const std::string&;

    /**
     * @return The MySQL client flags.
     */
    auto GetClientFlags() const -> uint64_t;

private:
    std::string m_host;
    uint16_t    m_port;
    std::string m_user;
    std::string m_password;
    std::string m_database;
    uint64_t    m_client_flags;
};

} // wing
