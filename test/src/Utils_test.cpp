#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/utils.h"
#include "krado/io.h"

using namespace krado;

TEST(UtilsTest, angle)
{
    Point p1(0, 0, 0);
    Point p2(1, 0, 0);
    Point p3(1, 1, 0);
    auto alpha = utils::angle(p1, p2, p3);
    EXPECT_DOUBLE_EQ(alpha, M_PI / 2.);
}

TEST(UtilsTest, distance)
{
    Point a(1., 2., 3.);
    Point b(2., -1., 4.);
    EXPECT_NEAR(utils::distance(a, b), std::sqrt(11), 1e-15);
}

TEST(UtilsTest, human_number)
{
    EXPECT_EQ(utils::human_number(0), "0");
    EXPECT_EQ(utils::human_number(10), "10");
    EXPECT_EQ(utils::human_number(999), "999");
    EXPECT_EQ(utils::human_number(1234), "1,234");
    EXPECT_EQ(utils::human_number(12345), "12,345");
    EXPECT_EQ(utils::human_number(1234567), "1,234,567");
}

TEST(UtilsTest, join)
{
    EXPECT_EQ(join(",", std::vector<int> { 1 }), "1");
    EXPECT_EQ(join(",", std::vector<int> { 3, 5 }), "3,5");
}

TEST(UtilsTest, human_time)
{
    EXPECT_EQ(utils::human_time(0), "0.00ms");
    EXPECT_EQ(utils::human_time(0.0005), "0.50ms");
    EXPECT_EQ(utils::human_time(0.5), "500.00ms");
    EXPECT_EQ(utils::human_time(10), "10.00s");
    EXPECT_EQ(utils::human_time(60), "1m");
    EXPECT_EQ(utils::human_time(70), "1m 10.00s");
    EXPECT_EQ(utils::human_time(70.5), "1m 10.50s");
    EXPECT_EQ(utils::human_time(3600), "1h");
    EXPECT_EQ(utils::human_time(3720), "1h 2m");
    EXPECT_EQ(utils::human_time(3725), "1h 2m 5.00s");
    EXPECT_EQ(utils::human_time(3725.2), "1h 2m 5.20s");
}

TEST(UtilsTest, debug)
{
    Log::set_verbosity(9);

    auto mesh = IO::import_mesh("/Users/andrsd/projects/sawtooth-2/run/thermal/unit_cell.exo");
}
