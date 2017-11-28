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
    const StringView s,
    char delim
) -> std::vector<StringView>;

/**
 * Splits a string view by the delimiter.
 * @param s The string to split.
 * @param delim The delimiter to split on.
 * @return String views into each partition.
 */
auto split_view(
    const StringView s,
    const StringView delim
) -> std::vector<StringView>;

} // wing
