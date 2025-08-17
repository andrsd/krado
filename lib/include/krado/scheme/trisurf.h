// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme.h"
#include "krado/scheme3d.h"
#include "krado/scheme2d.h"
#include "krado/scheme1d.h"

namespace krado {

class MeshVolume;
class MeshSurface;
class MeshCurve;

class SchemeTriSurf : public Scheme, public Scheme3D, public Scheme2D, public Scheme1D {
public:
    SchemeTriSurf();
    void mesh_volume(Ptr<MeshVolume> volume) override;
    void mesh_surface(Ptr<MeshSurface> surface) override;
    void mesh_curve(Ptr<MeshCurve> mcurve) override;

    void select_surface_scheme(Ptr<MeshSurface> surface) override;
    void select_curve_scheme(Ptr<MeshCurve> curve) override;
};

} // namespace krado
