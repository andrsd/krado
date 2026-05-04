#include "gmock/gmock.h"
#include "krado/element.h"
#include "krado/point.h"

using namespace krado;

TEST(ElementTest, line2)
{
    auto elem = Element::Line2({ 1, 2 });
    EXPECT_EQ(elem.num_vertices(), 2);
    EXPECT_EQ(elem.type(), ElementType::LINE2);
    EXPECT_EQ(elem.index(0), 1);
}

TEST(ElementTest, tri3)
{
    auto elem = Element::Tri3({ 1, 2, 5 });
    EXPECT_EQ(elem.num_vertices(), 3);
    EXPECT_EQ(elem.type(), ElementType::TRI3);
    EXPECT_EQ(elem.index(0), 1);
    EXPECT_EQ(elem.index(1), 2);
    EXPECT_EQ(elem.index(2), 5);
}

TEST(ElementTest, tri3_gamma)
{
    Point a(0., 0., 0.);
    Point b(1., 0., 0.);
    Point c(1., 1., 0.);
    EXPECT_NEAR(Tri3::gamma(a, b, c), 0.82842712, 1e-8);
}

TEST(ElementTest, tri3_eta)
{
    Point a(0., 0., 0.);
    Point b(1., 0., 0.);
    Point c(1., 1., 0.);
    EXPECT_NEAR(Tri3::eta(a, b, c), 0.75, 1e-10);
}

TEST(ElementTest, tet4)
{
    auto elem = Element::Tetra4({ 2, 5, 6, 8 });
    EXPECT_EQ(elem.num_vertices(), 4);
    EXPECT_EQ(elem.type(), ElementType::TETRA4);
    EXPECT_EQ(elem.index(0), 2);
    EXPECT_EQ(elem.index(1), 5);
    EXPECT_EQ(elem.index(2), 6);
    EXPECT_EQ(elem.index(3), 8);
}

TEST(ElementTest, str_type)
{
    EXPECT_EQ(Element::type(ElementType::LINE2), "LINE2");
    EXPECT_EQ(Element::type(ElementType::TRI3), "TRI3");
    EXPECT_EQ(Element::type(ElementType::TETRA4), "TETRA4");
}

TEST(ElementTest, shift)
{
    auto elem = Element::Tri3({ 1, 5, 9 });
    elem.shift(10);
    EXPECT_EQ(elem.index(0), 11);
    EXPECT_EQ(elem.index(1), 15);
    EXPECT_EQ(elem.index(2), 19);
}
