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
    Value(Value&&) = delete;
    auto operator = (const Value&) -> Value& = delete;
    auto operator = (Value&&) -> Value& = default;

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

private:
    explicit Value(StringView data);

    StringView m_data;
};

} // wing
