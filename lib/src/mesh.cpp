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

// Element reversal

// this defines how to "reverse"/permutate an element that was mirrored
template <ElementType ET>
struct ElementPermutation;

template <>
struct ElementPermutation<ElementType::TRI3> {
    static constexpr std::array<Index, 3> PERMUTATION = { 0, 2, 1 };
};

template <>
struct ElementPermutation<ElementType::QUAD4> {
    static constexpr std::array<Index, 4> PERMUTATION = { 0, 3, 2, 1 };
};

template <>
struct ElementPermutation<ElementType::TETRA4> {
    static constexpr std::array<Index, 4> PERMUTATION = { 0, 1, 3, 2 };
};

template <>
struct ElementPermutation<ElementType::PYRAMID5> {
    static constexpr std::array<Index, 5> PERMUTATION = { 0, 3, 2, 1, 4 };
};

template <>
struct ElementPermutation<ElementType::PRISM6> {
    static constexpr std::array<Index, 6> PERMUTATION = { 0, 2, 1, 3, 5, 4 };
};

template <>
struct ElementPermutation<ElementType::HEX8> {
    static constexpr std::array<Index, 8> PERMUTATION = { 0, 3, 2, 1, 4, 7, 6, 5 };
};

template <ElementType ET>
Element
reverse_element_impl(const Element & elem)
{
    assert(elem.type() == ET);

    constexpr auto N_VERTICES = ElementSelector<ET>::N_VERTICES;
    constexpr auto perm_idxs = ElementPermutation<ET>::PERMUTATION;
    auto idxs = permutate<Index, N_VERTICES>(elem.indices(), perm_idxs);
    return Element::create<ET>(idxs);
}

Element
reverse_element(const Element & elem)
{
    switch (elem.type()) {
    case ElementType::POINT:
        return elem;

    case ElementType::LINE2:
        return elem;

    case ElementType::TRI3:
        return reverse_element_impl<ElementType::TRI3>(elem);

    case ElementType::QUAD4:
        return reverse_element_impl<ElementType::QUAD4>(elem);

    case ElementType::TETRA4:
        return reverse_element_impl<ElementType::TETRA4>(elem);

    case ElementType::PYRAMID5:
        return reverse_element_impl<ElementType::PYRAMID5>(elem);

    case ElementType::PRISM6:
        return reverse_element_impl<ElementType::PRISM6>(elem);

    case ElementType::HEX8:
        return reverse_element_impl<ElementType::HEX8>(elem);

    default:
        throw Exception("`reverse_element` not implemented for {}", utils::to_str(elem.type()));
    }
}

std::size_t
mirror_local_side(ElementType et, std::size_t side)
{
    switch (et) {
    case ElementType::TRI3: {
        static constexpr std::array<std::size_t, 3> mapping = { 2, 1, 0 };
        return mapping.at(side);
    }
    case ElementType::QUAD4: {
        static constexpr std::array<std::size_t, 4> mapping = { 3, 2, 1, 0 };
        return mapping.at(side);
    }
    case ElementType::TETRA4: {
        static constexpr std::array<std::size_t, 4> mapping = { 1, 0, 2, 3 };
        return mapping.at(side);
    }
    case ElementType::PYRAMID5: {
        static constexpr std::array<std::size_t, 5> mapping = { 0, 4, 3, 2, 1 };
        return mapping.at(side);
    }
    case ElementType::PRISM6: {
        static constexpr std::array<std::size_t, 5> mapping = { 0, 3, 2, 1, 4 };
        return mapping.at(side);
    }
    case ElementType::HEX8: {
        static constexpr std::array<std::size_t, 6> mapping = { 2, 3, 0, 1, 4, 5 };
        return mapping.at(side);
    }
    default:
        return side;
    }
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

Ptr<Mesh>
Mesh::mirrored(const Axis2 & axis) const
{
    std::vector<Point> points;
    points.reserve(this->num_points());
    for (auto & pt : this->points())
        points.push_back(pt.mirrored(axis));

    std::vector<Element> elems;
    elems.reserve(this->num_elements());
    for (auto & elem : this->elements())
        elems.push_back(reverse_element(elem));

    auto mirrored_mesh = Ptr<Mesh>::alloc(std::move(points), std::move(elems));
    mirrored_mesh->cell_sets_ = this->cell_sets_;
    mirrored_mesh->cell_set_names_ = this->cell_set_names_;
    // parmute/correct side sets
    std::map<Marker, std::vector<SideEntry>> side_sets;
    for (auto const & [marker, side_entries] : this->side_sets_) {
        std::vector<SideEntry> mapped_entries;
        mapped_entries.reserve(side_entries.size());
        for (auto const & entry : side_entries) {
            auto et = element_type(entry.elem);
            auto mapped_side = mirror_local_side(et, entry.side);
            mapped_entries.emplace_back(entry.elem, mapped_side);
        }
        side_sets[marker] = std::move(mapped_entries);
    }
    mirrored_mesh->side_sets_ = std::move(side_sets);

    mirrored_mesh->side_set_names_ = this->side_set_names_;
    mirrored_mesh->node_sets_ = this->node_sets_;
    mirrored_mesh->node_set_names_ = this->node_set_names_;
    return mirrored_mesh;
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

    // Calculate final sizes and reserve memory
    std::map<Marker, std::size_t> final_sizes;
    for (auto const & [block_id, cells] : this->cell_sets_) {
        auto it = block_map.find(block_id);
        Marker new_id = (it == block_map.end()) ? block_id : it->second;
        final_sizes[new_id] += cells.size();
    }
    for (auto const & [id, size] : final_sizes)
        new_cell_sets[id].reserve(size);

    // Perform the merge
    for (auto const & [block_id, cells] : this->cell_sets_) {
        auto it = block_map.find(block_id);
        Marker new_id = (it == block_map.end()) ? block_id : it->second;

        new_cell_sets[new_id].insert(new_cell_sets[new_id].end(), cells.begin(), cells.end());

        auto it_name = this->cell_set_names_.find(block_id);
        if (it_name != this->cell_set_names_.end()) {
            // Only set the name for the new block if it hasn't been set yet.
            // First one wins because std::map iterates in sorted key order.
            if (new_cell_set_names.find(new_id) == new_cell_set_names.end()) {
                new_cell_set_names[new_id] = it_name->second;
            }
        }
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

Ptr<Mesh>
build_mesh(const GeomModel & model, const MeshBuildOptions & options)
{
    Log::debug("Building mesh");

    // 1. Determine spatial dimension
    int dim = 1;
    if (options.spatial_dimension.has_value()) {
        dim = options.spatial_dimension.value();
    }
    else {
        auto bbox = compute_bounding_box(model);
        auto sz = bbox.size();
        if (sz[2] > 1e-15) {
            dim = 3;
        }
        else if (sz[1] > 1e-15) {
            dim = 2;
        }
        else {
            dim = 1;
        }
    }

    // 2. Identify all meshed volumes, surfaces, and curves
    std::vector<Ptr<MeshVolume>> meshed_volumes;
    std::vector<Ptr<MeshSurface>> meshed_surfaces;
    std::vector<Ptr<MeshCurve>> meshed_curves;

    for (auto & [id, volume] : model.volumes()) {
        if (volume->is_meshed()) {
            meshed_volumes.push_back(volume);
        }
    }
    for (auto & [id, surface] : model.surfaces()) {
        if (surface->is_meshed()) {
            meshed_surfaces.push_back(surface);
        }
    }
    for (auto & [id, curve] : model.curves()) {
        if (curve->is_meshed()) {
            meshed_curves.push_back(curve);
        }
    }

    // 3. Identify boundary entities to filter out for TopLevelOnly strategy
    std::set<Ptr<MeshSurface>> volume_boundary_surfaces;
    for (auto & volume : meshed_volumes) {
        for (auto & surface : volume->surfaces()) {
            volume_boundary_surfaces.insert(surface);
        }
    }

    std::set<Ptr<MeshCurve>> surface_boundary_curves;
    for (auto & surface : meshed_surfaces) {
        for (auto & curve : surface->curves()) {
            surface_boundary_curves.insert(curve);
        }
    }

    // 4. Select which entities are included in the physical mesh
    std::vector<Ptr<MeshVolume>> included_volumes;
    std::vector<Ptr<MeshSurface>> included_surfaces;
    std::vector<Ptr<MeshCurve>> included_curves;

    if (options.element_selection == MeshBuildOptions::ElementSelection::AllMeshed) {
        included_volumes = meshed_volumes;
        included_surfaces = meshed_surfaces;
        included_curves = meshed_curves;
    }
    else if (options.element_selection == MeshBuildOptions::ElementSelection::MaxDimension) {
        int max_dim = 0;
        if (!meshed_volumes.empty()) {
            max_dim = 3;
        }
        else if (!meshed_surfaces.empty()) {
            max_dim = 2;
        }
        else if (!meshed_curves.empty()) {
            max_dim = 1;
        }

        if (max_dim == 3) {
            included_volumes = meshed_volumes;
        }
        else if (max_dim == 2) {
            included_surfaces = meshed_surfaces;
        }
        else if (max_dim == 1) {
            included_curves = meshed_curves;
        }
    }
    else { // TopLevelOnly
        included_volumes = meshed_volumes;
        for (auto & surface : meshed_surfaces) {
            if (volume_boundary_surfaces.find(surface) == volume_boundary_surfaces.end()) {
                included_surfaces.push_back(surface);
            }
        }
        for (auto & curve : meshed_curves) {
            if (surface_boundary_curves.find(curve) == surface_boundary_curves.end()) {
                included_curves.push_back(curve);
            }
        }
    }

    // 5. Collect referenced vertices
    std::vector<Ptr<MeshVertexAbstract>> referenced_vertices;
    std::set<Ptr<MeshVertexAbstract>> referenced_set;

    auto add_vertex = [&](Ptr<MeshVertexAbstract> vtx) {
        if (referenced_set.insert(vtx).second) {
            referenced_vertices.push_back(vtx);
        }
    };

    for (auto & [id, v] : model.vertices()) {
        add_vertex(v);
    }

    for (auto & volume : included_volumes) {
        for (auto & tet : volume->tetrahedra()) {
            for (int i = 0; i < Tetra4::N_VERTICES; ++i) {
                add_vertex(tet.vertex(i));
            }
        }
    }

    for (auto & surface : included_surfaces) {
        for (auto & tri : surface->triangles()) {
            for (int i = 0; i < Tri3::N_VERTICES; ++i) {
                add_vertex(tri.vertex(i));
            }
        }
        for (auto & quad : surface->quadrangles()) {
            for (int i = 0; i < Quad4::N_VERTICES; ++i) {
                add_vertex(quad.vertex(i));
            }
        }
    }

    for (auto & curve : included_curves) {
        for (auto & seg : curve->segments()) {
            for (int i = 0; i < Line2::N_VERTICES; ++i) {
                add_vertex(seg.vertex(i));
            }
        }
    }

    // 6. Map referenced vertices to global points
    std::vector<Point> points;
    std::map<Ptr<MeshVertexAbstract>, Index> vtx_map;
    points.reserve(referenced_vertices.size());
    Index gid = 0;
    for (auto & vtx : referenced_vertices) {
        vtx_map[vtx] = gid++;
        points.push_back(vtx->point());
    }

    // 7. Create physical elements and build cell sets
    std::vector<Element> elements;
    std::map<Marker, std::vector<Index>> cell_sets;
    std::map<Marker, std::string> cell_set_names;

    Index global_elem_idx = 0;

    for (auto & volume : included_volumes) {
        auto marker_opt = volume->marker();
        if (marker_opt.has_value()) {
            auto marker = marker_opt.value();
            cell_set_names[marker] = model.block_name(marker);
            auto & cell_ids = cell_sets[marker];
            for (auto & local_elem : volume->tetrahedra()) {
                std::array<Index, Tetra4::N_VERTICES> tet;
                for (int i = 0; i < Tetra4::N_VERTICES; ++i) {
                    tet[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Tetra4(tet));
                cell_ids.push_back(global_elem_idx++);
            }
        }
        else {
            for (auto & local_elem : volume->tetrahedra()) {
                std::array<Index, Tetra4::N_VERTICES> tet;
                for (int i = 0; i < Tetra4::N_VERTICES; ++i) {
                    tet[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Tetra4(tet));
                global_elem_idx++;
            }
        }
    }

    for (auto & surface : included_surfaces) {
        auto marker_opt = surface->marker();
        if (marker_opt.has_value()) {
            auto marker = marker_opt.value();
            cell_set_names[marker] = model.block_name(marker);
            auto & cell_ids = cell_sets[marker];
            for (auto & local_elem : surface->triangles()) {
                std::array<Index, Tri3::N_VERTICES> tri;
                for (int i = 0; i < Tri3::N_VERTICES; ++i) {
                    tri[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Tri3(tri));
                cell_ids.push_back(global_elem_idx++);
            }
            for (auto & local_elem : surface->quadrangles()) {
                std::array<Index, Quad4::N_VERTICES> quad;
                for (int i = 0; i < Quad4::N_VERTICES; ++i) {
                    quad[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Quad4(quad));
                cell_ids.push_back(global_elem_idx++);
            }
        }
        else {
            for (auto & local_elem : surface->triangles()) {
                std::array<Index, Tri3::N_VERTICES> tri;
                for (int i = 0; i < Tri3::N_VERTICES; ++i) {
                    tri[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Tri3(tri));
                global_elem_idx++;
            }
            for (auto & local_elem : surface->quadrangles()) {
                std::array<Index, Quad4::N_VERTICES> quad;
                for (int i = 0; i < Quad4::N_VERTICES; ++i) {
                    quad[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Quad4(quad));
                global_elem_idx++;
            }
        }
    }

    for (auto & curve : included_curves) {
        auto marker_opt = curve->marker();
        if (marker_opt.has_value()) {
            auto marker = marker_opt.value();
            cell_set_names[marker] = model.block_name(marker);
            auto & cell_ids = cell_sets[marker];
            for (auto & local_elem : curve->segments()) {
                std::array<Index, Line2::N_VERTICES> line;
                for (int i = 0; i < Line2::N_VERTICES; ++i) {
                    line[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Line2(line));
                cell_ids.push_back(global_elem_idx++);
            }
        }
        else {
            for (auto & local_elem : curve->segments()) {
                std::array<Index, Line2::N_VERTICES> line;
                for (int i = 0; i < Line2::N_VERTICES; ++i) {
                    line[i] = vtx_map.at(local_elem.vertex(i));
                }
                elements.emplace_back(Element::Line2(line));
                global_elem_idx++;
            }
        }
    }

    auto mesh = Ptr<Mesh>::alloc(points, elements);
    mesh->set_up();

    for (auto const & [marker, cell_ids] : cell_sets) {
        mesh->set_cell_set(marker, cell_ids);
    }
    for (auto const & [marker, name] : cell_set_names) {
        if (!name.empty()) {
            mesh->set_cell_set_name(marker, name);
        }
    }

    // 8. Create Node Sets
    std::map<Marker, std::vector<Index>> node_sets;
    std::map<Marker, std::string> node_set_names;

    for (auto & [id, vertex] : model.vertices()) {
        auto marker_opt = vertex->marker();
        if (marker_opt.has_value()) {
            auto marker = marker_opt.value();
            if (vtx_map.find(vertex) != vtx_map.end()) {
                node_sets[marker].push_back(vtx_map.at(vertex));
                node_set_names[marker] = model.node_set_name(marker);
            }
        }
    }

    for (auto const & [marker, node_ids] : node_sets) {
        mesh->set_node_set(marker, node_ids);
    }
    for (auto const & [marker, name] : node_set_names) {
        if (!name.empty()) {
            mesh->set_node_set_name(marker, name);
        }
    }

    // 9. Create Side Sets
    std::map<Index, std::vector<Index>> vertex_to_elements_map;
    for (Index i = 0; i < elements.size(); ++i) {
        for (auto v_idx : elements[i].indices()) {
            vertex_to_elements_map[v_idx].push_back(i);
        }
    }

    std::map<Marker, std::vector<SideEntry>> side_sets;
    std::map<Marker, std::string> side_set_names;

    if (!included_volumes.empty()) {
        for (auto & [id, surface] : model.surfaces()) {
            auto marker_opt = surface->marker();
            if (marker_opt.has_value()) {
                auto marker = marker_opt.value();
                auto & sset = side_sets[marker];
                side_set_names[marker] = model.side_set_name(marker);

                for (auto & tri : surface->triangles()) {
                    if (vtx_map.find(tri.vertex(0)) == vtx_map.end() ||
                        vtx_map.find(tri.vertex(1)) == vtx_map.end() ||
                        vtx_map.find(tri.vertex(2)) == vtx_map.end()) {
                        continue;
                    }
                    Index v0_id = vtx_map.at(tri.vertex(0));
                    Index v1_id = vtx_map.at(tri.vertex(1));
                    Index v2_id = vtx_map.at(tri.vertex(2));

                    bool found = false;
                    for (auto elem_idx : vertex_to_elements_map[v0_id]) {
                        const auto & element = elements[elem_idx];
                        if (element.type() == ElementType::TETRA4) {
                            const auto & elem_verts = element.indices();
                            for (u8 f_idx = 0; f_idx < Tetra4::N_FACES; ++f_idx) {
                                Index f_v0 = elem_verts[Tetra4::FACE_VERTICES[f_idx][0]];
                                Index f_v1 = elem_verts[Tetra4::FACE_VERTICES[f_idx][1]];
                                Index f_v2 = elem_verts[Tetra4::FACE_VERTICES[f_idx][2]];

                                std::set<Index> face_set = { f_v0, f_v1, f_v2 };
                                std::set<Index> tri_set = { v0_id, v1_id, v2_id };
                                if (face_set == tri_set) {
                                    sset.emplace_back(elem_idx, f_idx);
                                    found = true;
                                    break;
                                }
                            }
                        }
                        if (found)
                            break;
                    }
                }
            }
        }
    }
    else if (!included_surfaces.empty()) {
        for (auto & [id, curve] : model.curves()) {
            auto marker_opt = curve->marker();
            if (marker_opt.has_value()) {
                auto marker = marker_opt.value();
                auto & sset = side_sets[marker];
                side_set_names[marker] = model.side_set_name(marker);

                for (auto & seg : curve->segments()) {
                    if (vtx_map.find(seg.vertex(0)) == vtx_map.end() ||
                        vtx_map.find(seg.vertex(1)) == vtx_map.end()) {
                        continue;
                    }
                    Index v0_id = vtx_map.at(seg.vertex(0));
                    Index v1_id = vtx_map.at(seg.vertex(1));

                    bool found = false;
                    for (auto elem_idx : vertex_to_elements_map[v0_id]) {
                        const auto & element = elements[elem_idx];
                        if (element.type() == ElementType::TRI3) {
                            const auto & elem_verts = element.indices();
                            for (u8 e_idx = 0; e_idx < Tri3::N_EDGES; ++e_idx) {
                                Index e_v0 = elem_verts[Tri3::EDGE_VERTICES[e_idx][0]];
                                Index e_v1 = elem_verts[Tri3::EDGE_VERTICES[e_idx][1]];
                                if ((e_v0 == v0_id && e_v1 == v1_id) ||
                                    (e_v0 == v1_id && e_v1 == v0_id)) {
                                    sset.emplace_back(elem_idx, e_idx);
                                    found = true;
                                    break;
                                }
                            }
                        }
                        else if (element.type() == ElementType::QUAD4) {
                            const auto & elem_verts = element.indices();
                            for (u8 e_idx = 0; e_idx < Quad4::N_EDGES; ++e_idx) {
                                Index e_v0 = elem_verts[Quad4::EDGE_VERTICES[e_idx][0]];
                                Index e_v1 = elem_verts[Quad4::EDGE_VERTICES[e_idx][1]];
                                if ((e_v0 == v0_id && e_v1 == v1_id) ||
                                    (e_v0 == v1_id && e_v1 == v0_id)) {
                                    sset.emplace_back(elem_idx, e_idx);
                                    found = true;
                                    break;
                                }
                            }
                        }
                        if (found)
                            break;
                    }
                }
            }
        }
    }
    else if (!included_curves.empty()) {
        for (auto & [id, vertex] : model.vertices()) {
            auto marker_opt = vertex->marker();
            if (marker_opt.has_value()) {
                auto marker = marker_opt.value();
                if (vtx_map.find(vertex) == vtx_map.end()) {
                    continue;
                }
                Index vertex_gidx = vtx_map.at(vertex);
                auto & sset = side_sets[marker];
                side_set_names[marker] = model.side_set_name(marker);

                if (vertex_to_elements_map.find(vertex_gidx) != vertex_to_elements_map.end()) {
                    const auto & candidate_elems = vertex_to_elements_map[vertex_gidx];
                    if (!candidate_elems.empty()) {
                        Index elem_idx = candidate_elems[0];
                        const auto & element = elements[elem_idx];
                        if (element.type() == ElementType::LINE2) {
                            int local_side = -1;
                            if (element.index(0) == vertex_gidx) {
                                local_side = 0;
                            }
                            else if (element.index(1) == vertex_gidx) {
                                local_side = 1;
                            }
                            if (local_side != -1) {
                                sset.emplace_back(elem_idx, local_side);
                            }
                        }
                    }
                }
            }
        }
    }

    for (auto const & [marker, sset_entries] : side_sets) {
        mesh->set_side_set(marker, sset_entries);
    }
    for (auto const & [marker, name] : side_set_names) {
        if (!name.empty()) {
            mesh->set_side_set_name(marker, name);
        }
    }

    return mesh;
}

} // namespace krado
