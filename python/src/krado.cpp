// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include "krado/bounding_box_3d.h"
#include "krado/config.h"
#include "krado/extrude.h"
#include "krado/exodusii_file.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/geom_shape.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"
#include "krado/mesh.h"
#include "krado/mesh_model.h"
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
#include "krado/tetrahedralize.h"
#include "krado/transform.h"
#include "krado/vector.h"
#include "krado/io.h"

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
        .def(py::init<double, double, double>(), py::arg("x"), py::arg("y") = 0., py::arg("z") = 0.)
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def_readwrite("z", &Point::z)
    ;

    py::class_<Vector>(m, "Vector")
        .def(py::init<double, double, double>(), py::arg("x"), py::arg("y") = 0., py::arg("z") = 0.)
        .def(py::init<const Point &>())
        .def_readwrite("x", &Vector::x)
        .def_readwrite("y", &Vector::y)
        .def_readwrite("z", &Vector::z)
        .def("norm", &Vector::norm)
        .def("normalize", &Vector::normalize)
        .def("normalized", &Vector::normalized)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(float() * py::self)
        .def(py::self * float())
        .def(-py::self)
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
        .def("contains", static_cast<bool (BoundingBox3D::*)(const BoundingBox3D &)>(&BoundingBox3D::contains))
        .def("contains", static_cast<bool (BoundingBox3D::*)(const Point &)>(&BoundingBox3D::contains))
        .def("contains", static_cast<bool (BoundingBox3D::*)(double, double, double)>(&BoundingBox3D::contains))
        .def("transform", &BoundingBox3D::transform)
        .def("size", static_cast<std::array<double, 3> (BoundingBox3D::*)() const>(&BoundingBox3D::size))
        .def("size", static_cast<double (BoundingBox3D::*)(int) const>(&BoundingBox3D::size))
    ;

    py::class_<Element>(m, "Element")
        .def(py::init<Element::Type, const std::vector<std::size_t> &>())
        .def("type", py::overload_cast<>(&Element::type, py::const_))
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
        .def("vertex", py::overload_cast<int>(&GeomModel::vertex), py::return_value_policy::reference)
        .def("curves", &GeomModel::curves, py::return_value_policy::reference)
        .def("curve", py::overload_cast<int>(&GeomModel::curve), py::return_value_policy::reference)
        .def("surfaces", &GeomModel::surfaces, py::return_value_policy::reference)
        .def("surface", py::overload_cast<int>(&GeomModel::surface), py::return_value_policy::reference)
        .def("volumes", &GeomModel::volumes, py::return_value_policy::reference)
        .def("volume", py::overload_cast<int>(&GeomModel::volume), py::return_value_policy::reference)
        .def("mesh_vertex", py::overload_cast<int>(&GeomModel::mesh_vertex))
        .def("mesh_curve", py::overload_cast<int>(&GeomModel::mesh_curve))
        .def("mesh_surface", py::overload_cast<int>(&GeomModel::mesh_surface))
        .def("mesh_volume", py::overload_cast<int>(&GeomModel::mesh_volume))
        .def("build_mesh", &GeomModel::build_mesh)
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
        .def("points", &Mesh::points, py::return_value_policy::reference)
        .def("point", &Mesh::point, py::return_value_policy::reference)
        .def("scale", static_cast<Mesh &(Mesh::*)(double)>(&Mesh::scale))
        .def("scaled", static_cast<Mesh (Mesh::*)(double) const>(&Mesh::scaled))
        .def("scale", static_cast<Mesh &(Mesh::*)(double, double, double)>(&Mesh::scale),
            py::arg("factor_x"), py::arg("factor_y"), py::arg("factor_z") = 1.)
        .def("scaled", static_cast<Mesh (Mesh::*)(double, double, double) const>(&Mesh::scaled),
            py::arg("factor_x"), py::arg("factor_y"), py::arg("factor_z") = 1.)
        .def("translated", &Mesh::translated, py::arg("tx"), py::arg("ty") = 0., py::arg("tz") = 0.)
        .def("translate", &Mesh::translate, py::arg("tx"), py::arg("ty") = 0., py::arg("tz") = 0.)
        .def("transformed", &Mesh::transformed)
        .def("transform", &Mesh::transform)
        .def("add", &Mesh::add)
        .def("remove_duplicate_points", &Mesh::remove_duplicate_points)
        .def("compute_bounding_box", &Mesh::compute_bounding_box)
        .def("duplicate", &Mesh::duplicate)
        .def("set_cell_set_name", &Mesh::set_cell_set_name)
        .def("cell_set_name", &Mesh::cell_set_name)
        .def("cell_set_ids", &Mesh::cell_set_ids)
        .def("cell_set", &Mesh::cell_set)
        .def("set_cell_set", &Mesh::set_cell_set)
        .def("set_face_set_name", &Mesh::set_face_set_name)
        .def("face_set_name", &Mesh::face_set_name)
        .def("set_edge_set_name", &Mesh::set_edge_set_name)
        .def("edge_set_name", &Mesh::edge_set_name)
        .def("set_side_set_name", &Mesh::set_side_set_name)
        .def("side_set_name", &Mesh::side_set_name)
        .def("side_set_ids", &Mesh::side_set_ids)
        .def("side_set", &Mesh::side_set)
        .def("set_side_set", static_cast<Mesh &(Mesh::*)(marker_t, const std::vector<gidx_t> &)>(&Mesh::set_side_set))
        .def("remap_block_ids", &Mesh::remap_block_ids)
        .def("point_ids", &Mesh::point_ids)
        .def("edge_ids", &Mesh::edge_ids)
        .def("face_ids", &Mesh::face_ids)
        .def("cell_ids", &Mesh::cell_ids)
        .def("support", &Mesh::support)
        .def("cone", &Mesh::cone)
        .def("element_type", &Mesh::element_type)
        .def("set_up", &Mesh::set_up)
        .def("boundary_edges", &Mesh::boundary_edges)
        .def("boundary_faces", &Mesh::boundary_faces)
        .def("compute_centroid", &Mesh::compute_centroid)
        .def("outward_normal", &Mesh::outward_normal)
    ;

    py::class_<MeshingParameters>(m, "MeshingParameters")
        .def(py::init<>())
        .def("set_scheme", &MeshingParameters::set_scheme, py::return_value_policy::reference)
        .def("scheme", &MeshingParameters::scheme, py::return_value_policy::reference)
        .def("set", &MeshingParameters::set<int>, py::return_value_policy::reference)
        .def("set", &MeshingParameters::set<double>, py::return_value_policy::reference)
        .def("set", &MeshingParameters::set<std::string>, py::return_value_policy::reference)
        .def("get", &MeshingParameters::get<int>)
        .def("get", &MeshingParameters::get<double>)
        .def("get", &MeshingParameters::get<std::string>)
        .def("is_meshed", &MeshingParameters::is_meshed)
    ;

    py::class_<MeshVertexAbstract, PyMeshVertexAbstract>(m, "MeshVertexAbstract")
        .def(py::init<>())
        .def("point", &MeshVertexAbstract::point)
    ;

    py::class_<MeshVertex, MeshVertexAbstract>(m, "MeshVertex")
        .def(py::init<const GeomVertex &>())
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
    ;

    py::class_<MeshSurface, MeshingParameters>(m, "MeshSurface")
        .def(py::init<const GeomSurface &, const std::vector<MeshCurve *> &>())
        .def("curves", &MeshSurface::curves, py::return_value_policy::reference)
        .def("all_vertices", &MeshSurface::all_vertices, py::return_value_policy::reference)
        .def("surface_vertices", &MeshSurface::surface_vertices, py::return_value_policy::reference)
        .def("triangles", &MeshSurface::triangles, py::return_value_policy::reference)
    ;

    py::class_<MeshVolume, MeshingParameters>(m, "MeshVolume")
        .def(py::init<const GeomVolume &, const std::vector<MeshSurface *> &>())
        .def("surfaces", &MeshVolume::surfaces, py::return_value_policy::reference)
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
        .def("write", &ExodusIIFile::write)
    ;

    m.def("extrude", static_cast<Mesh(*)(const Mesh &, const Vector &, int, double)>(&extrude));
    m.def("extrude", static_cast<Mesh(*)(const Mesh &, const Vector &, const std::vector<double> &)>(&extrude));

    m.def("tetrahedralize", &tetrahedralize);

    m.def("export_mesh", &IO::export_mesh, py::arg("file_name"), py::arg("mesh"));
    m.def("import_mesh", &IO::import_mesh, py::arg("file_name"));

    // clang-format on
}
