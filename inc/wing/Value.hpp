#pragma once

#include <optional>
#include <string_view>

namespace wing {

class Row;

class Value {
    friend Row;

public:
    ~Value() = default;
    Value(const Value&) = default;
    Value(Value&&) = default;
    auto operator=(const Value&) -> Value& = default;
    auto operator=(Value &&) -> Value& = default;

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
    auto AsStringView() const -> std::optional<std::string_view>;
    auto AsUInt64() const -> std::optional<uint64_t>;
    auto AsInt64() const -> std::optional<int64_t>;
    auto AsUInt32() const -> std::optional<uint32_t>;
    auto AsInt32() const -> std::optional<int32_t>;
    auto AsUInt16() const -> std::optional<uint16_t>;
    auto AsInt16() const -> std::optional<int16_t>;
    auto AsUInt8() const -> std::optional<uint8_t>;
    auto AsInt8() const -> std::optional<int8_t>;
    auto AsBool() const -> std::optional<bool>;
    auto AsFloat() const -> std::optional<float>;
    auto AsDouble() const -> std::optional<double>;
    /** @} */

private:
    explicit Value(
        std::optional<std::string_view> data);

    std::optional<std::string_view> m_data;
};

} // wing
