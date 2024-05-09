// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_element.h"
#include <array>

namespace krado {

MeshElement::MeshElement(Type type, const std::vector<int> vtx_ids) :
    elem_type(type),
    vtx_id(vtx_ids)
{
}

MeshElement::Type
MeshElement::type() const
{
    return this->elem_type;
}

int
MeshElement::num_vertices() const
{
    return this->vtx_id.size();
}

int
MeshElement::vertex_id(int idx) const
{
    return this->vtx_id[idx];
}

int
MeshElement::operator()(int idx) const
{
    return this->vtx_id[idx];
}

MeshElement
MeshElement::Line2(const std::array<int, 2> & ids)
{
    MeshElement line2(LINE2, { ids[0], ids[1] });
    return line2;
}

MeshElement
MeshElement::Tri3(const std::array<int, 3> & ids)
{
    return MeshElement(TRI3, { ids[0], ids[1], ids[2] });
}

MeshElement
MeshElement::Tetra4(const std::array<int, 4> & ids)
{
    MeshElement tet4(TETRA4, { ids[0], ids[1], ids[2], ids[3] });
    return tet4;
}

} // namespace krado
