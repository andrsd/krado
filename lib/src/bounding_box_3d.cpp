// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later

#include "krado/bounding_box_3d.h"
#include <limits>

namespace krado {

constexpr auto MAX = std::numeric_limits<double>::max();

BoundingBox3D::BoundingBox3D() : min_pt_(MAX, MAX, MAX), max_pt_(-MAX, -MAX, -MAX) {}

BoundingBox3D::BoundingBox3D(const Point & pt) : min_pt_(pt), max_pt_(pt) {}

BoundingBox3D::BoundingBox3D(double xmin,
                             double ymin,
                             double zmin,
                             double xmax,
                             double ymax,
                             double zmax) :
    min_pt_(xmin, ymin, zmin),
    max_pt_(xmax, ymax, zmax)
{
}

bool
BoundingBox3D::empty()
{
    if (this->min_pt_.x == MAX || this->min_pt_.y == MAX || this->min_pt_.z == MAX ||
        this->max_pt_.x == -MAX || this->max_pt_.y == -MAX || this->max_pt_.z == -MAX)
        return true;
    else
        return false;
}

void
BoundingBox3D::reset()
{
    this->min_pt_ = Point(MAX, MAX, MAX);
    this->max_pt_ = Point(-MAX, -MAX, -MAX);
}

void
BoundingBox3D::operator+=(const Point & pt)
{
    // note: it is possible for pt[i] to be both > MaxPt[i] and < MinPt[i]
    // the first point always will be both
    if (pt.x < this->min_pt_.x)
        this->min_pt_.x = pt.x;
    if (pt.x > this->max_pt_.x)
        this->max_pt_.x = pt.x;

    if (pt.y < this->min_pt_.y)
        this->min_pt_.y = pt.y;
    if (pt.y > this->max_pt_.y)
        this->max_pt_.y = pt.y;

    if (pt.z < this->min_pt_.z)
        this->min_pt_.z = pt.z;
    if (pt.z > this->max_pt_.z)
        this->max_pt_.z = pt.z;
}

void
BoundingBox3D::operator+=(const BoundingBox3D & box)
{
    (*this) += box.min_pt_;
    (*this) += box.max_pt_;
}

void
BoundingBox3D::operator*=(double scale)
{
    Point center = (min_pt_ + max_pt_) * .5;
    this->max_pt_ -= center;
    this->min_pt_ -= center;
    this->max_pt_ *= scale;
    this->min_pt_ *= scale;
    this->max_pt_ += center;
    this->min_pt_ += center;
}

void
BoundingBox3D::scale(double sx, double sy, double sz)
{
    Point center = (min_pt_ + max_pt_) * .5;
    this->max_pt_ -= center;
    this->min_pt_ -= center;
    this->max_pt_.x *= sx;
    this->max_pt_.y *= sy;
    this->max_pt_.z *= sz;
    this->min_pt_.x *= sx;
    this->min_pt_.y *= sy;
    this->min_pt_.z *= sz;
    this->max_pt_ += center;
    this->min_pt_ += center;
}

Point
BoundingBox3D::min() const
{
    return this->min_pt_;
}

Point
BoundingBox3D::max() const
{
    return this->max_pt_;
}

Point
BoundingBox3D::center() const
{
    return (this->min_pt_ + this->max_pt_) * .5;
}

double
BoundingBox3D::diag() const
{
    return this->min_pt_.distance(this->max_pt_);
}

void
BoundingBox3D::make_cube()
{
    auto delta = this->max_pt_ - this->min_pt_;
    Vector len(delta.x, delta.y, delta.z);
    Point cc = center();
    this->max_pt_ = cc + Point(1, 1, 1);
    this->min_pt_ = cc + Point(-1, -1, -1);
    double sc = len.norm() * 0.5;
    scale(sc, sc, sc);
}

void
BoundingBox3D::thicken(double factor)
{
    double d = factor * diag();
    Point t(d, d, d);
    this->min_pt_ -= t;
    this->max_pt_ += t;
}

bool
BoundingBox3D::contains(const BoundingBox3D & bound)
{
    if (bound.min_pt_.x >= this->min_pt_.x && bound.min_pt_.y >= this->min_pt_.y &&
        bound.min_pt_.z >= this->min_pt_.z && bound.max_pt_.x <= this->max_pt_.x &&
        bound.max_pt_.y <= this->max_pt_.y && bound.max_pt_.z <= this->max_pt_.z)
        return true;
    else
        return false;
}

bool
BoundingBox3D::contains(const Point & p)
{
    if (p.x >= this->min_pt_.x && p.y >= this->min_pt_.y && p.z >= this->min_pt_.z &&
        p.x <= this->max_pt_.x && p.y <= this->max_pt_.y && p.z <= this->max_pt_.z)
        return true;
    else
        return false;
}

bool
BoundingBox3D::contains(double x, double y, double z)
{
    if (x >= this->min_pt_.x && y >= this->min_pt_.y && z >= this->min_pt_.z && x <= this->max_pt_.x &&
        y <= this->max_pt_.y && z <= this->max_pt_.z)
        return true;
    else
        return false;
}

bool
BoundingBox3D::transform(const std::vector<double> & tfo)
{
    if (tfo.size() != 16)
        return false;
    this->min_pt_.transform(tfo);
    this->max_pt_.transform(tfo);
    return true;
}

std::array<double, 3>
BoundingBox3D::size() const
{
    std::array<double, 3> sz = { std::abs(this->max_pt_.x - this->min_pt_.x),
                                 std::abs(this->max_pt_.y - this->min_pt_.y),
                                 std::abs(this->max_pt_.z - this->min_pt_.z) };
    return sz;
}

double
BoundingBox3D::size(int n) const
{
    if (n == 0)
        return std::abs(this->max_pt_.x - this->min_pt_.x);
    else if (n == 1)
        return std::abs(this->max_pt_.y - this->min_pt_.y);
    else if (n == 2)
        return std::abs(this->max_pt_.z - this->min_pt_.z);
    else
        return std::numeric_limits<double>::infinity();
}

} // namespace krado
