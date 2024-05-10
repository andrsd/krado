// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "krado/config.h"
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
#include "krado/mesh_element.h"
#include "krado/scheme.h"
#include "krado/point.h"
#include "krado/vector.h"

namespace py = pybind11;
using namespace krado;

class PyMeshVertexAbstract : public MeshVertexAbstract {
public:
    using MeshVertexAbstract::MeshVertexAbstract;

    Point point() const override {
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
        .def("elements", &Mesh::elements, py::return_value_policy::reference)
        .def("number_points", &Mesh::number_points)
        .def("build_elements", &Mesh::build_elements)
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
        .def("set", &Scheme::set<int>, py::return_value_policy::reference)
        .def("set", &Scheme::set<double>, py::return_value_policy::reference)
        .def("set", &Scheme::set<std::string>, py::return_value_policy::reference)
        .def("get", &Scheme::get<int>)
        .def("get", &Scheme::get<double>)
        .def("get", &Scheme::get<std::string>)
    ;

    py::class_<MeshElement>(m, "MeshElement")
        .def(py::init<MeshElement::Type, const std::vector<int> &, int>())
        .def("type", py::overload_cast<>(&MeshElement::type, py::const_))
        .def("marker", &MeshElement::marker)
        .def("num_vertices", &MeshElement::num_vertices)
        .def("vertex_id", &MeshElement::vertex_id)
        .def("ids", &MeshElement::ids)
    ;

    py::class_<Point>(m, "Point")
        .def(py::init<double, double, double>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def_readwrite("z", &Point::z)
    ;
    // clang-format on
}
