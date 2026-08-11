// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_vertex.h"
#include "krado/geom_model.h"
#include "krado/consts.h"
#include "BRep_Tool.hxx"
#include "TopoDS.hxx"

namespace krado {

GeomVertex::GeomVertex(const TopoDS_Vertex & vertex) : GeomShape(vertex), mesh_size_(MAX_LC)
{
    const auto & vertex1 = TopoDS::Vertex(this->shape_);
    if (!vertex1.IsNull()) {
        this->pt_ = Point::create(BRep_Tool::Pnt(vertex1));
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
    const auto & vertex = TopoDS::Vertex(this->shape_);
    return vertex.IsNull();
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

GeomVertex::operator const TopoDS_Vertex &() const
{
    return TopoDS::Vertex(this->shape_);
}

bool
GeomVertex::operator<(const GeomVertex & other) const
{
    return this->id() < other.id();
}

} // namespace krado

std::ostream &
operator<<(std::ostream & stream, const krado::GeomVertex & vtx)
{
    auto pt = vtx.point();
    stream << "Vertex: location=(x=" << pt.x << ", y=" << pt.y << ", z=" << pt.z << ")";
    return stream;
}
