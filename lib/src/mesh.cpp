// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh.h"
#include "krado/bounding_box_3d.h"
#include "krado/element.h"
#include "krado/hasse_diagram.h"
#include "krado/types.h"
#include "krado/utils.h"
#include "krado/vector.h"
#include "krado/log.h"
#include "nanoflann/nanoflann.hpp"
#include <array>
#include <iostream>
#include <unordered_map>

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

template <int N>
void
print_histogram(const std::array<std::size_t, N> & histogram)
{
    // Find the maximum bin count for scaling
    std::size_t max_count = *std::max_element(histogram.begin(), histogram.end());
    int max_bar_width = 50;

    std::size_t max_count_wd = 0;
    for (int b = 0; b < N; ++b) {
        std::size_t count = histogram[b];
        max_count_wd = std::max(max_count_wd, utils::human_number(count).size());
    }

    for (int b = 0; b < N; ++b) {
        double low = static_cast<double>(b) / N;
        double high = static_cast<double>(b + 1) / N;
        std::size_t count = histogram[b];

        int bar_len = (max_count > 0)
                          ? static_cast<int>(static_cast<double>(count) / max_count * max_bar_width)
                          : 0;

        std::string bar(bar_len, '+');
        auto count_str = utils::human_number(count);
        Log::info(2, "  {:.1f} – {:.1f} | {:>{}} {}", low, high, count_str, max_count_wd, bar);
    }
}

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
    std::size_t close_pairs_count = 0;
    double min_dist2 = std::numeric_limits<double>::max();
    const int BINS = 10;
    std::array<std::size_t, BINS> histogram {};

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
                // skip self
                if (match.first == i)
                    continue;
                ++close_pairs_count;
                min_dist2 = std::min(min_dist2, match.second);

                // optional: histogram by fraction of tolerance
                double dist = std::sqrt(match.second);
                int bin =
                    std::min<int>((dist / threshold) * histogram.size(), histogram.size() - 1);
                histogram[bin]++;
            }

            for (const auto & match : matches) {
                processed[match.first] = true;
                point_remap[match.first] = unique_points.size();
            }
            unique_points.push_back(cloud.points[i]);
        }
    }

    Log::info(2, "Diagnostics:");
    Log::info(2, "  Total close pairs: {} ", utils::human_number(close_pairs_count));
    Log::info(2, "  Histogram (0.0 = identical, 1.0 = exactly at tolerance)");
    print_histogram<BINS>(histogram);
    if (close_pairs_count > 0)
        Log::info(2, "  Smallest separation: {:.6g}", std::sqrt(min_dist2));

    return { unique_points, point_remap };
}

std::vector<gidx_t>
boundary_entities(const Mesh & mesh, const Range & range)
{
    std::size_t n = 0;
    for (auto & id : range) {
        auto supp = mesh.support(id);
        if (supp.size() == 1)
            n++;
    }

    std::vector<gidx_t> bnd_ents;
    bnd_ents.reserve(n);
    for (auto & id : range) {
        auto supp = mesh.support(id);
        if (supp.size() == 1)
            bnd_ents.push_back(id);
    }
    return bnd_ents;
}

void
expand_size(std::unordered_map<marker_t, std::size_t> & sizes,
            const std::map<marker_t, std::vector<gidx_t>> & face_sets)
{
    for (const auto & [id, fs] : face_sets) {
        auto it = sizes.find(id);
        if (it == sizes.end())
            sizes[id] = fs.size();
        else
            sizes[id] += fs.size();
    }
}

void
append(std::vector<side_set_entry_t> & dest, const std::vector<side_set_entry_t> & src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

} // namespace

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Point> points, std::vector<Element> elems) : pnts_(points), elems_(elems) {}

const std::vector<Point> &
Mesh::points() const
{
    return this->pnts_;
}

const Point &
Mesh::point(gidx_t idx) const
{
    return this->pnts_.at(idx);
}

const std::vector<Element> &
Mesh::elements() const
{
    return this->elems_;
}

const Element &
Mesh::element(gidx_t idx) const
{
    return this->elems_.at(idx);
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

    Mesh mesh(pts, this->elems_);
    mesh.cell_sets_ = this->cell_sets_;
    mesh.cell_set_names_ = this->cell_set_names_;
    mesh.face_sets_ = this->face_sets_;
    mesh.face_set_names_ = this->face_set_names_;
    mesh.edge_sets_ = this->edge_sets_;
    mesh.edge_set_names_ = this->edge_set_names_;
    mesh.vertex_sets_ = this->vertex_sets_;
    mesh.vertex_set_names_ = this->vertex_set_names_;
    mesh.set_up();
    return mesh;
}

Mesh &
Mesh::transform(const Trsf & tr)
{
    for (auto & p : this->pnts_)
        p = tr * p;
    return *this;
}

Mesh &
Mesh::add(const Mesh & other)
{
    auto n_elem_ofst = this->elems_.size();
    auto n_pt_ofst = this->pnts_.size();
    // merge points
    this->pnts_.insert(this->pnts_.end(), other.pnts_.begin(), other.pnts_.end());
    // merge elements
    this->elems_.reserve(this->elems_.size() + other.elems_.size());
    for (auto & elem : other.elems_) {
        auto ids = elem.ids();
        for (auto & id : ids)
            id += n_pt_ofst;
        auto new_elem = Element(elem.type(), ids);
        this->elems_.emplace_back(new_elem);
    }

    // merge cell sets
    for (auto & id : other.cell_set_ids()) {
        auto & cs = other.cell_set(id);
        for (auto & cell_id : cs)
            this->cell_sets_[id].emplace_back(cell_id + n_elem_ofst);

        auto name = other.cell_set_name(id);
        auto my_name = this->cell_set_names_[id];
        if (my_name.empty())
            this->cell_set_names_[id] = name;
        else if (name != my_name)
            Log::warn("Cell set with id={} already exists, but with a different name '{}'",
                      id,
                      name);
    }

    // merge face sets
    std::map<marker_t, std::vector<side_set_entry_t>> face_side_sets;
    {
        std::unordered_map<marker_t, std::size_t> face_side_sets_size;
        expand_size(face_side_sets_size, this->face_sets_);
        expand_size(face_side_sets_size, other.face_sets_);

        for (auto & [id, n] : face_side_sets_size)
            face_side_sets[id].reserve(n);
        for (auto & [id, fs] : this->face_sets_)
            append(face_side_sets[id], utils::create_side_set(*this, fs));
        for (auto & [id, fs] : other.face_sets_)
            append(face_side_sets[id], utils::create_side_set(other, fs, n_elem_ofst));

        for (auto & id : other.face_set_ids()) {
            auto name = other.face_set_name(id);
            auto my_name = this->face_set_names_[id];
            if (my_name.empty())
                this->face_set_names_[id] = name;
            else if (name != my_name)
                Log::warn("Face set with id={} already exists, but with a different name '{}'",
                          id,
                          name);
        }
    }

    // merge edge sets
    std::map<marker_t, std::vector<side_set_entry_t>> edge_side_sets;
    {
        std::unordered_map<marker_t, std::size_t> edge_side_sets_size;
        expand_size(edge_side_sets_size, this->edge_sets_);
        expand_size(edge_side_sets_size, other.edge_sets_);

        for (auto & [id, n] : edge_side_sets_size)
            edge_side_sets[id].reserve(n);
        for (auto & [id, fs] : this->edge_sets_)
            append(edge_side_sets[id], utils::create_side_set(*this, fs));
        for (auto & [id, fs] : other.edge_sets_)
            append(edge_side_sets[id], utils::create_side_set(other, fs, n_elem_ofst));

        for (auto & id : other.edge_set_ids()) {
            auto name = other.edge_set_name(id);
            auto my_name = this->edge_set_names_[id];
            if (my_name.empty())
                this->edge_set_names_[id] = name;
            else if (name != my_name)
                Log::warn("Edge set with id={} already exists, but with a different name '{}'",
                          id,
                          name);
        }
    }

    set_up();

    // reconstruct face sets
    this->face_sets_.clear();
    for (auto & [id, sset] : face_side_sets)
        this->face_sets_[id] = utils::set_from_side_set(*this, sset);

    // reconstruct edge sets
    this->edge_sets_.clear();
    for (auto & [id, sset] : edge_side_sets)
        this->edge_sets_[id] = utils::set_from_side_set(*this, sset);

    return *this;
}

Mesh &
Mesh::remove_duplicate_points(double tolerance)
{
    Log::info("Removing duplicates: tolerance={}", tolerance);

    PointCloud cloud(*this);
    auto [unique_points, point_map] = remove_duplicates(cloud, tolerance);
    this->pnts_ = unique_points;
    for (auto & elem : this->elems_) {
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
    for (const auto & pt : this->pnts_)
        bbox += pt;
    return bbox;
}

Mesh
Mesh::duplicate() const
{
    Mesh dup(this->pnts_, this->elems_);
    dup.cell_sets_ = this->cell_sets_;
    dup.face_sets_ = this->face_sets_;
    dup.edge_sets_ = this->edge_sets_;
    dup.vertex_sets_ = this->vertex_sets_;
    dup.set_up();
    return dup;
}

Mesh &
Mesh::set_cell_set_name(marker_t cell_set_id, const std::string & name)
{
    this->cell_set_names_[cell_set_id] = name;
    return *this;
}

std::string
Mesh::cell_set_name(marker_t cell_set_id) const
{
    try {
        return this->cell_set_names_.at(cell_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::string("");
    }
}

std::vector<marker_t>
Mesh::cell_set_ids() const
{
    return utils::map_keys(this->cell_sets_);
}

const std::vector<gidx_t> &
Mesh::cell_set(marker_t id) const
{
    try {
        return this->cell_sets_.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Cell set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_cell_set(marker_t id, const std::vector<gidx_t> & cell_ids)
{
    this->cell_sets_[id] = cell_ids;
    return *this;
}

Mesh &
Mesh::remove_cell_sets()
{
    this->cell_sets_.clear();
    this->cell_set_names_.clear();
    return *this;
}

Mesh &
Mesh::set_face_set_name(marker_t face_set_id, const std::string & name)
{
    this->face_set_names_[face_set_id] = name;
    return *this;
}

std::string
Mesh::face_set_name(marker_t face_set_id) const
{
    try {
        return this->face_set_names_.at(face_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::string("");
    }
}

std::vector<marker_t>
Mesh::face_set_ids() const
{
    return utils::map_keys(this->face_sets_);
}

const std::vector<gidx_t> &
Mesh::face_set(marker_t id) const
{
    try {
        return this->face_sets_.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Face set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_face_set(marker_t id, const std::vector<gidx_t> & face_ids)
{
    this->face_sets_[id] = face_ids;
    return *this;
}

Mesh &
Mesh::remove_face_sets()
{
    this->face_sets_.clear();
    this->face_set_names_.clear();
    return *this;
}

Mesh &
Mesh::set_edge_set_name(marker_t edge_set_id, const std::string & name)
{
    this->edge_set_names_[edge_set_id] = name;
    return *this;
}

std::string
Mesh::edge_set_name(marker_t edge_set_id) const
{
    try {
        return this->edge_set_names_.at(edge_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::string("");
    }
}

std::vector<marker_t>
Mesh::edge_set_ids() const
{
    return utils::map_keys(this->edge_sets_);
}

const std::vector<gidx_t> &
Mesh::edge_set(marker_t id) const
{
    try {
        return this->edge_sets_.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Edge set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_edge_set(marker_t id, const std::vector<gidx_t> & edge_ids)
{
    this->edge_sets_[id] = edge_ids;
    return *this;
}

Mesh &
Mesh::remove_edge_sets()
{
    this->edge_sets_.clear();
    this->edge_set_names_.clear();
    return *this;
}

Mesh &
Mesh::set_vertex_set_name(marker_t id, const std::string & name)
{
    this->vertex_set_names_[id] = name;
    return *this;
}

std::string
Mesh::vertex_set_name(marker_t id) const
{
    auto it = this->vertex_set_names_.find(id);
    if (it != this->vertex_set_names_.end())
        return it->second;
    else
        return std::string("");
}

std::vector<marker_t>
Mesh::vertex_set_ids() const
{
    return utils::map_keys(this->vertex_sets_);
}

const std::vector<gidx_t> &
Mesh::vertex_set(marker_t id) const
{
    try {
        return this->vertex_sets_.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Vertex set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_vertex_set(marker_t id, const std::vector<gidx_t> & vertex_ids)
{
    this->vertex_sets_[id] = vertex_ids;
    return *this;
}

Mesh &
Mesh::remove_vertex_sets()
{
    this->vertex_sets_.clear();
    this->vertex_set_names_.clear();
    return *this;
}

Mesh &
Mesh::remap_block_ids(const std::map<marker_t, marker_t> & block_map)
{
    Log::info("Remapping block IDs:");
    for (auto & [block_id, new_block_id] : block_map)
        Log::info("- {} -> {}", block_id, new_block_id);

    std::map<marker_t, std::string> new_cell_set_names;
    std::map<marker_t, std::vector<gidx_t>> new_cell_sets;
    for (auto & [block_id, cells] : this->cell_sets_) {
        auto new_block_id = block_map.at(block_id);
        new_cell_sets[new_block_id] = cells;
        new_cell_set_names[new_block_id] = cell_set_name(block_id);
    }
    this->cell_sets_ = new_cell_sets;
    this->cell_set_names_ = new_cell_set_names;
    return *this;
}

const Range &
Mesh::vertex_range() const
{
    return this->hasse_.vertices();
}

const Range &
Mesh::edge_range() const
{
    return this->hasse_.edges();
}

const Range &
Mesh::face_range() const
{
    return this->hasse_.faces();
}

const Range &
Mesh::cell_range() const
{
    return this->hasse_.cells();
}

std::vector<gidx_t>
Mesh::support(gidx_t index) const
{
    return this->hasse_.get_in_vertices(index);
}

std::vector<gidx_t>
Mesh::cone(gidx_t index) const
{
    return this->hasse_.get_out_vertices(index);
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

ElementType
Mesh::element_type(gidx_t index) const
{
    return this->elems_.at(index).type();
}

void
Mesh::set_up()
{
    this->hasse_.clear();
    build_hasse_diagram();
    this->key_map_.clear();
}

void
Mesh::build_hasse_diagram()
{
    Log::debug("Building Hasse diagram");

    auto n_cells = this->elems_.size();
    auto n_pnts = this->pnts_.size();
    this->hasse_.reserve(n_cells, n_pnts);

    // Add Hasse nodes for cells
    for (std::size_t i = 0; i < n_cells; ++i) {
        auto id = utils::key(-(i + 1));
        if (this->key_map_.find(id) == this->key_map_.end()) {
            auto elem_node_id = i;
            this->key_map_[id] = elem_node_id;
            this->hasse_.add_node(elem_node_id, HasseDiagram::NodeType::Cell);
        }
    }

    // Add Hasse nodes for points
    for (std::size_t i = 0; i < n_pnts; ++i) {
        auto vtx_id = utils::key(i);
        if (this->key_map_.find(vtx_id) == this->key_map_.end()) {
            gidx_t vtx_node_id = this->hasse_.size();
            this->key_map_[vtx_id] = vtx_node_id;
            this->hasse_.add_node(vtx_node_id, HasseDiagram::NodeType::Vertex);
        }
    }

    // Add faces
    for (std::size_t i = 0; i < n_cells; ++i) {
        const auto & cell = this->elems_[i];
        if (cell.type() == ElementType::TETRA4)
            hasse_add_faces<Tetra4>(i, cell);
        else if (cell.type() == ElementType::PYRAMID5)
            hasse_add_faces<Pyramid5>(i, cell);
        else if (cell.type() == ElementType::PRISM6)
            hasse_add_faces<Prism6>(i, cell);
        else if (cell.type() == ElementType::HEX8)
            hasse_add_faces<Hex8>(i, cell);
    }

    // Add edges
    for (std::size_t i = 0; i < n_cells; ++i) {
        const auto & cell = this->elems_[i];
        if (cell.type() == ElementType::TRI3) {
            hasse_add_edges<Tri3>(i, cell);
            hasse_add_edge_vertices<Tri3>(i, cell);
        }
        else if (cell.type() == ElementType::QUAD4) {
            hasse_add_edges<Quad4>(i, cell);
            hasse_add_edge_vertices<Quad4>(i, cell);
        }
        else if (cell.type() == ElementType::TETRA4) {
            hasse_add_face_edges<Tetra4>(i, cell);
            hasse_add_edge_vertices<Tetra4>(i, cell);
        }
        else if (cell.type() == ElementType::PYRAMID5) {
            hasse_add_face_edges<Pyramid5>(i, cell);
            hasse_add_edge_vertices<Pyramid5>(i, cell);
        }
        else if (cell.type() == ElementType::PRISM6) {
            hasse_add_face_edges<Prism6>(i, cell);
            hasse_add_edge_vertices<Prism6>(i, cell);
        }
        else if (cell.type() == ElementType::HEX8) {
            hasse_add_face_edges<Hex8>(i, cell);
            hasse_add_edge_vertices<Hex8>(i, cell);
        }
    }
}

std::vector<gidx_t>
Mesh::boundary_edges() const
{
    return boundary_entities(*this, edge_range());
}

std::vector<gidx_t>
Mesh::boundary_faces() const
{
    return boundary_entities(*this, face_range());
}

Point
Mesh::compute_centroid(gidx_t index) const
{
    auto connect = cone_vertices(index);
    auto pnts_ofst = this->elems_.size();
    Point ctr(0, 0, 0);
    for (auto & pt_id : connect) {
        auto & pt = this->pnts_[pt_id - pnts_ofst];
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

    auto side_type = this->hasse_.node_type(index);
    if (side_type == HasseDiagram::NodeType::Vertex) {
        throw Exception("Normals are not supported for points, yet");
    }
    else if (side_type == HasseDiagram::NodeType::Edge) {
        auto connect_verts = cone_vertices(index);
        std::vector<gidx_t> verts(connect_verts.begin(), connect_verts.end());
        auto pnts_ofst = this->elems_.size();

        auto v1 = Vector(this->pnts_[verts[1] - pnts_ofst] - this->pnts_[verts[0] - pnts_ofst]);
        auto n = Vector(-v1.y, v1.x, 0);
        n.normalize();
        auto c_v1 = Vector(this->pnts_[verts[0] - pnts_ofst] - cell_ctr);
        auto dot = dot_product(n, c_v1);
        if (dot <= 0)
            n = -n;
        return n;
    }
    else {
        auto side_ctr = compute_centroid(index);

        auto connect_verts = cone_vertices(index);
        std::vector<gidx_t> verts(connect_verts.begin(), connect_verts.end());
        auto pnts_ofst = this->elems_.size();

        auto v1 = Vector(this->pnts_[verts[0] - pnts_ofst] - side_ctr);
        auto v2 = Vector(this->pnts_[verts[1] - pnts_ofst] - side_ctr);
        auto c_v1 = Vector(this->pnts_[verts[0] - pnts_ofst] - cell_ctr);
        auto n = cross_product(v1, v2);
        n.normalize();
        auto dot = dot_product(n, c_v1);
        if (dot <= 0)
            n = -n;
        return n;
    }
}

} // namespace krado
