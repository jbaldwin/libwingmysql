#include "wing/Connection.h"

namespace wing
{

Connection::Connection(
    std::string host,
    uint16_t port,
    std::string user,
    std::string password,
    std::string database,
    uint64_t client_flags
)
    : m_host(std::move(host)),
      m_port(port),
      m_user(std::move(user)),
      m_password(std::move(password)),
      m_database(std::move(database)),
      m_client_flags(client_flags)
{

}

auto Connection::GetHost() const -> const std::string& {
    return m_host;
}

auto Connection::GetPort() const -> uint16_t {
    return m_port;
}

auto Connection::GetUser() const -> const std::string& {
    return m_user;
}

auto Connection::GetPassword() const -> const std::string& {
    return m_password;
}

auto Connection::GetDatabase() const -> const std::string& {
    return m_database;
}

auto Connection::GetClientFlags() const -> uint64_t {
    return m_client_flags;
}

} // wing
