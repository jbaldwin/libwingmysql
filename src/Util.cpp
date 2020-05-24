#include "wing/Util.hpp"

namespace wing {

auto get_thread_local_stream() -> std::stringstream&
{
    thread_local std::stringstream t_stream;

    t_stream.clear();
    t_stream.str("");

    return t_stream;
}

} // wing
