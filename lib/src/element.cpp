// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/element.h"
#include "krado/exception.h"
#include "krado/types.h"
#include <array>

namespace krado {

const std::vector<int> Line2::EDGE_VERTICES = { 0, 1 };

const std::vector<int> Tri3::EDGES = { 0, 1, 2 };
const std::vector<std::vector<int>> Tri3::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 0 } };

const std::vector<int> Quad4::EDGES = { 0, 1, 2, 3 };
const std::vector<std::vector<int>> Quad4::EDGE_VERTICES = { { 0, 1 },
                                                             { 1, 2 },
                                                             { 2, 3 },
                                                             { 3, 0 } };

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

const std::vector<int> Pyramid5::EDGES = { 0, 1, 2, 3, 4, 5, 6, 7 };
const std::vector<std::vector<int>> Pyramid5::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 3 },
                                                                { 3, 0 }, { 0, 4 }, { 1, 4 },
                                                                { 2, 4 }, { 3, 4 } };
const std::vector<std::vector<int>> Pyramid5::FACE_EDGES = { { 0, 1, 2, 3 },
                                                             { 0, 3, 4 },
                                                             { 1, 6, 5 },
                                                             { 2, 7, 6 },
                                                             { 3, 4, 7 } };
const std::vector<std::vector<int>> Pyramid5::FACE_VERTICES = { { 0, 1, 2, 3 },
                                                                { 0, 1, 4 },
                                                                { 1, 2, 4 },
                                                                { 2, 3, 4 },
                                                                { 3, 0, 4 } };

const std::vector<int> Prism6::EDGES = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
const std::vector<std::vector<int>> Prism6::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 0 },
                                                              { 0, 3 }, { 1, 4 }, { 2, 5 },
                                                              { 3, 4 }, { 4, 5 }, { 5, 3 } };
const std::vector<std::vector<int>> Prism6::FACE_EDGES = { { 2, 1, 0 },
                                                           { 0, 4, 6, 3 },
                                                           { 1, 5, 7, 4 },
                                                           { 2, 3, 8, 6 },
                                                           { 6, 7, 8 } };
const std::vector<std::vector<int>> Prism6::FACE_VERTICES = { { 0, 2, 1 },
                                                              { 0, 1, 4, 3 },
                                                              { 1, 2, 5, 4 },
                                                              { 2, 0, 3, 5 },
                                                              { 3, 4, 5 } };

const std::vector<int> Hex8::EDGES = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const std::vector<std::vector<int>> Hex8::EDGE_VERTICES = {
    { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 0, 4 }, { 1, 5 },
    { 2, 6 }, { 3, 7 }, { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }
};
const std::vector<std::vector<int>> Hex8::FACE_EDGES = { { 0, 5, 8, 4 },  { 2, 7, 10, 6 },
                                                         { 3, 4, 11, 7 }, { 1, 6, 9, 5 },
                                                         { 3, 2, 1, 0 },  { 8, 9, 10, 11 } };
const std::vector<std::vector<int>> Hex8::FACE_VERTICES = { { 0, 1, 5, 4 }, { 2, 3, 7, 6 },
                                                            { 3, 0, 4, 7 }, { 1, 2, 6, 5 },
                                                            { 0, 3, 2, 1 }, { 4, 5, 6, 7 } };

//

Element::Element(ElementType type, const std::vector<gidx_t> & vtx_ids) :
    elem_type_(type),
    vtx_id_(vtx_ids)
{
}

ElementType
Element::type() const
{
    return this->elem_type_;
}

int
Element::num_vertices() const
{
    return this->vtx_id_.size();
}

gidx_t
Element::vertex_id(int idx) const
{
    return this->vtx_id_[idx];
}

gidx_t
Element::operator()(int idx) const
{
    return this->vtx_id_[idx];
}

const std::vector<gidx_t> &
Element::ids() const
{
    return this->vtx_id_;
}

gidx_t
Element::id(int idx) const
{
    return this->vtx_id_[idx];
}

void
Element::set_ids(const std::vector<gidx_t> & ids)
{
    if (this->vtx_id_.size() == ids.size())
        this->vtx_id_ = ids;
    else
        throw Exception("Element::set_ids: size mismatch");
}

Element
Element::Line2(const std::array<gidx_t, 2> & ids)
{
    Element line2(ElementType::LINE2, { ids[0], ids[1] });
    return line2;
}

Element
Element::Tri3(const std::array<gidx_t, 3> & ids)
{
    return Element(ElementType::TRI3, { ids[0], ids[1], ids[2] });
}

Element
Element::Quad4(const std::array<gidx_t, 4> & ids)
{
    return Element(ElementType::QUAD4, { ids[0], ids[1], ids[2], ids[3] });
}

Element
Element::Tetra4(const std::array<gidx_t, 4> & ids)
{
    Element tet4(ElementType::TETRA4, { ids[0], ids[1], ids[2], ids[3] });
    return tet4;
}

Element
Element::Pyramid5(const std::array<gidx_t, 5> & ids)
{
    Element pyr5(ElementType::PYRAMID5, { ids[0], ids[1], ids[2], ids[3], ids[4] });
    return pyr5;
}

Element
Element::Prism6(const std::array<gidx_t, 6> & ids)
{
    Element wed6(ElementType::PRISM6, { ids[0], ids[1], ids[2], ids[3], ids[4], ids[5] });
    return wed6;
}

Element
Element::Hex8(const std::array<gidx_t, 8> & ids)
{
    Element hex8(ElementType::HEX8,
                 { ids[0], ids[1], ids[2], ids[3], ids[4], ids[5], ids[6], ids[7] });
    return hex8;
}

std::string
Element::type(ElementType type)
{
    std::string str_type;
    if (type == ElementType::POINT)
        return "POINT";
    else if (type == ElementType::LINE2)
        return "LINE2";
    else if (type == ElementType::TRI3)
        return "TRI3";
    else if (type == ElementType::QUAD4)
        return "QUAD4";
    else if (type == ElementType::TETRA4)
        return "TETRA4";
    else if (type == ElementType::PYRAMID5)
        return "PYRAMID5";
    else if (type == ElementType::PRISM6)
        return "PRISM6";
    else if (type == ElementType::HEX8)
        return "HEX8";
    else
        return "unknown";
}

bool
operator==(const Element & a, const Element & b)
{
    if (a.type() != b.type())
        return false;
    if (a.num_vertices() != b.num_vertices())
        return false;
    for (int i = 0; i < a.num_vertices(); ++i)
        if (a.vertex_id(i) != b.vertex_id(i))
            return false;
    return true;
}

} // namespace krado
