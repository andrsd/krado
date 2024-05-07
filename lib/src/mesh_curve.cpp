#include "krado/mesh_curve.h"
#include "krado/exception.h"

namespace krado {

MeshCurve::MeshCurve(const GeomCurve & gcurve, MeshVertex * v1, MeshVertex * v2) :
    gcurve(gcurve),
    vtxs({ v1, v2 }),
    meshed(false)
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

const std::vector<MeshVertex *> &
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

std::vector<MeshCurveVertex> &
MeshCurve::curve_vertices()
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

bool
MeshCurve::is_meshed() const
{
    return this->meshed;
}

void
MeshCurve::set_meshed()
{
    this->meshed = true;
}


} // namespace krado
