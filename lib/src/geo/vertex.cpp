#include "krado/geo/vertex.h"
#include "BRep_Tool.hxx"

namespace krado::geo {

Vertex::Vertex(const TopoDS_Vertex & vertex) : vertex(vertex)
{
    gp_Pnt pnt = BRep_Tool::Pnt(this->vertex);
    this->x = pnt.X();
    this->y = pnt.Y();
    this->z = pnt.Z();
}

} // namespace krado::geo
