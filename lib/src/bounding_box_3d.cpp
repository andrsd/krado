// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later

#include "krado/bounding_box_3d.h"

namespace krado {

constexpr auto MAX = std::numeric_limits<double>::max();

BoundingBox3D::BoundingBox3D() : min_pt(MAX, MAX, MAX), max_pt(-MAX, -MAX, -MAX) {}

BoundingBox3D::BoundingBox3D(const Point & pt) : min_pt(pt), max_pt(pt) {}

BoundingBox3D::BoundingBox3D(double xmin,
                             double ymin,
                             double zmin,
                             double xmax,
                             double ymax,
                             double zmax) :
    min_pt(xmin, ymin, zmin),
    max_pt(xmax, ymax, zmax)
{
}

bool
BoundingBox3D::empty()
{
    if (this->min_pt.x == MAX || this->min_pt.y == MAX || this->min_pt.z == MAX ||
        this->max_pt.x == -MAX || this->max_pt.y == -MAX || this->max_pt.z == -MAX)
        return true;
    else
        return false;
}

void
BoundingBox3D::reset()
{
    this->min_pt = Point(MAX, MAX, MAX);
    this->max_pt = Point(-MAX, -MAX, -MAX);
}

void
BoundingBox3D::operator+=(const Point & pt)
{
    // note: it is possible for pt[i] to be both > MaxPt[i] and < MinPt[i]
    // the first point always will be both
    if (pt.x < this->min_pt.x)
        this->min_pt.x = pt.x;
    if (pt.x > this->max_pt.x)
        this->max_pt.x = pt.x;

    if (pt.y < this->min_pt.y)
        this->min_pt.y = pt.y;
    if (pt.y > this->max_pt.y)
        this->max_pt.y = pt.y;

    if (pt.z < this->min_pt.z)
        this->min_pt.z = pt.z;
    if (pt.z > this->max_pt.z)
        this->max_pt.z = pt.z;
}

void
BoundingBox3D::operator+=(const BoundingBox3D & box)
{
    (*this) += box.min_pt;
    (*this) += box.max_pt;
}

void
BoundingBox3D::operator*=(double scale)
{
    Point center = (min_pt + max_pt) * .5;
    this->max_pt -= center;
    this->min_pt -= center;
    this->max_pt *= scale;
    this->min_pt *= scale;
    this->max_pt += center;
    this->min_pt += center;
}

void
BoundingBox3D::scale(double sx, double sy, double sz)
{
    Point center = (min_pt + max_pt) * .5;
    this->max_pt -= center;
    this->min_pt -= center;
    this->max_pt.x *= sx;
    this->max_pt.y *= sy;
    this->max_pt.z *= sz;
    this->min_pt.x *= sx;
    this->min_pt.y *= sy;
    this->min_pt.z *= sz;
    this->max_pt += center;
    this->min_pt += center;
}

Point
BoundingBox3D::min() const
{
    return this->min_pt;
}

Point
BoundingBox3D::max() const
{
    return this->max_pt;
}

Point
BoundingBox3D::center() const
{
    return (this->min_pt + this->max_pt) * .5;
}

double
BoundingBox3D::diag() const
{
    return this->min_pt.distance(this->max_pt);
}

void
BoundingBox3D::make_cube()
{
    auto delta = this->max_pt - this->min_pt;
    Vector len(delta.x, delta.y, delta.z);
    Point cc = center();
    this->max_pt = cc + Point(1, 1, 1);
    this->min_pt = cc + Point(-1, -1, -1);
    double sc = len.norm() * 0.5;
    scale(sc, sc, sc);
}

void
BoundingBox3D::thicken(double factor)
{
    double d = factor * diag();
    Point t(d, d, d);
    this->min_pt -= t;
    this->max_pt += t;
}

bool
BoundingBox3D::contains(const BoundingBox3D & bound)
{
    if (bound.min_pt.x >= this->min_pt.x && bound.min_pt.y >= this->min_pt.y &&
        bound.min_pt.z >= this->min_pt.z && bound.max_pt.x <= this->max_pt.x &&
        bound.max_pt.y <= this->max_pt.y && bound.max_pt.z <= this->max_pt.z)
        return true;
    else
        return false;
}

bool
BoundingBox3D::contains(const Point & p)
{
    if (p.x >= this->min_pt.x && p.y >= this->min_pt.y && p.z >= this->min_pt.z &&
        p.x <= this->max_pt.x && p.y <= this->max_pt.y && p.z <= this->max_pt.z)
        return true;
    else
        return false;
}

bool
BoundingBox3D::contains(double x, double y, double z)
{
    if (x >= this->min_pt.x && y >= this->min_pt.y && z >= this->min_pt.z && x <= this->max_pt.x &&
        y <= this->max_pt.y && z <= this->max_pt.z)
        return true;
    else
        return false;
}

bool
BoundingBox3D::transform(const std::vector<double> & tfo)
{
    if (tfo.size() != 16)
        return false;
    this->min_pt.transform(tfo);
    this->max_pt.transform(tfo);
    return true;
}

} // namespace krado
