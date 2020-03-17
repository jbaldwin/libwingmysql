#pragma once

#include <sstream>
#include <string_view>
#include <vector>

namespace wing {

/**
 * gets a thread local stream
 *
 * NOTE: places that use this must never call each other
 * If this stream is ever used in another method it must not be able to recursively
 * call itself or another method that uses this stream or they can trample each other.
 *
 * @return a reference to a stringstream unique to the calling thread
 */
auto get_thread_local_stream() -> std::stringstream&;

/**
 * Splits a string view by the delimiter.
 * @param s The string to split.
 * @param delim The delimiter to split on.
 * @return String views into each partition.
 */
auto split_view(
    std::string_view s,
    char delim) -> std::vector<std::string_view>;

/**
 * Splits a string view by the delimiter.
 * @param s The string to split.
 * @param delim The delimiter to split on.
 * @return String views into each partition.
 */
auto split_view(
    std::string_view s,
    std::string_view delim) -> std::vector<std::string_view>;

} // wing
