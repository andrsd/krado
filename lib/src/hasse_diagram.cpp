// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/hasse_diagram.h"
#include "krado/mesh.h"
#include "krado/log.h"
#include "krado/timer.h"
#include <iostream>

namespace krado {

HasseDiagram::HasseDiagram(const Mesh & mesh)
{
    Log::info(2, "Building Hasse diagram");
    LoggingTimer timer;

    auto elems = mesh.elements();
    auto pnts = mesh.points();

    std::unordered_map<HasseKey, HasseIndex> key_map;
    HasseKey n_entries(elems.size() + pnts.size());
    for (const auto & cell : elems) {
        if (cell.type() == ElementType::TRI3) {
            n_entries += Tri3::N_EDGES;
        }
        else if (cell.type() == ElementType::QUAD4) {
            n_entries += Quad4::N_EDGES;
        }
        else if (cell.type() == ElementType::TETRA4) {
            n_entries += Tetra4::N_EDGES;
            n_entries += Tetra4::N_FACES;
        }
        else if (cell.type() == ElementType::PYRAMID5) {
            n_entries += Pyramid5::N_EDGES;
            n_entries += Pyramid5::N_FACES;
        }
        else if (cell.type() == ElementType::PRISM6) {
            n_entries += Prism6::N_EDGES;
            n_entries += Prism6::N_FACES;
        }
        else if (cell.type() == ElementType::HEX8) {
            n_entries += Hex8::N_EDGES;
            n_entries += Hex8::N_FACES;
        }
        else if (cell.type() == ElementType::LINE2) {
            // do nothing
        }
    }
    key_map.reserve(n_entries.value());

    // Count number of nodes
    HasseKey n_rows(0);
    for (Index i : make_range(elems.size())) {
        auto id = key(-(HasseIndex(i) + 1));
        HasseIndex elem_node_id(n_rows++);
        key_map[id] = elem_node_id;
    }
    this->cell_rng_ = make_range(HasseIndex(0), HasseIndex(elems.size()));
    for (Index i : make_range(pnts.size())) {
        auto vtx_id = key(HasseIndex(i));
        HasseIndex vtx_node_id(n_rows++);
        key_map[vtx_id] = vtx_node_id;
    }
    this->vertex_rng_ = make_range(HasseIndex(elems.size()), HasseIndex(key_map.size()));
    u64 a = key_map.size();
    for (Index i : make_range(elems.size())) {
        const auto & cell = elems[i];
        if (cell.type() == ElementType::TETRA4)
            add_faces_nd<Tetra4>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::PYRAMID5)
            add_faces_nd<Pyramid5>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::PRISM6)
            add_faces_nd<Prism6>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::HEX8)
            add_faces_nd<Hex8>(key_map, n_rows, cell);
    }
    this->face_rng_ = make_range(HasseIndex(a), HasseIndex(key_map.size()));
    a = key_map.size();
    for (Index i : make_range(elems.size())) {
        const auto & cell = elems[i];
        if (cell.type() == ElementType::TRI3)
            add_edges_nd<Tri3>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::QUAD4)
            add_edges_nd<Quad4>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::TETRA4)
            add_face_edges_nd<Tetra4>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::PYRAMID5)
            add_face_edges_nd<Pyramid5>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::PRISM6)
            add_face_edges_nd<Prism6>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::HEX8)
            add_face_edges_nd<Hex8>(key_map, n_rows, cell);
        else if (cell.type() == ElementType::LINE2) {
            // do nothing
        }
    }
    this->edge_rng_ = make_range(HasseIndex(a), HasseIndex(key_map.size()));
    this->out_offsets_.resize(n_rows.value() + 1, 0);
    this->in_offsets_.resize(n_rows.value() + 1, 0);
    this->out_inc_.resize(n_rows.value(), 0);
    this->in_inc_.resize(n_rows.value(), 0);

    // Count number of edges
    std::unordered_map<HasseKey, HasseIndex> edge_key_map;
    edge_key_map.reserve(2 * n_entries.value());
    for (Index i : make_range(elems.size())) {
        const auto & cell = elems[i];
        if (cell.type() == ElementType::TRI3) {
            add_edges_ed<Tri3>(edge_key_map, key_map, HasseIndex(i), cell);
            add_edge_vertices_ed<Tri3>(edge_key_map, key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::QUAD4) {
            add_edges_ed<Quad4>(edge_key_map, key_map, HasseIndex(i), cell);
            add_edge_vertices_ed<Quad4>(edge_key_map, key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::TETRA4) {
            add_faces_ed<Tetra4>(edge_key_map, key_map, HasseIndex(i), cell);
            add_face_edges_ed<Tetra4>(edge_key_map, key_map, HasseIndex(i), cell);
            add_edge_vertices_ed<Tetra4>(edge_key_map, key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::PYRAMID5) {
            add_faces_ed<Pyramid5>(edge_key_map, key_map, HasseIndex(i), cell);
            add_face_edges_ed<Pyramid5>(edge_key_map, key_map, HasseIndex(i), cell);
            add_edge_vertices_ed<Pyramid5>(edge_key_map, key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::PRISM6) {
            add_faces_ed<Prism6>(edge_key_map, key_map, HasseIndex(i), cell);
            add_face_edges_ed<Prism6>(edge_key_map, key_map, HasseIndex(i), cell);
            add_edge_vertices_ed<Prism6>(edge_key_map, key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::HEX8) {
            add_faces_ed<Hex8>(edge_key_map, key_map, HasseIndex(i), cell);
            add_face_edges_ed<Hex8>(edge_key_map, key_map, HasseIndex(i), cell);
            add_edge_vertices_ed<Hex8>(edge_key_map, key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::LINE2) {
            add_vertices_ed(edge_key_map, key_map, HasseIndex(i), cell);
        }
    }
    u64 ofst = 0;
    for (auto i : make_range(this->out_offsets_.size() - 1)) {
        ofst += this->out_offsets_[i + 1];
        this->out_offsets_[i + 1] = ofst;
    }

    ofst = 0;
    for (auto i : make_range(this->in_offsets_.size() - 1)) {
        ofst += this->in_offsets_[i + 1];
        this->in_offsets_[i + 1] = ofst;
    }

    // Fill in the values
    u64 n_edges = edge_key_map.size();
    this->out_adjacency_.resize(n_edges);
    this->in_adjacency_.resize(n_edges);

    for (Index i : make_range(elems.size())) {
        const auto & cell = elems[i];
        if (cell.type() == ElementType::TRI3) {
            add_edges<Tri3>(key_map, HasseIndex(i), cell);
            add_edge_vertices<Tri3>(key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::QUAD4) {
            add_edges<Quad4>(key_map, HasseIndex(i), cell);
            add_edge_vertices<Quad4>(key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::TETRA4) {
            add_faces<Tetra4>(key_map, HasseIndex(i), cell);
            add_face_edges<Tetra4>(key_map, HasseIndex(i), cell);
            add_edge_vertices<Tetra4>(key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::PYRAMID5) {
            add_faces<Pyramid5>(key_map, HasseIndex(i), cell);
            add_face_edges<Pyramid5>(key_map, HasseIndex(i), cell);
            add_edge_vertices<Pyramid5>(key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::PRISM6) {
            add_faces<Prism6>(key_map, HasseIndex(i), cell);
            add_face_edges<Prism6>(key_map, HasseIndex(i), cell);
            add_edge_vertices<Prism6>(key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::HEX8) {
            add_faces<Hex8>(key_map, HasseIndex(i), cell);
            add_face_edges<Hex8>(key_map, HasseIndex(i), cell);
            add_edge_vertices<Hex8>(key_map, HasseIndex(i), cell);
        }
        else if (cell.type() == ElementType::LINE2) {
            add_vertices(key_map, HasseIndex(i), cell);
        }
    }

    this->out_inc_.clear();
    this->in_inc_.clear();
}

TRange<HasseIndex>
HasseDiagram::vertices() const
{
    return this->vertex_rng_;
}

TRange<HasseIndex>
HasseDiagram::edges() const
{
    return this->edge_rng_;
}

TRange<HasseIndex>
HasseDiagram::faces() const
{
    return this->face_rng_;
}

TRange<HasseIndex>
HasseDiagram::cells() const
{
    return this->cell_rng_;
}

Span<const HasseIndex>
HasseDiagram::out_vertices(HasseIndex entity_id) const
{
    auto start = this->out_offsets_[entity_id.value()];
    auto end = this->out_offsets_[entity_id.value() + 1];
    return { this->out_adjacency_.data() + start, static_cast<size_t>(end - start) };
}

Span<const HasseIndex>
HasseDiagram::in_vertices(HasseIndex entity_id) const
{
    auto start = this->in_offsets_[entity_id.value()];
    auto end = this->in_offsets_[entity_id.value() + 1];
    return { this->in_adjacency_.data() + start, static_cast<size_t>(end - start) };
}

void
HasseDiagram::print() const
{
    std::cerr << "vtx: " << this->vertex_rng_ << std::endl;
    std::cerr << "elm: " << this->cell_rng_ << std::endl;
    std::cerr << "fac: " << this->face_rng_ << std::endl;
    std::cerr << "edg: " << this->edge_rng_ << std::endl;
    for (auto i : make_range(this->out_offsets_.size() - 1)) {
        auto start = this->out_offsets_[i];
        auto end = this->out_offsets_[i + 1];

        std::cerr << i << ":";
        for (auto j : make_range(start, end)) {
            std::cerr << " " << this->out_adjacency_[j].value();
        }
        std::cerr << std::endl;
    }
    std::cerr << "--" << std::endl;
    for (auto i : make_range(this->in_offsets_.size() - 1)) {
        auto start = this->in_offsets_[i];
        auto end = this->in_offsets_[i + 1];

        std::cerr << i << ":";
        for (auto j : make_range(start, end)) {
            std::cerr << " " << this->in_adjacency_[j].value();
        }
        std::cerr << std::endl;
    }
}

} // namespace krado
