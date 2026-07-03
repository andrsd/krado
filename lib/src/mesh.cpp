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
#include "krado/geom_model.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/timer.h"
#include "nanoflann/nanoflann.hpp"
#include <array>
#include <unordered_map>
#include <algorithm>
#include <list>

namespace krado {

namespace {

struct PointCloud {
    PointCloud(const Mesh & mesh) : points(mesh.points()) {}

    Span<const Point> points;

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
    for (auto b : make_range(N)) {
        std::size_t count = histogram[b];
        max_count_wd = std::max(max_count_wd, utils::human_number(count).size());
    }

    for (auto b : make_range(N)) {
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

    for (auto i : make_range(cloud.points.size())) {
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

std::vector<Index>
boundary_entities(const Mesh & mesh, Range range)
{
    namespace ranges = std::ranges;

    auto is_boundary = [&](Index id) {
        return mesh.support(id).size() == 1;
    };

    std::size_t n = ranges::count_if(range, is_boundary);
    std::vector<Index> bnd_ents;
    bnd_ents.reserve(n);
    ranges::copy_if(range, std::back_inserter(bnd_ents), is_boundary);

    return bnd_ents;
}

template <typename T>
void
expand_size(std::unordered_map<Marker, std::size_t> & sizes,
            const std::map<Marker, std::vector<T>> & sets)
{
    for (const auto & [id, fs] : sets) {
        auto it = sizes.find(id);
        if (it == sizes.end())
            sizes[id] = fs.size();
        else
            sizes[id] += fs.size();
    }
}

void
append(std::vector<SideEntry> & dest, const std::vector<SideEntry> & src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

void
append(std::vector<Index> & dest, const std::vector<Index> & src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

} // namespace

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Point> points, std::vector<Element> elems) :
    pnts_(std::move(points)),
    elems_(std::move(elems))
{
}

std::size_t
Mesh::num_points() const
{
    return this->pnts_.size();
}

std::size_t
Mesh::num_elements() const
{
    return this->elems_.size();
}

Span<const Point>
Mesh::points() const
{
    return this->pnts_;
}

const Point &
Mesh::point(Index idx) const
{
    return this->pnts_.at(idx);
}

Span<const Element>
Mesh::elements() const
{
    return this->elems_;
}

const Element &
Mesh::element(Index idx) const
{
    return this->elems_.at(idx);
}

Ptr<Mesh>
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

Ptr<Mesh>
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

Ptr<Mesh>
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

Ptr<Mesh>
Mesh::transformed(const Trsf & tr) const
{
    std::vector<Point> pts;
    pts.reserve(this->pnts_.size());
    // clang-format off
    std::ranges::transform(
        this->pnts_, std::back_inserter(pts),
        [=](Point p) {
            return tr * p;
        }
    );
    // clang-format on

    auto mesh = Ptr<Mesh>::alloc(pts, this->elems_);
    mesh->cell_sets_ = this->cell_sets_;
    mesh->cell_set_names_ = this->cell_set_names_;
    mesh->side_sets_ = this->side_sets_;
    mesh->side_set_names_ = this->side_set_names_;
    mesh->node_sets_ = this->node_sets_;
    mesh->node_set_names_ = this->node_set_names_;
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
    this->elems_.insert(this->elems_.end(), other.elems_.begin(), other.elems_.end());
    for (auto i : make_range(n_elem_ofst, this->elems_.size())) {
        auto & ids = this->elems_[i].vtx_id_;
        for (auto & id : ids)
            id += n_pt_ofst;
    }

    // merge cell sets
    for (auto & id : other.cell_set_ids()) {
        auto cs = other.cell_set(id);
        for (auto & cell_id : cs)
            this->cell_sets_[id].emplace_back(cell_id + n_elem_ofst);

        auto name = other.cell_set_name(id);
        if (name.has_value()) {
            auto my_name = cell_set_name(id);
            if (!my_name.has_value()) {
                this->cell_set_names_[id] = name.value();
            }
            else if (name != my_name)
                Log::warn("Cell set with id={} already exists, but with a different name '{}'",
                          id,
                          name.value());
        }
    }

    // merge side sets
    std::map<Marker, std::vector<SideEntry>> side_sets;
    {
        std::unordered_map<Marker, std::size_t> side_sets_size;
        expand_size(side_sets_size, this->side_sets_);
        expand_size(side_sets_size, other.side_sets_);

        for (auto & [id, n] : side_sets_size)
            side_sets[id].reserve(n);
        for (auto & [id, ss] : this->side_sets_)
            append(side_sets[id], ss);
        for (auto & [id, ss] : other.side_sets_) {
            auto ss_shifted = ss;
            for (auto & ent : ss_shifted)
                ent.elem += n_elem_ofst;
            append(side_sets[id], ss_shifted);
        }

        for (auto & id : other.side_set_ids()) {
            auto name = other.side_set_name(id);
            if (name.has_value()) {
                auto my_name = side_set_name(id);
                if (my_name.has_value())
                    this->side_set_names_[id] = name.value();
                else if (name != my_name)
                    Log::warn("Side set with id={} already exists, but with a different name '{}'",
                              id,
                              name.value());
            }
        }
    }
    this->side_sets_ = side_sets;

    // merge node sets
    std::map<Marker, std::vector<Index>> node_sets;
    {
        std::unordered_map<Marker, std::size_t> node_sets_size;
        expand_size(node_sets_size, this->node_sets_);
        expand_size(node_sets_size, other.node_sets_);
        for (auto & [id, n] : node_sets_size)
            node_sets[id].reserve(n);

        for (auto & [id, ns] : this->node_sets_)
            append(node_sets[id], ns);
        for (auto & [id, ns] : other.node_sets_) {
            auto ns_shifted = ns;
            for (auto & idx : ns_shifted)
                idx += n_pt_ofst;
            append(node_sets[id], ns_shifted);
        }

        for (auto & id : other.node_set_ids()) {
            auto name = other.node_set_name(id);
            if (name.has_value()) {
                auto my_name = node_set_name(id);
                if (my_name.has_value())
                    this->node_set_names_[id] = name.value();
                else if (name != my_name)
                    Log::warn("Node set with id={} already exists, but with a different name '{}'",
                              id,
                              name.value());
            }
        }
    }
    this->node_sets_ = node_sets;

    return *this;
}

Mesh &
Mesh::remove_duplicate_points(double tolerance)
{
    Log::info("Removing duplicates: tolerance={}", tolerance);
    LoggingTimer timer;

    PointCloud cloud(*this);
    auto [unique_points, point_map] = remove_duplicates(cloud, tolerance);
    this->pnts_ = unique_points;
    for (auto & elem : this->elems_) {
        auto & ids = elem.vtx_id_;
        for (auto & id : ids)
            id = point_map[id];
    }

    return *this;
}

Ptr<Mesh>
Mesh::duplicate() const
{
    auto dup = Ptr<Mesh>::alloc(this->pnts_, this->elems_);
    dup->cell_sets_ = this->cell_sets_;
    dup->side_sets_ = this->side_sets_;
    dup->node_sets_ = this->node_sets_;
    return dup;
}

Mesh &
Mesh::set_cell_set_name(Marker cell_set_id, const std::string & name)
{
    this->cell_set_names_[cell_set_id] = name;
    return *this;
}

Optional<std::string>
Mesh::cell_set_name(Marker cell_set_id) const
{
    try {
        return this->cell_set_names_.at(cell_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::nullopt;
    }
}

std::vector<Marker>
Mesh::cell_set_ids() const
{
    return utils::map_keys(this->cell_sets_);
}

Span<const Index>
Mesh::cell_set(Marker id) const
{
    try {
        return this->cell_sets_.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Cell set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_cell_set(Marker id, const std::vector<Index> & cell_ids)
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
Mesh::set_side_set_name(Marker side_set_id, const std::string & name)
{
    this->side_set_names_[side_set_id] = name;
    return *this;
}

Optional<std::string>
Mesh::side_set_name(Marker side_set_id) const
{
    try {
        return this->side_set_names_.at(side_set_id);
    }
    catch (const std::out_of_range & e) {
        return std::nullopt;
    }
}

std::vector<Marker>
Mesh::side_set_ids() const
{
    return utils::map_keys(this->side_sets_);
}

Span<const SideEntry>
Mesh::side_set(Marker id) const
{
    try {
        return this->side_sets_.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Side set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_side_set(Marker id, const std::vector<SideEntry> & side_set)
{
    this->side_sets_[id] = side_set;
    return *this;
}

Mesh &
Mesh::remove_side_sets()
{
    this->side_sets_.clear();
    this->side_set_names_.clear();
    return *this;
}

Mesh &
Mesh::set_node_set_name(Marker id, const std::string & name)
{
    this->node_set_names_[id] = name;
    return *this;
}

Optional<std::string>
Mesh::node_set_name(Marker id) const
{
    auto it = this->node_set_names_.find(id);
    if (it != this->node_set_names_.end())
        return it->second;
    else
        return std::nullopt;
}

std::vector<Marker>
Mesh::node_set_ids() const
{
    return utils::map_keys(this->node_sets_);
}

Span<const Index>
Mesh::node_set(Marker id) const
{
    try {
        return this->node_sets_.at(id);
    }
    catch (const std::out_of_range & e) {
        throw Exception("Node set ID {} does not exist", id);
    }
}

Mesh &
Mesh::set_node_set(Marker id, const std::vector<Index> & node_ids)
{
    this->node_sets_[id] = node_ids;
    return *this;
}

Mesh &
Mesh::remove_node_sets()
{
    this->node_sets_.clear();
    this->node_set_names_.clear();
    return *this;
}

Mesh &
Mesh::remap_block_ids(const std::map<Marker, Marker> & block_map)
{
    Log::info("Remapping block IDs:");
    for (auto & [block_id, new_block_id] : block_map)
        Log::info("- {} -> {}", block_id, new_block_id);

    std::map<Marker, std::string> new_cell_set_names;
    std::map<Marker, std::vector<Index>> new_cell_sets;
    for (auto & [block_id, cells] : this->cell_sets_) {
        auto new_block_id = block_map.at(block_id);
        new_cell_sets[new_block_id] = cells;
        auto cs_name = cell_set_name(block_id);
        if (cs_name.has_value())
            new_cell_set_names[new_block_id] = cs_name.value();
    }
    this->cell_sets_ = new_cell_sets;
    this->cell_set_names_ = new_cell_set_names;
    return *this;
}

Span<const Index>
Mesh::support(Index index) const
{
    return this->hasse_.in_vertices(index);
}

Span<const Index>
Mesh::cone(Index index) const
{
    return this->hasse_.out_vertices(index);
}

std::set<Index>
Mesh::cone_vertices(Index index) const
{
    std::list<Index> pts_to_process;
    for (auto & v : cone(index))
        pts_to_process.push_back(v);

    std::set<Index> verts;
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
Mesh::element_type(Index index) const
{
    return this->elems_.at(index).type();
}

void
Mesh::set_up()
{
    this->hasse_ = HasseDiagram(*this);
}

std::vector<Index>
Mesh::boundary_edges() const
{
    return boundary_entities(*this, this->hasse_.edges());
}

std::vector<Index>
Mesh::boundary_faces() const
{
    return boundary_entities(*this, this->hasse_.faces());
}

Point
Mesh::compute_centroid(Index index) const
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
Mesh::outward_normal(Index index) const
{
    auto supp = support(index);
    if (supp.size() != 1)
        throw Exception("Normals are supported only for boundaries");

    auto cell_id = supp[0];
    auto cell_ctr = compute_centroid(cell_id);

    if (this->hasse_.vertices().contains(index)) {
        throw Exception("Normals are not supported for points, yet");
    }
    else if (this->hasse_.edges().contains(index)) {
        auto connect_verts = cone_vertices(index);
        std::vector<Index> verts(connect_verts.begin(), connect_verts.end());
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
    else if (this->hasse_.faces().contains(index)) {
        auto side_ctr = compute_centroid(index);

        auto connect_verts = cone_vertices(index);
        std::vector<Index> verts(connect_verts.begin(), connect_verts.end());
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
    else {
        throw Exception("Normals are not supported for cells");
    }
}

BoundingBox3D
compute_bounding_box(const Mesh & mesh)
{
    BoundingBox3D bbox;
    for (const auto & pt : mesh.points())
        bbox += pt;
    return bbox;
}

BoundingBox3D
compute_bounding_box(Ptr<const Mesh> mesh)
{
    return compute_bounding_box(*mesh);
}

//

std::tuple<std::vector<Point>, std::map<Ptr<MeshVertexAbstract>, Index>>
build_points(const GeomModel & model)
{
    Log::debug("Building points");

    std::vector<Point> pnts;
    std::size_t sz = model.vertices().size();
    pnts.reserve(sz);
    for (auto & [_, curve] : model.curves())
        sz += curve->curve_vertices().size();
    for (auto & [id, surface] : model.surfaces())
        sz += surface->surface_vertices().size();
    std::map<Ptr<MeshVertexAbstract>, Index> vtx_map;
    Index gid = 0;

    for (auto & [id, v] : model.vertices()) {
        vtx_map.emplace(v, gid);
        pnts.emplace_back(v->point());
        gid++;
    }
    for (auto & [id, curve] : model.curves())
        for (auto & v : curve->curve_vertices()) {
            vtx_map.emplace(v, gid);
            pnts.emplace_back(v->point());
            gid++;
        }
    for (auto & [id, surface] : model.surfaces())
        for (auto & v : surface->surface_vertices()) {
            vtx_map.emplace(v, gid);
            pnts.emplace_back(v->point());
            gid++;
        }
    return { pnts, vtx_map };
}

std::vector<Element>
build_1d_elements(const GeomModel & model, const std::map<Ptr<MeshVertexAbstract>, Index> & vtx_map)
{
    Log::debug("Building 1D elements");

    std::vector<Element> elems;
    std::size_t sz = 0;
    for (auto & [id, curve] : model.curves())
        sz += curve->segments().size();
    elems.reserve(sz);
    for (auto & [id, curve] : model.curves()) {
        std::array<Index, Line2::N_VERTICES> line;
        for (auto & local_elem : curve->segments()) {
            for (auto i : make_range(Line2::N_VERTICES)) {
                auto vtx = local_elem.vertex(i);
                auto gid = vtx_map.at(vtx);
                line[i] = gid;
            }
            elems.emplace_back(Element::Line2(line));
        }
    }
    return elems;
}

std::vector<Element>
build_2d_elements(const GeomModel & model, const std::map<Ptr<MeshVertexAbstract>, Index> & vtx_map)
{
    Log::debug("Building 2D elements");

    std::vector<Element> elems;
    std::size_t sz = 0;
    for (auto & [id, surface] : model.surfaces()) {
        sz += surface->triangles().size();
        sz += surface->quadrangles().size();
    }
    elems.reserve(sz);
    for (auto & [id, surface] : model.surfaces()) {
        std::array<Index, Tri3::N_VERTICES> tri;
        for (auto & local_elem : surface->triangles()) {
            for (auto i : make_range(Tri3::N_VERTICES)) {
                auto vtx = local_elem.vertex(i);
                auto gid = vtx_map.at(vtx);
                tri[i] = gid;
            }
            elems.emplace_back(Element::Tri3(tri));
        }

        std::array<Index, Quad4::N_VERTICES> quad;
        for (auto & local_elem : surface->quadrangles()) {
            for (auto i : make_range(Quad4::N_VERTICES)) {
                auto vtx = local_elem.vertex(i);
                auto gid = vtx_map.at(vtx);
                quad[i] = gid;
            }
            elems.emplace_back(Element::Quad4(quad));
        }
    }
    return elems;
}

std::vector<Element>
build_3d_elements(const GeomModel & model, const std::map<Ptr<MeshVertexAbstract>, Index> & vtx_map)
{
    Log::debug("Building 3D elements");

    std::vector<Element> elems;
    std::size_t sz = 0;
    for (auto & [id, volume] : model.volumes()) {
        sz += volume->tetrahedra().size();
    }
    elems.reserve(sz);
    for (auto & [id, volume] : model.volumes()) {
        std::array<Index, Tetra4::N_VERTICES> tet;
        for (auto & local_elem : volume->tetrahedra()) {
            for (auto i : make_range(Tetra4::N_VERTICES)) {
                auto vtx = local_elem.vertex(i);
                auto gid = vtx_map.at(vtx);
                tet[i] = gid;
            }
            elems.emplace_back(Element::Tetra4(tet));
        }
    }
    return elems;
}

std::vector<Element>
build_elements(const GeomModel & model, const std::map<Ptr<MeshVertexAbstract>, Index> & vtx_map)
{
    Log::debug("Building elements");

    auto bbox = compute_bounding_box(model);
    auto dim = determine_spatial_dim(bbox);

    if (dim == 1)
        return build_1d_elements(model, vtx_map);
    else if (dim == 2) {
        if (model.surfaces().size() > 0)
            return build_2d_elements(model, vtx_map);
        else
            return build_1d_elements(model, vtx_map);
    }
    else if (dim == 3)
        return build_3d_elements(model, vtx_map);
    else
        throw Exception("Element construction for your setup is not implemented yet");
}

Ptr<Mesh>
build_mesh(const GeomModel & model)
{
    Log::debug("Building mesh");

    auto [points, vtx_map] = build_points(model);
    auto elements = build_elements(model, vtx_map);
    auto mesh = Ptr<Mesh>::alloc(points, elements);
    // TODO: create cell sets
    // TODO: create side sets
    // TODO: create node sets
    return mesh;
}

} // namespace krado
