#include "gmock/gmock.h"
#include "krado/prism.h"
#include "krado/point.h"
#include "krado/polygon.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"

using namespace krado;

TEST(PrismTest, test)
{
    std::vector<Point> pts = { Point(0, 0, 0), Point(2, 0, 0), Point(2, 1, 0) };
    auto plgn = Polygon::create(pts);
    auto base = GeomSurface::create(plgn);

    auto prm = Prism::create(base, Vector(0, 0, 5));
    auto vol = GeomVolume::create(prm);
    EXPECT_NEAR(vol.volume(), 5., 1e-10);
}
