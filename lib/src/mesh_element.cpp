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
