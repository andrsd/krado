#include "krado/mesh/curve.h"

namespace krado::mesh {

Curve::Curve(const geo::Curve & gcurve, const Vertex * v1, const Vertex * v2) :
    gcurve(gcurve),
    v1(v1),
    v2(v2)
{
}

const geo::Curve &
Curve::geom_curve() const
{
    return this->gcurve;
}

} // namespace krado::mesh
