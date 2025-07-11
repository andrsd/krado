// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <string>

namespace krado {

/// Simple wrapper around spdlog to allow for system-wide logging
class Log {
public:
    static void set_verbosity(int level);

    template <typename... Args>
    static void
    log(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    log(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    info(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    info(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    warn(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            spdlog::warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    warn(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            spdlog::warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    error(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            spdlog::error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    error(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            spdlog::error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    trace(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            spdlog::trace(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    trace(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            spdlog::trace(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    debug(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    debug(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    critical(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            spdlog::critical(fmt, std::forward<Args>(args)...);
    }

private:
    /// Verbosity level of the logger
    static int verbosity_;
};

} // namespace krado
