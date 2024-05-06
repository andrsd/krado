#include "krado/size_parameters.h"

namespace krado {

Parameters &
SizeParameters::size_parameters()
{
    return this->spars;
}

const Parameters &
SizeParameters::size_parameters() const
{
    return this->spars;
}

} // namespace krado
