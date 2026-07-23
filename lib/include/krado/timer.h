// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/log.h"
#include "krado/utils.h"
#include <chrono>

namespace krado {

class LoggingTimer {
public:
    explicit LoggingTimer() : msg_("- took {}"), start_time_(std::chrono::steady_clock::now()) {}

    explicit LoggingTimer(std::string fmt) :
        msg_(fmt),
        start_time_(std::chrono::steady_clock::now())
    {
    }

    // Destructor calculates and prints the elapsed time automatically
    ~LoggingTimer()
    {
        auto end_time = std::chrono::steady_clock::now();

        std::chrono::duration<double> elapsed_seconds = end_time - this->start_time_;
        if (elapsed_seconds.count() > 1.)
            Log::info(fmt::runtime(this->msg_), utils::human_time(elapsed_seconds.count()));
    }

    // Prevent copying to avoid duplicate timing logs
    LoggingTimer(const LoggingTimer &) = delete;
    LoggingTimer & operator=(const LoggingTimer &) = delete;

    // Allow moving to transfer ownership of the timer
    LoggingTimer(LoggingTimer &&) noexcept = default;
    LoggingTimer & operator=(LoggingTimer &&) noexcept = default;

private:
    std::string msg_;
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
};

} // namespace krado
