#pragma once

#include "wing/Types.h"

#include <vector>

namespace wing
{

/**
 * Splits a string view by the delimiter.
 * @param s The string to split.
 * @param delim The delimiter to split on.
 * @return String views into each partition.
 */
auto split_view(
    const std::string_view s,
    char delim
) -> std::vector<std::string_view>;

/**
 * Splits a string view by the delimiter.
 * @param s The string to split.
 * @param delim The delimiter to split on.
 * @return String views into each partition.
 */
auto split_view(
    const std::string_view s,
    const std::string_view delim
) -> std::vector<std::string_view>;

} // wing
