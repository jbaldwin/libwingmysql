#include "wing/ConnectionInfo.hpp"

namespace wing {

ConnectionInfo::ConnectionInfo(
    std::string host,
    uint16_t port,
    std::string user,
    std::string password,
    std::string database,
    uint64_t client_flags)
    : m_host(std::move(host))
    , m_port(port)
    , m_socket("0")
    , m_user(std::move(user))
    , m_password(std::move(password))
    , m_database(std::move(database))
    , m_client_flags(client_flags)
{
}

ConnectionInfo::ConnectionInfo(
    std::string socket,
    std::string user,
    std::string password,
    std::string database,
    uint64_t client_flags)
    : m_host("localhost")
    , m_port(0)
    , m_socket(std::move(socket))
    , m_user(std::move(user))
    , m_password(std::move(password))
    , m_database(std::move(database))
    , m_client_flags(client_flags)
{
}

} // wing
