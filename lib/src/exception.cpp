#include "krado/exception.h"

namespace krado {

const char *
Exception::what() const noexcept
{
    return this->msg.c_str();
}

} // namespace krado
