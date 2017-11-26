#include "wing/Value.h"

namespace wing
{

Value::Value(StringView data)
    : m_data(data)
{

}

auto Value::IsNull() const -> bool {
    return m_data.empty();
}

auto Value::AsString() const -> const StringView {
    return m_data;
}

auto Value::AsUInt64() const -> uint64_t {
    uint64_t value = 0;
    try
    {
        value = std::stoul(m_data.to_string());
    }
    catch(...) { }

    return value;
}

auto Value::AsInt64() const -> int64_t {
    int64_t value = 0;
    try
    {
        value = std::stol(m_data.to_string());
    }
    catch(...) { }
    return value;
}

auto Value::AsUInt32() const -> uint32_t {
    uint32_t value = 0;
    try
    {
        value = static_cast<uint32_t>(std::stoul(m_data.to_string()));
    }
    catch(...) { }
    return value;
}

auto Value::AsInt32() const -> int32_t {
    int32_t value = 0;
    try
    {
        value = std::stoi(m_data.to_string());
    }
    catch(...) { }
    return value;
}

auto Value::AsUInt16() const -> uint16_t {
    uint16_t value = 0;
    try
    {
        value = static_cast<uint16_t>(std::stoul(m_data.to_string()));
    }
    catch(...) { }
    return value;
}

auto Value::AsInt16() const -> int16_t {
    int16_t value = 0;
    try
    {
        value = static_cast<int16_t>(std::stoi(m_data.to_string()));
    }
    catch(...) { }
    return value;
}

auto Value::AsUInt8() const -> uint8_t {
    uint8_t value = 0;
    try
    {
        value = static_cast<uint8_t>(std::stoul(m_data.to_string()));
    }
    catch(...) { }
    return value;
}

auto Value::AsInt8() const -> int8_t {
    int8_t value = 0;
    try
    {
        value = static_cast<int8_t>(std::stoi(m_data.to_string()));
    }
    catch(...) { }
    return value;
}

auto Value::AsBool() const -> bool {
    try
    {
        int64_t value = stol(m_data.to_string());
        return (value != 0);
    }
    catch(...) { }

    return false;
}

auto Value::AsFloat() const -> float {
    float value = 0;
    try
    {
        value = stof(m_data.to_string());
    } catch(...) { }
    return value;
}

auto Value::AsDouble() const -> double {
    double value = 0;
    try
    {
        value = stod(m_data.to_string());
    } catch(...) { }
    return value;
}

} // wing
