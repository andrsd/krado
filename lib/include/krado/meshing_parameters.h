// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/parameters.h"

namespace krado {

class MeshingParameters {
public:
    Parameters & meshing_parameters();

    const Parameters & meshing_parameters() const;

    MeshingParameters & set_scheme(const std::string & name);

    std::string get_scheme() const;

    template<typename T>
    MeshingParameters & set(const std::string & param_name, const T & value) {
        meshing_parameters().set<T>(param_name) = value;
        return *this;
    }

private:
    Parameters mpars;
};

} // namespace krado
