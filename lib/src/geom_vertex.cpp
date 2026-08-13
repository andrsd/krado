// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_vertex.h"
#include "krado/geom_model.h"
#include "krado/consts.h"
#include "BRep_Tool.hxx"

namespace krado {

GeomVertex::GeomVertex(const TopoDS_Vertex & vertex) :
    GeomShape(vertex),
    vertex_(vertex),
    mesh_size_(MAX_LC)
{
    if (!this->vertex_.IsNull()) {
        this->pt_ = Point::create(BRep_Tool::Pnt(this->vertex_));
    }
}

int
GeomVertex::dim() const
{
    return 0;
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

double
GeomVertex::mesh_size() const
{
    return this->mesh_size_;
}

bool
GeomVertex::operator<(const GeomVertex & other) const
{
    return this->id() < other.id();
}

GeomVertex::operator const TopoDS_Shape &() const
{
    return this->vertex_;
}

} // namespace krado

std::ostream &
operator<<(std::ostream & stream, const krado::GeomVertex & vtx)
{
    auto pt = vtx.point();
    stream << "Vertex: location=(x=" << pt.x << ", y=" << pt.y << ", z=" << pt.z << ")";
    return stream;
}
