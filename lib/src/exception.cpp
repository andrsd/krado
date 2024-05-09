// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/exception.h"

namespace krado {

const char *
Exception::what() const noexcept
{
    return this->msg.c_str();
}

} // namespace krado
