// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/element.h"
#include "krado/exception.h"
#include <array>

namespace krado {

const int Line2::N_VERTICES = 2;
const std::vector<int> Line2::EDGE_VERTICES = { 0, 1 };

const int Tri3::N_VERTICES = 3;
const int Tri3::N_EDGES = 3;
const std::vector<int> Tri3::EDGES = { 0, 1, 2 };
const std::vector<std::vector<int>> Tri3::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 0 } };

const int Quad4::N_VERTICES = 4;
const int Quad4::N_EDGES = 4;
const std::vector<int> Quad4::EDGES = { 0, 1, 2, 3 };
const std::vector<std::vector<int>> Quad4::EDGE_VERTICES = { { 0, 1 },
                                                             { 1, 2 },
                                                             { 2, 3 },
                                                             { 3, 0 } };

const int Tetra4::N_VERTICES = 4;
const int Tetra4::N_EDGES = 6;
const int Tetra4::N_FACES = 4;
const std::vector<int> Tetra4::EDGES = { 0, 1, 2, 3, 4, 5 };
const std::vector<std::vector<int>> Tetra4::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 0 },
                                                              { 0, 3 }, { 1, 3 }, { 2, 3 } };
const std::vector<std::vector<int>> Tetra4::FACE_EDGES = { { 0, 1, 2 },
                                                           { 0, 4, 3 },
                                                           { 1, 5, 4 },
                                                           { 2, 3, 5 } };
const std::vector<std::vector<int>> Tetra4::FACE_VERTICES = { { 0, 1, 2 },
                                                              { 0, 1, 3 },
                                                              { 1, 2, 3 },
                                                              { 2, 0, 3 } };

//

Element::Element(Type type, const std::vector<std::size_t> & vtx_ids, marker_t marker) :
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

marker_t
Element::marker() const
{
    return this->mrkr;
}

void
Element::set_marker(marker_t marker)
{
    this->mrkr = marker;
}

int
Element::num_vertices() const
{
    return this->vtx_id.size();
}

std::size_t
Element::vertex_id(int idx) const
{
    return this->vtx_id[idx];
}

std::size_t
Element::operator()(int idx) const
{
    return this->vtx_id[idx];
}

const std::vector<std::size_t> &
Element::ids() const
{
    return this->vtx_id;
}

void
Element::set_ids(const std::vector<std::size_t> & ids)
{
    if (this->vtx_id.size() == ids.size())
        this->vtx_id = ids;
    else
        throw Exception("Element::set_ids: size mismatch");
}

Element
Element::Point(std::size_t id, marker_t marker)
{
    Element pt(POINT, { id }, marker);
    return pt;
}

Element
Element::Line2(const std::array<std::size_t, 2> & ids, marker_t marker)
{
    Element line2(LINE2, { ids[0], ids[1] }, marker);
    return line2;
}

Element
Element::Tri3(const std::array<std::size_t, 3> & ids, marker_t marker)
{
    return Element(TRI3, { ids[0], ids[1], ids[2] }, marker);
}

Element
Element::Quad4(const std::array<std::size_t, 4> & ids, marker_t marker)
{
    return Element(QUAD4, { ids[0], ids[1], ids[2], ids[3] }, marker);
}

Element
Element::Tetra4(const std::array<std::size_t, 4> & ids, marker_t marker)
{
    Element tet4(TETRA4, { ids[0], ids[1], ids[2], ids[3] }, marker);
    return tet4;
}

Element
Element::Pyramid5(const std::array<std::size_t, 5> & ids, marker_t marker)
{
    Element pyr5(PYRAMID5, { ids[0], ids[1], ids[2], ids[3], ids[4] }, marker);
    return pyr5;
}

Element
Element::Wedge6(const std::array<std::size_t, 6> & ids, marker_t marker)
{
    Element wed6(PRISM6, { ids[0], ids[1], ids[2], ids[3], ids[4], ids[5] }, marker);
    return wed6;
}

Element
Element::Hex8(const std::array<std::size_t, 8> & ids, marker_t marker)
{
    Element hex8(HEX8, { ids[0], ids[1], ids[2], ids[3], ids[4], ids[5], ids[6], ids[7] }, marker);
    return hex8;
}

std::string
Element::type(Type type)
{
    std::string str_type;
    if (type == krado::Element::POINT)
        return "POINT";
    else if (type == krado::Element::LINE2)
        return "LINE2";
    else if (type == krado::Element::TRI3)
        return "TRI3";
    else if (type == krado::Element::QUAD4)
        return "QUAD4";
    else if (type == krado::Element::TETRA4)
        return "TETRA4";
    else if (type == krado::Element::PYRAMID5)
        return "PYRAMID5";
    else if (type == krado::Element::PRISM6)
        return "PRISM6";
    else if (type == krado::Element::HEX8)
        return "HEX8";
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
