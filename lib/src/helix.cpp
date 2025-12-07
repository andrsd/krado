// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/helix.h"
#include "krado/vector.h"
#include "krado/axis1.h"
#include "krado/axis2.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "TColgp_HArray1OfPnt.hxx"
#include "GeomAPI_PointsToBSpline.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"

namespace krado {

// NOTE: 4 was not enough to get circular shape
const int N_SEGS_PER_TURN = 8;

Helix::Helix(const TopoDS_Edge & edge, Handle(Geom_BSplineCurve) spline) :
    GeomCurve(edge),
    spline_(spline)
{
}

Helix
Helix::create(const Axis2 & ax2, double radius, double height, double turns, double start_angle)
{
    auto ax1 = ax2.axis();
    auto n_pts = turns * N_SEGS_PER_TURN;
    auto dh = height / turns / N_SEGS_PER_TURN;
    auto dangle = 2 * M_PI / N_SEGS_PER_TURN;
    auto axial_dir = ax2.direction();

    // partial translate transformation
    gp_Trsf translate;
    translate.SetTranslation(dh * axial_dir);
    // partial rotate transformation
    gp_Trsf rotate;
    rotate.SetRotation(ax1, dangle);

    auto pt = occ::to_pnt(ax1.location());
    pt.Translate(radius * ax2.x_direction());
    TColgp_HArray1OfPnt pnts(0, n_pts);
    for (int idx = 0; idx < n_pts; idx++) {
        pnts.SetValue(idx, pt);
        pt.Transform(translate);
        pt.Transform(rotate);
    }
    pnts.SetValue(n_pts, pt);

    GeomAPI_PointsToBSpline mk(pnts, 3, 3, GeomAbs_C2);
    if (!mk.IsDone())
        throw Exception("Spline was not created");
    BRepBuilderAPI_MakeEdge make_edge(mk.Curve());
    make_edge.Build();
    if (!make_edge.IsDone())
        throw Exception("Edge was not created");
    return Helix(make_edge.Edge(), mk.Curve());
}

} // namespace krado
