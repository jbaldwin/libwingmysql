#include "wing/Util.h"

namespace wing
{

auto get_thread_local_stream() -> std::stringstream& {
    thread_local std::stringstream t_stream;

    t_stream.clear();
    t_stream.str("");

    return t_stream;
}

auto split_view(
    const std::string_view s,
    char delim
) -> std::vector<std::string_view>
{
    return split_view(s, std::string_view{&delim, 1});
}

auto split_view(
    const std::string_view s,
    const std::string_view delim
) -> std::vector<std::string_view>
{
    std::vector<std::string_view> output;

    size_t length;
    size_t start = 0;

    while(true)
    {

        size_t next = s.find(delim, start);
        if(next == s.npos)
        {
            // The length of this split is the full string length - start.
            // This is also true if there were no delimiters found at all.
            length = s.length() - start;
            output.emplace_back(s.data() + start, length);
            break;
        }

        // The length of this split is from start to next.
        length = next - start;
        output.emplace_back(s.data() + start, length);

        // Update our iteration to skip the 'next' delimiter found.
        start = next + delim.length();
    }

    return output;
}

} // wing
