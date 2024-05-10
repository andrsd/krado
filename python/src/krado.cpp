// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include <pybind11/pybind11.h>
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
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/parameters.h"

namespace py = pybind11;
using namespace krado;

PYBIND11_MODULE(krado, m)
{
    m.doc() = "pybind11 plugin for krado";
    py::setattr(m, "version", py::str(KRADO_VERSION));

    // clang-format off
    py::class_<STEPFile>(m, "STEPFile")
        .def(py::init<std::string>())
        .def("load", &STEPFile::load)
    ;

    py::class_<GeomShape>(m, "GeomShape")
        .def(py::init<const TopoDS_Shape &>())
        .def("clean", &GeomShape::clean)
        .def("heal", &GeomShape::heal)
        .def("scale", &GeomShape::scale)
    ;

    py::class_<GeomModel>(m, "GeomModel")
        .def(py::init<const GeomShape &>())
        .def("vertices", &GeomModel::vertices)
        .def("vertex", &GeomModel::vertex)
        .def("curves", &GeomModel::curves)
        .def("curve", &GeomModel::curve)
        .def("surfaces", &GeomModel::surfaces)
        .def("surface", &GeomModel::surface)
        .def("volumes", &GeomModel::volumes)
        .def("volume", &GeomModel::volume)
    ;

    py::class_<GeomVertex>(m, "GeomVertex")
        .def("x", &GeomVertex::x)
        .def("y", &GeomVertex::y)
        .def("z", &GeomVertex::z)
        .def("is_null", &GeomVertex::is_null)
    ;

    py::class_<GeomCurve>(m, "GeomCurve")
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
        .def("volume", &GeomVolume::volume)
        .def("surfaces", &GeomVolume::surfaces)
    ;

    py::class_<Mesh>(m, "Mesh")
        .def(py::init<>())
        .def(py::init<GeomModel>())
        .def("vertex", py::overload_cast<int>(&Mesh::vertex))
        .def("curve", py::overload_cast<int>(&Mesh::curve))
        .def("surface", py::overload_cast<int>(&Mesh::surface))
        .def("volume", py::overload_cast<int>(&Mesh::volume))
        .def("mesh_vertex", py::overload_cast<int>(&Mesh::mesh_vertex))
        .def("mesh_curve", py::overload_cast<int>(&Mesh::mesh_curve))
        .def("mesh_surface", py::overload_cast<int>(&Mesh::mesh_surface))
        .def("mesh_volume", py::overload_cast<int>(&Mesh::mesh_volume))
        .def("points", &Mesh::points)
        .def("elements", &Mesh::elements)
        .def("number_points", &Mesh::number_points)
        .def("build_elements", &Mesh::build_elements)
    ;
    // clang-format on

    py::class_<MeshVertex>(m, "MeshVertex")
        .def(py::init<GeomVertex>())
        .def("point", &MeshVertex::point)
        .def("is_meshed", &MeshVertex::is_meshed)
    ;

    py::class_<MeshCurve>(m, "MeshCurve")
        .def(py::init<MeshCurve>())
        .def("is_meshed", &MeshCurve::is_meshed)
    ;

    py::class_<MeshSurface>(m, "MeshSurface")
        .def(py::init<MeshSurface>())
        .def("is_meshed", &MeshSurface::is_meshed)
    ;

    py::class_<MeshVolume>(m, "MeshVolume")
        .def(py::init<MeshVolume>())
        .def("is_meshed", &MeshVolume::is_meshed)
    ;
}
