#include "krado/mesh_curve.h"

namespace krado::mesh {

MeshCurve::MeshCurve(const geo::GeomCurve & gcurve, const MeshVertex * v1, const MeshVertex * v2) :
    gcurve(gcurve),
    v1(v1),
    v2(v2)
{
}

const geo::GeomCurve &
MeshCurve::geom_curve() const
{
    return this->gcurve;
}

} // namespace krado::mesh
