// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/exodusii_file.h"
#include "krado/exception.h"
#include "krado/mesh.h"
#include "krado/element.h"
#include "krado/config.h"
#include "krado/mesh_element.h"
#include "krado/utils.h"
#include "krado/log.h"
#include "fmt/format.h"
#include "fmt/chrono.h"

namespace krado {

namespace exII {

// NOTE: functions in this namespace should be called by the write_ methods, since they
// are providing their outputs in ExodusII indexing

/// Get ExodusII element name
///
/// @param et Element type
/// @return ExodusII element name
const char *
element_name(ElementType et)
{
    switch (et) {
    case ElementType::LINE2:
        return "BAR2";
    case ElementType::TRI3:
        return "TRI3";
    case ElementType::QUAD4:
        return "QUAD4";
    case ElementType::TETRA4:
        return "TET4";
    case ElementType::PYRAMID5:
        return "PYRAMID5";
    case ElementType::PRISM6:
        return "WEDGE6";
    case ElementType::HEX8:
        return "HEX8";
    default:
        break;
    }
    throw Exception("Unsupported element type {}.", utils::to_str(et));
}

/// Convert krado element to ExodusII element
///
/// @param el Element (krado)
/// @return Element connectivity (ExodusII)
std::vector<int>
build_element(const Element & el)
{
    std::vector<int> connect(el.ids().size());
    for (int j = 0; j < el.ids().size(); ++j)
        connect[j] = el.vertex_id(j) + 1;
    return connect;
}

/// Remap krado side index to ExodusII side index
///
/// @param et Element type
/// @param idx Local side index (krado)
/// @return Local side index (ExodusII)
int
local_side_index(ElementType et, int idx)
{
    if (utils::in(et, { ElementType::LINE2, ElementType::TRI3, ElementType::QUAD4 }))
        return idx + 1;
    else if (et == ElementType::TETRA4) {
        std::array<int, 4> sides = { 4, 1, 2, 3 };
        return sides[idx];
    }
    else if (et == ElementType::PYRAMID5) {
        std::array<int, 5> sides = { 5, 1, 2, 3, 4 };
        return sides[idx];
    }
    else if (et == ElementType::PRISM6) {
        std::array<int, 5> sides = { 4, 1, 2, 3, 5 };
        return sides[idx];
    }
    else if (et == ElementType::HEX8) {
        std::array<int, 6> sides = { 1, 3, 4, 2, 5, 6 };
        return sides[idx];
    }
    else
        throw Exception("Unsupported element type {}", Element::type(et));
}

} // namespace exII

/// Get krado element type from ExodusII element type
///
/// @param elem_type_name ExodusII element type name
/// @return Element type (krado)
ElementType
element_type(const std::string elem_type_name)
{
    auto ellc = utils::to_lower(elem_type_name);
    if (utils::in(ellc, { "circle", "sphere" }))
        return ElementType::POINT;
    else if (utils::in(ellc, { "bar", "bar2" }))
        return ElementType::LINE2;
    else if (utils::in(ellc, { "tri3", "tri" }))
        return ElementType::TRI3;
    else if (utils::in(ellc, { "quad", "quad4" }))
        return ElementType::QUAD4;
    else if (utils::in(ellc, { "tet", "tet4", "tetra", "tetra4" }))
        return ElementType::TETRA4;
    else if (utils::in(ellc, { "pyramid", "pyramid5", "pyr5" }))
        return ElementType::PYRAMID5;
    else if (utils::in(ellc, { "wedge", "wedge6" }))
        return ElementType::PRISM6;
    else if (utils::in(ellc, { "hex", "hex8" }))
        return ElementType::HEX8;
    else
        throw std::runtime_error("Unsupported element type: " + elem_type_name);
}

/// Construct krado element connectivity from ExodusII element connectivity
///
/// @param connect Element connectivity (ExodusII)
/// @param n_elem_nodes Number of nodes per element
/// @return Element vertex indices (krado)
std::vector<gidx_t>
build_element(const int * connect, int n_elem_nodes)
{
    std::vector<gidx_t> elem_connect(n_elem_nodes);
    // krado local vertex indices match ExodusII local vertex indices (they just start from 0)
    for (int i = 0; i < n_elem_nodes; ++i, ++connect)
        elem_connect[i] = *connect - 1;
    return elem_connect;
}

/// Get krado local side index
///
/// @param et Element type
/// @param idx Local side index (ExodusII)
/// @return Element local side index (krado)
std::size_t
local_side_index(ElementType et, int idx)
{
    if (utils::in(et, { ElementType::LINE2, ElementType::TRI3, ElementType::QUAD4 }))
        return idx - 1;
    else if (et == ElementType::TETRA4) {
        std::array<int, 4> sides = { 1, 2, 3, 0 };
        return sides[idx - 1];
    }
    else if (et == ElementType::PYRAMID5) {
        std::array<int, 5> sides = { 1, 2, 3, 4, 0 };
        return sides[idx - 1];
    }
    else if (et == ElementType::PRISM6) {
        std::array<int, 5> sides = { 1, 2, 3, 0, 4 };
        return sides[idx - 1];
    }
    else if (et == ElementType::HEX8) {
        std::array<int, 6> sides = { 0, 3, 1, 2, 4, 5 };
        return sides[idx - 1];
    }
    else {
        throw Exception("Unsupported element type {}", Element::type(et));
    }
}

//

ExodusIIFile::ExodusIIFile(const std::string & file_name) : fn_(file_name) {}

Mesh
ExodusIIFile::read()
{
    Log::info("Reading ExodusII file '{}'", this->fn_);

    this->exo_.open(this->fn_);
    this->exo_.init();
    auto pnts = read_points();
    auto [elems, cell_sets] = read_elements();
    auto side_sets = read_side_sets(elems);
    auto cell_set_names = this->exo_.read_block_names();
    auto side_set_names = this->exo_.read_side_set_names();
    this->exo_.close();

    Mesh mesh(pnts, elems);
    for (auto & [id, cs] : cell_sets)
        mesh.set_cell_set(id, cs);
    for (auto [id, name] : cell_set_names)
        mesh.set_cell_set_name(id, cell_set_names[id]);

    for (auto & [id, ss] : side_sets)
        mesh.set_side_set(id, ss);
    for (auto [id, name] : side_set_names)
        mesh.set_side_set_name(id, side_set_names[id]);

    return mesh;
}

std::vector<Point>
ExodusIIFile::read_points()
{
    std::vector<Point> points;

    this->exo_.read_coords();
    int dim = this->exo_.get_dim();
    int n_nodes = this->exo_.get_num_nodes();
    if (dim == 1) {
        auto x = this->exo_.get_x_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i]);
    }
    else if (dim == 2) {
        auto x = this->exo_.get_x_coords();
        auto y = this->exo_.get_y_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i], y[i]);
    }
    else if (dim == 3) {
        auto x = this->exo_.get_x_coords();
        auto y = this->exo_.get_y_coords();
        auto z = this->exo_.get_z_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i], y[i], z[i]);
    }
    return points;
}

std::tuple<std::vector<Element>, std::map<int, std::vector<gidx_t>>>
ExodusIIFile::read_elements()
{
    this->exo_.read_blocks();

    int n = 0;
    for (auto & eb : this->exo_.get_element_blocks())
        n += eb.get_num_elements();
    std::vector<Element> elems;
    elems.reserve(n);

    std::map<int, std::vector<gidx_t>> cell_sets;
    for (auto & eb : this->exo_.get_element_blocks()) {
        auto et = element_type(eb.get_element_type());
        auto connect = eb.get_connectivity();
        auto n_elem_nodes = eb.get_num_nodes_per_element();
        auto blk_id = eb.get_id();
        for (int i = 0; i < eb.get_num_elements(); i++) {
            auto idx = i * n_elem_nodes;
            cell_sets[blk_id].push_back(elems.size());
            elems.emplace_back(et, build_element(connect.data() + idx, n_elem_nodes));
        }
    }

    return { elems, cell_sets };
}

std::map<int, std::vector<side_set_entry_t>>
ExodusIIFile::read_side_sets(const std::vector<Element> & elems)
{
    std::map<int, std::vector<side_set_entry_t>> side_sets;
    this->exo_.read_side_sets();
    for (auto & ss : this->exo_.get_side_sets()) {
        auto id = ss.get_id();
        auto & elem_ids = ss.get_element_ids();
        auto & sides = ss.get_side_ids();
        side_sets[id].reserve(ss.get_size());
        for (auto i = 0; i < ss.get_size(); ++i) {
            auto eid = elem_ids[i] - 1;
            auto et = elems[eid].type();
            auto lsi = local_side_index(et, sides[i]);
            side_sets[id].emplace_back(eid, lsi);
        }
    }
    return side_sets;
}

void
ExodusIIFile::write(const Mesh & mesh)
{
    Log::info("Writing ExodusII file '{}'", this->fn_);

    this->exo_.create(this->fn_);
    this->dim_ = determine_spatial_dim(mesh);

    int n_nodes = (int) mesh.points().size();
    int n_elems = (int) mesh.elements().size();
    int n_elem_blks = mesh.cell_set_ids().empty() ? 1 : mesh.cell_set_ids().size();
    int n_node_sets = mesh.vertex_ids().size();
    int n_side_sets = mesh.side_set_ids().size();
    this->exo_.init("", this->dim_, n_nodes, n_elems, n_elem_blks, n_node_sets, n_side_sets);

    write_info();
    write_coords(mesh);
    write_elements(mesh);
    write_side_sets(mesh);
    write_node_sets(mesh);

    this->exo_.close();
}

int
ExodusIIFile::determine_spatial_dim(const Mesh & mesh)
{
    auto sz = mesh.compute_bounding_box().size();
    if ((sz[0] > 0) && (sz[1] < 1e-15) && (sz[2] < 1e-15))
        return 1;
    else if ((sz[0] > 0) && (sz[1] > 0) && (sz[2] < 1e-15))
        return 2;
    else if ((sz[0] > 0) && (sz[1] > 0) && (sz[2] > 0))
        return 3;
    else
        throw Exception("Unusual mesh, unable to write.");
}

void
ExodusIIFile::write_info()
{
    std::time_t now = std::time(nullptr);
    std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", *std::localtime(&now));

    std::vector<std::string> info(1);
    info[0] = fmt::format("Created by krado v{} on {}", KRADO_VERSION, datetime);
    this->exo_.write_info(info);
}

void
ExodusIIFile::write_coords(const Mesh & mesh)
{
    std::vector<double> x, y, z;
    for (auto & pt : mesh.points()) {
        if (this->dim_ >= 1)
            x.push_back(pt.x);
        if (this->dim_ >= 2)
            y.push_back(pt.y);
        if (this->dim_ >= 3)
            z.push_back(pt.z);
    }

    if (this->dim_ == 1)
        this->exo_.write_coords(x);
    else if (this->dim_ == 2)
        this->exo_.write_coords(x, y);
    else if (this->dim_ == 3)
        this->exo_.write_coords(x, y, z);
    this->exo_.write_coord_names();
}

void
ExodusIIFile::write_elements(const Mesh & mesh)
{
    if (mesh.cell_set_ids().empty()) {
        std::map<ElementType, std::vector<gidx_t>> elem_blks;
        int exii_idx = 1;
        for (gidx_t cell_id = 0; cell_id < mesh.elements().size(); ++cell_id) {
            this->exii_elem_ids_[cell_id] = exii_idx++;
            auto & cell = mesh.element(cell_id);
            auto et = cell.type();
            elem_blks[et].push_back(cell_id);
        }

        int blk_id = 1;
        for (auto & [blk_type, elems] : elem_blks) {
            if (!elems.empty()) {
                auto cell_id = elems[0];
                auto & cell = mesh.element(cell_id);

                auto el_type = exII::element_name(cell.type());
                auto n = cell.num_vertices() * elems.size();
                std::vector<int> connect;
                connect.reserve(n);
                for (auto & cell_id : elems) {
                    auto & el = mesh.element(cell_id);
                    for (auto id : exII::build_element(el))
                        connect.push_back(id);
                }
                this->exo_.write_block(blk_id, el_type, elems.size(), connect);
                blk_id++;
            }
        }
    }
    else {
        // NOTE: krado allows to have different cell types in a single cell set, but not exodusII.
        // So, we need to filter on both cell set id and cell type, i.e. put all cells of the same
        // type in the same block.
        // Currently, we just assume cell sets are homogeneous in terms of cell type.

        int exii_idx = 1;
        std::vector<std::string> blk_names;
        auto cell_set_ids = mesh.cell_set_ids();
        for (auto & blk_id : cell_set_ids) {
            auto & elem_ids = mesh.cell_set(blk_id);
            if (!elem_ids.empty()) {
                const auto & elem = mesh.element(elem_ids[0]);
                auto el_type = exII::element_name(elem.type());
                auto n = elem.num_vertices() * elem_ids.size();
                std::vector<int> connect;
                connect.reserve(n);
                for (auto & cell_id : elem_ids) {
                    this->exii_elem_ids_[cell_id] = exii_idx++;
                    const auto & el = mesh.element(cell_id);
                    for (auto id : exII::build_element(el))
                        connect.push_back(id);
                }
                this->exo_.write_block(blk_id, el_type, elem_ids.size(), connect);
                blk_names.push_back(mesh.cell_set_name(blk_id));
            }
        }

        if (!blk_names.empty())
            this->exo_.write_block_names(blk_names);
    }
}

void
ExodusIIFile::write_side_sets(const Mesh & mesh)
{
    std::vector<std::string> side_sets_names;
    auto set_ids = mesh.side_set_ids();
    for (auto & sid : set_ids) {
        std::vector<int> elems;
        std::vector<int> sides;
        auto & entries = mesh.side_set(sid);
        auto n = entries.size();
        elems.reserve(n);
        sides.reserve(n);
        for (auto & en : entries) {
            elems.push_back(this->exii_elem_ids_.at(en.elem));
            auto et = mesh.element_type(en.elem);
            sides.push_back(exII::local_side_index(et, en.side));
        }
        this->exo_.write_side_set(sid, elems, sides);
        side_sets_names.push_back(mesh.side_set_name(sid));
    }

    this->exo_.write_side_set_names(side_sets_names);
}

void
ExodusIIFile::write_node_sets(const Mesh & mesh)
{
    auto rng = mesh.vertex_ids();
    std::vector<std::string> node_set_names;
    auto set_ids = mesh.vertex_set_ids();
    for (auto & id : set_ids) {
        auto & vtx_ids = mesh.vertex_set(id);
        auto n = vtx_ids.size();
        std::vector<int> nodes;
        nodes.reserve(n);
        for (auto & v : vtx_ids)
            nodes.push_back(v - rng.first() + 1);
        this->exo_.write_node_set(id, nodes);
        node_set_names.push_back(mesh.vertex_set_name(id));
    }

    this->exo_.write_node_set_names(node_set_names);
}

} // namespace krado
