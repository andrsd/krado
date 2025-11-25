// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/log.h"
#include "krado/utils.h"
#include <chrono>

namespace krado {

class ScopeTimer {
    using clock = std::chrono::high_resolution_clock;

public:
    template <typename... ARGS>
    ScopeTimer(std::format_string<ARGS...> fmt, ARGS &&... args) :
        level_(1),
        text_(std::format(fmt, std::forward<ARGS>(args)...)),
        start_(clock::now())
    {
        // Log::info(this->level_, "{}", this->text_);
    }

    template <typename... ARGS>
    ScopeTimer(int level, std::format_string<ARGS...> fmt, ARGS &&... args) :
        level_(level),
        text_(std::format(fmt, std::forward<ARGS>(args)...)),
        start_(clock::now())
    {
        // Log::info(this->level_, "{}", this->text_);
    }

    ~ScopeTimer()
    {
        auto dt = std::chrono::duration<double, std::micro>(clock::now() - this->start_);
        Log::info(this->level_, "{}... took {}", this->text_, utils::human_time(dt));
    }

private:
    int level_;
    std::string text_;
    clock::time_point start_;
};

} // namespace krado
