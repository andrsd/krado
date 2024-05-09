// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "TopoDS_Vertex.hxx"

namespace krado {

class Model;

class GeomVertex {
public:
    explicit GeomVertex(const TopoDS_Vertex & vertex);

    double x() const;
    double y() const;
    double z() const;

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Vertex vertex;
    double x_coord, y_coord, z_coord;
};

} // namespace krado
