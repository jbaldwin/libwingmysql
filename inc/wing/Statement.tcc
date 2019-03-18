#include "wing/Statement.h"
#include "wing/Util.h"

#include <type_traits>

namespace wing {

template <typename T>
Statement::Arg::Arg(
    const T& parameter_value)
{
    using RawType = typename std::remove_cv<T>::type;

    // it's a string type if you could construct a string with it as a parameter
    constexpr bool is_string_type = std::is_constructible_v<std::string, RawType>;

    // all string values must be escaped by MySQL
    m_requires_escaping = is_string_type;

    if constexpr (is_string_type) {
        // we know we can just construct the value, don't use the stream
        m_string_value = parameter_value;
    } else {
        // if it's not a string it better be streamable so we can convert
        auto& stream = get_thread_local_stream();
        stream << parameter_value; // error on this line means you're trying to bind using a non-streamable type
        m_string_value = stream.str();
    }
    if (m_string_value.empty()) {
        throw std::invalid_argument("Argument evaulates to empty string");
    }
}

template <typename T>
auto Statement::operator<<(
    const T& statement_part) -> Statement&
{
    auto& stream = get_thread_local_stream();
    stream << statement_part;
    m_statement_parts.emplace_back(stream.str(), false); // false because all items that need escaping use the Arg stream method

    return *this;
}

template <typename EscapeFunctor>
auto Statement::prepareStatement(
    EscapeFunctor escape_functor) -> std::string
{
    std::string final_statement;

    for (auto& part : m_statement_parts) {
        if (part.m_requires_escaping) {
            final_statement.append(escape_functor(part.m_string_value));
        } else {
            final_statement.append(part.m_string_value);
        }
    }

    return final_statement;
}

} // wing
