#include "krado/mesh_curve.h"
#include "krado/exception.h"

namespace krado {

MeshCurve::MeshCurve(const GeomCurve & gcurve, const MeshVertex * v1, const MeshVertex * v2) :
    gcurve(gcurve),
    vtxs({ v1, v2 })
{
    auto & mpars = meshing_parameters();
    mpars.set<std::string>("scheme") = "auto";
    mpars.set<int>("marker") = 0;
}

const GeomCurve &
MeshCurve::geom_curve() const
{
    return this->gcurve;
}

const std::vector<const MeshVertex *> &
MeshCurve::vertices() const
{
    return this->vtxs;
}

void
MeshCurve::add_curve_vertex(const MeshCurveVertex & curve_vertex)
{
    this->curve_vtx.push_back(curve_vertex);
}

const std::vector<MeshCurveVertex> &
MeshCurve::curve_vertices() const
{
    return this->curve_vtx;
}

void
MeshCurve::add_curve_segment(int idx1, int idx2)
{
    auto line2 = MeshElement::Line2(idx1, idx2);
    this->curve_segs.emplace_back(line2);
}

const std::vector<MeshElement> &
MeshCurve::curve_segments() const
{
    return this->curve_segs;
}

} // namespace krado
