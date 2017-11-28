#pragma once

#include "wing/Types.h"

#include <vector>

namespace wing
{

auto split_view(const StringView s, char delim) -> std::vector<StringView>;

auto split_view(const StringView s, const StringView delim) -> std::vector<StringView>;

} // wing
