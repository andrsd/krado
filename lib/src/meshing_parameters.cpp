// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/meshing_parameters.h"
#include "krado/scheme.h"
#include "krado/scheme_factory.h"

namespace krado {

MeshingParameters::MeshingParameters() :
    scheme_factory(SchemeFactory::instance()),
    schm(this->scheme_factory.create("auto"))
{
}

Scheme &
MeshingParameters::set_scheme(const std::string & name)
{
    this->schm = this->scheme_factory.create(name);
    return *this->schm;
}

Scheme &
MeshingParameters::scheme() const
{
    if (this->schm == nullptr)
        throw Exception("Scheme is null");
    return *this->schm;
}

} // namespace krado
