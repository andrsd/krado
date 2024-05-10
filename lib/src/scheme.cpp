// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme.h"
#include "krado/mesh.h"

namespace krado {

Scheme::Scheme(const std::string & name) : nm(name) {}

std::string
Scheme::name() const
{
    return this->nm;
}

const Parameters &
Scheme::parameters() const
{
    return this->params;
}

} // namespace krado
