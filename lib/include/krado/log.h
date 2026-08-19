// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

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
            logger_->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    log(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            logger_->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    info(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            logger_->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    info(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            logger_->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    warn(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            logger_->warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    warn(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            logger_->warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    error(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            logger_->error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    error(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            logger_->error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    trace(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            logger_->trace(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    trace(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            logger_->trace(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    debug(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            logger_->debug(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    debug(int level, fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= level)
            logger_->debug(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void
    critical(fmt::format_string<Args...> fmt, Args &&... args)
    {
        if (verbosity_ >= 1)
            logger_->critical(fmt, std::forward<Args>(args)...);
    }

    static void set_logger(std::shared_ptr<spdlog::logger> gui_logger);

public:
    static bool initialize();

private:
    static std::shared_ptr<spdlog::logger> logger_;

    /// Verbosity level of the logger
    static int verbosity_;

    static constexpr std::string LOGGER_NAME = "libkrado";
};

} // namespace krado
