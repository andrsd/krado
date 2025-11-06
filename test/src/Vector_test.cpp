#include "gmock/gmock.h"
#include "krado/vector.h"
#include "krado/exception.h"
#include "krado/axis1.h"

using namespace krado;

TEST(VectorTest, ctor_empty)
{
    Vector vec;
    EXPECT_DOUBLE_EQ(vec.x, 0.);
    EXPECT_DOUBLE_EQ(vec.y, 0.);
    EXPECT_DOUBLE_EQ(vec.z, 0.);
}

TEST(VectorTest, ctor)
{
    Vector vec(1., 2., 3.);
    EXPECT_DOUBLE_EQ(vec.x, 1.);
    EXPECT_DOUBLE_EQ(vec.y, 2.);
    EXPECT_DOUBLE_EQ(vec.z, 3.);
}

TEST(VectorTest, op_call)
{
    Vector vec(5., 6., 9.);
    EXPECT_DOUBLE_EQ(vec(0), 5.);
    EXPECT_DOUBLE_EQ(vec(1), 6.);
    EXPECT_DOUBLE_EQ(vec(2), 9.);

    EXPECT_THROW({ auto r = vec(10); }, Exception);
}

TEST(VectorTest, norm)
{
    Vector a(3, 4, 0);
    EXPECT_DOUBLE_EQ(a.magnitude(), 5.);
}

TEST(VectorTest, normalize)
{
    Vector a(3, 4, 5);
    a.normalize();
    double l = std::sqrt(3 * 3 + 4 * 4 + 5 * 5);
    EXPECT_DOUBLE_EQ(a(0), 3. / l);
    EXPECT_DOUBLE_EQ(a(1), 4. / l);
    EXPECT_DOUBLE_EQ(a(2), 5. / l);
}

TEST(VectorTest, normalized)
{
    Vector a(3, 4, 5);
    auto b = a.normalized();
    double l = std::sqrt(3 * 3 + 4 * 4 + 5 * 5);
    EXPECT_DOUBLE_EQ(b(0), 3. / l);
    EXPECT_DOUBLE_EQ(b(1), 4. / l);
    EXPECT_DOUBLE_EQ(b(2), 5. / l);
}

TEST(VectorTest, op_add)
{
    Vector a(1, 2, 3);
    Vector b(4, 1, -4);
    auto c = a + b;
    EXPECT_DOUBLE_EQ(c(0), 5.);
    EXPECT_DOUBLE_EQ(c(1), 3.);
    EXPECT_DOUBLE_EQ(c(2), -1.);
}

TEST(VectorTest, op_add2)
{
    Vector a(1, 2, 3);
    Vector b(4, 1, -4);
    b += a;
    EXPECT_DOUBLE_EQ(b(0), 5.);
    EXPECT_DOUBLE_EQ(b(1), 3.);
    EXPECT_DOUBLE_EQ(b(2), -1.);
}

TEST(VectorTest, op_sub)
{
    Vector a(1, 2, 3);
    Vector b(4, 1, -4);
    auto c = a - b;
    EXPECT_DOUBLE_EQ(c(0), -3);
    EXPECT_DOUBLE_EQ(c(1), 1.);
    EXPECT_DOUBLE_EQ(c(2), 7.);
}

TEST(VectorTest, op_sub2)
{
    Vector a(1, 2, 3);
    Vector b(4, 1, -4);
    a -= b;
    EXPECT_DOUBLE_EQ(a(0), -3);
    EXPECT_DOUBLE_EQ(a(1), 1.);
    EXPECT_DOUBLE_EQ(a(2), 7.);
}

TEST(VectorTest, op_mult_scalar)
{
    Vector a(1, 2, 3);
    auto c = 2. * a;
    EXPECT_DOUBLE_EQ(c(0), 2.);
    EXPECT_DOUBLE_EQ(c(1), 4.);
    EXPECT_DOUBLE_EQ(c(2), 6.);

    auto d = a * 3.;
    EXPECT_DOUBLE_EQ(d(0), 3.);
    EXPECT_DOUBLE_EQ(d(1), 6.);
    EXPECT_DOUBLE_EQ(d(2), 9.);

    auto e = a;
    e *= 4.;
    EXPECT_DOUBLE_EQ(e(0), 4.);
    EXPECT_DOUBLE_EQ(e(1), 8.);
    EXPECT_DOUBLE_EQ(e(2), 12.);
}

TEST(VectorTest, cross_product)
{
    Vector a(1, 0, 0);
    Vector b(0, 1, 0);
    auto c = cross_product(a, b);
    EXPECT_DOUBLE_EQ(c(0), 0.);
    EXPECT_DOUBLE_EQ(c(1), 0.);
    EXPECT_DOUBLE_EQ(c(2), 1.);
}

TEST(VectorTest, dot_product)
{
    Vector a(1, 2, 3);
    Vector b(2, -1, 3);
    auto c = dot_product(a, b);
    EXPECT_DOUBLE_EQ(c, 9.);
}

TEST(VectorTest, rotate_around_z_90deg)
{
    Vector v(1, 0, 0); // X-axis
    Axis1 axis(Point(0, 0, 0), Vector(0, 0, 1));

    Vector v_rot = v.rotated(axis, M_PI / 2);

    EXPECT_NEAR(v_rot.x, 0.0, 1e-12);
    EXPECT_NEAR(v_rot.y, 1.0, 1e-12);
    EXPECT_NEAR(v_rot.z, 0.0, 1e-12);
}

TEST(VectorTest, is_equal)
{
    Vector a(1, 0, 0);
    Vector b(1, 0, 0);
    Vector c(-1, 0, 0);

    EXPECT_TRUE(a.is_equal(b));
    EXPECT_FALSE(a.is_equal(c));
}
