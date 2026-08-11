// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include "krado/color.h"
#include "TopoDS_Shape.hxx"

namespace krado {

class GeomShape {
public:
    explicit GeomShape(const TopoDS_Shape & shape);
    virtual ~GeomShape() = default;

    [[nodiscard]] virtual int dim() const;

    /// Get shape ID
    [[nodiscard]] int id() const;

    void clean();

    void scale(double scale_factor);

    /// Get shape name
    ///
    /// @return Shape name
    [[nodiscard]] std::string name() const;

    /// Set shape name
    ///
    /// @param name New shape name
    void set_name(const std::string & name);

    /// Query if this shape has material assigned to it
    ///
    /// @return `true` if material is assigned, `false` otherwise
    [[nodiscard]] bool has_material() const;

    /// Set material
    ///
    /// @param name Material name
    /// @param density Density [g/cm^3]
    void set_material(const std::string & name,
                      const std::string & description = "",
                      double density = 0.);

    /// Get material description
    ///
    /// @return Material description
    [[nodiscard]] const std::string & material_description() const;

    /// Get material
    ///
    /// @return Material name
    [[nodiscard]] const std::string & material() const;

    /// Get color
    [[nodiscard]] Color color() const;

    /// Set color
    void set_color(const Color & color);

    /// Compute the length of the edge
    ///
    /// @return Length of the edge
    [[nodiscard]] double length() const;

    /// Compute the area of the face
    ///
    /// @return Area of the face
    [[nodiscard]] double area() const;

    /// Compute the volume of the shape
    ///
    /// @return Volume of the shape
    [[nodiscard]] double volume() const;

    /// Return density
    ///
    /// @return Density [g/cm^3]
    [[nodiscard]] double density() const;

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

    /// Shape ID
    ShapeID id_ = -1;
    ///
    TopoDS_Shape shape_;
    /// Shape name
    std::string name_;
    /// Color of this shape
    Color clr_;
    /// Material name
    std::string material_name_;
    /// Material description
    std::string material_description_;
    /// Density
    double density_ = 0.;

    friend class GeomModel;
    friend class GeomVertex;
    friend class GeomCurve;
    friend class GeomSurface;
    friend class GeomShell;
    friend class GeomVolume;
};

} // namespace krado
