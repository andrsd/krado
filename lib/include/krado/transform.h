#pragma once

#include "krado/point.h"

namespace krado {

/// Transformation
///
/// This class represents a transformation in 3D space.  It can be used to scale, translate and
/// rotate points.  This transformation is using homogeneous coordinates under the hood.
/// Transformations can be chained together using the multiplication operator or by applying
/// transformation using method chaining.
class Trsf {
public:
    Trsf();

    /// Scale this transformation isotropicly
    ///
    /// @param factor Scaling factor
    /// @return Resulting transformation
    Trsf & scale(double factor);

    /// Scale this transformation anisotropicly
    ///
    /// @param factor_x scaling factor in x direction
    /// @param factor_y scaling factor in y direction
    /// @param factor_z scaling factor in z direction
    /// @return Resulting transformation
    Trsf & scale(double factor_x, double factor_y, double factor_z = 1.);

    /// Translate this transformation
    ///
    /// @param tx Translation in x direction
    /// @param ty Translation in y direction
    /// @param tz Translation in z direction
    /// @return Resulting transformation
    Trsf & translate(double tx, double ty, double tz = 0.);

    /// Rotate this transformation around x axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Resulting transformation
    Trsf & rotate_x(double theta);

    /// Rotate this transformation around y axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Resulting transformation
    Trsf & rotate_y(double theta);

    /// Rotate this transformation around z axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Resulting transformation
    Trsf & rotate_z(double theta);

    /// Apply transformation
    ///
    /// @param other Transformation to apply
    /// @return Composition of transformations
    [[nodiscard]] Trsf operator*(const Trsf & other) const;

    /// Apply transformation
    ///
    /// @param other Transformation to apply
    /// @return Composition of transformations
    Trsf & operator*=(const Trsf & other);

    /// Apply transformation to point
    ///
    /// @param point Point to transform
    /// @return Transformed point
    [[nodiscard]] Point operator*(const Point & other) const;

private:
    static constexpr int N = 4;

    double mat[N][N];

public:
    /// Create isotropic scaling transformation
    ///
    /// @param factor Scaling factor
    /// @return Scaling transformation
    [[nodiscard]] static Trsf scaled(double factor);

    /// Create anisotropic scaling transformation
    ///
    /// @param factor_x scaling factor in x direction
    /// @param factor_y scaling factor in y direction
    /// @param factor_z scaling factor in z direction
    /// @return Scaling transformation
    [[nodiscard]] static Trsf scaled(double factor_x, double factor_y, double factor_z = 1.);

    /// Create translation transformation
    ///
    /// @param tx Translation in x direction
    /// @param ty Translation in y direction
    /// @param tz Translation in z direction
    /// @return Translation transformation
    [[nodiscard]] static Trsf translated(double tx, double ty, double tz = 0.);

    /// Create rotation transformation around z axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Rotation transformation
    [[nodiscard]] static Trsf rotated_x(double theta);

    /// Create rotation transformation around y axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Rotation transformation
    [[nodiscard]] static Trsf rotated_y(double theta);

    /// Create rotation transformation around z axis
    ///
    /// @param theta Rotation angle in radians
    /// @return Rotation transformation
    [[nodiscard]] static Trsf rotated_z(double theta);

    /// Create identity transformation
    ///
    /// @return Identity transformation
    [[nodiscard]] static Trsf identity();
};

} // namespace krado
