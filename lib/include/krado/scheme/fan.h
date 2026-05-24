// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme2d.h"

namespace krado {

class MeshSurface;
class MeshCurve;

/**
 * Fan meshing scheme for circular wedges.
 *
 * This scheme meshes a surface bounded by one circular curve and two line curves
 * that meet at a center point. It propagates from the outer circular edge
 * towards the center, reducing the number of segments by one in each strip.
 */
class SchemeFan : public Scheme2D {
public:
    struct Options {
        //
    };

public:
    SchemeFan(Options options);

private:
    void on_mesh_surface(Ptr<MeshSurface> surface) override;
};

} // namespace krado
