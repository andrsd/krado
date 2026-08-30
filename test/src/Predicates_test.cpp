// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmock/gmock.h"
#include "krado/predicates.h"
#include "krado/point.h"
#include "krado/uv_param.h"

using namespace krado;

TEST(PredicatesTest, orient2d_point_ccw)
{
    Point pa(0.0, 0.0, 0.0);
    Point pb(1.0, 0.0, 0.0);
    Point pc(0.0, 1.0, 0.0);
    EXPECT_GT(orient2d(pa, pb, pc), 0.0);
}

TEST(PredicatesTest, orient2d_point_cw)
{
    Point pa(0.0, 0.0, 0.0);
    Point pb(1.0, 0.0, 0.0);
    Point pc(0.0, -1.0, 0.0);
    EXPECT_LT(orient2d(pa, pb, pc), 0.0);
}

TEST(PredicatesTest, orient2d_point_collinear)
{
    Point pa(0.0, 0.0, 0.0);
    Point pb(1.0, 0.0, 0.0);
    Point pc(2.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(orient2d(pa, pb, pc), 0.0);
}

TEST(PredicatesTest, orient2d_uv_ccw)
{
    UVParam pa(0.0, 0.0);
    UVParam pb(1.0, 0.0);
    UVParam pc(0.0, 1.0);
    EXPECT_GT(orient2d(pa, pb, pc), 0.0);
}

TEST(PredicatesTest, orient2d_uv_cw)
{
    UVParam pa(0.0, 0.0);
    UVParam pb(1.0, 0.0);
    UVParam pc(0.0, -1.0);
    EXPECT_LT(orient2d(pa, pb, pc), 0.0);
}

TEST(PredicatesTest, orient2d_uv_collinear)
{
    UVParam pa(0.0, 0.0);
    UVParam pb(1.0, 0.0);
    UVParam pc(2.0, 0.0);
    EXPECT_DOUBLE_EQ(orient2d(pa, pb, pc), 0.0);
}

TEST(PredicatesTest, orient3d_point_negative)
{
    Point pa(0.0, 0.0, 0.0);
    Point pb(1.0, 0.0, 0.0);
    Point pc(0.0, 1.0, 0.0);
    Point pd(0.0, 0.0, 1.0);
    EXPECT_LT(orient3d(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, orient3d_point_positive)
{
    Point pa(0.0, 0.0, 0.0);
    Point pb(1.0, 0.0, 0.0);
    Point pc(0.0, 1.0, 0.0);
    Point pd(0.0, 0.0, -1.0);
    EXPECT_GT(orient3d(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, orient3d_point_coplanar)
{
    Point pa(0.0, 0.0, 0.0);
    Point pb(1.0, 0.0, 0.0);
    Point pc(0.0, 1.0, 0.0);
    Point pd(0.5, 0.5, 0.0);
    EXPECT_DOUBLE_EQ(orient3d(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, incircle_point_inside)
{
    Point pa(1.0, 0.0, 0.0);
    Point pb(0.0, 1.0, 0.0);
    Point pc(-1.0, 0.0, 0.0);
    Point pd(0.0, 0.0, 0.0);
    EXPECT_GT(incircle(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, incircle_point_outside)
{
    Point pa(1.0, 0.0, 0.0);
    Point pb(0.0, 1.0, 0.0);
    Point pc(-1.0, 0.0, 0.0);
    Point pd(2.0, 0.0, 0.0);
    EXPECT_LT(incircle(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, incircle_point_on)
{
    Point pa(1.0, 0.0, 0.0);
    Point pb(0.0, 1.0, 0.0);
    Point pc(-1.0, 0.0, 0.0);
    Point pd(0.0, -1.0, 0.0);
    EXPECT_DOUBLE_EQ(incircle(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, incircle_uv_inside)
{
    UVParam pa(1.0, 0.0);
    UVParam pb(0.0, 1.0);
    UVParam pc(-1.0, 0.0);
    UVParam pd(0.0, 0.0);
    EXPECT_GT(incircle(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, incircle_uv_outside)
{
    UVParam pa(1.0, 0.0);
    UVParam pb(0.0, 1.0);
    UVParam pc(-1.0, 0.0);
    UVParam pd(2.0, 0.0);
    EXPECT_LT(incircle(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, incircle_uv_on)
{
    UVParam pa(1.0, 0.0);
    UVParam pb(0.0, 1.0);
    UVParam pc(-1.0, 0.0);
    UVParam pd(0.0, -1.0);
    EXPECT_DOUBLE_EQ(incircle(pa, pb, pc, pd), 0.0);
}

TEST(PredicatesTest, insphere_point_inside)
{
    Point pa(1.0, 0.0, 0.0);
    Point pb(0.0, 1.0, 0.0);
    Point pc(0.0, 0.0, 1.0);
    Point pd(-1.0, 0.0, 0.0);
    Point pe(0.0, 0.0, 0.0);
    EXPECT_GT(insphere(pa, pb, pc, pd, pe), 0.0);
}

TEST(PredicatesTest, insphere_point_outside)
{
    Point pa(1.0, 0.0, 0.0);
    Point pb(0.0, 1.0, 0.0);
    Point pc(0.0, 0.0, 1.0);
    Point pd(-1.0, 0.0, 0.0);
    Point pe(2.0, 2.0, 2.0);
    EXPECT_LT(insphere(pa, pb, pc, pd, pe), 0.0);
}

TEST(PredicatesTest, insphere_point_on)
{
    Point pa(1.0, 0.0, 0.0);
    Point pb(0.0, 1.0, 0.0);
    Point pc(0.0, 0.0, 1.0);
    Point pd(-1.0, 0.0, 0.0);
    Point pe(0.0, 0.0, -1.0);
    EXPECT_DOUBLE_EQ(insphere(pa, pb, pc, pd, pe), 0.0);
}
