// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_element.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/point.h"
#include "krado/utils.h"
#include "krado/types.h"
#include "krado/exception.h"
#include "krado/point.h"
#include "krado/element.h"
#include "krado/range.h"
#include <ranges>
#include <cassert>

namespace krado {

MeshElement::MeshElement(ElementType type, const std::vector<Ptr<MeshVertexAbstract>> & vtx) :
    type_(type),
    vtx_(vtx)
{
}

MeshElement::~MeshElement() = default;

ElementType
MeshElement::type() const
{
    return this->type_;
}

int
MeshElement::num_vertices() const
{
    return this->vtx_.size();
}

Ptr<MeshVertexAbstract>
MeshElement::vertex(int idx) const
{
    assert(!this->vtx_[idx].is_null());
    return this->vtx_[idx];
}

Span<const Ptr<MeshVertexAbstract>>
MeshElement::vertices() const
{
    return this->vtx_;
}

MeshElement
MeshElement::get_edge(int i) const
{
    if (this->type_ == ElementType::TRI3) {
        if (i == 0)
            return MeshElement::Line2({ this->vtx_[0], this->vtx_[1] });
        else if (i == 1)
            return MeshElement::Line2({ this->vtx_[1], this->vtx_[2] });
        else if (i == 2)
            return MeshElement::Line2({ this->vtx_[2], this->vtx_[0] });
        else
            throw Exception("Invalid edge index");
    }
    else
        throw Exception("Unsupported element to get edge of");
}

void
MeshElement::swap_vertices(int idx1, int idx2)
{
    std::swap(this->vtx_[idx1], this->vtx_[idx2]);
}

MeshElement
MeshElement::Line2(const std::array<Ptr<MeshVertexAbstract>, 2> & vtx)
{
    return MeshElement(ElementType::LINE2, { vtx[0], vtx[1] });
}

MeshElement
MeshElement::Tri3(const std::array<Ptr<MeshVertexAbstract>, 3> & vtx)
{
    return MeshElement(ElementType::TRI3, { vtx[0], vtx[1], vtx[2] });
}

MeshElement
MeshElement::Quad4(const std::array<Ptr<MeshVertexAbstract>, 4> & vtx)
{
    return MeshElement(ElementType::QUAD4, { vtx[0], vtx[1], vtx[2], vtx[3] });
}

std::vector<int>
sorted_vertex_nums(const MeshElement & elem)
{
    std::vector<int> nums;
    // clang-format off
    std::ranges::transform(
        elem.vertices(),
        std::back_inserter(nums),
        [](Ptr<MeshVertexAbstract> v) {
            return v->num();
        }
    );
    // clang-format on
    std::sort(nums.begin(), nums.end());
    return nums;
}

double
circum_radius_quality(const MeshElement & tri)
{
    if (tri.type() == ElementType::TRI3) {
        const auto pa = tri.vertex(0)->point();
        const auto pb = tri.vertex(1)->point();
        const auto pc = tri.vertex(2)->point();
        return Tri3::circum_radius_quality(pa, pb, pc);
    }
    else
        throw Exception("Not implemented");
}

double
circum_radius_euclidian(const MeshElement & tri, double lc)
{
    if (tri.type() == ElementType::TRI3) {
        const auto pa = tri.vertex(0)->point();
        const auto pb = tri.vertex(1)->point();
        const auto pc = tri.vertex(2)->point();
        return Tri3::circum_radius_euclidian(pa, pb, pc, lc);
    }
    else
        throw Exception("Not implemented");
}

Point
barycenter(const MeshElement & elem)
{
    Point p(0., 0., 0.);
    auto n = elem.num_vertices();
    for (auto i : make_range(n)) {
        const auto v = elem.vertex(i);
        p += v->point();
    }
    p *= 1. / n;
    return p;
}

} // namespace krado

std::ostream &
operator<<(std::ostream & stream, const krado::MeshElement & el)
{
    stream << "(" << krado::utils::to_str(el.type()) << ":";
    for (auto & vtx : el.vertices())
        stream << " " << vtx->num();
    stream << ")";
    return stream;
}
