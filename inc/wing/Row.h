#pragma once

#include "wing/Value.h"

#include <vector>

#include <mysql/mysql.h>

namespace wing
{

class Query;

class Row
{
    friend Query;
public:
    ~Row() = default;
    Row(const Row&) = delete;
    Row(Row&&) = default;
    auto operator = (const Row&) -> Row& = delete;
    auto operator = (Row&&) -> Row& = default;

    /**
     * @return The number of values/columns in this row.
     */
    auto GetColumnCount() const -> size_t;

    /**
     * @param idx The value to fetch at this index.
     * @return The specified value at idx.
     */
    auto GetColumn(
        size_t idx
    ) const -> const Value&;

private:
    Row(
        MYSQL_ROW mysql_row,
        size_t field_count,
        unsigned long* lengths
    );

    std::vector<Value> m_values;
    size_t m_column_count;
};

} // wing
