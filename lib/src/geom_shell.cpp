// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_shell.h"
#include "krado/geom_model.h"
#include "TopoDS.hxx"
#include "TopExp_Explorer.hxx"

namespace krado {

GeomShell::GeomShell(const TopoDS_Shell & shell) : GeomShape(shell) {}

int
GeomShell::dim() const
{
    return 3;
}

std::vector<GeomSurface>
GeomShell::surfaces() const
{
    const auto & shell = TopoDS::Shell(this->shape_);
    std::vector<GeomSurface> surfs;
    TopExp_Explorer exp;
    for (exp.Init(shell, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        auto gface = GeomSurface(face);
        surfs.emplace_back(gface);
    }
    return surfs;
}

GeomShell::operator const TopoDS_Shell &() const
{
    return TopoDS::Shell(this->shape_);
}

} // namespace krado
