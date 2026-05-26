// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace krado {

class Scheme {
public:
    Scheme(const std::string & name) : name_(name) {}
    virtual ~Scheme() = default;

    const std::string
    name() const
    {
        return this->name_;
    }

    virtual std::string
    params_to_str()
    {
        return "";
    }

private:
    std::string name_;
};

} // namespace krado
