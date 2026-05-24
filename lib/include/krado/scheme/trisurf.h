// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme3d.h"
#include "krado/scheme2d.h"
#include "krado/scheme1d.h"

namespace krado {

class MeshVolume;
class MeshSurface;
class MeshCurve;

class SchemeTriSurf : public Scheme3D, public Scheme2D, public Scheme1D {
public:
    struct Options {
        double linear_deflection;
        double angular_deflection;
        bool is_relative;
    };

public:
    SchemeTriSurf(Options options);

private:
    void on_mesh_volume(Ptr<MeshVolume> volume) override;
    void on_mesh_surface(Ptr<MeshSurface> surface) override;
    void on_mesh_curve(Ptr<MeshCurve> mcurve) override;

    void on_select_surface_scheme(Ptr<MeshSurface> surface) override;
    void on_select_curve_scheme(Ptr<MeshCurve> curve) override;

    Options opts_;
};

} // namespace krado
