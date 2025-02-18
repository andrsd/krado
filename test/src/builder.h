#pragma once

#include "krado/point.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"

namespace testing {

krado::GeomVertex build_vertex(krado::Point pt);
krado::GeomCurve build_line(krado::Point pt1, krado::Point pt2);
krado::GeomCurve build_arc();
krado::GeomSurface build_circle(const krado::Point & center, double radius);
krado::GeomSurface build_triangle(const krado::Point & center, double radius);
krado::GeomSurface build_rect(krado::Point pt1, krado::Point pt2);
krado::GeomVolume build_box(const krado::Point & v1, const krado::Point & v2);
krado::GeomVolume build_cylinder(const krado::Point & center, double radius, double height);

} // namespace testing
