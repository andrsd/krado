// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace krado {

namespace {

static bool initialized_ = Log::initialize();

} // namespace

int Log::verbosity_ = 1;
std::shared_ptr<spdlog::logger> Log::logger_;

bool
Log::initialize()
{
    logger_ = spdlog::stdout_color_mt(krado::Log::LOGGER_NAME);
    spdlog::set_pattern("[%^%l%$] %v");
    return true;
}

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

void
Log::set_logger(std::shared_ptr<spdlog::logger> logger)
{
    spdlog::drop(LOGGER_NAME);
    logger_ = logger;
}

} // namespace krado
