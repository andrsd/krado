// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/meshing_parameters.h"

namespace krado {

MeshingParameters &
MeshingParameters::set_scheme(const std::string & name)
{
    this->mpars.set<std::string>("scheme") = name;
    return *this;
}

std::string
MeshingParameters::get_scheme() const
{
    return this->mpars.get<std::string>("scheme");
}

Parameters &
MeshingParameters::meshing_parameters()
{
    return this->mpars;
}

const Parameters &
MeshingParameters::meshing_parameters() const
{
    return this->mpars;
}

} // namespace krado
