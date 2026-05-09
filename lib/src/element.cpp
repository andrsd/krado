// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/element.h"
#include "krado/exception.h"
#include "krado/types.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/numerics.h"
#include <array>

namespace krado {

const std::array<u8, 2> Line2::EDGE_VERTICES = { 0, 1 };

const std::vector<u8> Tri3::EDGES = { 0, 1, 2 };
const std::vector<std::array<u8, 2>> Tri3::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 0 } };

const std::vector<u8> Quad4::EDGES = { 0, 1, 2, 3 };
const std::vector<std::array<u8, 2>> Quad4::EDGE_VERTICES = { { 0, 1 },
                                                              { 1, 2 },
                                                              { 2, 3 },
                                                              { 3, 0 } };

const std::vector<u8> Tetra4::EDGES = { 0, 1, 2, 3, 4, 5 };
const std::vector<std::array<u8, 2>> Tetra4::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 0 },
                                                               { 0, 3 }, { 1, 3 }, { 2, 3 } };
const std::vector<std::vector<u8>> Tetra4::FACE_EDGES = { { 0, 1, 2 },
                                                          { 0, 4, 3 },
                                                          { 1, 5, 4 },
                                                          { 2, 3, 5 } };
const std::vector<std::vector<u8>> Tetra4::FACE_VERTICES = { { 0, 1, 2 },
                                                             { 0, 1, 3 },
                                                             { 1, 2, 3 },
                                                             { 2, 0, 3 } };

const std::vector<u8> Pyramid5::EDGES = { 0, 1, 2, 3, 4, 5, 6, 7 };
const std::vector<std::array<u8, 2>> Pyramid5::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 3 },
                                                                 { 3, 0 }, { 0, 4 }, { 1, 4 },
                                                                 { 2, 4 }, { 3, 4 } };
const std::vector<std::vector<u8>> Pyramid5::FACE_EDGES = { { 0, 1, 2, 3 },
                                                            { 0, 3, 4 },
                                                            { 1, 6, 5 },
                                                            { 2, 7, 6 },
                                                            { 3, 4, 7 } };
const std::vector<std::vector<u8>> Pyramid5::FACE_VERTICES = { { 0, 1, 2, 3 },
                                                               { 0, 1, 4 },
                                                               { 1, 2, 4 },
                                                               { 2, 3, 4 },
                                                               { 3, 0, 4 } };

const std::vector<u8> Prism6::EDGES = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
const std::vector<std::array<u8, 2>> Prism6::EDGE_VERTICES = { { 0, 1 }, { 1, 2 }, { 2, 0 },
                                                               { 0, 3 }, { 1, 4 }, { 2, 5 },
                                                               { 3, 4 }, { 4, 5 }, { 5, 3 } };
const std::vector<std::vector<u8>> Prism6::FACE_EDGES = { { 2, 1, 0 },
                                                          { 0, 4, 6, 3 },
                                                          { 1, 5, 7, 4 },
                                                          { 2, 3, 8, 6 },
                                                          { 6, 7, 8 } };
const std::vector<std::vector<u8>> Prism6::FACE_VERTICES = { { 0, 2, 1 },
                                                             { 0, 1, 4, 3 },
                                                             { 1, 2, 5, 4 },
                                                             { 2, 0, 3, 5 },
                                                             { 3, 4, 5 } };

const std::vector<u8> Hex8::EDGES = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const std::vector<std::array<u8, 2>> Hex8::EDGE_VERTICES = {
    { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 0, 4 }, { 1, 5 },
    { 2, 6 }, { 3, 7 }, { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }
};
const std::vector<std::vector<u8>> Hex8::FACE_EDGES = { { 0, 5, 8, 4 },  { 2, 7, 10, 6 },
                                                        { 3, 4, 11, 7 }, { 1, 6, 9, 5 },
                                                        { 3, 2, 1, 0 },  { 8, 9, 10, 11 } };
const std::vector<std::vector<u8>> Hex8::FACE_VERTICES = { { 0, 1, 5, 4 }, { 2, 3, 7, 6 },
                                                           { 3, 0, 4, 7 }, { 1, 2, 6, 5 },
                                                           { 0, 3, 2, 1 }, { 4, 5, 6, 7 } };

//

Element::Element(ElementType type, const std::vector<Index> & vtx_ids) : elem_type_(type)
{
    if (vtx_ids.size() < MAX_INDICES) {
        this->n_ids_ = vtx_ids.size();
        for (int i = 0; i < this->n_ids_; i++)
            this->vtx_id_[i] = vtx_ids[i];
    }
    else
        throw Exception("Unable to handle more then {} indices", MAX_INDICES);
}

ElementType
Element::type() const
{
    return this->elem_type_;
}

u8
Element::num_vertices() const
{
    return this->n_ids_;
}

Index
Element::index(u8 idx) const
{
    if (idx < MAX_INDICES)
        return this->vtx_id_[idx];
    else
        throw Exception("Access out of range. `idx` can be 0..{}", MAX_INDICES);
}

Span<const Index>
Element::indices() const
{
    return { this->vtx_id_.data(), this->n_ids_ };
}

void
Element::shift(Index ofst)
{
    for (int i = 0; i < this->n_ids_; i++)
        this->vtx_id_[i] += ofst;
}

Element
Element::Line2(const std::array<Index, 2> & ids)
{
    return Element(ElementType::LINE2, ids);
}

Element
Element::Tri3(const std::array<Index, 3> & ids)
{
    return Element(ElementType::TRI3, ids);
}

Element
Element::Quad4(const std::array<Index, 4> & ids)
{
    return Element(ElementType::QUAD4, ids);
}

Element
Element::Tetra4(const std::array<Index, 4> & ids)
{
    return Element(ElementType::TETRA4, ids);
}

Element
Element::Pyramid5(const std::array<Index, 5> & ids)
{
    return Element(ElementType::PYRAMID5, ids);
}

Element
Element::Prism6(const std::array<Index, 6> & ids)
{
    return Element(ElementType::PRISM6, ids);
}

Element
Element::Hex8(const std::array<Index, 8> & ids)
{
    return Element(ElementType::HEX8, ids);
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
        if (a.index(i) != b.index(i))
            return false;
    return true;
}

double
Tri3::gamma(Point pa, Point pb, Point pc)
{
    const auto a = (pc - pb).normalized();
    const auto b = (pa - pc).normalized();
    const auto c = (pb - pa).normalized();
    const auto pva = cross_product(b, c);
    const double sina = pva.magnitude();
    const auto pvb = cross_product(c, a);
    const double sinb = pvb.magnitude();
    const auto pvc = cross_product(a, b);
    const double sinc = pvc.magnitude();
    if (sina == 0.0 && sinb == 0.0 && sinc == 0.0)
        return 0.0;
    else
        return 2 * (2 * sina * sinb * sinc / (sina + sinb + sinc));
}

double
Tri3::eta(Point pa, Point pb, Point pc)
{
    const auto a1 = 180. * utils::angle(pa, pb, pc) / M_PI;
    const auto a2 = 180. * utils::angle(pb, pc, pa) / M_PI;
    const auto a3 = 180. * utils::angle(pc, pa, pb) / M_PI;
    const auto amin = std::min(std::min(a1, a2), a3);
    const auto angle = std::abs(60. - amin);
    return 1. - angle / 60.;
}

Optional<UVParam>
Tri3::circum_center(UVParam p1, UVParam p2, UVParam p3)
{
    const double x1 = p1.u;
    const double x2 = p2.u;
    const double x3 = p3.u;
    const double y1 = p1.v;
    const double y2 = p2.v;
    const double y3 = p3.v;

    auto d = 2. * (y1 * (x2 - x3) + y2 * (x3 - x1) + y3 * (x1 - x2));
    if (d == 0.0)
        return std::nullopt;

    auto a1 = x1 * x1 + y1 * y1;
    auto a2 = x2 * x2 + y2 * y2;
    auto a3 = x3 * x3 + y3 * y3;

    return UVParam(((a1 * (y3 - y2) + a2 * (y1 - y3) + a3 * (y2 - y1)) / d),
                   ((a1 * (x2 - x3) + a2 * (x3 - x1) + a3 * (x1 - x2)) / d));
}

Optional<Point>
Tri3::circum_center(Point p1, Point p2, Point p3)
{
    auto v1 = p2 - p1;
    auto v2 = p3 - p1;
    auto vx = p2 - p1;
    auto vy = p3 - p1;
    auto vz = cross_product(vx, vy);
    vy = cross_product(vz, vx);
    vx.normalize();
    vy.normalize();
    vz.normalize();

    UVParam p1P(0., 0.);
    UVParam p2P(dot_product(v1, vx), dot_product(v1, vy));
    UVParam p3P(dot_product(v2, vx), dot_product(v2, vy));
    auto resP = circum_center(p1P, p2P, p3P);
    if (resP.has_value()) {
        auto r = resP.value();
        auto center = p1 + r.u * vx + r.v * vy;
        return center;
    }
    else
        return std::nullopt;
}

double
Tri3::circum_radius_quality(Point pa, Point pb, Point pc)
{
    auto denom = Tri3::gamma(pa, pb, pc);
    if (denom == 0.)
        throw Exception("Quality metric gamma is 0.");
    return 1. / denom;
}

double
Tri3::circum_radius_euclidian(Point pa, Point pb, Point pc, double lc)
{
    const auto center = circum_center(pa, pb, pc);
    if (center.has_value()) {
        const auto delta = pa - center.value();
        return delta.magnitude() / lc;
    }
    else
        return std::numeric_limits<double>::lowest();
}

} // namespace krado
