// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/netgen.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/log.h"
#include "occgeom.hpp"

namespace krado {

static const std::string scheme_name = "netgen";

namespace {

inline void
NOOP_Deleter(void *)
{
}

} // namespace

class SchemeNetgen::NetgenSession {
public:
    NetgenSession() {}

    netgen::Mesh mesh;
};

SchemeNetgen::SchemeNetgen(Options options) :
    Scheme3D(scheme_name),
    Scheme2D(scheme_name),
    Scheme1D(scheme_name),
    opts_(options),
    netgen_(std::make_unique<NetgenSession>())
{
}

SchemeNetgen::~SchemeNetgen() = default;

void
SchemeNetgen::mesh_volume(Ptr<MeshVolume> volume)
{
    Log::info("Meshing volume {}: scheme='{}'", volume->id(), scheme_name);
}

void
SchemeNetgen::mesh_surface(Ptr<MeshSurface> surface)
{
    Log::info("Meshing surface {}: scheme='{}'", surface->id(), scheme_name);
}

void
SchemeNetgen::mesh_curve(Ptr<MeshCurve> curve)
{
    Log::info("Meshing curve {}: scheme='{}'", curve->id(), scheme_name);
}

void
SchemeNetgen::select_surface_scheme(Ptr<MeshSurface> surface)
{
    if (!surface->has_scheme())
        surface->set_scheme<SchemeNetgen>(this->opts_);
    else {
        if (auto scheme = dynamic_cast<SchemeNetgen *>(&surface->scheme()) == nullptr)
            throw Exception("Unable to use {} in combination with scheme {}",
                            scheme_name,
                            surface->scheme().name());
    }
}

void
SchemeNetgen::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        curve->set_scheme<SchemeNetgen>(this->opts_);
    }
    else {
        if (auto scheme = dynamic_cast<SchemeNetgen *>(&curve->scheme()) == nullptr)
            throw Exception("Unable to use {} in combination with scheme {}",
                            scheme_name,
                            curve->scheme().name());
    }
}

} // namespace krado
