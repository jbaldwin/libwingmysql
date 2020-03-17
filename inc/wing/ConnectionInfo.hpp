#pragma once

#include <string>

namespace wing {

class ConnectionInfo {
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
        uint64_t client_flags = 0);

    /**
     * Creates connection information for which MySQL server to connect to.
     * @param socket The unix-socket to communicate over.
     * @param user The user to authenticate with.
     * @param password The password to authenticate with.
     * @param database The database to use upon connecting (optional).
     * @param client_flags MySQL client flags (optional).
     */
    ConnectionInfo(
        std::string socket,
        std::string user,
        std::string password,
        std::string database = "",
        uint64_t client_flags = 0);

    /**
     * @return The MySQL server hostname.
     */
    auto Host() const -> const std::string& { return m_host; }

    /**
     * @return The MySQL server port.
     */
    auto Port() const -> uint16_t { return m_port; }

    /**
     * @return The MySQL unix-socket.
     */
    auto Socket() const -> const std::string& { return m_socket; }

    /**
     * @return The MySQL user to authenticate with.
     */
    auto User() const -> const std::string& { return m_user; }

    /**
     * @return The MySQL password to authenticate with.
     */
    auto Password() const -> const std::string& { return m_password; }

    /**
     * @return The database to use upon connecting.
     */
    auto Database() const -> const std::string& { return m_database; }

    /**
     * @return The MySQL client flags.
     */
    auto ClientFlags() const -> uint64_t { return m_client_flags; }

private:
    std::string m_host;
    uint16_t m_port;
    std::string m_socket;
    std::string m_user;
    std::string m_password;
    std::string m_database;
    uint64_t m_client_flags;
};

} // wing
