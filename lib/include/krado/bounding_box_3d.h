// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include <vector>
#include <array>

namespace krado {

/// Bounding box in 3D
///
/// As points are added it grows to be the bounding box of the point set
class BoundingBox3D {
public:
    BoundingBox3D();
    BoundingBox3D(const Point & pt);
    BoundingBox3D(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);

    [[nodiscard]] bool empty();

    void reset();

    void operator+=(const Point & pt);

    void operator+=(const BoundingBox3D & box);

    void operator*=(double scale);

    void scale(double sx, double sy, double sz);

    [[nodiscard]] Point min() const;

    [[nodiscard]] Point max() const;

    [[nodiscard]] Point center() const;

    [[nodiscard]] double diag() const;

    void make_cube();

    void thicken(double factor);

    [[nodiscard]] bool contains(const BoundingBox3D & bound);

    [[nodiscard]] bool contains(const Point & p);

    [[nodiscard]] bool contains(double x, double y, double z);

    [[nodiscard]] bool transform(const std::vector<double> & tfo);

    [[nodiscard]] std::array<double, 3> size() const;

    /// Return n-th dimension of the bounding box
    ///
    /// @param n dimension index (0, 1, 2)
    /// @return size of the n-th dimension
    [[nodiscard]] double size(int n) const;

private:
    Point min_pt_, max_pt_;
};

} // namespace krado
