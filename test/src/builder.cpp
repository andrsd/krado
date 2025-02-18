#include "builder.h"
#include "gp_Circ.hxx"
#include "BRepLib_MakeVertex.hxx"
#include "BRepLib_MakeEdge.hxx"
#include "BRepLib_MakeWire.hxx"
#include "BRepLib_MakeFace.hxx"
#include "BRepPrimAPI_MakeBox.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Face.hxx"
#include "Geom_Line.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include "BRepPrimAPI_MakeCylinder.hxx"
#include "GC_MakeArcOfCircle.hxx"
#include "krado/geom_surface.h"

using namespace krado;

namespace testing {

GeomVertex
build_vertex(Point pt)
{
    gp_Pnt gpnt(pt.x, pt.y, pt.z);
    BRepLib_MakeVertex make_vtx(gpnt);
    make_vtx.Build();
    return GeomVertex(make_vtx.Vertex());
}

GeomCurve
build_line(Point pt1, Point pt2)
{
    gp_Pnt pnt1(pt1.x, pt1.y, pt1.z);
    gp_Pnt pnt2(pt2.x, pt2.y, pt2.z);
    BRepLib_MakeEdge make_edge(pnt1, pnt2);
    make_edge.Build();
    return GeomCurve(make_edge.Edge());
}

GeomCurve
build_arc()
{
    gp_Pnt pt1(-1, 0, 0);
    gp_Pnt pt2(0, 1, 0);
    gp_Pnt pt3(1, 0, 0);
    GC_MakeArcOfCircle mk_arc(pt1, pt2, pt3);
    BRepBuilderAPI_MakeEdge make_edge(mk_arc.Value());
    make_edge.Build();
    return GeomCurve(make_edge.Edge());
}

GeomSurface
build_circle(const Point & center, double radius)
{
    gp_Ax2 ax2;
    gp_Circ circ(ax2, radius);
    BRepLib_MakeEdge make_edge(circ);
    make_edge.Build();
    auto edge = make_edge.Edge();
    BRepLib_MakeWire make_wire(edge);
    make_wire.Build();
    auto wire = make_wire.Wire();
    BRepLib_MakeFace make_face(wire);
    make_face.Build();
    return GeomSurface(make_face.Face());
}

GeomSurface
build_triangle(const Point & center, double radius)
{
    gp_Pnt ctr(center.x, center.y, center.z);
    gp_Pnt pt1(center.x, center.y + radius, center.z);
    gp_Pnt pt2(center.x + radius, center.y, center.z);

    BRepLib_MakeEdge make_edge0(pt1, pt2);
    make_edge0.Build();
    auto edge0 = make_edge0.Edge();

    BRepLib_MakeEdge make_edge1(ctr, pt1);
    make_edge1.Build();
    auto edge1 = make_edge1.Edge();

    BRepLib_MakeEdge make_edge2(ctr, pt2);
    make_edge2.Build();
    auto edge2 = make_edge2.Edge();

    BRepLib_MakeWire make_wire(edge0, edge1, edge2);
    make_wire.Build();
    auto wire = make_wire.Wire();
    BRepLib_MakeFace make_face(wire);
    make_face.Build();
    return GeomSurface(make_face.Face());
}

GeomSurface
build_rect(Point pt1, Point pt2)
{
    gp_Pnt p1(pt1.x, pt1.y, 0);
    gp_Pnt p2(pt2.x, pt1.y, 0);
    gp_Pnt p3(pt2.x, pt2.y, 0);
    gp_Pnt p4(pt1.x, pt2.y, 0);

    auto edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
    auto edge2 = BRepBuilderAPI_MakeEdge(p2, p3);
    auto edge3 = BRepBuilderAPI_MakeEdge(p3, p4);
    auto edge4 = BRepBuilderAPI_MakeEdge(p4, p1);

    auto wire = BRepBuilderAPI_MakeWire(edge1, edge2, edge3, edge4);

    return GeomSurface(BRepBuilderAPI_MakeFace(wire));
}

GeomVolume
build_box(const Point & v1, const Point & v2)
{
    gp_Pnt pnt1(v1.x, v1.y, v1.z);
    gp_Pnt pnt2(v2.x, v2.y, v2.z);
    BRepPrimAPI_MakeBox make_box(pnt1, pnt2);
    make_box.Build();
    return GeomVolume(make_box.Solid());
}

GeomVolume
build_cylinder(const Point & center, double radius, double height)
{
    BRepPrimAPI_MakeCylinder maker(radius, height);
    maker.Build();
    return GeomVolume(maker.Solid());
}

} // namespace testing
