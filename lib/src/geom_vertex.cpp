// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_vertex.h"
#include "BRep_Tool.hxx"

namespace krado {

GeomVertex::GeomVertex(const TopoDS_Vertex & vertex) : GeomShape(0, vertex), vertex_(vertex)
{
    if (!this->vertex_.IsNull()) {
        gp_Pnt pnt = BRep_Tool::Pnt(this->vertex_);
        this->pt_ = Point(pnt.X(), pnt.Y(), pnt.Z());
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
    return this->pt_.x;
}

double
GeomVertex::y() const
{
    return this->pt_.y;
}

double
GeomVertex::z() const
{
    return this->pt_.z;
}

Point
GeomVertex::point() const
{
    return this->pt_;
}

GeomVertex::operator const TopoDS_Shape &() const
{
    return this->vertex_;
}

} // namespace krado
