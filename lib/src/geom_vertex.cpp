// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_vertex.h"
#include "BRep_Tool.hxx"

namespace krado {

GeomVertex::GeomVertex(const TopoDS_Vertex & vertex) : vertex_(vertex)
{
    if (!this->vertex_.IsNull()) {
        gp_Pnt pnt = BRep_Tool::Pnt(this->vertex_);
        this->x_ = pnt.X();
        this->y_ = pnt.Y();
        this->z_ = pnt.Z();
    }
}

bool
GeomVertex::is_null() const
{
    return this->vertex_.IsNull();
}

double
GeomVertex::x() const
{
    return this->x_;
}

double
GeomVertex::y() const
{
    return this->y_;
}

double
GeomVertex::z() const
{
    return this->z_;
}

GeomVertex::operator const TopoDS_Shape &() const
{
    return this->vertex_;
}

} // namespace krado
