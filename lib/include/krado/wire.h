// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "krado/geom_shape.h"
#include "TopoDS_Wire.hxx"
#include <vector>

namespace krado {

class Wire : public GeomShape {
public:
    /// Compute the length of the edge
    ///
    /// @return Length of the edge
    double length() const;

protected:
    explicit Wire(const TopoDS_Wire & wire);

private:
    TopoDS_Wire wire_;

public:
    static Wire create(const std::vector<GeomCurve> & curves);
};

} // namespace krado
