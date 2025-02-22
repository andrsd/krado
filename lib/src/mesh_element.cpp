#include "krado/mesh_element.h"
#include "krado/mesh_vertex_abstract.h"

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
    return this->vtx_[idx];
}

} // namespace krado
