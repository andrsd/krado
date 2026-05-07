// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/geom_surface.h"
#include "TopoDS_Shell.hxx"
#include <vector>

namespace krado {

class GeomModel;

class GeomShell : public GeomShape {
public:
    explicit GeomShell(const TopoDS_Shell & shell);

    int dim() const final;

    /// Get surfaces bounding this volume
    ///
    /// @return Curves bounding the surface
    [[nodiscard]] std::vector<GeomSurface> surfaces() const;

    operator const TopoDS_Shell &() const;

private:
    TopoDS_Shell shell_;
};

} // namespace krado
