#pragma once

#include "wing/Request.h"

namespace wing
{

class IRequestCallback
{
public:
    virtual ~IRequestCallback() = default;
    virtual auto OnComplete(wing::Request request) -> void = 0;
};

} // wing