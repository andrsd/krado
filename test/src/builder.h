#pragma once

#include "krado/point.h"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"

using namespace krado;

namespace testing {

TopoDS_Vertex build_vertex(Point pt);
TopoDS_Edge build_line(Point pt1, Point pt2);
TopoDS_Edge build_arc();
TopoDS_Face build_circle(const Point & center, double radius);
TopoDS_Face build_triangle(const Point & center, double radius);
TopoDS_Face build_rect(Point pt1, Point pt2);
TopoDS_Solid build_box(const Point & v1, const Point & v2);
TopoDS_Solid build_cylinder(const Point & center, double radius, double height);

} // namespace testing
