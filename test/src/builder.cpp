#include "builder.h"
#include "gp_Circ.hxx"
#include "BRepLib_MakeVertex.hxx"
#include "BRepLib_MakeEdge.hxx"
#include "BRepLib_MakeWire.hxx"
#include "BRepLib_MakeFace.hxx"
#include "BRepPrimAPI_MakeBox.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Face.hxx"

namespace testing {

TopoDS_Vertex
build_vertex(Point pt)
{
    gp_Pnt gpnt(pt.x, pt.y, pt.z);
    BRepLib_MakeVertex make_vtx(gpnt);
    make_vtx.Build();
    return make_vtx.Vertex();
}

TopoDS_Edge
build_line(Point pt1, Point pt2)
{
    gp_Pnt pnt1(pt1.x, pt1.y, pt1.z);
    gp_Pnt pnt2(pt2.x, pt2.y, pt2.z);
    BRepLib_MakeEdge make_edge(pnt1, pnt2);
    make_edge.Build();
    return make_edge.Edge();
}

TopoDS_Face
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
    return make_face.Face();
}

TopoDS_Face
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
    return make_face.Face();
}

TopoDS_Solid
build_box(const Point & v1, const Point & v2)
{
    gp_Pnt pnt1(v1.x, v1.y, v1.z);
    gp_Pnt pnt2(v2.x, v2.y, v2.z);
    BRepPrimAPI_MakeBox make_box(pnt1, pnt2);
    make_box.Build();
    return make_box.Solid();
}

} // namespace testing
