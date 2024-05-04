#pragma once

#include "TopoDS_Shape.hxx"

namespace krado::geo {

class Shape {
public:
    Shape(const TopoDS_Shape & shape);

    void clean();
    void heal(double tolerance);
    void scale(double scale_factor);

    operator const TopoDS_Shape &() const;

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

    TopoDS_Shape shape;
};

} // namespace krado::geo
