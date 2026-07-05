// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <pybind11/numpy.h>
#include "krado/arc_of_circle.h"
#include "krado/axis1.h"
#include "krado/axis2.h"
#include "krado/box.h"
#include "krado/bounding_box_3d.h"
#include "krado/scheme/bamg.h"
#include "krado/scheme/bias.h"
#include "krado/scheme/curvature.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/pinpoint.h"
#include "krado/scheme/size.h"
#include "krado/scheme/structured.h"
#include "krado/scheme/tricircle.h"
#include "krado/scheme/fan.h"
#include "krado/scheme/trisurf.h"
#include "krado/line.h"
#include "krado/circle.h"
#include "krado/circumscribed_polygon.h"
#include "krado/cone.h"
#include "krado/color.h"
#include "krado/color_map.h"
#include "krado/cylinder.h"
#include "krado/helix.h"
#include "krado/iges_file.h"
#include "krado/inscribed_polygon.h"
#include "krado/sphere.h"
#include "krado/spline.h"
#include "krado/classifier.h"
#include "krado/dagmc_file.h"
#include "krado/extrude.h"
#include "krado/exodusii_file.h"
#include "krado/meshable.h"
#include "krado/ops.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/geom_shape.h"
#include "krado/geom_shell.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"
#include "krado/mesh.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/linear_pattern.h"
#include "krado/circular_pattern.h"
#include "krado/hexagonal_pattern.h"
#include "krado/element.h"
#include "krado/plane.h"
#include "krado/point.h"
#include "krado/polygon.h"
#include "krado/wire.h"
#include "krado/tetrahedralize.h"
#include "krado/transform.h"
#include "krado/vector.h"
#include "krado/io.h"
#include "krado/log.h"
#include "krado/timer.h"
#include <fmt/core.h>

namespace py = pybind11;
using namespace krado;

PYBIND11_DECLARE_HOLDER_TYPE(T, krado::Ptr<T>);

class PyMeshVertexAbstract : public MeshVertexAbstract {
public:
    using MeshVertexAbstract::MeshVertexAbstract;

    Point
    point() const override
    {
        PYBIND11_OVERRIDE_PURE(Point, MeshVertexAbstract, point);
    }
};

void
py_log_log(int level, const std::string & msg)
{
    Log::log(level, "{}", msg);
}

void
py_log_info(int level, const std::string & msg)
{
    Log::info(level, "{}", msg);
}

void
py_log_warn(int level, const std::string & msg)
{
    Log::warn(level, "{}", msg);
}

void
py_log_error(int level, const std::string & msg)
{
    Log::error(level, "{}", msg);
}

void
py_log_trace(int level, const std::string & msg)
{
    Log::trace(level, "{}", msg);
}

void
py_log_debug(int level, const std::string & msg)
{
    Log::debug(level, "{}", msg);
}

auto make_span_view = [](auto & self, auto span_getter) {
    // Invoke the getter function pointer to get our span instance
    auto s = (self.*span_getter)();

    // Deduce the primitive element type (e.g., int, float, double)
    using ElementType = typename decltype(s)::element_type;

    // Standard anchoring capsule to tie NumPy's life to 'self'
    py::capsule base(&self, [](void *) {});

    return py::array_t<ElementType>({ s.size() }, { sizeof(ElementType) }, s.data(), base);
};

PYBIND11_MODULE(krado, m)
{
    m.doc() = "pybind11 plugin for krado";
    m.attr("__version__") = KRADO_VERSION;

    // clang-format off

    py::class_<Color>(m, "Color")
        .def(py::init<>())
        .def(py::init<int, int, int>(),
            py::arg("red"), py::arg("green"), py::arg("blue"))
        .def("red", &Color::red)
        .def("redF", &Color::redF)
        .def("green", &Color::green)
        .def("greenF", &Color::greenF)
        .def("blue", &Color::blue)
        .def("blueF", &Color::blueF)
    ;

    py::class_<ColorMap>(m, "ColorMap")
        .def_readonly_static("black", &ColorMap::black)
        .def_readonly_static("blue", &ColorMap::blue)
        .def_readonly_static("red", &ColorMap::red)

        .def_readonly_static("medium_blue", &ColorMap::medium_blue)
        .def_readonly_static("medium_grey", &ColorMap::medium_grey)
        .def_readonly_static("dark_blue", &ColorMap::dark_blue)
        .def_readonly_static("light_grey", &ColorMap::light_grey)
        .def_readonly_static("light_blue", &ColorMap::light_blue)
        .def_readonly_static("orange", &ColorMap::orange)
        .def_readonly_static("dark_grey", &ColorMap::dark_grey)
        .def_readonly_static("yellow", &ColorMap::yellow)

        .def_readonly_static("grey1", &ColorMap::grey1)
        .def_readonly_static("grey2", &ColorMap::grey2)
        .def_readonly_static("grey3", &ColorMap::grey3)
        .def_readonly_static("grey4", &ColorMap::grey4)
        .def_readonly_static("grey5", &ColorMap::grey5)
        .def_readonly_static("grey6", &ColorMap::grey6)
        .def_readonly_static("grey7", &ColorMap::grey7)

        .def_readonly_static("gold1", &ColorMap::gold1)
        .def_readonly_static("gold2", &ColorMap::gold2)
        .def_readonly_static("gold3", &ColorMap::gold3)
        .def_readonly_static("gold4", &ColorMap::gold4)
        .def_readonly_static("gold5", &ColorMap::gold5)
        .def_readonly_static("gold6", &ColorMap::gold6)
        .def_readonly_static("gold7", &ColorMap::gold7)

        .def_readonly_static("silver1", &ColorMap::silver1)
        .def_readonly_static("silver2", &ColorMap::silver2)
        .def_readonly_static("silver3", &ColorMap::silver3)
        .def_readonly_static("silver4", &ColorMap::silver4)
        .def_readonly_static("silver5", &ColorMap::silver5)
        .def_readonly_static("silver6", &ColorMap::silver6)
        .def_readonly_static("silver7", &ColorMap::silver7)

        .def_readonly_static("red1", &ColorMap::red1)
        .def_readonly_static("red2", &ColorMap::red2)
        .def_readonly_static("red3", &ColorMap::red3)
        .def_readonly_static("red4", &ColorMap::red4)
        .def_readonly_static("red5", &ColorMap::red5)
        .def_readonly_static("red6", &ColorMap::red6)
        .def_readonly_static("red7", &ColorMap::red7)

        .def_readonly_static("orange1", &ColorMap::orange1)
        .def_readonly_static("orange2", &ColorMap::orange2)
        .def_readonly_static("orange3", &ColorMap::orange3)
        .def_readonly_static("orange4", &ColorMap::orange4)
        .def_readonly_static("orange5", &ColorMap::orange5)
        .def_readonly_static("orange6", &ColorMap::orange6)
        .def_readonly_static("orange7", &ColorMap::orange7)

        .def_readonly_static("yellow1", &ColorMap::yellow1)
        .def_readonly_static("yellow2", &ColorMap::yellow2)
        .def_readonly_static("yellow3", &ColorMap::yellow3)
        .def_readonly_static("yellow4", &ColorMap::yellow4)
        .def_readonly_static("yellow5", &ColorMap::yellow5)
        .def_readonly_static("yellow6", &ColorMap::yellow6)
        .def_readonly_static("yellow7", &ColorMap::yellow7)

        .def_readonly_static("green1", &ColorMap::green1)
        .def_readonly_static("green2", &ColorMap::green2)
        .def_readonly_static("green3", &ColorMap::green3)
        .def_readonly_static("green4", &ColorMap::green4)
        .def_readonly_static("green5", &ColorMap::green5)
        .def_readonly_static("green6", &ColorMap::green6)
        .def_readonly_static("green7", &ColorMap::green7)

        .def_readonly_static("teal1", &ColorMap::teal1)
        .def_readonly_static("teal2", &ColorMap::teal2)
        .def_readonly_static("teal3", &ColorMap::teal3)
        .def_readonly_static("teal4", &ColorMap::teal4)
        .def_readonly_static("teal5", &ColorMap::teal5)
        .def_readonly_static("teal6", &ColorMap::teal6)
        .def_readonly_static("teal7", &ColorMap::teal7)

        .def_readonly_static("blue1", &ColorMap::blue1)
        .def_readonly_static("blue2", &ColorMap::blue2)
        .def_readonly_static("blue3", &ColorMap::blue3)
        .def_readonly_static("blue4", &ColorMap::blue4)
        .def_readonly_static("blue5", &ColorMap::blue5)
        .def_readonly_static("blue6", &ColorMap::blue6)
        .def_readonly_static("blue7", &ColorMap::blue7)
    ;

    py::enum_<ElementType>(m, "ElementType")
        .value("POINT", ElementType::POINT)
        .value("LINE2", ElementType::LINE2)
        .value("TRI3", ElementType::TRI3)
        .value("QUAD4", ElementType::QUAD4)
        .value("TETRA4", ElementType::TETRA4)
        .value("PYRAMID5", ElementType::PYRAMID5)
        .value("PRISM6", ElementType::PRISM6)
        .value("HEX8", ElementType::HEX8)
        .export_values();

    py::class_<Axis1>(m, "Axis1")
        .def(py::init<const Point &, const Vector &>())
        .def("location", &Axis1::location)
        .def("direction", &Axis1::direction)
        .def("is_equal", &Axis1::is_equal)
    ;

    py::class_<Axis2>(m, "Axis2")
        .def(py::init<const Point &, const Vector &>())
        .def(py::init<const Point &, const Vector &, const Vector &>())
        .def("location", &Axis2::location)
        .def("x_direction", &Axis2::x_direction)
        .def("y_direction", &Axis2::y_direction)
        .def("direction", &Axis2::direction)
        .def("axis", &Axis2::axis)
    ;

    py::class_<STEPFile>(m, "STEPFile")
        .def(py::init<const std::string &>())
        .def("read", &STEPFile::read, py::return_value_policy::move)
        .def("write", &STEPFile::write)
    ;

    py::class_<IGESFile>(m, "IGESFile")
        .def(py::init<const std::string &>())
        .def("read", &IGESFile::read, py::return_value_policy::move)
        .def("write", &IGESFile::write)
    ;

    py::class_<Point>(m, "Point")
        .def(py::init<double, double, double>(), py::arg("x"), py::arg("y") = 0., py::arg("z") = 0.)
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def_readwrite("z", &Point::z)
        .def("is_equal", &Point::is_equal,
            py::arg("other"), py::arg("tol"))
        .def("distance", &Point::distance,
            py::arg("pt"))
        .def("mirror", static_cast<void (Point::*)(const Point &)>(&Point::mirror),
            py::arg("pt"))
        .def("mirror", static_cast<void (Point::*)(const Axis1 &)>(&Point::mirror),
            py::arg("ax1"))
        .def("mirror", static_cast<void (Point::*)(const Axis2 &)>(&Point::mirror),
            py::arg("ax2"))
        .def("mirrored", static_cast<Point (Point::*)(const Point &) const>(&Point::mirrored),
            py::arg("pt"))
        .def("mirrored", static_cast<Point (Point::*)(const Axis1 &) const>(&Point::mirrored),
            py::arg("ax1"))
        .def("mirrored", static_cast<Point (Point::*)(const Axis2 &) const>(&Point::mirrored),
            py::arg("ax2"))
        .def("rotate", &Point::rotate,
            py::arg("ax1"), py::arg("angle"))
        .def("rotated", &Point::rotated,
            py::arg("ax1"), py::arg("angle"))
        .def("scale", &Point::scale,
            py::arg("pt"), py::arg("s"))
        .def("scaled", &Point::scaled,
            py::arg("pt"), py::arg("s"))
        .def("translate", static_cast<void (Point::*)(const Vector &)>(&Point::translate),
            py::arg("vec"))
        .def("translate", static_cast<void (Point::*)(const Point &, const Point &)>(&Point::translate),
            py::arg("p1"), py::arg("p2"))
        .def("translated", static_cast<Point (Point::*)(const Vector &) const>(&Point::translated),
            py::arg("vec"))
        .def("translated", static_cast<Point (Point::*)(const Point &, const Point &) const>(&Point::translated),
            py::arg("p1"), py::arg("p2"))
        .def("__add__", [](const Point& a, const Point& b) {
            return a + b;
        }, py::is_operator())
        .def("__add__", [](const Point& p, const Vector& v) {
            return p + v;
        }, py::is_operator())
        .def("__add__", [](const Point& p, const Vector& v) {
            return p + v;
        }, py::is_operator())
        .def("__repr__", [](const Point &pt) {
            return "<Point x=" + std::to_string(pt.x) + " y=" + std::to_string(pt.y) + ", z=" + std::to_string(pt.z) + ">";
        })
    ;

    py::class_<Vector>(m, "Vector")
        .def(py::init<double, double, double>(), py::arg("x"), py::arg("y") = 0., py::arg("z") = 0.)
        .def(py::init<const Point &>())
        .def_readwrite("x", &Vector::x)
        .def_readwrite("y", &Vector::y)
        .def_readwrite("z", &Vector::z)
        .def("norm", &Vector::magnitude)
        .def("normalize", &Vector::normalize)
        .def("normalized", &Vector::normalized)
        .def("is_equal", &Vector::is_equal, py::arg("other"), py::arg("tol") = 1e-10)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(float() * py::self)
        .def(py::self * float())
        .def(-py::self)
        .def("__repr__", [](const Vector &pt) {
            return "<Vector x=" + std::to_string(pt.x) + " y=" + std::to_string(pt.y) + ", z=" + std::to_string(pt.z) + ">";
        })
    ;

    py::class_<Trsf>(m, "Trsf")
        .def(py::init<>())
        .def("scale", static_cast<Trsf &(Trsf::*)(double)>(&Trsf::scale))
        .def("scale", static_cast<Trsf &(Trsf::*)(double, double, double)>(&Trsf::scale))
        .def("translate", &Trsf::translate)
        .def("rotate_x", &Trsf::rotate_x)
        .def("rotate_y", &Trsf::rotate_y)
        .def("rotate_z", &Trsf::rotate_z)
        .def_static("scaled", static_cast<Trsf (*)(double)>(&Trsf::scaled))
        .def_static("scaled", static_cast<Trsf (*)(double, double, double)>(&Trsf::scaled))
        .def_static("translated", &Trsf::translated)
        .def_static("rotated_x", &Trsf::rotated_x)
        .def_static("rotated_y", &Trsf::rotated_y)
        .def_static("rotated_z", &Trsf::rotated_z)
        .def_static("identity", &Trsf::identity)
        .def(py::self * Point())
        .def(py::self * Trsf())
    ;

    py::class_<BoundingBox3D>(m, "BoundingBox3D")
        .def(py::init<>())
        .def(py::init<const Point &>())
        .def(py::init<double, double, double, double, double, double>())
        .def("empty", &BoundingBox3D::empty)
        .def("reset", &BoundingBox3D::reset)
        .def("scale", &BoundingBox3D::scale)
        .def("min", &BoundingBox3D::min)
        .def("max", &BoundingBox3D::max)
        .def("diag", &BoundingBox3D::diag)
        .def("make_cube", &BoundingBox3D::make_cube)
        .def("thicken", &BoundingBox3D::thicken)
        .def("contains", static_cast<bool (BoundingBox3D::*)(const BoundingBox3D &) const>(&BoundingBox3D::contains))
        .def("contains", static_cast<bool (BoundingBox3D::*)(Point) const>(&BoundingBox3D::contains))
        .def("contains", static_cast<bool (BoundingBox3D::*)(double, double, double) const>(&BoundingBox3D::contains))
        .def("transform", &BoundingBox3D::transform)
        .def("size", static_cast<std::array<double, 3> (BoundingBox3D::*)() const>(&BoundingBox3D::size))
        .def("size", static_cast<double (BoundingBox3D::*)(int) const>(&BoundingBox3D::size))
    ;

    py::class_<Element>(m, "Element")
        .def(py::init<ElementType, const std::vector<Index> &>())
        .def("type", py::overload_cast<>(&Element::type, py::const_))
        .def("num_vertices", &Element::num_vertices)
        .def("index", &Element::index)
        .def("indices", [=](Element & self) { return make_span_view(self, &Element::indices); })
    ;

    py::class_<GeomShape>(m, "GeomShape")
        .def("clean", &GeomShape::clean)
        .def("heal", &GeomShape::heal)
        .def("scale", &GeomShape::scale)
        .def("name", &GeomShape::name)
        .def("set_name", &GeomShape::set_name)
        .def("has_material", &GeomShape::has_material)
        .def("set_material", &GeomShape::set_material)
        .def("material_description", &GeomShape::material_description)
        .def("material", &GeomShape::material)
        .def("color", &GeomShape::color)
        .def("set_color", &GeomShape::set_color)
        .def("length", &GeomShape::length)
        .def("area", &GeomShape::area)
        .def("volume", &GeomShape::volume)
        .def("density", &GeomShape::density)
    ;

    py::class_<Wire, GeomShape>(m, "Wire")
        .def(py::init([](const std::vector<GeomCurve> & curves) {
                 return Wire::create(curves);
             }),
             py::arg("curves"))
        .def("length", &Wire::length)
    ;

    py::class_<Polygon, Wire>(m, "Polygon")
        .def(py::init([](const std::vector<Point> & points, bool closed) {
                 return Polygon::create(points, closed);
             }),
             py::arg("points"), py::arg("closed") = true)
    ;

    py::class_<InscribedPolygon, Polygon>(m, "InscribedPolygon")
        .def(py::init([](const Axis2 & ax2, double radius, int n_sides) {
                 return InscribedPolygon::create(ax2, radius, n_sides);
             }),
             py::arg("ax2"), py::arg("radius"), py::arg("n_sides"))
        .def(py::init([](const Axis2 & ax2, const Point & pt1, int n_sides) {
                 return InscribedPolygon::create(ax2, pt1, n_sides);
             }),
             py::arg("ax2"), py::arg("pt1"), py::arg("n_sides"))
    ;

    py::class_<CircumscribedPolygon, Polygon>(m, "CircumscribedPolygon")
        .def(py::init([](const Axis2 & ax2, double radius, int n_sides) {
                 return CircumscribedPolygon::create(ax2, radius, n_sides);
             }),
             py::arg("ax2"), py::arg("radius"), py::arg("n_sides"))
        .def(py::init([](const Axis2 & ax2, const Point & pt1, int n_sides) {
                 return CircumscribedPolygon::create(ax2, pt1, n_sides);
             }),
             py::arg("ax2"), py::arg("pt1"), py::arg("n_sides"))
    ;

    py::class_<GeomModel>(m, "GeomModel")
        .def(py::init<const GeomShape &>())
        .def("vertices", &GeomModel::vertices, py::return_value_policy::reference)
        .def("vertex", py::overload_cast<ShapeID>(&GeomModel::vertex))
        .def("curves", &GeomModel::curves, py::return_value_policy::reference)
        .def("curve", py::overload_cast<ShapeID>(&GeomModel::curve))
        .def("surfaces", &GeomModel::surfaces, py::return_value_policy::reference)
        .def("surface", py::overload_cast<ShapeID>(&GeomModel::surface))
        .def("volumes", &GeomModel::volumes, py::return_value_policy::reference)
        .def("volume", py::overload_cast<ShapeID>(&GeomModel::volume))
        .def("mesh_vertex", py::overload_cast<ShapeID>(&GeomModel::mesh_vertex))
        .def("mesh_curve", py::overload_cast<ShapeID>(&GeomModel::mesh_curve))
        .def("mesh_surface", py::overload_cast<ShapeID>(&GeomModel::mesh_surface))
        .def("mesh_volume", py::overload_cast<ShapeID>(&GeomModel::mesh_volume))
        .def("set_block_name", &GeomModel::set_block_name)
        .def("block_name", &GeomModel::block_name)
        .def("set_side_set_name", &GeomModel::set_side_set_name)
        .def("side_set_name", &GeomModel::side_set_name)
        .def("set_node_set_name", &GeomModel::set_node_set_name)
        .def("node_set_name", &GeomModel::node_set_name)
    ;

    py::class_<GeomVertex, GeomShape>(m, "GeomVertex")
        .def(py::init<const TopoDS_Vertex &>())
        .def("x", &GeomVertex::x)
        .def("y", &GeomVertex::y)
        .def("z", &GeomVertex::z)
        .def("point", &GeomVertex::point)
        .def("is_null", &GeomVertex::is_null)
    ;

    py::class_<GeomCurve, GeomShape>(m, "GeomCurve")
        .def(py::init<const TopoDS_Edge &>())
        .def("type", &GeomCurve::type)
        .def("is_degenerated", &GeomCurve::is_degenerated)
        .def("point", &GeomCurve::point)
        .def("d1", &GeomCurve::d1)
        .def("curvature", &GeomCurve::curvature)
        .def("length", &GeomCurve::length)
        .def("param_range", &GeomCurve::param_range)
        .def("first_vertex", &GeomCurve::first_vertex)
        .def("last_vertex", &GeomCurve::last_vertex)
        .def("parameter_from_point", &GeomCurve::parameter_from_point)
        .def("nearest_point", &GeomCurve::nearest_point)
        .def("contains_point", &GeomCurve::contains_point)
        .def("is_seam", &GeomCurve::is_seam)
    ;

    py::class_<Line, GeomCurve>(m, "Line")
        .def(py::init([](Point pt1, Point pt2) {
                 return Line::create(pt1, pt2);
             }),
             py::arg("pt1"), py::arg("pt2"))
    ;

    py::class_<Circle, GeomCurve>(m, "Circle")
        .def(py::init([](const Axis2 & origin, double radius) {
                 return Circle::create(origin, radius);
             }),
             py::arg("origin"), py::arg("radius"))
        .def(py::init([](const Point & center, double radius, const Vector & normal) {
                 return Circle::create(center, radius, normal);
             }),
             py::arg("center"), py::arg("radius"), py::arg("normal") = Vector(0., 0., 1.))
        .def(py::init([](const Point & center, const Point & pt, const Vector & normal) {
                 return Circle::create(center, pt, normal);
             }),
             py::arg("center"), py::arg("pt"), py::arg("normal") = Vector(0., 0., 1.))
        .def(py::init([](const Point & pt1, const Point & pt2, const Point & pt3) {
                 return Circle::create(pt1, pt2, pt3);
             }),
             py::arg("pt1"), py::arg("pt2"), py::arg("pt3"))
        .def("area", &Circle::area)
        .def("radius", &Circle::radius)
        .def("location", &Circle::location)
    ;

    py::class_<ArcOfCircle, GeomCurve>(m, "ArcOfCircle")
        .def(py::init([](const Point & pt1, const Point & pt2, const Point & pt3) {
                 return ArcOfCircle::create(pt1, pt2, pt3);
             }),
             py::arg("pt1"), py::arg("pt2"), py::arg("pt3"))
        .def(py::init([](const Circle & circ, const Point & pt1, const Point & pt2, bool sense) {
                 return ArcOfCircle::create(circ, pt1, pt2, sense);
             }),
             py::arg("circ"), py::arg("pt1"), py::arg("pt2"), py::arg("sense") = true)
        .def(py::init([](const Point & pt1, const Vector & tangent, const Point & pt2) {
                 return ArcOfCircle::create(pt1, tangent, pt2);
             }),
             py::arg("pt1"), py::arg("tangent"), py::arg("pt2"))
        .def("start_point", &ArcOfCircle::start_point)
        .def("end_point", &ArcOfCircle::end_point)
    ;

    py::class_<Spline, GeomCurve>(m, "Spline")
        .def(py::init([](const std::vector<Point> & points) { return Spline::create(points); }),
             py::arg("points"))
        .def(py::init([](const std::vector<Point> & points, const Vector & initial_tg,
                         const Vector & final_tg) {
                 return Spline::create(points, initial_tg, final_tg);
             }),
             py::arg("points"), py::arg("initial_tg"), py::arg("final_tg"))
    ;

    py::class_<Helix, GeomCurve>(m, "Helix")
        .def(py::init([](const Axis2 & ax2, double radius, double height, double turns,
                         double start_angle) {
                 return Helix::create(ax2, radius, height, turns, start_angle);
             }),
             py::arg("ax2"), py::arg("radius"), py::arg("height"), py::arg("turns"),
             py::arg("start_angle") = 0.)
    ;

    py::class_<GeomSurface, GeomShape>(m, "GeomSurface")
        .def(py::init([](const Wire & wire) {
                return GeomSurface::create(wire);
             }),
             py::arg("wire"))
        .def("point", &GeomSurface::point)
        .def("normal", &GeomSurface::normal)
        .def("d1", &GeomSurface::d1)
        .def("area", &GeomSurface::area)
        .def("param_range", &GeomSurface::param_range)
        .def("curves", &GeomSurface::curves)
        .def("parameter_from_point", &GeomSurface::parameter_from_point)
        .def("nearest_point", &GeomSurface::nearest_point)
        .def("contains_point", &GeomSurface::contains_point)
    ;

    py::class_<GeomVolume, GeomShape>(m, "GeomVolume")
        .def(py::init<const TopoDS_Solid &>())
        .def("volume", &GeomVolume::volume)
        .def("surfaces", &GeomVolume::surfaces)
    ;

    py::class_<Sphere, GeomVolume>(m, "Sphere")
        .def(py::init([](const Point & center, double radius) { return Sphere::create(center, radius); }),
             py::arg("center"), py::arg("radius"))
    ;

    py::class_<Cylinder, GeomVolume>(m, "Cylinder")
        .def(py::init([](const Axis2 & location, double radius, double height) {
                 return Cylinder::create(location, radius, height);
             }),
             py::arg("location"), py::arg("radius"), py::arg("height"))
    ;

    py::class_<Cone, GeomVolume>(m, "Cone")
        .def(py::init([](const Axis2 & location, double radius1, double radius2, double height) {
                 return Cone::create(location, radius1, radius2, height);
             }),
             py::arg("location"), py::arg("radius1"), py::arg("radius2"), py::arg("height"))
    ;

    py::class_<Box, GeomVolume>(m, "Box")
        .def(py::init([](const Point & pt1, const Point & pt2) { return Box::create(pt1, pt2); }))
    ;

    py::class_<Mesh, Ptr<Mesh>>(m, "Mesh")
        .def(py::init<>())
        .def(py::init<std::vector<Point>, std::vector<Element>>())
        .def("num_points", &Mesh::num_points)
        // .def("points", /* TODO */)
        .def("point", &Mesh::point, py::return_value_policy::reference)
        // .def("elements", /* TODO */)
        .def("num_elements", &Mesh::num_elements)
        .def("element", &Mesh::element, py::return_value_policy::reference)
        .def("scale", static_cast<Mesh &(Mesh::*)(double)>(&Mesh::scale))
        .def("scaled", static_cast<Ptr<Mesh> (Mesh::*)(double) const>(&Mesh::scaled))
        .def("scale", static_cast<Mesh &(Mesh::*)(double, double, double)>(&Mesh::scale),
            py::arg("factor_x"), py::arg("factor_y"), py::arg("factor_z") = 1.)
        .def("scaled", static_cast<Ptr<Mesh> (Mesh::*)(double, double, double) const>(&Mesh::scaled),
            py::arg("factor_x"), py::arg("factor_y"), py::arg("factor_z") = 1.)
        .def("translated", &Mesh::translated, py::arg("tx"), py::arg("ty") = 0., py::arg("tz") = 0.)
        .def("translate", &Mesh::translate, py::arg("tx"), py::arg("ty") = 0., py::arg("tz") = 0.)
        .def("transformed", &Mesh::transformed)
        .def("transform", &Mesh::transform)
        .def("add", &Mesh::add)
        .def("remove_duplicate_points", &Mesh::remove_duplicate_points)
        .def("duplicate", &Mesh::duplicate)

        .def("set_cell_set", &Mesh::set_cell_set)
        .def("set_cell_set_name", &Mesh::set_cell_set_name)
        .def("cell_set_name", &Mesh::cell_set_name)
        .def("cell_set_ids", &Mesh::cell_set_ids)
        // .def("cell_set", /* TODO */)
        .def("remove_cell_sets", &Mesh::remove_cell_sets)

        .def("set_side_set", &Mesh::set_side_set)
        .def("set_side_set_name", &Mesh::set_side_set_name)
        .def("side_set_name", &Mesh::side_set_name)
        .def("side_set_ids", &Mesh::side_set_ids)
        // .def("side_set", /* TODO */)
        .def("remove_side_sets", &Mesh::remove_side_sets)

        .def("set_node_set", &Mesh::set_node_set)
        .def("set_node_set_name", &Mesh::set_node_set_name)
        .def("node_set_name", &Mesh::node_set_name)
        .def("node_set_ids", &Mesh::node_set_ids)
        // .def("node_set", /* TODO */)
        .def("remove_node_sets", &Mesh::remove_node_sets)

        .def("remap_block_ids", &Mesh::remap_block_ids)
        .def("element_type", &Mesh::element_type)
        .def("set_up", &Mesh::set_up)
        .def("boundary_edges", &Mesh::boundary_edges)
        .def("boundary_faces", &Mesh::boundary_faces)
        .def("compute_centroid", &Mesh::compute_centroid)
        .def("outward_normal", &Mesh::outward_normal)

        .def("create_side_set", [](Mesh & self, Marker id, const std::vector<Index> & indices) {
            auto sset = create_side_set(self, indices);
            self.set_side_set(id, sset);
        })
    ;

    py::class_<Meshable, Ptr<Meshable>>(m, "Meshable")
        .def(py::init<>())
        .def("is_meshed", &Meshable::is_meshed)
        .def("set_marker", &Meshable::set_marker)
        .def("marker", [](const Meshable & self) -> py::object {
            if (self.marker().has_value()) {
                return py::cast(self.marker().value());
            } else {
                return py::none();
            }
        })
    ;

    py::class_<MeshElement>(m, "MeshElement")
        .def(py::init<ElementType, const std::vector<Ptr<MeshVertexAbstract>> &>())
        .def("type", &MeshElement::type)
        .def("num_vertices", &MeshElement::num_vertices)
        .def("vertex", &MeshElement::vertex)
        // .def("vertices", /* TODO */)
        .def("get_edge", &MeshElement::get_edge)
        .def("swap_vertices", &MeshElement::swap_vertices)
    ;

    py::class_<MeshVertexAbstract, PyMeshVertexAbstract, Ptr<MeshVertexAbstract>>(m, "MeshVertexAbstract")
        .def("point", &MeshVertexAbstract::point)
    ;

    py::class_<MeshVertex, MeshVertexAbstract, Ptr<MeshVertex>>(m, "MeshVertex")
        .def(py::init<ShapeID, const GeomVertex &>())
        .def("id", &MeshVertex::id)
        .def("point", &MeshVertex::point)
        .def("mesh_size", &MeshVertex::mesh_size)
        .def("set_mesh_size", &MeshVertex::set_mesh_size)
    ;

    py::class_<MeshCurveVertex, MeshVertexAbstract, Ptr<MeshCurveVertex>>(m, "MeshCurveVertex")
        .def(py::init<const GeomCurve &, double>())
        .def("parameter", &MeshCurveVertex::parameter)
        .def("point", &MeshCurveVertex::point)
    ;

    py::class_<MeshSurfaceVertex, MeshVertexAbstract, Ptr<MeshSurfaceVertex>>(m, "MeshSurfaceVertex")
        .def(py::init<const GeomSurface &, double, double>())
        .def(py::init<const GeomSurface &, UVParam>())
        .def("parameter", &MeshSurfaceVertex::parameter)
        .def("point", &MeshSurfaceVertex::point)
    ;

    py::class_<MeshCurve, Meshable, Ptr<MeshCurve>>(m, "MeshCurve")
        .def(py::init<ShapeID, const GeomCurve &, Ptr<MeshVertex>, Ptr<MeshVertex>>())
        .def("id", &MeshCurve::id)
        // .def("bounding_vertices", /* TODO */)
        // .def("curve_vertices", /* TODO */)
        .def("add_vertex", py::overload_cast<Ptr<MeshCurveVertex>>(&MeshCurve::add_vertex))
        .def("add_segment", &MeshCurve::add_segment)
        // .def("segments", /* TODO */)
        .def("is_mesh_degenerated", &MeshCurve::is_mesh_degenerated)
        .def("mesh_size_at_param", &MeshCurve::mesh_size_at_param)
        .def("mesh_size", &MeshCurve::mesh_size)
        .def("set_mesh_size", &MeshCurve::set_mesh_size)
        .def("set_scheme",
             [](MeshCurve & self, const std::string & name, py::kwargs kwargs) {
                 if (name == "equal") {
                     SchemeEqual::Options opts;
                     if (kwargs.contains("intervals"))
                         opts.intervals = kwargs["intervals"].cast<int>();
                     self.set_scheme<SchemeEqual>(opts);
                 }
                 else if (name == "bias") {
                     SchemeBias::Options opts;
                     if (kwargs.contains("intervals"))
                         opts.intervals = kwargs["intervals"].cast<int>();
                     if (kwargs.contains("factor"))
                         opts.factor = kwargs["factor"].cast<double>();
                     self.set_scheme<SchemeBias>(opts);
                 }
                 else if (name == "curvature") {
                     SchemeCurvature::Options opts;
                     if (kwargs.contains("min_size"))
                         opts.min_size = kwargs["min_size"].cast<double>();
                     if (kwargs.contains("max_size"))
                         opts.max_size = kwargs["max_size"].cast<double>();
                     if (kwargs.contains("deflection"))
                         opts.deflection = kwargs["deflection"].cast<double>();
                     self.set_scheme<SchemeCurvature>(opts);
                 }
                 else if (name == "pinpoint") {
                     SchemePinpoint::Options opts;
                     if (kwargs.contains("positions"))
                         opts.positions = kwargs["positions"].cast<std::vector<double>>();
                     self.set_scheme<SchemePinpoint>(opts);
                 }
                 else if (name == "size") {
                     SchemeSize::Options opts;
                     if (kwargs.contains("size"))
                         opts.size = kwargs["size"].cast<double>();
                     self.set_scheme<SchemeSize>(opts);
                 }
             },
             "Set the meshing scheme for the curve.")
    ;

    py::class_<MeshSurface, Meshable, Ptr<MeshSurface>>(m, "MeshSurface")
        .def(py::init<ShapeID, const GeomSurface &, const std::vector<Ptr<MeshCurve>> &>())
        .def("id", &MeshSurface::id)
        // .def("curves", /* TODO */)
        // .def("surface_vertices", /* TODO */)
        // .def("triangles", /* TODO */)
        // .def("quadrangles", /* TODO */)
        .def("add_vertex", py::overload_cast<Ptr<MeshSurfaceVertex>>(&MeshSurface::add_vertex))
        .def("add_triangle", &MeshSurface::add_triangle)
        .def("add_quadrangle", &MeshSurface::add_quadrangle)
        .def("add_element", &MeshSurface::add_element)
        .def("reserve_mem", &MeshSurface::reserve_mem)
        .def("set_triangles", &MeshSurface::set_triangles)
        // .def("elements", /* TODO */)
        .def("remove_all_triangles", &MeshSurface::remove_all_triangles)
        .def("delete_mesh", &MeshSurface::delete_mesh)
        .def("quads_to_tris", [](MeshSurface & self, py::kwargs kwargs) {
                QuadSplitMode split = QuadSplitMode::SPLIT2;
                if (kwargs.contains("split")) {
                    auto n = kwargs["split"].cast<int>();
                    if (n == 2)
                        split = QuadSplitMode::SPLIT2;
                    else if (n == 4)
                        split = QuadSplitMode::SPLIT4;
                    else
                        throw Exception("Unsupported split");
                }
                self.quads_to_tris(split);
            })
        .def("set_scheme",
             [](MeshSurface & self, const std::string & name, py::kwargs kwargs) {
                 if (name == "bamg") {
                     SchemeBAMG::Options opts;
                     if (kwargs.contains("max_area"))
                         opts.max_area = kwargs["max_area"].cast<double>();
                     self.set_scheme<SchemeBAMG>(opts);
                 }
                 else if (name == "structured") {
                     SchemeStructured::Options opts;
                     self.set_scheme<SchemeStructured>(opts);
                 }
                 else if (name == "tricircle") {
                     SchemeTriCircle::Options opts;
                     if (kwargs.contains("radial_intervals"))
                         opts.radial_intervals = kwargs["radial_intervals"].cast<int>();
                     if (kwargs.contains("symmetry_type")) {
                         auto st = kwargs["symmetry_type"].cast<std::string>();
                         if (st == "quadrant")
                             opts.symmetry_type = SchemeTriCircle::SymmetryType::QUADRANT;
                         else if (st == "hexagonal")
                             opts.symmetry_type = SchemeTriCircle::SymmetryType::HEXAGONAL;
                     }
                     self.set_scheme<SchemeTriCircle>(opts);
                 }
                 else if (name == "fan") {
                     SchemeFan::Options opts;
                     self.set_scheme<SchemeFan>(opts);
                 }
             },
             "Set the meshing scheme for the surface.")
    ;

    py::class_<MeshVolume, Meshable, Ptr<MeshVolume>>(m, "MeshVolume")
        .def(py::init<ShapeID, const GeomVolume &, const std::vector<Ptr<MeshSurface>> &>())
        .def("id", &MeshVolume::id)
        // .def("surfaces", /* TODO */)
        .def("set_scheme",
             [](MeshVolume & self, const std::string & name, py::kwargs kwargs) {
                 if (name == "trisurf") {
                     SchemeTriSurf::Options opts;
                     if (kwargs.contains("linear_deflection"))
                         opts.linear_deflection = kwargs["linear_deflection"].cast<double>();
                     if (kwargs.contains("angular_deflection"))
                         opts.angular_deflection = kwargs["angular_deflection"].cast<double>();
                     if (kwargs.contains("is_relative"))
                         opts.is_relative = kwargs["is_relative"].cast<bool>();
                     self.set_scheme<SchemeTriSurf>(opts);
                 }
             },
             "Set the meshing scheme for the volume.")
        // .def("tetrahedra", /* TODO */)
    ;

    py::class_<ExodusIIFile>(m, "ExodusIIFile")
        .def(py::init<const std::filesystem::path &>())
        .def("read", &ExodusIIFile::read)
        .def("write", py::overload_cast<Ptr<const Mesh>>(&ExodusIIFile::write))
        .def("write", py::overload_cast<const GeomModel &>(&ExodusIIFile::write))
    ;

    py::class_<DAGMCFile>(m, "DAGMCFile")
        .def(py::init<const std::string &>())
        .def("write", &DAGMCFile::write)
    ;

    py::class_<Pattern>(m, "Pattern")
        .def("points", &Pattern::points)
    ;

    py::class_<LinearPattern, Pattern>(m, "LinearPattern")
        .def(py::init<const Axis2 &, int, double>())
        .def(py::init<const Axis2 &, int, int, double, double>())
        .def("nx", &LinearPattern::nx)
        .def("ny", &LinearPattern::ny)
        .def("dx", &LinearPattern::dx)
        .def("dy", &LinearPattern::dy)
    ;

    py::class_<CircularPattern, Pattern>(m, "CircularPattern")
        .def(py::init<const Axis2 &, double, int, double>(),
            py::arg("center"), py::arg("radius"), py::arg("divisions"), py::arg("start_angle") = 0.)
        .def("radius", &CircularPattern::radius)
    ;

    py::class_<HexagonalPattern, Pattern>(m, "HexagonalPattern")
        .def(py::init<const Axis2 &, double, int>())
        .def("flat_to_flat", &HexagonalPattern::flat_to_flat)
    ;

    py::class_<SolidClassifier>(m, "SolidClassifier")
        .def(py::init<const GeomShape &>())
        .def("inside", &SolidClassifier::inside)
        .def("outside", &SolidClassifier::outside)
    ;

    // extrude.h

    m.def("extrude", static_cast<Ptr<Mesh>(*)(const Mesh &, Vector, int, double)>(&extrude));
    m.def("extrude", static_cast<Ptr<Mesh>(*)(const Mesh &, Vector, const std::vector<double> &)>(&extrude));

    // ops.h

    m.def("translate", py::overload_cast<const GeomShape &, Vector>(&translate),
        py::arg("shape"), py::arg("vector"));
    m.def("translate", py::overload_cast<const GeomShape &, Point, Point>(&translate),
        py::arg("shape"), py::arg("pt1"), py::arg("pt2"));

    m.def("scale", py::overload_cast<const GeomShape &, double>(&scale),
        py::arg("shape"), py::arg("scale_factor"));

    m.def("mirror", py::overload_cast<const GeomShape &, const Axis1 &>(&mirror),
        py::arg("shape"), py::arg("axis1"));

    // TODO:split curve

    m.def("imprint", py::overload_cast<const GeomSurface &, const GeomCurve &>(&imprint),
        py::arg("surface"), py::arg("curve"));
    m.def("imprint", py::overload_cast<const GeomVolume &, const GeomCurve &>(&imprint),
        py::arg("volume"), py::arg("curve"));
    m.def("imprint", py::overload_cast<const GeomVolume &, const GeomVolume &>(&imprint),
        py::arg("volume"), py::arg("imp_volume"));

    m.def("compute_volume", [](const Mesh & mesh) {
        auto vols = compute_volume(mesh);
        py::dict py_vols;
        for (const auto & [marker, volume] : vols) {
            auto name = mesh.cell_set_name(marker);
            if (name.has_value())
                py_vols[py::cast(name.value())] = volume;
            else
                py_vols[py::cast(marker)] = volume;
        }
        return py_vols;
    });
    m.def("combine", [](const std::vector<Ptr<Mesh>>& parts) -> Ptr<Mesh> {
            return combine(parts);
    });

    m.def("fuse", py::overload_cast<const GeomShape &, const GeomShape &, bool>(&fuse),
        py::arg("shape"), py::arg("tool"), py::arg("simplify") = true);
    m.def("fuse", py::overload_cast<const std::vector<GeomShape> &, bool>(&fuse),
        py::arg("tools"), py::arg("simplify") = true);

    m.def("cut", py::overload_cast<const GeomShape &, const GeomShape &>(&cut),
        py::arg("shape"), py::arg("tool"));

    m.def("intersect", py::overload_cast<const GeomShape &, const GeomShape &>(&intersect),
         py::arg("shape"), py::arg("tool"));

    m.def("fillet", py::overload_cast<const GeomShape &, const std::vector<GeomCurve> &, double>(&fillet),
        py::arg("shape"), py::arg("curves"), py::arg("radius"));

    m.def("hollow", py::overload_cast<const GeomShape &, const std::vector<GeomSurface> &, double, double>(&hollow),
        py::arg("shape"), py::arg("faces_to_remove"), py::arg("thickness"), py::arg("tolerance"));

    m.def("extrude", py::overload_cast<const GeomShape &, Vector>(&extrude),
        py::arg("shape"), py::arg("vector"));

    m.def("revolve", py::overload_cast<const GeomShape &, const Axis1 &, double>(&revolve),
        py::arg("shape"), py::arg("axis1"), py::arg("angle") = 2. * M_PI);

    m.def("rotate", py::overload_cast<const GeomShape &, const Axis1 &, double>(&rotate),
        py::arg("shape"), py::arg("axis1"), py::arg("angle"));

    m.def("section", py::overload_cast<const GeomShape &, const Plane &>(&section),
        py::arg("shape"), py::arg("plane"));

    m.def("draft", py::overload_cast<const GeomShape &, const Plane &, const std::vector<GeomSurface> &, double>(&draft),
        py::arg("shape"), py::arg("pln"), py::arg("faces"), py::arg("angle"));

    m.def("hole", py::overload_cast<const GeomShape &, const Axis1 &, double>(&hole),
        py::arg("shape"), py::arg("axis"), py::arg("diameter"));
    m.def("hole", py::overload_cast<const GeomShape &, const Axis1 &, double, double>(&hole),
        py::arg("shape"), py::arg("axis"), py::arg("diameter"), py::arg("length"));

    m.def("sweep", py::overload_cast<const GeomShape &, const Wire &>(&sweep),
        py::arg("profile"), py::arg("spine"));

    m.def("sew", &sew,
         py::arg("faces"), py::arg("tolerance") = 1e-6);

    m.def("smooth", &smooth, py::arg("surface"), py::arg("iterations") = 1);

    // tetrahedralize.h

    m.def("tetrahedralize", &tetrahedralize);

    // io.h

    m.def("export_mesh", &IO::export_mesh, py::arg("file_name"), py::arg("mesh"));
    m.def("import_mesh", &IO::import_mesh, py::arg("file_name"));

    auto log = m.def_submodule("log", "Submodule for logging");
    log.def("set_verbosity", &Log::set_verbosity);
    log.def("log", &py_log_log, py::arg("level"), py::arg("msg"));
    log.def("info", &py_log_info, py::arg("level"), py::arg("msg"));
    log.def("warn", &py_log_warn, py::arg("level"), py::arg("msg"));
    log.def("error", &py_log_error, py::arg("level"), py::arg("msg"));
    log.def("trace", &py_log_trace, py::arg("level"), py::arg("msg"));
    log.def("debug", &py_log_debug, py::arg("level"), py::arg("msg"));
    // clang-format on
}
