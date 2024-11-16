#pragma once

#include "krado/point.h"

namespace krado {

class Trsf {
public:
    Trsf();

    /// Apply transformation
    ///
    /// @param other Transformation to apply
    /// @return Composition of transformations
    Trsf operator*(const Trsf & other) const;

    /// Apply transformation to point
    ///
    /// @param point Point to transform
    /// @return Transformed point
    Point operator*(const Point & other) const;

private:
    static constexpr int N = 4;

    double mat[N][N];

public:
    /// Create isotropic scaling transformation
    ///
    /// @param factor Scaling factor
    /// @return Scaling transformation
    static Trsf scale(double factor);

    /// Create anisotropic scaling transformation
    ///
    /// @param factor_x scaling factor in x direction
    /// @param factor_y scaling factor in y direction
    /// @param factor_z scaling factor in z direction
    /// @return Scaling transformation
    static Trsf scale(double factor_x, double factor_y, double factor_z = 1.);

    /// Create translation transformation
    ///
    /// @param tx Translation in x direction
    /// @param ty Translation in y direction
    /// @param tz Translation in z direction
    /// @return Translation transformation
    static Trsf translate(double tx, double ty, double tz = 0.);

    /// Create rotation transformation around z axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Rotation transformation
    static Trsf rotate_x(double theta);

    /// Create rotation transformation around y axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Rotation transformation
    static Trsf rotate_y(double theta);

    /// Create rotation transformation around z axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Rotation transformation
    static Trsf rotate_z(double theta);

    /// Create identity transformation
    ///
    /// @return Identity transformation
    static Trsf identity();
};

} // namespace krado
