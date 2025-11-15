#include "gmock/gmock.h"
#include "builder.h"
#include "krado/classifier.h"

using namespace krado;

TEST(ClassifierTest, points)
{
    auto box = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    SolidClassifier clsfr(box);
    EXPECT_TRUE(clsfr.inside(Point(0.5, 1., 1.5)));
    EXPECT_TRUE(clsfr.outside(Point(-1., -1., -1.)));
}
