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

const std::vector<int> &
MeshElement::ids() const
{
    return this->vtx_id;
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

std::string
MeshElement::type(Type type)
{
    std::string str_type;
    if (type == krado::MeshElement::LINE2)
        return "LINE2";
    else if (type == krado::MeshElement::TRI3)
        return "TRI3";
    else if (type == krado::MeshElement::TETRA4)
        return "TETRA4";
    else
        return "unknown";

}

} // namespace krado
