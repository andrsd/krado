#pragma once

#include "krado/parameters.h"

namespace krado {

class SizeParameters {
public:
    Parameters & size_parameters();

    const Parameters & size_parameters() const;

private:
    Parameters spars;
};

} // namespace krado
