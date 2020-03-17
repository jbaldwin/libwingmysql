#include "wing/Statement.hpp"
#include "wing/Util.hpp"

namespace wing {

auto Statement::operator<<(
    Arg parameter) -> Statement&
{
    // convert into StatementPart respecting whether this arg needed escaping
    m_statement_parts.emplace_back(std::move(parameter.m_string_value), parameter.m_requires_escaping);

    return *this;
}

Statement::StatementPart::StatementPart(
    std::string value,
    bool requires_escaping)
    : m_string_value(std::move(value))
    , m_requires_escaping(requires_escaping)
{
}

} // wing
