#include "krado/mesh_curve.h"
#include "krado/exception.h"

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

const MeshVertex &
MeshCurve::vertex(int id) const
{
    if (id == 0)
        return *this->v1;
    else if (id == 1)
        return *this->v2;
    else
        throw Exception("Vertex index can be only 0 or 1.");
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
