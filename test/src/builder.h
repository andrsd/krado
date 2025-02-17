#pragma once

#include "krado/point.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"

using namespace krado;

namespace testing {

GeomVertex build_vertex(Point pt);
GeomCurve build_line(Point pt1, Point pt2);
GeomCurve build_arc();
GeomSurface build_circle(const Point & center, double radius);
GeomSurface build_triangle(const Point & center, double radius);
GeomSurface build_rect(Point pt1, Point pt2);
GeomVolume build_box(const Point & v1, const Point & v2);
GeomVolume build_cylinder(const Point & center, double radius, double height);

} // namespace testing
