// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/parameters.h"

namespace krado {

class Scheme;
class SchemeFactory;

class MeshingParameters {
public:
    MeshingParameters();

    /// Set meshing scheme
    ///
    /// @param name Name od the scheme to assign
    /// @return Pointer to the scheme
    Scheme & set_scheme(const std::string & name);

    /// Get meshing scheme
    Scheme & scheme() const;

    /// Set parameter value
    ///
    /// @tparam T C++ type
    /// @param param_name Parameter name
    /// @param value Parameter value
    /// @return This scheme
    template <typename T>
    MeshingParameters &
    set(const std::string & param_name, const T & value)
    {
        this->mparams.set<T>(param_name) = value;
        return *this;
    }

    /// Has a parameter of given name
    ///
    /// @tparam T C++ type
    /// @param param_name Parameter name
    /// @return `true` if haveing the parameter, `false` otherwise
    template <typename T>
    bool
    has(const std::string & param_name) const
    {
        return this->mparams.has<T>(param_name);
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
        return this->mparams.get<T>(param_name);
    }

    /// Check if the curve is already meshed
    ///
    /// @return `true` if mesh is already present, `false` otherwise
    bool is_meshed() const;

    /// Mark curve as meshed
    void set_meshed();

private:
    SchemeFactory & scheme_factory;
    Scheme * schm;
    Parameters mparams;
    /// Flag indicating if the entity is meshed
    bool meshed;
};

} // namespace krado
