// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/parameters.h"

namespace krado {

class MeshingParameters {
public:
    Parameters & meshing_parameters();

    const Parameters & meshing_parameters() const;

private:
    Parameters mpars;
};

} // namespace krado
