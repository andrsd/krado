// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "krado/bounding_box_3d.h"
#include "krado/config.h"
#include "krado/exodusii_file.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/geom_shape.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"
#include "krado/mesh.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/element.h"
#include "krado/scheme.h"
#include "krado/point.h"
#include "krado/transform.h"
#include "krado/vector.h"
#include "krado/export.h"

namespace py = pybind11;
using namespace krado;

class PyMeshVertexAbstract : public MeshVertexAbstract {
public:
    using MeshVertexAbstract::MeshVertexAbstract;

    Point
    point() const override
    {
        PYBIND11_OVERRIDE_PURE(Point, MeshVertexAbstract, point);
    }
};

PYBIND11_MODULE(krado, m)
{
    m.doc() = "pybind11 plugin for krado";
    py::setattr(m, "version", py::str(KRADO_VERSION));

    // clang-format off
    py::class_<STEPFile>(m, "STEPFile")
        .def(py::init<const std::string &>())
        .def("load", &STEPFile::load, py::return_value_policy::move)
    ;

    py::class_<Point>(m, "Point")
        .def(py::init<double, double, double>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def_readwrite("z", &Point::z)
    ;

    py::class_<Vector>(m, "Vector")
        .def(py::init<double, double, double>())
        .def_readwrite("x", &Vector::x)
        .def_readwrite("y", &Vector::y)
        .def_readwrite("z", &Vector::z)
        .def("norm", &Vector::norm)
        .def("normalize", &Vector::normalize)
    ;

    py::class_<Trsf>(m, "Trsf")
        .def(py::init<>())
        .def_static("scale", static_cast<Trsf (*)(double)>(&Trsf::scale))
        .def_static("scale", static_cast<Trsf (*)(double, double, double)>(&Trsf::scale))
        .def_static("translate", &Trsf::translate)
        .def_static("rotate_x", &Trsf::rotate_x)
        .def_static("rotate_y", &Trsf::rotate_y)
        .def_static("rotate_z", &Trsf::rotate_z)
        .def_static("identity", &Trsf::identity)
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
        .def("contains", static_cast<bool (BoundingBox3D::*)(const BoundingBox3D &)>(&BoundingBox3D::contains))
        .def("contains", static_cast<bool (BoundingBox3D::*)(const Point &)>(&BoundingBox3D::contains))
        .def("contains", static_cast<bool (BoundingBox3D::*)(double, double, double)>(&BoundingBox3D::contains))
        .def("transform", &BoundingBox3D::transform)
        .def("size", static_cast<std::array<double, 3> (BoundingBox3D::*)() const>(&BoundingBox3D::size))
        .def("size", static_cast<double (BoundingBox3D::*)(int) const>(&BoundingBox3D::size))
    ;

    py::class_<Element>(m, "Element")
        .def(py::init<Element::Type, const std::vector<int> &, int>())
        .def("type", py::overload_cast<>(&Element::type, py::const_))
        .def("marker", &Element::marker)
        .def("num_vertices", &Element::num_vertices)
        .def("vertex_id", &Element::vertex_id)
        .def("ids", &Element::ids)
        .def("set_ids", &Element::set_ids)
    ;

    py::class_<GeomShape>(m, "GeomShape")
        .def(py::init<const TopoDS_Shape &>())
        .def("clean", &GeomShape::clean)
        .def("heal", &GeomShape::heal)
        .def("scale", &GeomShape::scale)
    ;

    py::class_<GeomModel>(m, "GeomModel")
        .def(py::init<const GeomShape &>())
        .def("vertices", &GeomModel::vertices, py::return_value_policy::reference)
        .def("vertex", &GeomModel::vertex, py::return_value_policy::reference)
        .def("curves", &GeomModel::curves, py::return_value_policy::reference)
        .def("curve", &GeomModel::curve, py::return_value_policy::reference)
        .def("surfaces", &GeomModel::surfaces, py::return_value_policy::reference)
        .def("surface", &GeomModel::surface, py::return_value_policy::reference)
        .def("volumes", &GeomModel::volumes, py::return_value_policy::reference)
        .def("volume", &GeomModel::volume, py::return_value_policy::reference)
    ;

    py::class_<GeomVertex>(m, "GeomVertex")
        .def(py::init<const TopoDS_Vertex &>())
        .def("x", &GeomVertex::x)
        .def("y", &GeomVertex::y)
        .def("z", &GeomVertex::z)
        .def("is_null", &GeomVertex::is_null)
    ;

    py::class_<GeomCurve>(m, "GeomCurve")
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
    ;

    py::class_<GeomSurface>(m, "GeomSurface")
        .def(py::init<const TopoDS_Face &>())
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

    py::class_<GeomVolume>(m, "GeomVolume")
        .def(py::init<const TopoDS_Solid &>())
        .def("volume", &GeomVolume::volume)
        .def("surfaces", &GeomVolume::surfaces)
    ;

    py::class_<Mesh>(m, "Mesh")
        .def(py::init<>())
        .def(py::init<const GeomModel &>())
        .def("vertex", py::overload_cast<int>(&Mesh::vertex), py::return_value_policy::reference)
        .def("curve", py::overload_cast<int>(&Mesh::curve), py::return_value_policy::reference)
        .def("surface", py::overload_cast<int>(&Mesh::surface), py::return_value_policy::reference)
        .def("volume", py::overload_cast<int>(&Mesh::volume), py::return_value_policy::reference)
        .def("mesh_vertex", py::overload_cast<int>(&Mesh::mesh_vertex))
        .def("mesh_curve", py::overload_cast<int>(&Mesh::mesh_curve))
        .def("mesh_surface", py::overload_cast<int>(&Mesh::mesh_surface))
        .def("mesh_volume", py::overload_cast<int>(&Mesh::mesh_volume))
        .def("points", &Mesh::points, py::return_value_policy::reference)
        .def("point", &Mesh::point, py::return_value_policy::reference)
        .def("elements", &Mesh::elements, py::return_value_policy::reference)
        .def("number_points", &Mesh::number_points)
        .def("build_elements", &Mesh::build_elements)
        .def("bounding_box", &Mesh::bounding_box)
        .def("scaled", static_cast<Mesh (Mesh::*)(double) const>(&Mesh::scaled))
        .def("scaled", static_cast<Mesh (Mesh::*)(double, double, double) const>(&Mesh::scaled))
        .def("translated", &Mesh::translated)
        .def("transformed", &Mesh::transformed)
        .def("add", &Mesh::add)
        .def("remove_duplicate_points", &Mesh::remove_duplicate_points)
        .def("compute_bounding_box", &Mesh::compute_bounding_box)
        .def("duplicate", &Mesh::duplicate)
    ;

    py::class_<MeshingParameters>(m, "MeshingParameters")
        .def(py::init<>())
        .def("set_scheme", &MeshVertex::set_scheme, py::return_value_policy::reference)
        .def("scheme", &MeshVertex::scheme, py::return_value_policy::reference)
        .def("set", &MeshVertex::set<int>, py::return_value_policy::reference)
        .def("set", &MeshVertex::set<double>, py::return_value_policy::reference)
        .def("set", &MeshVertex::set<std::string>, py::return_value_policy::reference)
        .def("get", &MeshVertex::get<int>)
        .def("get", &MeshVertex::get<double>)
        .def("get", &MeshVertex::get<std::string>)
    ;

    py::class_<MeshVertexAbstract, PyMeshVertexAbstract>(m, "MeshVertexAbstract")
        .def(py::init<>())
        .def("point", &MeshVertexAbstract::point)
    ;

    py::class_<MeshVertex, MeshVertexAbstract, MeshingParameters>(m, "MeshVertex")
        .def(py::init<const GeomVertex &>())
        .def("is_meshed", &MeshVertex::is_meshed)
    ;

    py::class_<MeshCurveVertex, MeshVertexAbstract>(m, "MeshCurveVertex")
        .def(py::init<const GeomCurve &, double>())
        .def("parameter", &MeshCurveVertex::parameter)
    ;

    py::class_<MeshSurfaceVertex, MeshVertexAbstract>(m, "MeshSurfaceVertex")
        .def(py::init<const GeomSurface &, double, double>())
        .def("parameter", &MeshSurfaceVertex::parameter)
    ;

    py::class_<MeshCurve, MeshingParameters>(m, "MeshCurve")
        .def(py::init<const GeomCurve &, MeshVertex *, MeshVertex *>())
        .def("all_vertices", &MeshCurve::all_vertices, py::return_value_policy::reference)
        .def("bounding_vertices", &MeshCurve::bounding_vertices, py::return_value_policy::reference)
        .def("curve_vertices", py::overload_cast<>(&MeshCurve::curve_vertices, py::const_), py::return_value_policy::reference)
        .def("segments", &MeshCurve::segments, py::return_value_policy::reference)
        .def("is_meshed", &MeshCurve::is_meshed)
    ;

    py::class_<MeshSurface, MeshingParameters>(m, "MeshSurface")
        .def(py::init<const GeomSurface &, const std::vector<MeshCurve *> &>())
        .def("curves", &MeshSurface::curves, py::return_value_policy::reference)
        .def("all_vertices", &MeshSurface::all_vertices, py::return_value_policy::reference)
        .def("surface_vertices", &MeshSurface::surface_vertices, py::return_value_policy::reference)
        .def("triangles", &MeshSurface::triangles, py::return_value_policy::reference)
        .def("is_meshed", &MeshSurface::is_meshed)
    ;

    py::class_<MeshVolume, MeshingParameters>(m, "MeshVolume")
        .def(py::init<const GeomVolume &, const std::vector<MeshSurface *> &>())
        .def("surfaces", &MeshVolume::surfaces, py::return_value_policy::reference)
        .def("is_meshed", &MeshVolume::is_meshed)
    ;

    py::class_<Scheme>(m, "Scheme")
        .def(py::init<const std::string &>())
        .def("name", &Scheme::name)
        .def("set", [](Scheme & scheme, const py::args &args, const py::kwargs & kwargs) {
                for (auto & [name, value] : kwargs) {
                    if (py::isinstance<py::int_>(value))
                        scheme.set(py::str(name), value.cast<int>());
                    else if (py::isinstance<py::float_>(value))
                        scheme.set(py::str(name), value.cast<double>());
                    else if (py::isinstance<py::str>(value))
                        scheme.set(py::str(name), value.cast<std::string>());
                    else
                        throw krado::Exception("Unsupported type");
                }
            })
        .def("get", [](Scheme & scheme) -> py::dict {
                py::dict d;
                auto & pars = scheme.parameters();
                for (auto & [name, value] : pars) {
                    if (pars.has<int>(name))
                        d[name.c_str()] = py::cast(pars.get<int>(name), py::return_value_policy::reference);
                    else if (pars.has<double>(name))
                        d[name.c_str()] = py::cast(pars.get<double>(name), py::return_value_policy::reference);
                    else if (pars.has<std::string>(name))
                        d[name.c_str()] = py::cast(scheme.get<std::string>(name), py::return_value_policy::reference);
                    else
                        throw krado::Exception("Unsupported type");
                }
                return d;
            })
    ;

    py::class_<ExodusIIFile>(m, "ExodusIIFile")
        .def(py::init<const std::string &>())
        .def("read", &ExodusIIFile::read)
    ;

    m.def("write_exodusii", &write_exodusii);
    // clang-format on
}
