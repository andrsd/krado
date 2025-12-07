// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "TopoDS_Wire.hxx"
#include <vector>

namespace krado {

class Wire {
public:
    /// Compute the length of the edge
    ///
    /// @return Length of the edge
    double length() const;

private:
    explicit Wire(const TopoDS_Wire & wire);

    TopoDS_Wire wire_;

public:
    static Wire create(const std::vector<GeomCurve> & curves);
};

} // namespace krado
