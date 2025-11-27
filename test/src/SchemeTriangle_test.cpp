#include "gmock/gmock.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/triangle.h"
#include "krado/io.h"
#include "ExceptionTestMacros.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

#ifdef KRADO_WITH_TRIANGLE

TEST(SchemeTriangleTest, mesh_quarter_circle)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    SchemeEqual::Options opts1;
    opts1.intervals = 4;
    model.curve(1)->set_scheme<SchemeEqual>(opts1);
    model.curve(1)->set_marker(101);
    model.mesh_curve(1);

    model.curve(2)->set_marker(102);

    SchemeTriangle::Options opts;
    opts.max_area = 0.5;
    opts.region_point = { 0.1, 0.1 };
    model.surface(1)->set_scheme<SchemeTriangle>(opts);
    model.surface(1)->set_marker(10);
    model.mesh_surface(1);

    auto qcirc = model.surface(1);
    EXPECT_EQ(qcirc->all_vertices().size(), 6);
    EXPECT_EQ(qcirc->triangles().size(), 4);

    auto mesh = model.build_mesh();

    auto & pts = mesh.points();
    ASSERT_EQ(pts.size(), 6);

    auto elems = mesh.elements();
    ASSERT_EQ(elems.size(), 4);
}

#else

TEST(SchemeTriangleTest, mesh)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    auto qcirc = model.surface(1);
    SchemeTriangle::Options opts;
    qcirc->set_scheme<SchemeTriangle>(opts);
    EXPECT_THROW_MSG({ model.mesh_surface(1); }, "krado was not built with triangle support.");
}

#endif
