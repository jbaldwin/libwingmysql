#include "wing/Value.hpp"

#include <string>

namespace wing {

Value::Value(
    std::optional<std::string_view> data)
    : m_data(std::move(data))
{
}

auto Value::IsNull() const -> bool
{
    return !m_data.has_value();
}

auto Value::AsStringView() const -> std::optional<std::string_view>
{
    return m_data;
}

auto Value::AsUInt64() const -> std::optional<uint64_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return std::stoul(std::string(m_data.value()));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsInt64() const -> std::optional<int64_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return std::stol(std::string(m_data.value()));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsUInt32() const -> std::optional<uint32_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return static_cast<uint32_t>(std::stoul(std::string(m_data.value())));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsInt32() const -> std::optional<int32_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return std::stoi(std::string(m_data.value()));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsUInt16() const -> std::optional<uint16_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return static_cast<uint16_t>(std::stoul(std::string(m_data.value())));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsInt16() const -> std::optional<int16_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return static_cast<int16_t>(std::stoi(std::string(m_data.value())));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsUInt8() const -> std::optional<uint8_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return static_cast<uint8_t>(std::stoul(std::string(m_data.value())));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsInt8() const -> std::optional<int8_t>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return static_cast<int8_t>(std::stoi(std::string(m_data.value())));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsBool() const -> std::optional<bool>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return (std::stol(std::string(m_data.value())) != 0);
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsFloat() const -> std::optional<float>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return std::stof(std::string(m_data.value()));
    } catch (...) {
    }

    return std::nullopt;
}

auto Value::AsDouble() const -> std::optional<double>
{
    if (IsNull()) {
        return std::nullopt;
    }

    try {
        return std::stod(std::string(m_data.value()));
    } catch (...) {
    }

    return std::nullopt;
}

} // wing
