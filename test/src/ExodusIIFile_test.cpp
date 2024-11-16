#include "gmock/gmock.h"
#include "krado/exodusii_file.h"
#include "krado/mesh_element.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(ExodusIIFileTest, read_2d)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read();

    auto & pnts = mesh.points();
    EXPECT_EQ(pnts.size(), 4);
    EXPECT_EQ(pnts[0], Point(0, 0));
    EXPECT_EQ(pnts[1], Point(2, 0));
    EXPECT_EQ(pnts[2], Point(0, 2));
    EXPECT_EQ(pnts[3], Point(2, 2));

    auto & elems = mesh.elements();
    EXPECT_EQ(elems.size(), 2);
    EXPECT_THAT(elems[0].type(), Eq(MeshElement::TRI3));
    EXPECT_THAT(elems[0].ids(), ElementsAre(0, 1, 2));
    EXPECT_THAT(elems[1].type(), Eq(MeshElement::TRI3));
    EXPECT_THAT(elems[1].ids(), ElementsAre(2, 1, 3));
}
