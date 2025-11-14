// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/geom_shape.h"
#include "BRepClass3d_SolidClassifier.hxx"

namespace krado {

/// Provides capability to classify if points are inside or outside of a given shape
///
class SolidClassifier {
public:
    /// Solid classfier
    ///
    /// @param shape Shape that we will be using for classification
    SolidClassifier(const GeomShape & shape);

    /// Test if a point is inside the shape
    ///
    /// @param pt Point to test
    /// @return `true` if the point is inside, `false` otherwise
    bool inside(const Point & pt);

    /// Test if a point is outside the shape
    ///
    /// @param pt Point to test
    /// @return `true` if the point is outside, `false` otherwise
    bool outside(const Point & pt);

private:
    BRepClass3d_SolidClassifier classifier_;
};

} // namespace krado
