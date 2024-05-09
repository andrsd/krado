// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include <vector>

namespace krado {

// A bounding box class - add points and it grows to be the bounding box of the point set
class BoundingBox3D {
public:
    BoundingBox3D();
    BoundingBox3D(const Point & pt);
    BoundingBox3D(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);

    bool empty();

    void reset();

    void operator+=(const Point & pt);

    void operator+=(const BoundingBox3D & box);

    void operator*=(double scale);

    void scale(double sx, double sy, double sz);

    Point min() const;

    Point max() const;

    Point center() const;

    double diag() const;

    void make_cube();

    void thicken(double factor);

    bool contains(const BoundingBox3D & bound);

    bool contains(const Point & p);

    bool contains(double x, double y, double z);

    bool transform(const std::vector<double> & tfo);

private:
    Point min_pt, max_pt;
};

} // namespace krado
