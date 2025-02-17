// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/meshing_parameters.h"
#include "krado/scheme.h"
#include "krado/scheme_factory.h"

namespace krado {

MeshingParameters::MeshingParameters() :
    scheme_factory_(SchemeFactory::instance()),
    scheme_(this->scheme_factory_.create("auto")),
    meshed_(false)
{
}

Scheme &
MeshingParameters::set_scheme(const std::string & name)
{
    this->scheme_ = this->scheme_factory_.create(name);
    return *this->scheme_;
}

Scheme &
MeshingParameters::scheme() const
{
    if (this->scheme_ == nullptr)
        throw Exception("Scheme is null");
    return *this->scheme_;
}

bool
MeshingParameters::is_meshed() const
{
    return this->meshed_;
}

void
MeshingParameters::set_meshed()
{
    this->meshed_ = true;
}

} // namespace krado
