// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex.h"
#include "krado/point.h"
#include "krado/geom_vertex.h"

namespace krado {

MeshVertex::MeshVertex(ShapeID id, GeomVertex & geom_vertex) :
    MeshVertexAbstract(geom_vertex),
    id_(id),
    gvtx_(geom_vertex)
{
}

ShapeID
MeshVertex::id() const
{
    return this->id_;
}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx_;
}

Point
MeshVertex::point() const
{
    return this->gvtx_.point();
}

void
MeshVertex::relocate(const Point & /*p*/)
{
    // A corner vertex cannot be relocated
}

void
MeshVertex::set_mesh_size(double size)
{
    this->gvtx_.mesh_size_ = size;
}

} // namespace krado

std::ostream &
operator<<(std::ostream & stream, const krado::MeshVertex & vtx)
{
    auto pt = vtx.point();
    stream << "Vertex " << vtx.id() << ": ";
    stream << "location=(x=" << pt.x << ", y=" << pt.y << ", z=" << pt.z << ")";
    return stream;
}
