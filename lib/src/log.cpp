// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/log.h"

namespace krado {

namespace {

bool
initialize()
{
    spdlog::set_pattern("[%^%l%$] %v");
    return true;
}

static bool initialized_ = initialize();

} // namespace

int Log::verbosity_ = 1;

void
Log::set_verbosity(int level)
{
    verbosity_ = level;
    switch (level) {
    case 0:
        spdlog::set_level(spdlog::level::err);
        break;
    case 1:
        spdlog::set_level(spdlog::level::warn);
        break;
    case 2:
        spdlog::set_level(spdlog::level::info);
        break;
    case 3:
    default:
        spdlog::set_level(spdlog::level::debug);
        break;
    }
}

} // namespace krado
