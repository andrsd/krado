// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/meshing_parameters.h"

namespace krado {

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
