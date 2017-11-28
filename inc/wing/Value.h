#pragma once

#include "wing/Types.h"

namespace wing
{

class Row;

class Value
{
    friend Row;
public:
    ~Value() = default;
    Value(const Value&) = default;
    Value(Value&&) = default;
    auto operator = (const Value&) -> Value& = default;
    auto operator = (Value&&) -> Value& = default;

    /**
     * Transforms the result value into the specified type.
     * Note that if the conversion fails a default value for
     * that type will be returned.
     *
     * It is also safe to always call AsString() since the
     * MySQL server will always return the data as char[].
     *
     * @return The value as type T.
     * @{
     */
    auto IsNull() const -> bool;
    auto AsString() const -> const StringView;
    auto AsUInt64() const -> uint64_t;
    auto AsInt64() const -> int64_t;
    auto AsUInt32() const -> uint32_t;
    auto AsInt32() const -> int32_t;
    auto AsUInt16() const -> uint16_t;
    auto AsInt16() const -> int16_t;
    auto AsUInt8() const -> uint8_t;
    auto AsInt8() const -> int8_t;
    auto AsBool() const -> bool;
    auto AsFloat() const -> float;
    auto AsDouble() const -> double;
    /** @} */

private:
    explicit Value(StringView data);

    StringView m_data;
};

} // wing
