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
    [[nodiscard]] std::string name() const;

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
        this->params_.set<T>(param_name) = value;
        return *this;
    }

    /// Has a parameter of given name
    ///
    /// @tparam T C++ type
    /// @param param_name Parameter name
    /// @return `true` if having the parameter, `false` otherwise
    template <typename T>
    [[nodiscard]] bool
    has(const std::string & param_name) const
    {
        return this->params_.has<T>(param_name);
    }

    /// Get parameter value
    ///
    /// @tparam T C++ type
    /// @param param_name Parameter name
    /// @return Value of the parameter
    template <typename T>
    [[nodiscard]] T
    get(const std::string & param_name) const
    {
        return this->params_.get<T>(param_name);
    }

    /// Get parameters
    ///
    /// @return Scheme parameters
    [[nodiscard]] const Parameters & parameters() const;

private:
    /// Scheme name
    std::string name_;
    /// Parameters used by the scheme
    Parameters params_;
};

} // namespace krado
