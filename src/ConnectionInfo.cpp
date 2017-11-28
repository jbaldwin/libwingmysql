#include "wing/ConnectionInfo.h"

namespace wing
{

ConnectionInfo::ConnectionInfo(
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

auto ConnectionInfo::GetHost() const -> const std::string& {
    return m_host;
}

auto ConnectionInfo::GetPort() const -> uint16_t {
    return m_port;
}

auto ConnectionInfo::GetUser() const -> const std::string& {
    return m_user;
}

auto ConnectionInfo::GetPassword() const -> const std::string& {
    return m_password;
}

auto ConnectionInfo::GetDatabase() const -> const std::string& {
    return m_database;
}

auto ConnectionInfo::GetClientFlags() const -> uint64_t {
    return m_client_flags;
}

} // wing
