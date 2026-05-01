#include "gmock/gmock.h"
#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/geom_model.h"
#include "krado/scheme/equal.h"
#include "builder.h"

using namespace krado;

// this does not work on github (IDK why)
TEST(MeshCurveTest, DISABLED_api)
{
    auto edge = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));

    auto v1 = Ptr<MeshVertex>::alloc(1, edge.first_vertex());
    auto v2 = Ptr<MeshVertex>::alloc(2, edge.last_vertex());
    MeshCurve mcurve(1, edge, v1, v2);

    EXPECT_EQ(&mcurve.geom_curve(), &edge);

    // EXPECT_EQ(mcurve.scheme().name(), "auto");

    auto & vtx = mcurve.bounding_vertices();
    ASSERT_EQ(vtx.size(), 2);
    EXPECT_EQ(vtx[0], v1);
    EXPECT_EQ(vtx[1], v2);
}

TEST(MeshCurveTest, mesh)
{
    auto edge = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    auto gvtx1 = edge.first_vertex();
    auto gvtx2 = edge.last_vertex();

    auto v1 = Ptr<MeshVertex>::alloc(1, gvtx1);
    auto v2 = Ptr<MeshVertex>::alloc(2, gvtx2);
    auto mcurve = Ptr<MeshCurve>::alloc(1, edge, v1, v2);

    SchemeEqual::Options opts;
    opts.intervals = 4;
    SchemeEqual equal(opts);
    equal.mesh_curve(mcurve);

    auto & bv = mcurve->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_EQ(&bv[0]->geom_vertex(), &gvtx1);
    EXPECT_EQ(&bv[1]->geom_vertex(), &gvtx2);

    auto & cv = mcurve->curve_vertices();
    ASSERT_EQ(cv.size(), 3);
    EXPECT_DOUBLE_EQ(cv[1]->parameter(), 2.5);
    EXPECT_TRUE(cv[1]->point().is_equal(Point(1.5, 2.0, 0), 1e-10));
    EXPECT_EQ(&cv[1]->geom_curve(), &edge);

    auto & segs = mcurve->segments();
    EXPECT_EQ(segs.size(), 4);
    EXPECT_EQ(segs[0].type(), ElementType::LINE2);
    EXPECT_EQ(segs[0].num_vertices(), 2);

    EXPECT_EQ(segs[3].type(), ElementType::LINE2);
    EXPECT_EQ(segs[3].num_vertices(), 2);
}

TEST(MeshCurveTest, op_shl)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomModel model(line);

    auto crv = model.curve(1);
    std::stringstream ss;
    ss << *crv;
    EXPECT_EQ(ss.str(), "Curve 1: type=line, vertices=[1, 2], u=[0, 5], length=5");
}
