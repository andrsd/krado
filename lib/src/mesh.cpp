// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh.h"
#include "krado/bounding_box_3d.h"
#include "krado/element.h"
#include "krado/hasse_diagram.h"
#include "krado/vector.h"
#include "nanoflann/nanoflann.hpp"
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

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Point> points, std::vector<Element> elems) : pnts(points), elems(elems) {}

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

Mesh
Mesh::scaled(double factor) const
{
    auto tr = Trsf::scaled(factor);
    return transformed(tr);
}

Mesh &
Mesh::scale(double factor)
{
    auto tr = Trsf::scaled(factor);
    return transform(tr);
}

Mesh
Mesh::scaled(double factor_x, double factor_y, double factor_z) const
{
    auto tr = Trsf::scaled(factor_x, factor_y, factor_z);
    return transformed(tr);
}

Mesh &
Mesh::scale(double factor_x, double factor_y, double factor_z)
{
    auto tr = Trsf::scaled(factor_x, factor_y, factor_z);
    return transform(tr);
}

Mesh
Mesh::translated(double tx, double ty, double tz) const
{
    auto tr = Trsf::translated(tx, ty, tz);
    return transformed(tr);
}

Mesh &
Mesh::translate(double tx, double ty, double tz)
{
    auto tr = Trsf::translated(tx, ty, tz);
    return transform(tr);
}

Mesh
Mesh::transformed(const Trsf & tr) const
{
    auto pts = points();
    for (auto & p : pts)
        p = tr * p;

    Mesh mesh(pts, this->elems);
    mesh.cell_sets = this->cell_sets;
    mesh.side_sets = this->side_sets;
    return mesh;
}

Mesh &
Mesh::transform(const Trsf & tr)
{
    auto pts = points();
    for (auto & p : pts)
        p = tr * p;
    return *this;
}

Mesh &
Mesh::add(const Mesh & other)
{
    auto n_elem_ofst = this->elems.size();
    auto n_pt_ofst = this->pnts.size();
    // merge points
    this->pnts.insert(this->pnts.end(), other.pnts.begin(), other.pnts.end());
    // merge elements
    for (auto & elem : other.elems) {
        auto ids = elem.ids();
        for (auto & id : ids)
            id += n_pt_ofst;
        auto new_elem = Element(elem.type(), ids);
        this->elems.emplace_back(new_elem);
    }

    // merge cell sets
    for (auto & id : other.cell_set_ids()) {
        auto & cs = other.cell_set(id);
        for (auto & cell_id : cs)
            this->cell_sets[id].emplace_back(cell_id + n_elem_ofst);
    }

    // merge side sets
    for (auto & id : other.side_set_ids()) {
        auto & ss = other.side_set(id);
        for (auto & entry : ss) {
            auto cell_id = entry.elem + n_elem_ofst;
            this->side_sets[id].emplace_back(cell_id, entry.side);
        }
    }

    return *this;
}

Mesh &
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

    return *this;
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
    Mesh dup(this->pnts, this->elems);
    dup.cell_sets = this->cell_sets;
    dup.side_sets = this->side_sets;
    return dup;
}

Mesh &
Mesh::set_cell_set_name(marker_t cell_set_id, const std::string & name)
{
    this->cell_set_names[cell_set_id] = name;
    return *this;
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

const std::vector<gidx_t> &
Mesh::cell_set(marker_t id) const
{
    try {
        return this->cell_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Cell set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_cell_set(marker_t id, const std::vector<gidx_t> & cell_ids)
{
    this->cell_sets[id] = cell_ids;
    return *this;
}

Mesh &
Mesh::set_face_set_name(marker_t face_set_id, const std::string & name)
{
    this->face_set_names[face_set_id] = name;
    return *this;
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

const std::vector<gidx_t> &
Mesh::face_set(marker_t id) const
{
    try {
        return this->face_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Face set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_face_set(marker_t id, const std::vector<gidx_t> & face_ids)
{
    this->face_sets[id] = face_ids;
    return *this;
}

Mesh &
Mesh::set_edge_set_name(marker_t edge_set_id, const std::string & name)
{
    this->edge_set_names[edge_set_id] = name;
    return *this;
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

const std::vector<gidx_t> &
Mesh::edge_set(marker_t id) const
{
    try {
        return this->edge_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Edge set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_edge_set(marker_t id, const std::vector<gidx_t> & edge_ids)
{
    this->edge_sets[id] = edge_ids;
    return *this;
}

Mesh &
Mesh::set_side_set_name(marker_t id, const std::string & name)
{
    this->side_set_names[id] = name;
    return *this;
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

const std::vector<side_set_entry_t> &
Mesh::side_set(marker_t id) const
{
    try {
        return this->side_sets.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Side set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_side_set(marker_t id, const std::vector<gidx_t> & elem_ids)
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
    return *this;
}

Mesh &
Mesh::set_side_set(marker_t id, const std::vector<side_set_entry_t> & side_set_entries)
{
    this->side_sets[id] = side_set_entries;
    return *this;
}

Mesh &
Mesh::remap_block_ids(const std::map<marker_t, marker_t> & block_map)
{
    std::map<marker_t, std::string> new_cell_set_names;
    std::map<marker_t, std::vector<gidx_t>> new_cell_sets;
    for (auto & [block_id, cells] : this->cell_sets) {
        auto new_block_id = block_map.at(block_id);
        new_cell_sets[new_block_id] = cells;
        new_cell_set_names[new_block_id] = cell_set_name(block_id);
    }
    this->cell_sets = new_cell_sets;
    this->cell_set_names = new_cell_set_names;
    return *this;
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
Mesh::set_up()
{
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
