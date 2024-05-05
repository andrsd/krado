#include "krado/mesh_element.h"

namespace krado {

MeshElement::MeshElement(Type type, const std::vector<int> vtx_ids) :
    elem_type(type),
    vertex_id(vtx_ids)
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
    return this->vertex_id.size();
}

MeshElement
MeshElement::Line2(int v1, int v2)
{
    MeshElement line2(LINE2, { v1, v2 });
    return line2;
}

MeshElement
MeshElement::Tri3(int v1, int v2, int v3)
{
    MeshElement tri3(TRI3, { v1, v2, v3 });
    return tri3;
}

MeshElement
MeshElement::Tetra4(int v1, int v2, int v3, int v4)
{
    MeshElement tet4(TETRA4, { v1, v2, v3, v4 });
    return tet4;
}

} // namespace krado
