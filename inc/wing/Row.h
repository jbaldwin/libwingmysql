#pragma once

#include "wing/Value.h"

#include <vector>

#include <mysql/mysql.h>

namespace wing
{

class QueryHandle;

class Row
{
    friend QueryHandle;
public:
    ~Row() = default;
    Row(const Row&) = delete;                       ///< No copying
    Row(Row&&) = default;                           ///< Can move
    auto operator = (const Row&) -> Row& = delete;  ///< No copying
    auto operator = (Row&&) -> Row& = default;      ///< Can move

    /**
     * @param idx The value to fetch at this index.
     * @return The specified value at idx.
     */
    auto GetColumn(
        size_t idx
    ) const -> const Value&;

    /**
     * @return The ordered set of values in this row.
     */
    auto GetValues() const -> const std::vector<Value>&;

private:
    Row(
        MYSQL_ROW mysql_row,
        size_t field_count
    );

    std::vector<Value> m_values;
};

} // wing
