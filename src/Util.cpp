#include "wing/Util.h"

namespace wing
{

auto split_view(const StringView s, char delim) -> std::vector<StringView>
{
    return split_view(s, StringView{&delim, 1});
}


auto split_view(const StringView s, const StringView delim) -> std::vector<StringView> {

    std::vector<StringView> output;

    size_t length;
    size_t start = 0;

    while(true) {

        size_t next = s.find(delim, start);
        if(next == s.npos) {
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
