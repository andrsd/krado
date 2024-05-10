// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/parameters.h"

namespace krado {

class Scheme {
public:
    Scheme(const std::string & name);
    virtual ~Scheme() = default;

    /// Get scheme name
    ///
    /// @return Scheme name
    std::string name() const;

    /// Set parameter value
    ///
    /// @tparam T C++ type
    /// @param param_name Parameter name
    /// @param value Parameter value
    /// @return This scheme
    template <typename T>
    Scheme &
    set(const std::string & param_name, const T & value)
    {
        this->params.set<T>(param_name) = value;
        return *this;
    }

    /// Get parameter value
    ///
    /// @tparam T C++ type
    /// @param param_name Parameter name
    /// @return Value of the parameter
    template <typename T>
    T
    get(const std::string & param_name) const
    {
        return this->params.get<T>(param_name);
    }

private:
    /// Scheme name
    std::string nm;
    /// Parameters used by the scheme
    Parameters params;
};

} // namespace krado
