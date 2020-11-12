#include "wing/Row.hpp"

#include <cstring>

namespace wing {

Row::Row(
    MYSQL_ROW mysql_row,
    size_t field_count,
    unsigned long* lengths)
    : m_column_count(field_count)
{
    m_values.reserve(field_count);
    for (size_t i = 0; i < field_count; ++i) {
        auto* mysql_value = mysql_row[i];
        std::optional<std::string_view> data;
        if (mysql_value != nullptr) {
            auto length = (lengths != nullptr) ? lengths[i] : std::strlen(mysql_value);
            data = std::string_view(mysql_value, length);
        }
        Value value(std::move(data));
        m_values.emplace_back(value);
    }
}

} // wing
