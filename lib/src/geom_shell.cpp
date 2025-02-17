// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_shell.h"
#include "TopoDS.hxx"
#include "TopExp_Explorer.hxx"

namespace krado {

GeomShell::GeomShell(const TopoDS_Shell & shell) : shell_(shell) {}

std::vector<GeomSurface>
GeomShell::surfaces() const
{
    std::vector<GeomSurface> surfs;
    TopExp_Explorer exp;
    for (exp.Init(this->shell_, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        auto gface = GeomSurface(face);
        surfs.emplace_back(gface);
    }
    return surfs;
}

GeomShell::operator const TopoDS_Shell &() const
{
    return this->shell_;
}

} // namespace krado
