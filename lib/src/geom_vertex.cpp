// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_vertex.h"
#include "BRep_Tool.hxx"

namespace krado {

GeomVertex::GeomVertex(const TopoDS_Vertex & vertex) : vertex(vertex)
{
    gp_Pnt pnt = BRep_Tool::Pnt(this->vertex);
    this->x_coord = pnt.X();
    this->y_coord = pnt.Y();
    this->z_coord = pnt.Z();
}

double
GeomVertex::x() const
{
    return this->x_coord;
}

double
GeomVertex::y() const
{
    return this->y_coord;
}

double
GeomVertex::z() const
{
    return this->z_coord;
}

GeomVertex::operator const TopoDS_Shape &() const
{
    return this->vertex;
}

} // namespace krado
