#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/utils.h"

using namespace krado;

TEST(UtilsTest, to_upper)
{
    EXPECT_EQ(utils::to_upper("asdf"), "ASDF");
    EXPECT_EQ(utils::to_upper("ASDF"), "ASDF");
    EXPECT_EQ(utils::to_upper("AsDf"), "ASDF");
}

TEST(UtilsTest, to_lower)
{
    EXPECT_EQ(utils::to_lower("asdf"), "asdf");
    EXPECT_EQ(utils::to_lower("ASDF"), "asdf");
    EXPECT_EQ(utils::to_lower("AsDf"), "asdf");
}

TEST(UtilsTest, sub_connect)
{
    std::vector<Index> connect = { 10, 20, 30, 40, 50, 60 };
    std::vector<u8> idxs = { 1, 3, 5 };
    EXPECT_THAT(utils::sub_connect(connect, idxs), testing::ElementsAreArray({ 20, 40, 60 }));
}

TEST(UtilsTest, to_str)
{
    EXPECT_EQ(utils::to_str(ElementType::POINT), "POINT");
    EXPECT_EQ(utils::to_str(ElementType::LINE2), "LINE2");
    EXPECT_EQ(utils::to_str(ElementType::TRI3), "TRI3");
    EXPECT_EQ(utils::to_str(ElementType::QUAD4), "QUAD4");
    EXPECT_EQ(utils::to_str(ElementType::TETRA4), "TETRA4");
    EXPECT_EQ(utils::to_str(ElementType::PYRAMID5), "PYRAMID5");
    EXPECT_EQ(utils::to_str(ElementType::PRISM6), "PRISM6");
    EXPECT_EQ(utils::to_str(ElementType::HEX8), "HEX8");
    EXPECT_EQ(utils::to_str(ElementType::INVALID), "UNKNOWN");
}

TEST(UtilsTest, angle)
{
    Point p1(0, 0, 0);
    Point p2(1, 0, 0);
    Point p3(1, 1, 0);
    auto alpha = utils::angle(p1, p2, p3);
    EXPECT_DOUBLE_EQ(alpha, M_PI / 2.);
}

TEST(UtilsTest, distance_pt)
{
    Point a(1., 2., 3.);
    Point b(2., -1., 4.);
    EXPECT_NEAR(utils::distance(a, b), std::sqrt(11), 1e-15);
}

TEST(UtilsTest, distance_uv)
{
    UVParam a(1., 2.);
    UVParam b(2., -1.);
    EXPECT_NEAR(utils::distance(a, b), std::sqrt(10), 1e-15);
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

TEST(UtilsTest, shift_span)
{
    std::vector<Index> idxs = { 10, 11, 15, 23 };
    auto shifted = shift(idxs, -10);
    EXPECT_THAT(shifted, testing::ElementsAreArray({ 0, 1, 5, 13 }));
}

TEST(UtilsTest, shift_set)
{
    std::set<HasseIndex> idxs = { HasseIndex(10), HasseIndex(15), HasseIndex(11) };
    auto shifted = shift(idxs, -10);
    EXPECT_THAT(shifted, testing::UnorderedElementsAreArray({ 0, 1, 5 }));
}
