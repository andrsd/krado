// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "krado/point.h"
#include <vector>
#include <array>

namespace krado {

class GeomShape;

/// Bounding box in 3D
///
/// As points are added it grows to be the bounding box of the point set
class BoundingBox3D {
public:
    BoundingBox3D();
    BoundingBox3D(Point pt);
    BoundingBox3D(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);
    BoundingBox3D(const GeomShape & shape);

    [[nodiscard]] bool empty() const;

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

    [[nodiscard]] bool contains(const BoundingBox3D & bound) const;

    [[nodiscard]] bool contains(Point p) const;

    [[nodiscard]] bool contains(double x, double y, double z) const;

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

/// Determine spatial dimension of a bounding box
///
/// @param bbox Boudning box
/// @return Spatial dimension
int determine_spatial_dim(const BoundingBox3D & bbox);

} // namespace krado

std::ostream & operator<<(std::ostream & stream, const krado::BoundingBox3D & bbox);
