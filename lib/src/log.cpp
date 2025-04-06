// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/log.h"

namespace krado {

int Log::verbosity_ = 0;

void
Log::set_verbosity(int level)
{
    verbosity_ = level;
}

} // namespace krado
