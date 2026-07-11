// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmock/gmock.h"
#include "krado/io.h"
#include "krado/box.h"
#include <gtest/gtest.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace krado;

TEST(IOTest, import_geom_step)
{
    auto shapes = IO::import_geometry(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets/geo/box.step");
    EXPECT_EQ(shapes.size(), 1);
    EXPECT_DOUBLE_EQ(shapes[0].volume(), 24.);
}

TEST(IOTest, import_geom_iges)
{
    auto shapes = IO::import_geometry(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets/geo/box.iges");
    EXPECT_EQ(shapes.size(), 1);
    EXPECT_DOUBLE_EQ(shapes[0].volume(), 1.);
}

TEST(IOTest, export_geom)
{
    auto box = Box::create(Point(0, 0, 0), Point(2, 3, 4));
    std::vector<GeomShape> shapes = { box };
    IO::export_geometry(shapes, "box.step");
    IO::export_geometry(shapes, "box.iges");
}
