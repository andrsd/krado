#include "krado/mesh_element.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/types.h"
#include "krado/exception.h"
#include <cassert>

namespace krado {

MeshElement::MeshElement(ElementType type, const std::vector<MeshVertexAbstract *> & vtx) :
    type_(type),
    vtx_(vtx)
{
}

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

MeshVertexAbstract *
MeshElement::vertex(int idx) const
{
    assert(this->vtx_[idx] != nullptr);
    return this->vtx_[idx];
}

Point
MeshElement::barycenter() const
{
    Point p(0., 0., 0.);
    for (auto & v : this->vtx_)
        p += v->point();
    p /= this->vtx_.size();
    return p;
}

double
MeshElement::volume() const
{
    // TODO: build a free function for volume computation templated on ElementType and most likely
    // delete this method
    if (this->type_ == ElementType::LINE2) {
        return utils::distance(this->vtx_[0]->point(), this->vtx_[1]->point());
    }
    else if (this->type_ == ElementType::TRI3) {
        auto p0 = this->vtx_[0]->point();
        auto p1 = this->vtx_[1]->point();
        auto p2 = this->vtx_[2]->point();
        auto v1 = p1 - p0;
        auto v2 = p2 - p0;
        return cross_product(v1, v2).magnitude() / 2.;
    }
    else if (this->type_ == ElementType::TETRA4) {
        throw Exception("Computation of volume for TETRA4 is not implemented, yet");
    }
    else
        return 0.;
}

const std::vector<MeshVertexAbstract *> &
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
MeshElement::Line2(const std::array<MeshVertexAbstract *, 2> & vtx)
{
    return MeshElement(ElementType::LINE2, { vtx[0], vtx[1] });
}

MeshElement
MeshElement::Tri3(const std::array<MeshVertexAbstract *, 3> & vtx)
{
    return MeshElement(ElementType::TRI3, { vtx[0], vtx[1], vtx[2] });
}

MeshElement
MeshElement::Quad4(const std::array<MeshVertexAbstract *, 4> & vtx)
{
    return MeshElement(ElementType::QUAD4, { vtx[0], vtx[1], vtx[2], vtx[3] });
}

} // namespace krado
