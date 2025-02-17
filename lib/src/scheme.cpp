// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme.h"
#include "krado/mesh.h"

namespace krado {

Scheme::Scheme(const std::string & name) : name_(name) {}

std::string
Scheme::name() const
{
    return this->name_;
}

const Parameters &
Scheme::parameters() const
{
    return this->params_;
}

} // namespace krado
