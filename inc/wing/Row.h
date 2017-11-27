#pragma once

#include "wing/Value.h"

#include <mysql/mysql.h>
#include <vector>

namespace wing
{

class QueryHandle;

class Row
{
    friend QueryHandle;
public:
    ~Row() = default;
    Row(const Row&) = delete;
    Row(Row&&) = default;
    auto operator = (const Row&) -> Row& = delete;
    auto operator = (Row&&) -> Row& = default;

    auto GetColumn(size_t idx) const -> const Value&;
    auto GetValues() const -> const std::vector<Value>&;

private:
    Row(MYSQL_ROW mysql_row, size_t field_count);

    std::vector<Value> m_values;
};

} // wing
