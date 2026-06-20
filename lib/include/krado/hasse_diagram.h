// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/range.h"
#include "krado/element.h"
#include "krado/utils.h"
#include <vector>
#include <limits>

namespace krado {

class Mesh;

/// Hasse diagram for large graphs
///
/// in-edges and out-edges are stored separately in two adjacency "matrices"
/// using CSR format. We number cells first, then vertices, then faces (if we
/// have them), and edges (if we have them).
class HasseDiagram {
public:
    HasseDiagram() = default;
    HasseDiagram(const Mesh & mesh);

    Range vertices() const;
    Range edges() const;
    Range faces() const;
    Range cells() const;

    Span<const Index> out_vertices(Index entity_id) const;
    Span<const Index> in_vertices(Index entity_id) const;

    void print() const;

private:
    // Helpers to count number of Hasse nodes

    void
    add_edge_nd(std::unordered_map<std::size_t, Index> & key_map,
                u64 & count,
                const std::array<Index, 2> & edge_connect)
    {
        auto k = utils::key(edge_connect);
        auto [it, inserted] = key_map.try_emplace(k, count);
        if (inserted)
            count++;
    }

    template <class ELEMENT_TYPE>
    void
    add_faces_nd(std::unordered_map<std::size_t, Index> & key_map,
                 u64 & count,
                 const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto k = utils::key(face_connect);
            auto [it, inserted] = key_map.try_emplace(k, count);
            if (inserted)
                count++;
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_face_edges_nd(std::unordered_map<std::size_t, Index> & key_map,
                      u64 & count,
                      const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto i : make_range(ELEMENT_TYPE::N_FACES)) {
            for (std::size_t j = 0; j < ELEMENT_TYPE::FACE_EDGES[i].size(); ++j) {
                auto edge = ELEMENT_TYPE::FACE_EDGES[i][j];
                auto edge_connect =
                    utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[edge]);
                add_edge_nd(key_map, count, edge_connect);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edges_nd(std::unordered_map<std::size_t, Index> & key_map,
                 u64 & count,
                 const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            add_edge_nd(key_map, count, edge_connect);
        }
    }

    // Helpers to count number of Hasse edges

    void
    add_vertices_ed(std::unordered_map<std::size_t, Index> & edge_key_map,
                    const std::unordered_map<std::size_t, Index> & key_map,
                    Index elem_node_id,
                    const Element & elem)
    {
        auto connect = elem.indices();
        for (auto j : make_range(Line2::N_VERTICES)) {
            auto vtx = connect[Line2::EDGE_VERTICES[j]];
            auto vtx_id = utils::key(vtx);
            auto vtx_node_id = key_map.at(vtx_id);

            auto k2 = utils::key(elem_node_id, vtx_node_id);
            auto [it, inserted] = edge_key_map.try_emplace(k2, 0);
            if (inserted) {
                this->out_offsets_[elem_node_id + 1]++;
                this->in_offsets_[vtx_node_id + 1]++;
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_faces_ed(std::unordered_map<std::size_t, Index> & edge_key_map,
                 const std::unordered_map<std::size_t, Index> & key_map,
                 Index id,
                 const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto k = utils::key(face_connect);
            auto face_node_id = key_map.at(k);
            create_edge_ed(edge_key_map, elem_node_id, face_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_face_edges_ed(std::unordered_map<std::size_t, Index> & edge_key_map,
                      const std::unordered_map<std::size_t, Index> & key_map,
                      Index /*id*/,
                      const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto i : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[i]);
            auto face_node_id = key_map.at(utils::key(face_connect));

            for (std::size_t j = 0; j < ELEMENT_TYPE::FACE_EDGES[i].size(); ++j) {
                auto edge = ELEMENT_TYPE::FACE_EDGES[i][j];
                auto edge_connect =
                    utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[edge]);
                auto k = utils::key(edge_connect);
                auto edge_node_id = key_map.at(k);
                create_edge_ed(edge_key_map, face_node_id, edge_node_id);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edges_ed(std::unordered_map<std::size_t, Index> & edge_key_map,
                 const std::unordered_map<std::size_t, Index> & key_map,
                 Index id,
                 const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto k = utils::key(edge_connect);
            auto edge_node_id = key_map.at(k);
            create_edge_ed(edge_key_map, elem_node_id, edge_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edge_vertices_ed(std::unordered_map<std::size_t, Index> & edge_key_map,
                         const std::unordered_map<std::size_t, Index> & key_map,
                         Index /*id*/,
                         const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto edge_node_id = key_map.at(utils::key(edge_connect));
            for (auto & vtx : edge_connect) {
                auto vtx_id = utils::key(vtx);
                auto vtx_node_id = key_map.at(vtx_id);
                create_edge_ed(edge_key_map, edge_node_id, vtx_node_id);
            }
        }
    }

    void
    create_edge_ed(std::unordered_map<std::size_t, Index> & edge_key_map, Index v1, Index v2)
    {
        auto k2 = utils::key(v1, v2);
        auto [it, inserted] = edge_key_map.try_emplace(k2, 0);
        if (inserted) {
            this->out_offsets_[v1 + 1]++;
            this->in_offsets_[v2 + 1]++;
        }
    }

    // Helpers for building the grahc edges

    template <class ELEMENT_TYPE>
    void
    add_faces(const std::unordered_map<std::size_t, Index> & key_map,
              Index id,
              const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto k = utils::key(face_connect);
            auto face_node_id = key_map.at(k);
            create_edge(elem_node_id, face_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_face_edges(const std::unordered_map<std::size_t, Index> & key_map,
                   Index /*id*/,
                   const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto i : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[i]);
            auto face_node_id = key_map.at(utils::key(face_connect));

            for (std::size_t j = 0; j < ELEMENT_TYPE::FACE_EDGES[i].size(); ++j) {
                auto edge = ELEMENT_TYPE::FACE_EDGES[i][j];
                auto edge_connect =
                    utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[edge]);
                auto k = utils::key(edge_connect);
                auto edge_node_id = key_map.at(k);
                create_edge(face_node_id, edge_node_id);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edges(const std::unordered_map<std::size_t, Index> & key_map,
              Index id,
              const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto k = utils::key(edge_connect);
            auto edge_node_id = key_map.at(k);
            create_edge(elem_node_id, edge_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edge_vertices(const std::unordered_map<std::size_t, Index> & key_map,
                      Index /*id*/,
                      const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto edge_node_id = key_map.at(utils::key(edge_connect));
            for (auto & vtx : edge_connect) {
                auto vtx_id = utils::key(vtx);
                auto vtx_node_id = key_map.at(vtx_id);
                create_edge(edge_node_id, vtx_node_id);
            }
        }
    }

    void
    add_vertices(const std::unordered_map<std::size_t, Index> & key_map,
                 Index elem_node_id,
                 const Element & elem)
    {
        auto connect = elem.indices();
        for (auto j : make_range(Line2::N_VERTICES)) {
            auto vtx = connect[Line2::EDGE_VERTICES[j]];
            auto vtx_id = utils::key(vtx);
            auto vtx_node_id = key_map.at(vtx_id);
            create_edge(elem_node_id, vtx_node_id);
        }
    }

    void
    create_edge(Index v1, Index v2)
    {
        // edge from v1 -> v2
        {
            auto ofst = this->out_inc_[v1];
            bool already_in = false;
            for (auto i : make_range(ofst)) {
                auto idx = this->out_offsets_[v1] + i;
                if (this->out_adjacency_[idx] == v2) {
                    already_in = true;
                    break;
                }
            }
            if (not already_in) {
                auto idx = this->out_offsets_[v1] + ofst;
                this->out_adjacency_[idx] = v2;
                this->out_inc_[v1]++;
            }
        }
        // edge from v2 -> v1
        {
            auto ofst = this->in_inc_[v2];
            bool already_in = false;
            for (auto i : make_range(ofst)) {
                auto idx = this->in_offsets_[v2] + i;
                if (this->in_adjacency_[idx] == v1) {
                    already_in = true;
                    break;
                }
            }
            if (not already_in) {
                auto idx = this->in_offsets_[v2] + ofst;
                this->in_adjacency_[idx] = v1;
                this->in_inc_[v2]++;
            }
        }
    }

    Range vertex_rng_ = { std::numeric_limits<Index>::max(), std::numeric_limits<Index>::min() };
    Range edge_rng_ = { std::numeric_limits<Index>::max(), std::numeric_limits<Index>::min() };
    Range face_rng_ = { std::numeric_limits<Index>::max(), std::numeric_limits<Index>::min() };
    Range cell_rng_ = { std::numeric_limits<Index>::max(), std::numeric_limits<Index>::min() };

    std::vector<u64> out_offsets_;
    std::vector<Index> out_adjacency_;
    std::vector<u64> out_inc_;

    std::vector<u64> in_offsets_;
    std::vector<Index> in_adjacency_;
    std::vector<u64> in_inc_;
};

} // namespace krado
