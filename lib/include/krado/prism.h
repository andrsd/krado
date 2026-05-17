// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/vector.h"

namespace krado {

class Prism : public GeomShape {
public:
    Prism(const TopoDS_Shape & shape);

public:
    static Prism create(const GeomShape & shape, Vector vec);
};

} // namespace krado
