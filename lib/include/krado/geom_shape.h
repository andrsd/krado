// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/flags.h"
#include "krado/color.h"
#include "TopoDS_Shape.hxx"

namespace krado {

class GeomShape {
public:
    enum HealFlag { FIX_DEGENERATED, FIX_SMALL_EDGES, FIX_SMALL_FACES, SEW_FACES, MAKE_SOLIDS };

    GeomShape() = default;
    explicit GeomShape(const TopoDS_Shape & shape);
    [[deprecated]] GeomShape(int dim, const TopoDS_Shape & shape);

    [[deprecated]] int dim() const;

    void clean();
    void heal(double tolerance,
              Flags<HealFlag> flags = FIX_DEGENERATED | FIX_SMALL_EDGES | FIX_SMALL_FACES |
                                      SEW_FACES | MAKE_SOLIDS);
    void scale(double scale_factor);

    /// Get the unique identifier of the shape
    ///
    /// @return The unique identifier of the shape.
    int id() const;

    /// Set the unique identifier of the shape
    ///
    /// @param id The unique identifier of the shape.
    void set_id(int id);

    /// Query if this shape has material assigned to it
    ///
    /// @return `true` if material is assigned, `false` otherwise
    bool has_material() const;

    /// Set material
    ///
    /// @param name Material name
    /// @param density Density [g/cm^3]
    void set_material(const std::string & name, double density = 0.);

    /// Get material
    ///
    /// @return Material name
    const std::string & material() const;

    /// Get color
    Color color() const;

    /// Set color
    void set_color(const Color & color);

    /// Compute the length of the edge
    ///
    /// @return Length of the edge
    double length() const;

    /// Compute the area of the face
    ///
    /// @return Area of the face
    double area() const;

    /// Compute the volume of the shape
    ///
    /// @return Volume of the shape
    double volume() const;

    /// Return density
    ///
    /// @return Density [g/cm^3]
    double density() const;

    operator const TopoDS_Shape &() const;

protected:
    void assign_color();

private:
    void remove_degenerated_edges();
    void remove_small_edges(double tolerance);
    void repair_faces();
    void fix_degenerated();
    void fix_small_edges(double tolerance);
    void fix_small_wires(double tolerance);
    void fix_small_faces(double tolerance);
    void sew_faces(double tolerance);
    void make_solids(double tolerance);

    int dim_;
    TopoDS_Shape shape_;
    int id_;
    /// Color of this shape
    Color clr_;
    /// Material name
    std::string material_name_;
    /// Material description
    std::string material_description_;
    /// Density
    double density_;
};

} // namespace krado
