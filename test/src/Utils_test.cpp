#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/utils.h"

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

TEST(UtilsTest, human_time)
{
    EXPECT_EQ(utils::human_time(std::chrono::duration<double, std::micro>(10)), "10.00 µs");
    EXPECT_EQ(utils::human_time(std::chrono::duration<double, std::micro>(2340)), "2.34 ms");
    EXPECT_EQ(utils::human_time(std::chrono::duration<double, std::milli>(2)), "2.00 ms");
    EXPECT_EQ(utils::human_time(std::chrono::duration<double, std::milli>(5670)), "5.67 s");
    EXPECT_EQ(utils::human_time(std::chrono::duration<double, std::milli>(64120)), "1 min 4 s");
    EXPECT_EQ(utils::human_time(std::chrono::duration<double, std::milli>(7'386'000)),
              "2 h 3 min 6 s");
}
