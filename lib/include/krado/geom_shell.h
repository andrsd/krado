// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_surface.h"
#include "TopoDS_Shell.hxx"

namespace krado {

class GeomModel;

class GeomShell {
public:
    explicit GeomShell(const TopoDS_Shell & shell);

    /// Get surfaces bounding this volume
    ///
    /// @return Curves bounding the surface
    std::vector<GeomSurface> surfaces() const;

    operator const TopoDS_Shell &() const;

private:
    TopoDS_Shell shell;
};

} // namespace krado
