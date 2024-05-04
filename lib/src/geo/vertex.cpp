#include "krado/geo/vertex.h"
#include "BRep_Tool.hxx"

namespace krado::geo {

Vertex::Vertex(const TopoDS_Vertex & vertex) : vertex(vertex)
{
    gp_Pnt pnt = BRep_Tool::Pnt(this->vertex);
    this->x_coord = pnt.X();
    this->y_coord = pnt.Y();
    this->z_coord = pnt.Z();
}

double
Vertex::x() const
{
    return this->x_coord;
}

double
Vertex::y() const
{
    return this->y_coord;
}

double
Vertex::z() const
{
    return this->z_coord;
}

} // namespace krado::geo
