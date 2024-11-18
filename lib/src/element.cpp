// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/element.h"
#include "krado/exception.h"
#include <array>

namespace krado {

Element::Element(Type type, const std::vector<int> & vtx_ids, int marker) :
    elem_type(type),
    vtx_id(vtx_ids),
    mrkr(marker)
{
}

Element::Type
Element::type() const
{
    return this->elem_type;
}

int
Element::marker() const
{
    return this->mrkr;
}

void
Element::set_marker(int marker)
{
    this->mrkr = marker;
}

int
Element::num_vertices() const
{
    return this->vtx_id.size();
}

int
Element::vertex_id(int idx) const
{
    return this->vtx_id[idx];
}

int
Element::operator()(int idx) const
{
    return this->vtx_id[idx];
}

const std::vector<int> &
Element::ids() const
{
    return this->vtx_id;
}

void
Element::set_ids(const std::vector<int> & ids)
{
    if (this->vtx_id.size() == ids.size())
        this->vtx_id = ids;
    else
        throw Exception("Element::set_ids: size mismatch");
}

Element
Element::Line2(const std::array<int, 2> & ids, int marker)
{
    Element line2(LINE2, { ids[0], ids[1] }, marker);
    return line2;
}

Element
Element::Tri3(const std::array<int, 3> & ids, int marker)
{
    return Element(TRI3, { ids[0], ids[1], ids[2] }, marker);
}

Element
Element::Tetra4(const std::array<int, 4> & ids, int marker)
{
    Element tet4(TETRA4, { ids[0], ids[1], ids[2], ids[3] }, marker);
    return tet4;
}

std::string
Element::type(Type type)
{
    std::string str_type;
    if (type == krado::Element::LINE2)
        return "LINE2";
    else if (type == krado::Element::TRI3)
        return "TRI3";
    else if (type == krado::Element::TETRA4)
        return "TETRA4";
    else
        return "unknown";
}

bool
operator==(const Element & a, const Element & b)
{
    if (a.type() != b.type())
        return false;
    if (a.marker() != b.marker())
        return false;
    if (a.num_vertices() != b.num_vertices())
        return false;
    for (int i = 0; i < a.num_vertices(); ++i)
        if (a.vertex_id(i) != b.vertex_id(i))
            return false;
    return true;
}

} // namespace krado
