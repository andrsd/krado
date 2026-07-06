// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/exodusii_file.h"
#include "krado/exception.h"
#include "krado/mesh.h"
#include "krado/element.h"
#include "krado/types.h"
#include "krado/utils.h"
#include "krado/log.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/timer.h"
#include "fmt/format.h"
#include "fmt/chrono.h"

namespace krado {

namespace exII {

// NOTE: functions in this namespace provide their outputs in ExodusII indexing

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
    std::vector<int> connect(el.indices().size());
    for (std::size_t j = 0; j < el.indices().size(); ++j)
        connect[j] = el.index(j) + 1;
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

/// Convert local exodusII side index into local krado face index
int
local_face_index(ElementType et, int idx)
{
    if (utils::in(et, { ElementType::TRI3, ElementType::QUAD4 }))
        return idx;
    else if (et == ElementType::TETRA4) {
        std::array<int, Tetra4::N_FACES> face = { 1, 2, 3, 0 };
        return face[idx - 1];
    }
    else if (et == ElementType::HEX8) {
        std::array<int, Hex8::N_FACES> face = { 0, 3, 1, 2, 4, 5 };
        return face[idx - 1];
    }
    else if (et == ElementType::PYRAMID5) {
        std::array<int, Pyramid5::N_FACES> face = { 1, 2, 3, 4, 0 };
        return face[idx - 1];
    }
    else if (et == ElementType::PRISM6) {
        std::array<int, Prism6::N_FACES> face = { 1, 2, 3, 0, 4 };
        return face[idx - 1];
    }
    else
        throw Exception("Not implemented");
}

} // namespace exII

namespace {

struct SideSet {
    std::vector<int> elems;
    std::vector<int> sides;
};

using NodeSet = std::vector<int>;

using VertexIdMap = std::map<Ptr<MeshVertexAbstract>, Index>;
using BlocksMap = std::map<Marker, std::vector<Element>>;
using SideSetMap = std::map<Marker, SideSet>;
using NodeSetMap = std::map<Marker, NodeSet>;
using NamesMap = std::map<Marker, std::string>;

std::string
create_name(Marker id, const NamesMap & names)
{
    if (names.contains(id))
        return names.at(id);
    else
        return fmt::format("{}", id);
}

// Helpers for building things from `GeomModel`

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
build_coords(const VertexIdMap & pnt_map, int dim)
{
    std::vector<double> x, y, z;
    int n_nodes = pnt_map.size();
    if (dim >= 1)
        x.resize(n_nodes);
    if (dim >= 2)
        y.resize(n_nodes);
    if (dim >= 3)
        z.resize(n_nodes);
    for (auto & [vtx, id] : pnt_map) {
        auto pt = vtx->point();
        if (dim >= 1)
            x[id] = pt.x;
        if (dim >= 2)
            y[id] = pt.y;
        if (dim >= 3)
            z[id] = pt.z;
    }
    return { x, y, z };
}

VertexIdMap
build_points(const GeomModel & model)
{
    VertexIdMap pnts;
    Index gid = 0;

    for (auto & [id, v] : model.vertices()) {
        auto [_, inserted] = pnts.try_emplace(v, gid);
        if (inserted)
            gid++;
    }
    for (auto & [id, curve] : model.curves())
        for (auto & v : curve->curve_vertices()) {
            auto [_, inserted] = pnts.try_emplace(v, gid);
            if (inserted)
                gid++;
        }
    for (auto & [id, surface] : model.surfaces())
        for (auto & v : surface->surface_vertices()) {
            auto [_, inserted] = pnts.try_emplace(v, gid);
            if (inserted)
                gid++;
        }
    return pnts;
}

template <int N>
std::array<Index, N>
elem2idxs(const MeshElement & mesh_elem, const VertexIdMap & pnt_map)
{
    std::array<Index, N> idxs;
    for (int i = 0; i < N; ++i) {
        auto vtx = mesh_elem.vertex(i);
        idxs[i] = pnt_map.at(vtx);
    }
    return idxs;
}

std::tuple<BlocksMap, NamesMap>
build_1d_blocks(const GeomModel & model, const VertexIdMap & pnt_map)
{
    Log::debug("Building 1D elements");

    BlocksMap blocks;
    NamesMap names;
    for (auto & [id, curve] : model.curves()) {
        auto blk_id = curve->marker().value();
        for (auto & mseg : curve->segments()) {
            auto line = elem2idxs<Line2::N_VERTICES>(mseg, pnt_map);
            blocks[blk_id].emplace_back(Element::Line2(line));
        }
        names[blk_id] = model.block_name(blk_id);
    }
    return { blocks, names };
}

std::tuple<BlocksMap, NamesMap>
build_2d_blocks(const GeomModel & model, const VertexIdMap & pnt_map)
{
    Log::debug("Building 2D elements");

    BlocksMap blocks;
    NamesMap names;
    for (auto & [id, surface] : model.surfaces()) {
        auto blk_id = surface->marker().value();
        auto tris = surface->triangles();
        auto quads = surface->quadrangles();
        if (tris.size() > 0 and quads.size() == 0) {
            for (auto & mt : tris) {
                auto tri = elem2idxs<Tri3::N_VERTICES>(mt, pnt_map);
                blocks[blk_id].emplace_back(Element::Tri3(tri));
            }
            names[blk_id] = model.block_name(blk_id);
        }
        else if (quads.size() > 0 and tris.size() == 0) {
            for (auto & mq : quads) {
                auto quad = elem2idxs<Quad4::N_VERTICES>(mq, pnt_map);
                blocks[blk_id].emplace_back(Element::Quad4(quad));
            }
            names[blk_id] = model.block_name(blk_id);
        }
        else if (quads.size() > 0 and tris.size() > 0) {
            throw Exception("Heterogeneous meshes are not supported, yet");
        }
    }
    return { blocks, names };
}

std::tuple<BlocksMap, NamesMap>
build_3d_blocks(const GeomModel & model, const VertexIdMap & pnt_map)
{
    Log::debug("Building 3D elements");

    BlocksMap blocks;
    NamesMap names;
    for (auto & [id, volume] : model.volumes()) {
        auto blk_id = volume->marker().value();
        auto tets = volume->tetrahedra();
        for (auto & mt : tets) {
            auto tetra = elem2idxs<Tetra4::N_VERTICES>(mt, pnt_map);
            blocks[blk_id].emplace_back(Element::Tetra4(tetra));
        }
        names[blk_id] = model.block_name(blk_id);
    }
    return { blocks, names };
}

std::tuple<BlocksMap, NamesMap>
build_blocks(const GeomModel & model, const VertexIdMap & pnt_map, int dim)
{
    if (dim == 1)
        return build_1d_blocks(model, pnt_map);
    else if (dim == 2)
        return build_2d_blocks(model, pnt_map);
    else if (dim == 3)
        return build_3d_blocks(model, pnt_map);
    else
        throw Exception("Unsupported dimension {}", dim);
}

std::tuple<SideSetMap, NamesMap>
build_side_sets_1d(const GeomModel & model, const BlocksMap & blocks, const VertexIdMap & pnt_map)
{
    SideSetMap side_sets;
    NamesMap names;

    std::map<Index, std::vector<Index>> vertex_to_elements_map;
    std::map<Index, const Element *> global_elem_map;
    Index elem_idx_counter = 0;
    for (const auto & [blk_id, blk_elements] : blocks) {
        for (const auto & element : blk_elements) {
            global_elem_map[elem_idx_counter] = &element;
            for (const auto & vertex_id : element.indices()) {
                vertex_to_elements_map[vertex_id].push_back(elem_idx_counter);
            }
            elem_idx_counter++;
        }
    }

    for (const auto & [id, vertex] : model.vertices()) {
        auto marker = vertex->marker();
        if (marker.has_value()) {
            Marker side_set_id = marker.value();
            names[side_set_id] = model.side_set_name(side_set_id);

            Index vertex_gidx = pnt_map.at(vertex);

            const auto & a_elements = vertex_to_elements_map.find(vertex_gidx);
            if (a_elements != vertex_to_elements_map.end()) {
                const auto & elements = a_elements->second;
                if (!elements.empty()) {
                    Index elem_idx = elements[0];
                    const Element * element = global_elem_map.at(elem_idx);

                    int local_side = -1;
                    if (element->index(0) == vertex_gidx) {
                        local_side = 1;
                    }
                    else if (element->index(1) == vertex_gidx) {
                        local_side = 2;
                    }

                    if (local_side != -1) {
                        side_sets[side_set_id].elems.push_back(elem_idx + 1);
                        side_sets[side_set_id].sides.push_back(local_side);
                    }
                }
            }
        }
    }

    return { side_sets, names };
}

std::tuple<SideSetMap, NamesMap>
build_side_sets_2d(const GeomModel & model, const BlocksMap & blocks, const VertexIdMap & pnt_map)
{
    SideSetMap side_sets;
    NamesMap names;

    std::map<Index, std::vector<Index>> vertex_to_elements_map;
    Index elem_idx_counter = 0;
    for (const auto & [blk_id, blk_elements] : blocks) {
        for (const auto & element : blk_elements) {
            for (const auto & vertex_id : element.indices()) {
                vertex_to_elements_map[vertex_id].push_back(elem_idx_counter);
            }
            elem_idx_counter++;
        }
    }

    Index global_elem_idx = 0;
    std::map<Index, const Element *> global_elem_map;
    for (const auto & [blk_id, blk_elements] : blocks) {
        for (const auto & element : blk_elements) {
            global_elem_map[global_elem_idx++] = &element;
        }
    }

    for (const auto & [id, curve] : model.curves()) {
        auto marker_opt = curve->marker();
        if (!marker_opt.has_value()) {
            continue;
        }
        Marker side_set_id = marker_opt.value();
        names[side_set_id] = model.side_set_name(side_set_id);

        for (const auto & mseg : curve->segments()) {
            auto v0_ptr = mseg.vertex(0);
            auto v1_ptr = mseg.vertex(1);

            Index v0_id = pnt_map.at(v0_ptr);
            Index v1_id = pnt_map.at(v1_ptr);

            const auto & elements_of_v0 = vertex_to_elements_map[v0_id];

            for (Index elem_idx : elements_of_v0) {
                const auto * element = global_elem_map.at(elem_idx);
                bool v1_found = false;
                for (const auto & elem_vertex_id : element->indices()) {
                    if (elem_vertex_id == v1_id) {
                        v1_found = true;
                        break;
                    }
                }

                if (v1_found) {
                    int local_side = -1;
                    auto elem_type = element->type();
                    const auto & elem_vertex_ids = element->indices();

                    if (elem_type == ElementType::TRI3) {
                        auto p0 = elem_vertex_ids[0];
                        auto p1 = elem_vertex_ids[1];
                        auto p2 = elem_vertex_ids[2];
                        if ((v0_id == p0 && v1_id == p1) || (v0_id == p1 && v1_id == p0))
                            local_side = 0;
                        else if ((v0_id == p1 && v1_id == p2) || (v0_id == p2 && v1_id == p1))
                            local_side = 1;
                        else if ((v0_id == p2 && v1_id == p0) || (v0_id == p0 && v1_id == p2))
                            local_side = 2;
                    }
                    else if (elem_type == ElementType::QUAD4) {
                        auto p0 = elem_vertex_ids[0];
                        auto p1 = elem_vertex_ids[1];
                        auto p2 = elem_vertex_ids[2];
                        auto p3 = elem_vertex_ids[3];
                        if ((v0_id == p0 && v1_id == p1) || (v0_id == p1 && v1_id == p0))
                            local_side = 0;
                        else if ((v0_id == p1 && v1_id == p2) || (v0_id == p2 && v1_id == p1))
                            local_side = 1;
                        else if ((v0_id == p2 && v1_id == p3) || (v0_id == p3 && v1_id == p2))
                            local_side = 2;
                        else if ((v0_id == p3 && v1_id == p0) || (v0_id == p0 && v1_id == p3))
                            local_side = 3;
                    }

                    if (local_side != -1) {
                        side_sets[side_set_id].elems.push_back(elem_idx + 1);
                        side_sets[side_set_id].sides.push_back(
                            exII::local_side_index(elem_type, local_side));
                        break;
                    }
                }
            }
        }
    }

    return { side_sets, names };
}

std::tuple<SideSetMap, NamesMap>
build_side_sets(const GeomModel & model,
                const BlocksMap & blocks,
                const VertexIdMap & pnt_map,
                int dim)
{
    if (dim == 1)
        return build_side_sets_1d(model, blocks, pnt_map);
    else if (dim == 2)
        return build_side_sets_2d(model, blocks, pnt_map);
    else
        throw Exception("Building side sets in {}-D is not supported yet", dim);
}

std::tuple<NodeSetMap, NamesMap>
build_node_sets(const GeomModel & model, const VertexIdMap & pnt_map)
{
    NodeSetMap node_sets;
    NamesMap names;

    for (auto & [id, vertex] : model.vertices()) {
        auto marker = vertex->marker();
        if (marker.has_value()) {
            auto ns_id = marker.value();
            auto & node_set = node_sets[ns_id];
            node_set.emplace_back(pnt_map.at(vertex) + 1);

            names[ns_id] = model.node_set_name(ns_id);
        }
    }

    return { node_sets, names };
}

// Helpers for element handling

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
std::vector<Index>
build_element(const int * connect, int n_elem_nodes)
{
    std::vector<Index> elem_connect(n_elem_nodes);
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
        std::array<int, Tetra4::N_FACES> sides = { 1, 2, 3, 0 };
        return sides[idx - 1];
    }
    else if (et == ElementType::PYRAMID5) {
        std::array<int, Pyramid5::N_FACES> sides = { 1, 2, 3, 4, 0 };
        return sides[idx - 1];
    }
    else if (et == ElementType::PRISM6) {
        std::array<int, Prism6::N_FACES> sides = { 1, 2, 3, 0, 4 };
        return sides[idx - 1];
    }
    else if (et == ElementType::HEX8) {
        std::array<int, Hex8::N_FACES> sides = { 0, 3, 1, 2, 4, 5 };
        return sides[idx - 1];
    }
    else {
        throw Exception("Unsupported element type {}", Element::type(et));
    }
}

// Helpers for building things from `Mesh`

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
build_coords(const Mesh & mesh, int dim)
{
    std::vector<double> x, y, z;
    auto n_nodes = mesh.points().size();
    if (dim >= 1) {
        x.reserve(n_nodes);
        for (auto & pt : mesh.points())
            x.push_back(pt.x);
    }
    if (dim >= 2) {
        y.reserve(n_nodes);
        for (auto & pt : mesh.points())
            y.push_back(pt.y);
    }
    if (dim >= 3) {
        z.reserve(n_nodes);
        for (auto & pt : mesh.points())
            z.push_back(pt.z);
    }
    return { x, y, z };
}

std::tuple<BlocksMap, NamesMap>
build_blocks(const Mesh & mesh, std::map<Index, int> & exii_elem_ids)
{
    BlocksMap blocks;
    NamesMap names;

    if (mesh.cell_set_ids().empty()) {
        std::unordered_map<ElementType, std::size_t> elem_blks_size;
        for (Index cell_id = 0; cell_id < mesh.elements().size(); ++cell_id) {
            auto et = mesh.element(cell_id).type();
            elem_blks_size[et]++;
        }
        std::map<ElementType, std::vector<Index>> elem_blks;
        for (auto & [et, size] : elem_blks_size)
            elem_blks[et].reserve(size);
        int exii_idx = 1;
        for (Index cell_id = 0; cell_id < mesh.elements().size(); ++cell_id) {
            exii_elem_ids[cell_id] = exii_idx++;
            auto & cell = mesh.element(cell_id);
            auto et = cell.type();
            elem_blks[et].push_back(cell_id);
        }

        int blk_id = 1;
        for (auto & [blk_type, elems] : elem_blks) {
            if (!elems.empty()) {
                auto & block = blocks[blk_id];
                block.reserve(elems.size());
                for (auto & cell_id : elems) {
                    auto & el = mesh.element(cell_id);
                    block.push_back(el);
                }
                blk_id++;
            }
        }
    }
    else {
        // NOTE: krado allows to have different cell types in a single cell set, but not
        // exodusII. So, we need to filter on both cell set id and cell type, i.e. put all cells
        // of the same type in the same block. Currently, we just assume cell sets are
        // homogeneous in terms of cell type.

        int exii_idx = 1;
        auto cell_set_ids = mesh.cell_set_ids();
        for (auto & blk_id : cell_set_ids) {
            auto elem_ids = mesh.cell_set(blk_id);
            if (!elem_ids.empty()) {
                auto & block = blocks[blk_id];
                block.reserve(elem_ids.size());
                for (auto & id : elem_ids) {
                    exii_elem_ids[id] = exii_idx++;
                    auto & el = mesh.element(id);
                    block.push_back(el);
                }
                auto cs_name = mesh.cell_set_name(blk_id);
                if (cs_name.has_value())
                    names[blk_id] = cs_name.value();
            }
        }
    }

    return { blocks, names };
}

/// Create side set
///
/// @param mesh Mesh object
/// @param elem_ids Face or edge IDs
/// @param exii_elem_ids Map that converts from krado cell IDs to exodus element numbers
SideSet
create_side_set(const Mesh & mesh,
                Span<const SideEntry> side_entries,
                const std::map<Index, int> & exii_elem_ids)
{
    SideSet side_set;
    auto n = side_entries.size();
    side_set.elems.reserve(n);
    side_set.sides.reserve(n);
    for (auto & [cell, side] : side_entries) {
        side_set.elems.push_back(exii_elem_ids.at(cell));
        auto et = mesh.element_type(cell);
        side_set.sides.push_back(exII::local_side_index(et, side));
    }
    return side_set;
}

std::tuple<SideSetMap, NamesMap>
build_side_sets(const Mesh & mesh, const std::map<Index, int> & exii_elem_ids)
{
    SideSetMap side_sets;
    NamesMap names;
    for (auto & id : mesh.side_set_ids()) {
        side_sets[id] = create_side_set(mesh, mesh.side_set(id), exii_elem_ids);
        auto ss_name = mesh.side_set_name(id);
        if (ss_name.has_value())
            names[id] = ss_name.value();
    }
    return { side_sets, names };
}

std::tuple<NodeSetMap, NamesMap>
build_node_sets(const Mesh & mesh)
{
    NodeSetMap node_sets;
    NamesMap names;

    auto set_ids = mesh.node_set_ids();
    for (auto & id : set_ids) {
        auto vtx_ids = mesh.node_set(id);
        auto n = vtx_ids.size();
        auto & nodes = node_sets[id];
        nodes.reserve(n);
        for (auto & v : vtx_ids)
            nodes.push_back(v + 1);
        auto ns_name = mesh.node_set_name(id);
        if (ns_name.has_value())
            names[id] = ns_name.value();
    }

    return { node_sets, names };
}

std::vector<SideEntry>
build_side_set(const Mesh & mesh, const SideSet & side_set)
{
    std::vector<SideEntry> sset;
    auto n = side_set.elems.size();
    sset.reserve(n);
    for (std::size_t i = 0; i < n; i++) {
        Index eid = side_set.elems[i] - 1;
        auto et = mesh.element_type(eid);
        auto lei = local_side_index(et, side_set.sides[i]);
        sset.push_back(SideEntry(eid, lei));
    }
    return sset;
}

std::vector<Index>
build_node_set(const NodeSet & ns)
{
    std::vector<Index> vertex_ids;
    vertex_ids.reserve(ns.size());
    for (auto & id : ns)
        vertex_ids.push_back(id - 1);
    return vertex_ids;
}

// Helpers for writing into the exodusIIfile

void
write_info(exodusIIcpp::File & exo)
{
    std::time_t now = std::time(nullptr);
    std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", *std::localtime(&now));

    std::vector<std::string> info(1);
    info[0] = fmt::format("Created by krado v{} on {}", KRADO_VERSION, datetime);
    exo.write_info(info);
}

/// Write coordinates
void
write_coords(exodusIIcpp::File & exo,
             int dim,
             const std::vector<double> & x,
             const std::vector<double> & y,
             const std::vector<double> & z)
{
    if (dim == 1)
        exo.write_coords(x);
    else if (dim == 2)
        exo.write_coords(x, y);
    else if (dim == 3)
        exo.write_coords(x, y, z);
    exo.write_coord_names();
}

void
write_element_blocks(exodusIIcpp::File & exo, const BlocksMap & blocks, const NamesMap & names)
{
    std::vector<std::string> blk_names;
    for (auto & [blk_id, elems] : blocks) {
        auto & cell = elems[0];
        auto el_type = exII::element_name(cell.type());

        auto n = cell.num_vertices() * elems.size();
        std::vector<int> connect;
        connect.reserve(n);
        for (auto & cell : elems) {
            for (auto id : exII::build_element(cell))
                connect.push_back(id);
        }
        exo.write_block(blk_id, el_type, elems.size(), connect);

        auto name = create_name(blk_id, names);
        blk_names.push_back(name);
    }

    if (!blk_names.empty())
        exo.write_block_names(blk_names);
}

void
write_side_sets(exodusIIcpp::File & exo, const SideSetMap & side_sets, const NamesMap & names)
{
    std::vector<std::string> side_sets_names;

    for (auto & [id, side_set] : side_sets) {
        auto name = create_name(id, names);

        if (side_set.elems.size() > 0) {
            exo.write_side_set(id, side_set.elems, side_set.sides);
            side_sets_names.push_back(name);
        }
        else
            Log::warn("Side set '{}' is empty", name);
    }

    if (!side_sets_names.empty())
        exo.write_side_set_names(side_sets_names);
}

void
write_node_sets(exodusIIcpp::File & exo, const NodeSetMap & node_sets, const NamesMap & names)
{
    std::vector<std::string> node_set_names;

    for (auto & [id, nodes] : node_sets) {
        auto name = create_name(id, names);

        if (nodes.size() > 0) {
            exo.write_node_set(id, nodes);
            node_set_names.push_back(name);
        }
        else
            Log::warn("Node set '{}' is empty", name);
    }

    if (!node_set_names.empty())
        exo.write_node_set_names(node_set_names);
}

// Reading

/// Read nodes
///
/// @return Vector of points
std::vector<Point>
read_points(exodusIIcpp::File & exo)
{
    std::vector<Point> points;

    exo.read_coords();
    int dim = exo.get_dim();
    int n_nodes = exo.get_num_nodes();
    points.reserve(n_nodes);
    if (dim == 1) {
        auto x = exo.get_x_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i]);
    }
    else if (dim == 2) {
        auto x = exo.get_x_coords();
        auto y = exo.get_y_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i], y[i]);
    }
    else if (dim == 3) {
        auto x = exo.get_x_coords();
        auto y = exo.get_y_coords();
        auto z = exo.get_z_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i], y[i], z[i]);
    }
    return points;
}

/// Read elements
///
/// @return Vector of elements
std::tuple<std::vector<Element>, std::map<int, std::vector<Index>>>
read_elements(exodusIIcpp::File & exo)
{
    exo.read_blocks();

    int n = 0;
    for (auto & eb : exo.get_element_blocks())
        n += eb.get_num_elements();
    std::vector<Element> elems;
    elems.reserve(n);

    std::map<int, std::vector<Index>> cell_sets;
    for (auto & eb : exo.get_element_blocks()) {
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

/// Read side sets
std::tuple<SideSetMap, NamesMap>
read_side_sets(exodusIIcpp::File & exo)
{
    SideSetMap side_sets;
    NamesMap side_set_names;
    if (exo.get_num_side_sets() > 0) {
        exo.read_side_sets();
        for (auto & ss : exo.get_side_sets()) {
            auto id = ss.get_id();
            side_sets[id].elems = ss.get_element_ids();
            side_sets[id].sides = ss.get_side_ids();
        }

        side_set_names = exo.read_side_set_names();
    }

    return { side_sets, side_set_names };
}

/// Read node sets
std::tuple<NodeSetMap, NamesMap>
read_node_sets(exodusIIcpp::File & exo)
{
    NodeSetMap node_sets;
    NamesMap node_set_names;
    if (exo.get_num_node_sets() > 0) {
        exo.read_node_sets();
        for (auto & ns : exo.get_node_sets()) {
            auto id = ns.get_id();
            node_sets[id] = ns.get_node_ids();
        }

        node_set_names = exo.read_node_set_names();
    }

    return { node_sets, node_set_names };
}

} // namespace

//

ExodusIIFile::ExodusIIFile(const std::filesystem::path & file_name) : fn_(file_name.string()) {}

Ptr<Mesh>
ExodusIIFile::read()
{
    Log::info("Reading ExodusII file '{}'", this->fn_);
    LoggingTimer timer;

    this->exo_.open(this->fn_);
    this->exo_.init();
    auto pnts = read_points(this->exo_);
    auto [elems, cell_sets] = read_elements(this->exo_);
    auto cell_set_names = this->exo_.read_block_names();
    auto [side_sets, side_set_names] = read_side_sets(this->exo_);
    auto [node_sets, node_set_names] = read_node_sets(this->exo_);

    auto mesh = Ptr<Mesh>::alloc(pnts, elems);
    for (auto & [id, cs] : cell_sets)
        mesh->set_cell_set(id, cs);
    for (auto [id, name] : cell_set_names)
        if (!name.empty())
            mesh->set_cell_set_name(id, name);

    // side sets
    for (auto & [id, sides] : side_sets) {
        auto sset = build_side_set(*mesh, sides);
        mesh->set_side_set(id, sset);
    }
    for (auto [id, name] : side_set_names)
        if (!name.empty())
            mesh->set_side_set_name(id, name);

    // node sets
    for (auto & [id, ns] : node_sets) {
        auto node_ids = build_node_set(ns);
        mesh->set_node_set(id, node_ids);
    }
    for (auto [id, name] : node_set_names)
        if (!name.empty())
            mesh->set_node_set_name(id, name);

    return mesh;
}

void
ExodusIIFile::write(Ptr<const Mesh> mesh)
{
    Log::info("Writing ExodusII file '{}'", this->fn_);
    LoggingTimer timer;

    this->exo_.create(this->fn_);
    auto bbox = compute_bounding_box(mesh);
    auto dim = determine_spatial_dim(bbox);

    auto [x, y, z] = build_coords(*mesh, dim);
    std::map<Index, int> exii_elem_ids;
    auto [blocks, block_names] = build_blocks(*mesh, exii_elem_ids);
    auto [side_sets, side_set_names] = build_side_sets(*mesh, exii_elem_ids);
    auto [node_sets, node_set_names] = build_node_sets(*mesh);

    int n_nodes = (int) mesh->points().size();
    int n_elems = (int) mesh->elements().size();
    int n_elem_blks = blocks.size();
    int n_node_sets = node_sets.size();
    int n_side_sets = side_sets.size();
    this->exo_.init("", dim, n_nodes, n_elems, n_elem_blks, n_node_sets, n_side_sets);

    write_info(this->exo_);
    write_coords(this->exo_, dim, x, y, z);
    write_element_blocks(this->exo_, blocks, block_names);
    write_side_sets(this->exo_, side_sets, side_set_names);
    write_node_sets(this->exo_, node_sets, node_set_names);

    Log::info("- dim={}, nodes={}, elements={}, element blocks={}, node_sets={}, side_sets={}",
              dim,
              utils::human_number(n_nodes),
              utils::human_number(n_elems),
              utils::human_number(n_elem_blks),
              utils::human_number(n_node_sets),
              utils::human_number(n_side_sets));
}

void
ExodusIIFile::write(const GeomModel & model)
{
    Log::info("Writing ExodusII file '{}'", this->fn_);
    LoggingTimer timer;

    this->exo_.create(this->fn_);

    auto bbox = compute_bounding_box(model);
    auto dim = determine_spatial_dim(bbox);

    auto pnt_map = build_points(model);
    auto [x, y, z] = build_coords(pnt_map, dim);
    auto [blocks, block_names] = build_blocks(model, pnt_map, dim);
    auto [side_sets, side_set_names] = build_side_sets(model, blocks, pnt_map, dim);
    auto [node_sets, node_set_names] = build_node_sets(model, pnt_map);

    int n_nodes = pnt_map.size();
    int n_elems = 0;
    for (auto & [marker, blk] : blocks)
        n_elems += blk.size();
    int n_elem_blks = blocks.size();
    int n_node_sets = node_sets.size();
    int n_side_sets = side_sets.size();

    this->exo_.init("", dim, n_nodes, n_elems, n_elem_blks, n_node_sets, n_side_sets);

    write_info(this->exo_);
    write_coords(this->exo_, dim, x, y, z);
    write_element_blocks(this->exo_, blocks, block_names);
    write_side_sets(this->exo_, side_sets, side_set_names);
    write_node_sets(this->exo_, node_sets, node_set_names);

    Log::info("- dim={}, nodes={}, elements={}, element blocks={}, node sets={}, side sets={}",
              dim,
              utils::human_number(n_nodes),
              utils::human_number(n_elems),
              utils::human_number(n_elem_blks),
              utils::human_number(n_node_sets),
              utils::human_number(n_side_sets));
}

} // namespace krado
