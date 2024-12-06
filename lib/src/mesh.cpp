// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh.h"
#include "krado/bounding_box_3d.h"
#include "krado/element.h"
#include "krado/geom_model.h"
#include "krado/hasse_diagram.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme1d.h"
#include "krado/scheme2d.h"
#include "krado/scheme3d.h"
#include "krado/vector.h"
#include "nanoflann/nanoflann.hpp"
#include <Extrema_ElementType.hxx>
#include <array>
#include <iostream>

namespace krado {

namespace {

struct PointCloud {
    PointCloud(const Mesh & mesh) : points(mesh.points()) {}

    const std::vector<Point> & points;

    inline std::size_t
    kdtree_get_point_count() const
    {
        return this->points.size();
    }

    // Returns the distance between the vector "p1[0:size-1]" and the mesh point with index "idx_p2"
    inline double
    kdtree_distance(const double * p1, const std::size_t idx_p2, std::size_t /*size*/) const
    {
        const double d0 = p1[0] - this->points[idx_p2].x;
        const double d1 = p1[1] - this->points[idx_p2].y;
        const double d2 = p1[2] - this->points[idx_p2].z;
        return d0 * d0 + d1 * d1 + d2 * d2;
    }

    // Returns the dim'th component of the idx'th point in the class
    inline double
    kdtree_get_pt(const std::size_t idx, int dim) const
    {
        return this->points[idx](dim);
    }

    // Optional bounding-box computation
    template <class BBOX>
    bool
    kdtree_get_bbox(BBOX & /*bb*/) const
    {
        return false;
    }
};

/// Find dupllicate points (i.e. points that are closer than a threshold distance)
///
/// @return Tuple where the first element is a vector of unique points and the second element is a
///         map that maps the index of the original point to the index of the unique point.
std::tuple<std::vector<Point>, std::map<std::size_t, std::size_t>>
remove_duplicates(const PointCloud & cloud, double threshold)
{
    constexpr int32_t DIM3 = 3;
    using namespace nanoflann;
    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<double, PointCloud>,
                                     PointCloud,
                                     DIM3,
                                     std::size_t>
        KDTree;

    // Construct a kd-tree index
    // 30 is the maxium number of neighboring nodes we can have per any node
    KDTree tree(DIM3, cloud, KDTreeSingleIndexAdaptorParams(30));
    tree.buildIndex();

    std::vector<Point> unique_points;
    std::map<std::size_t, std::size_t> point_remap;
    std::vector<bool> processed(cloud.points.size(), false);
    nanoflann::SearchParameters params;

    for (std::size_t i = 0; i < cloud.points.size(); ++i) {
        if (!processed[i]) {
            processed[i] = true;

            const double query_point[DIM3] = { cloud.points[i].x,
                                               cloud.points[i].y,
                                               cloud.points[i].z };
            std::vector<ResultItem<std::size_t, double>> matches;
            const auto search_radius = threshold * threshold;
            // Search for points within the threshold distance
            tree.radiusSearch(query_point, search_radius, matches, params);

            for (const auto & match : matches) {
                processed[match.first] = true;
                point_remap[match.first] = unique_points.size();
            }
            unique_points.push_back(cloud.points[i]);
        }
    }

    return { unique_points, point_remap };
}

} // namespace

Mesh::Mesh() : scheme_factory(SchemeFactory::instance()), gid_ctr(0) {}

Mesh::Mesh(const GeomModel & model) : scheme_factory(SchemeFactory::instance()), gid_ctr(0)
{
    initialize(model);
}

Mesh::Mesh(std::vector<Point> points, std::vector<Element> elems) :
    scheme_factory(SchemeFactory::instance()),
    pnts(points),
    elems(elems),
    gid_ctr(0)
{
}

const MeshVertex &
Mesh::vertex(int id) const
{
    try {
        return this->vtxs.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

MeshVertex &
Mesh::vertex(int id)
{
    try {
        return this->vtxs.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

const std::map<int, MeshVertex> &
Mesh::vertices() const
{
    return this->vtxs;
}

const MeshCurve &
Mesh::curve(int id) const
{
    try {
        return this->crvs.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

MeshCurve &
Mesh::curve(int id)
{
    try {
        return this->crvs.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

const std::map<int, MeshCurve> &
Mesh::curves() const
{
    return this->crvs;
}

const MeshSurface &
Mesh::surface(int id) const
{
    try {
        return this->surfs.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

MeshSurface &
Mesh::surface(int id)
{
    try {
        return this->surfs.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

const std::map<int, MeshSurface> &
Mesh::surfaces() const
{
    return this->surfs;
}

const MeshVolume &
Mesh::volume(int id) const
{
    try {
        return this->vols.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

MeshVolume &
Mesh::volume(int id)
{
    try {
        return this->vols.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

const std::map<int, MeshVolume> &
Mesh::volumes() const
{
    return this->vols;
}

void
Mesh::initialize(const GeomModel & model)
{
    for (auto & [id, gvtx] : model.vertices()) {
        MeshVertex mvtx(gvtx);
        this->vtxs.emplace(id, mvtx);
    }

    for (auto & [id, geom_curve] : model.curves()) {
        auto id1 = model.vertex_id(geom_curve.first_vertex());
        auto id2 = model.vertex_id(geom_curve.last_vertex());
        auto & v1 = vertex(id1);
        auto & v2 = vertex(id2);

        MeshCurve mesh_crv(geom_curve, &v1, &v2);
        this->crvs.emplace(id, mesh_crv);
    }

    for (auto & [id, geom_surface] : model.surfaces()) {
        auto surface_curves = geom_surface.curves();
        std::vector<MeshCurve *> mesh_curves;
        for (auto & gcurve : surface_curves) {
            int cid = model.curve_id(gcurve);
            auto * mcurve = &curve(cid);
            mesh_curves.push_back(mcurve);
        }

        MeshSurface mesh_surf(geom_surface, mesh_curves);
        this->surfs.emplace(id, mesh_surf);
    }

    for (auto & [id, geom_volume] : model.volumes()) {
        auto volume_surfaces = geom_volume.surfaces();
        std::vector<MeshSurface *> mesh_surfaces;
        for (auto & gsurface : volume_surfaces) {
            auto sid = model.surface_id(gsurface);
            auto * msurface = &surface(sid);
            mesh_surfaces.push_back(msurface);
        }

        MeshVolume mesh_vol(geom_volume, mesh_surfaces);
        this->vols.emplace(id, mesh_vol);
    }
}

void
Mesh::mesh_vertex(int id)
{
    auto & vertex = this->vtxs.at(id);
    mesh_vertex(vertex);
}

void
Mesh::mesh_vertex(MeshVertex & vertex)
{
    if (!vertex.is_meshed()) {
        vertex.set_meshed();
    }
}

void
Mesh::mesh_curve(int id)
{
    auto & curve = this->crvs.at(id);
    mesh_curve(curve);
}

void
Mesh::mesh_curve(MeshCurve & curve)
{
    if (!curve.is_meshed()) {
        auto & geom_curve = curve.geom_curve();
        if ((geom_curve.length() == 0.) || (geom_curve.is_degenerated()))
            return;

        auto & scheme = get_scheme<Scheme1D>(curve);

        auto bnd_vtxs = curve.bounding_vertices();
        for (auto & v : bnd_vtxs)
            mesh_vertex(*v);

        scheme.mesh_curve(curve);
        curve.set_meshed();
    }
}

void
Mesh::mesh_surface(int id)
{
    auto & surface = this->surfs.at(id);
    mesh_surface(surface);
}

void
Mesh::mesh_surface(MeshSurface & surface)
{
    if (!surface.is_meshed()) {
        auto & scheme = get_scheme<Scheme2D>(surface);

        auto curves = surface.curves();
        for (auto & crv : curves)
            scheme.select_curve_scheme(*crv);
        for (auto & crv : curves)
            mesh_curve(*crv);

        scheme.mesh_surface(surface);
        surface.set_meshed();
    }
}

void
Mesh::mesh_volume(int id)
{
    auto & volume = this->vols.at(id);
    mesh_volume(volume);
}

void
Mesh::mesh_volume(MeshVolume & volume)
{
    if (!volume.is_meshed()) {
        auto & scheme = get_scheme<Scheme3D>(volume);
        scheme.mesh_volume(volume);
        volume.set_meshed();
    }
}

void
Mesh::assign_gid(MeshVertex & vertex)
{
    this->pnts.emplace_back(vertex.point());
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

void
Mesh::assign_gid(MeshCurveVertex & vertex)
{
    this->pnts.emplace_back(vertex.point());
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

void
Mesh::assign_gid(MeshSurfaceVertex & vertex)
{
    this->pnts.emplace_back(vertex.point());
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

const std::vector<Point> &
Mesh::points() const
{
    return this->pnts;
}

const Point &
Mesh::point(gidx_t idx) const
{
    return this->pnts.at(idx);
}

const std::vector<Element> &
Mesh::elements() const
{
    return this->elems;
}

const Element &
Mesh::element(gidx_t idx) const
{
    return this->elems.at(idx);
}

void
Mesh::add_mesh_point(Point & mpnt)
{
    this->pnts.emplace_back(mpnt);
}

void
Mesh::number_points()
{
    this->exp_bbox.reset();
    for (auto & [id, v] : this->vtxs)
        if (v.is_meshed()) {
            assign_gid(v);
            this->exp_bbox += v.point();
        }
    for (auto & [id, curve] : this->crvs)
        if (curve.is_meshed())
            for (auto & v : curve.curve_vertices()) {
                assign_gid(*v);
                this->exp_bbox += v->point();
            }
    for (auto & [id, surface] : this->surfs)
        if (surface.is_meshed())
            for (auto & v : surface.surface_vertices()) {
                assign_gid(*v);
                this->exp_bbox += v->point();
            }
}

void
Mesh::build_elements()
{
    auto dims = this->exp_bbox.size();
    if ((dims[0] > 0) && (dims[1] < 1e-15) && (dims[2] < 1e-15))
        build_1d_elements();
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] < 1e-15))
        build_2d_elements();
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] > 0))
        throw Exception("3D element construction is not implemented yet");
    else
        throw Exception("Element construction for your setup is not implemented yet");
}

void
Mesh::build_1d_elements()
{
    for (auto & [id, curve] : this->crvs) {
        if (curve.is_meshed()) {
            auto verts = curve.all_vertices();
            std::array<gidx_t, 2> line;
            for (auto & local_elem : curve.segments()) {
                for (int i = 0; i < 2; i++) {
                    auto lid = local_elem.ids()[i];
                    auto gid = verts[lid]->global_id();
                    line[i] = gid;
                }
                this->elems.emplace_back(Element::Line2(line));
            }
        }
    }
}

void
Mesh::build_2d_elements()
{
    for (auto & [id, surface] : this->surfs) {
        if (surface.is_meshed()) {
            auto verts = surface.all_vertices();
            std::array<gidx_t, 3> tri;
            for (auto & local_elem : surface.triangles()) {
                for (int i = 0; i < 3; i++) {
                    auto lid = local_elem.ids()[i];
                    auto gid = verts[lid]->global_id();
                    tri[i] = gid;
                }
                this->elems.emplace_back(Element::Tri3(tri));
            }
        }
    }
}

BoundingBox3D
Mesh::bounding_box() const
{
    return this->exp_bbox;
}

Mesh
Mesh::scaled(double factor) const
{
    auto tr = Trsf::scale(factor);
    return transformed(tr);
}

Mesh
Mesh::scaled(double factor_x, double factor_y, double factor_z) const
{
    auto tr = Trsf::scale(factor_x, factor_y, factor_z);
    return transformed(tr);
}

Mesh
Mesh::translated(double tx, double ty, double tz) const
{
    auto tr = Trsf::translate(tx, ty, tz);
    return transformed(tr);
}

Mesh
Mesh::transformed(const Trsf & tr) const
{
    auto pts = points();
    for (auto & p : pts)
        p = tr * p;
    auto elems = this->elems;
    return Mesh(pts, elems);
}

void
Mesh::add(const Mesh & other)
{
    auto n_pt_ofst = this->pnts.size();
    this->pnts.insert(this->pnts.end(), other.pnts.begin(), other.pnts.end());
    for (auto & elem : other.elems) {
        auto ids = elem.ids();
        for (auto & id : ids)
            id += n_pt_ofst;
        auto new_elem = Element(elem.type(), ids, elem.marker());
        this->elems.emplace_back(new_elem);
    }
}

void
Mesh::remove_duplicate_points(double tolerance)
{
    PointCloud cloud(*this);
    auto [unique_points, point_map] = remove_duplicates(cloud, tolerance);
    this->pnts = unique_points;
    for (auto & elem : this->elems) {
        auto ids = elem.ids();
        for (auto & id : ids)
            id = point_map[id];
        elem.set_ids(ids);
    }
}

BoundingBox3D
Mesh::compute_bounding_box() const
{
    BoundingBox3D bbox;
    for (const auto & pt : this->pnts)
        bbox += pt;
    return bbox;
}

Mesh
Mesh::duplicate() const
{
    return Mesh(this->pnts, this->elems);
}

void
Mesh::set_cell_set_name(marker_t cell_set_id, const std::string & name)
{
    this->cell_set_names[cell_set_id] = name;
}

std::string
Mesh::cell_set_name(marker_t cell_set_id) const
{
    try {
        return this->cell_set_names.at(cell_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::string("");
    }
}

std::vector<marker_t>
Mesh::cell_set_ids() const
{
    return utils::map_keys(this->cell_sets);
}

const std::vector<gidx_t>
Mesh::cell_set(marker_t id) const
{
    try {
        return this->cell_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Cell set ID {} does not exist", id);
    }
}

void
Mesh::set_cell_set(marker_t id, const std::vector<gidx_t> cell_ids)
{
    this->cell_sets[id] = cell_ids;
}

void
Mesh::set_face_set_name(marker_t face_set_id, const std::string & name)
{
    this->face_set_names[face_set_id] = name;
}

std::string
Mesh::face_set_name(marker_t face_set_id) const
{
    try {
        return this->face_set_names.at(face_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::string("");
    }
}

std::vector<marker_t>
Mesh::face_set_ids() const
{
    return utils::map_keys(this->face_sets);
}

const std::vector<gidx_t>
Mesh::face_set(marker_t id) const
{
    try {
        return this->face_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Face set ID {} does not exist", id);
    }
}

void
Mesh::set_face_set(marker_t id, const std::vector<gidx_t> face_ids)
{
    this->face_sets[id] = face_ids;
}

void
Mesh::set_edge_set_name(marker_t edge_set_id, const std::string & name)
{
    this->edge_set_names[edge_set_id] = name;
}

std::string
Mesh::edge_set_name(marker_t edge_set_id) const
{
    try {
        return this->edge_set_names.at(edge_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::string("");
    }
}

std::vector<marker_t>
Mesh::edge_set_ids() const
{
    return utils::map_keys(this->edge_sets);
}

const std::vector<gidx_t>
Mesh::edge_set(marker_t id) const
{
    try {
        return this->edge_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Edge set ID {} does not exist", id);
    }
}

void
Mesh::set_edge_set(marker_t id, const std::vector<gidx_t> edge_ids)
{
    this->edge_sets[id] = edge_ids;
}

void
Mesh::set_side_set_name(marker_t id, const std::string & name)
{
    this->side_set_names[id] = name;
}

std::string
Mesh::side_set_name(marker_t id) const
{
    try {
        return this->side_set_names.at(id);
    }
    catch (const std::out_of_range & e) {
        return std::string("");
    }
}

std::vector<marker_t>
Mesh::side_set_ids() const
{
    return utils::map_keys(this->side_sets);
}

const std::vector<side_set_entry_t>
Mesh::side_set(marker_t id) const
{
    try {
        return this->side_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Side set ID {} does not exist", id);
    }
}

void
Mesh::set_side_set(marker_t id, const std::vector<gidx_t> elem_ids)
{
    std::vector<side_set_entry_t> side_set;
    for (auto & eid : elem_ids) {
        auto supp = support(eid);
        if (supp.size() == 2)
            throw Exception("Internal side sets are not supported, yet");
        if (supp.size() == 0)
            throw Exception("Edge {} has no support", eid);
        auto cell = supp[0];
        auto cell_connect = cone(cell);
        auto side = utils::index_of(cell_connect, eid);
        side_set.emplace_back(cell, side);
    }
    this->side_sets[id] = side_set;
}

void
Mesh::remap_block_ids(const std::map<marker_t, marker_t> & block_map)
{
    for (auto & elem : this->elems) {
        auto block_id = elem.marker();
        if (block_map.find(block_id) != block_map.end())
            elem.set_marker(block_map.at(block_id));
    }
}

const Range &
Mesh::point_ids() const
{
    return this->hasse.vertices();
}

const Range &
Mesh::edge_ids() const
{
    return this->hasse.edges();
}

const Range &
Mesh::face_ids() const
{
    return this->hasse.faces();
}

const Range &
Mesh::cell_ids() const
{
    return this->hasse.cells();
}

std::vector<gidx_t>
Mesh::support(gidx_t index) const
{
    return this->hasse.get_in_vertices(index);
}

std::vector<gidx_t>
Mesh::cone(gidx_t index) const
{
    return this->hasse.get_out_vertices(index);
}

std::set<gidx_t>
Mesh::cone_vertices(gidx_t index) const
{
    std::list<gidx_t> pts_to_process;
    for (auto & v : cone(index))
        pts_to_process.push_back(v);

    std::set<gidx_t> verts;
    for (auto & v : pts_to_process) {
        auto cn = cone(v);
        if (cn.size() == 0)
            verts.insert(v);
        else {
            for (auto & c : cn)
                pts_to_process.push_back(c);
        }
    }

    return verts;
}

Element::Type
Mesh::element_type(gidx_t index) const
{
    return this->elems.at(index).type();
}

void
Mesh::build_cell_sets()
{
    this->cell_sets.clear();
    for (std::size_t i = 0; i < this->elems.size(); ++i) {
        const auto & cell = this->elems[i];
        auto marker = cell.marker();
        this->cell_sets[marker].push_back(i);
    }
}

void
Mesh::set_up()
{
    build_cell_sets();
    build_hasse_diagram();
}

void
Mesh::build_hasse_diagram()
{
    auto n_cells = this->elems.size();
    // Add Hasse nodes for cells
    for (std::size_t i = 0; i < n_cells; ++i) {
        auto id = utils::key(-(i + 1));
        if (this->key_map.find(id) == this->key_map.end()) {
            auto elem_node_id = i;
            this->key_map[id] = elem_node_id;
            this->hasse.add_node(elem_node_id, HasseDiagram::NodeType::Cell);
        }
    }

    // Add Hasse nodes for points
    for (std::size_t i = 0; i < this->pnts.size(); ++i) {
        auto vtx_id = utils::key(i);
        if (this->key_map.find(vtx_id) == this->key_map.end()) {
            gidx_t vtx_node_id = this->hasse.size();
            this->key_map[vtx_id] = vtx_node_id;
            this->hasse.add_node(vtx_node_id, HasseDiagram::NodeType::Vertex);
        }
    }

    // Add faces
    for (std::size_t i = 0; i < n_cells; ++i) {
        const auto & cell = this->elems[i];
        if (cell.type() == Element::TETRA4)
            hasse_add_faces<Tetra4>(i, cell);
        else if (cell.type() == Element::PYRAMID5)
            hasse_add_faces<Pyramid5>(i, cell);
        else if (cell.type() == Element::PRISM6)
            hasse_add_faces<Prism6>(i, cell);
        else if (cell.type() == Element::HEX8)
            hasse_add_faces<Hex8>(i, cell);
    }

    // Add edges
    for (std::size_t i = 0; i < n_cells; ++i) {
        const auto & cell = this->elems[i];
        if (cell.type() == Element::TRI3) {
            hasse_add_edges<Tri3>(i, cell);
            hasse_add_edge_vertices<Tri3>(i, cell);
        }
        else if (cell.type() == Element::QUAD4) {
            hasse_add_edges<Quad4>(i, cell);
            hasse_add_edge_vertices<Quad4>(i, cell);
        }
        else if (cell.type() == Element::TETRA4) {
            hasse_add_face_edges<Tetra4>(i, cell);
            hasse_add_edge_vertices<Tetra4>(i, cell);
        }
        else if (cell.type() == Element::PYRAMID5) {
            hasse_add_face_edges<Pyramid5>(i, cell);
            hasse_add_edge_vertices<Pyramid5>(i, cell);
        }
        else if (cell.type() == Element::PRISM6) {
            hasse_add_face_edges<Prism6>(i, cell);
            hasse_add_edge_vertices<Prism6>(i, cell);
        }
        else if (cell.type() == Element::HEX8) {
            hasse_add_face_edges<Hex8>(i, cell);
            hasse_add_edge_vertices<Hex8>(i, cell);
        }
    }
}

std::vector<gidx_t>
Mesh::boundary_edges() const
{
    std::vector<gidx_t> bnd_edges;
    for (auto & edge : edge_ids()) {
        auto supp = support(edge);
        if (supp.size() == 1)
            bnd_edges.push_back(edge);
    }
    return bnd_edges;
}

std::vector<gidx_t>
Mesh::boundary_faces() const
{
    std::vector<gidx_t> bnd_faces;
    for (auto & face : face_ids()) {
        auto supp = support(face);
        if (supp.size() == 1)
            bnd_faces.push_back(face);
    }
    return bnd_faces;
}

Point
Mesh::compute_centroid(gidx_t index) const
{
    auto connect = cone_vertices(index);
    auto pnts_ofst = this->elems.size();
    Point ctr(0, 0, 0);
    for (auto & pt_id : connect) {
        auto & pt = this->pnts[pt_id - pnts_ofst];
        ctr += pt;
    }
    ctr *= 1. / connect.size();
    return ctr;
}

Vector
Mesh::outward_normal(gidx_t index) const
{
    auto supp = support(index);
    if (supp.size() != 1)
        throw Exception("Normals are supported only for sides on boundaries");

    auto cell_id = supp[0];
    auto cell_ctr = compute_centroid(cell_id);

    auto side_type = this->hasse.node_type(index);
    if (side_type == HasseDiagram::NodeType::Vertex) {
        throw Exception("Normals are not supported for points, yet");
    }
    else if (side_type == HasseDiagram::NodeType::Edge) {
        auto connect_verts = cone_vertices(index);
        std::vector<gidx_t> verts(connect_verts.begin(), connect_verts.end());
        auto pnts_ofst = this->elems.size();

        auto v1 = Vector(this->pnts[verts[1] - pnts_ofst] - this->pnts[verts[0] - pnts_ofst]);
        auto n = Vector(-v1.y, v1.x, 0);
        n.normalize();
        auto c_v1 = Vector(this->pnts[verts[0] - pnts_ofst] - cell_ctr);
        auto dot = dot_product(n, c_v1);
        if (dot <= 0)
            n = -n;
        return n;
    }
    else {
        auto side_ctr = compute_centroid(index);

        auto connect_verts = cone_vertices(index);
        std::vector<gidx_t> verts(connect_verts.begin(), connect_verts.end());
        auto pnts_ofst = this->elems.size();

        auto v1 = Vector(this->pnts[verts[0] - pnts_ofst] - side_ctr);
        auto v2 = Vector(this->pnts[verts[1] - pnts_ofst] - side_ctr);
        auto c_v1 = Vector(this->pnts[verts[0] - pnts_ofst] - cell_ctr);
        auto n = cross_product(v1, v2);
        n.normalize();
        auto dot = dot_product(n, c_v1);
        if (dot <= 0)
            n = -n;
        return n;
    }
}

} // namespace krado
