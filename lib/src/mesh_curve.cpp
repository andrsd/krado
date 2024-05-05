#include "krado/mesh_curve.h"

namespace krado {

MeshCurve::MeshCurve(const GeomCurve & gcurve, const MeshVertex * v1, const MeshVertex * v2) :
    gcurve(gcurve),
    v1(v1),
    v2(v2),
    curve_marker(0)
{
}

const GeomCurve &
MeshCurve::geom_curve() const
{
    return this->gcurve;
}

int
MeshCurve::marker() const
{
    return this->curve_marker;
}

void
MeshCurve::set_marker(int marker)
{
    this->curve_marker = marker;
}

} // namespace krado
