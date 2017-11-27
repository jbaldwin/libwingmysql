#pragma once

#include "wing/Query.h"

namespace wing
{

class IQueryCallback
{
public:
    virtual ~IQueryCallback() = default;
    virtual auto OnComplete(wing::Query request) -> void = 0;
};

} // wing