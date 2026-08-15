// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/range.h"
#include "krado/element.h"
#include "krado/types.h"
#include <vector>
#include <limits>
#include <unordered_map>

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

    [[nodiscard]] TRange<HasseIndex> vertices() const;
    [[nodiscard]] TRange<HasseIndex> edges() const;
    [[nodiscard]] TRange<HasseIndex> faces() const;
    [[nodiscard]] TRange<HasseIndex> cells() const;

    [[nodiscard]] Span<const HasseIndex> out_vertices(HasseIndex entity_id) const;
    [[nodiscard]] Span<const HasseIndex> in_vertices(HasseIndex entity_id) const;

    void print() const;

private:
    /// Create a key from the supplied index. Use this to construct keys for cells
    ///
    /// @param id The index to create a key from
    /// @return The key
    [[nodiscard]] HasseKey
    key(const HasseIndex id)
    {
        std::size_t hash_value = 0;
        boost::hash_combine(hash_value, id.value());
        return HasseKey(hash_value);
    }

    /// @note idxs should be sorted, use `to_hasse`
    [[nodiscard]] HasseKey
    key(const std::array<HasseIndex, 2> & idxs)
    {
        std::size_t hash_value = 0;
        boost::hash_combine(hash_value, idxs[0].value());
        boost::hash_combine(hash_value, idxs[1].value());

        return HasseKey(hash_value);
    }

    [[nodiscard]] HasseKey
    key(HasseIndex one, HasseIndex two)
    {
        return key(std::array<HasseIndex, 2> { one, two });
    }

    /// Create a key from the supplied indices. Use this to construct keys for edges and faces
    ///
    /// @param idxs The indices to create a key from
    /// @return The key
    [[nodiscard]] HasseKey
    key(const std::vector<HasseIndex> & idxs)
    {
        std::size_t hash_value = 0;
        for (auto v : idxs)
            boost::hash_combine(hash_value, v.value());
        return HasseKey(hash_value);
    }

    //

    std::array<HasseIndex, 2>
    to_hasse(const std::array<Index, 2> & idxs)
    {
        std::array<HasseIndex, 2> hasse;
        if (idxs[0] <= idxs[1]) {
            hasse[0] = HasseIndex(idxs[0]);
            hasse[1] = HasseIndex(idxs[1]);
        }
        else {
            hasse[0] = HasseIndex(idxs[1]);
            hasse[1] = HasseIndex(idxs[0]);
        }
        return hasse;
    }

    std::vector<HasseIndex>
    to_hasse(const std::vector<Index> & idxs)
    {
        std::vector<HasseIndex> hasse(idxs.begin(), idxs.end());
        std::sort(hasse.begin(), hasse.end());
        return hasse;
    }

    // Helpers to count number of Hasse nodes

    void
    add_edge_nd(std::unordered_map<HasseKey, HasseIndex> & key_map,
                HasseKey & count,
                const std::array<HasseIndex, 2> & edge_connect)
    {
        auto k = key(edge_connect);
        auto [it, inserted] = key_map.try_emplace(k, count);
        if (inserted)
            count++;
    }

    template <class ELEMENT_TYPE>
    void
    add_faces_nd(std::unordered_map<HasseKey, HasseIndex> & key_map,
                 HasseKey & count,
                 const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto hasse_face = to_hasse(face_connect);
            auto k = key(hasse_face);
            auto [it, inserted] = key_map.try_emplace(k, count);
            if (inserted)
                count++;
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_face_edges_nd(std::unordered_map<HasseKey, HasseIndex> & key_map,
                      HasseKey & count,
                      const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto i : make_range(ELEMENT_TYPE::N_FACES)) {
            for (std::size_t j = 0; j < ELEMENT_TYPE::FACE_EDGES[i].size(); ++j) {
                auto edge = ELEMENT_TYPE::FACE_EDGES[i][j];
                auto edge_connect =
                    utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[edge]);
                auto hasse_edge = to_hasse(edge_connect);
                add_edge_nd(key_map, count, hasse_edge);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edges_nd(std::unordered_map<HasseKey, HasseIndex> & key_map,
                 HasseKey & count,
                 const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto hasse_edge = to_hasse(edge_connect);
            add_edge_nd(key_map, count, hasse_edge);
        }
    }

    // Helpers to count number of Hasse edges

    void
    add_vertices_ed(std::unordered_map<HasseKey, HasseIndex> & edge_key_map,
                    const std::unordered_map<HasseKey, HasseIndex> & key_map,
                    HasseIndex elem_node_id,
                    const Element & elem)
    {
        auto connect = elem.indices();
        for (auto j : make_range(Line2::N_VERTICES)) {
            auto vtx = HasseIndex(connect[Line2::EDGE_VERTICES[j]]);
            auto kv = key(vtx);
            auto vtx_node_id = key_map.at(kv);

            auto k2 = key(elem_node_id, vtx_node_id);
            auto [it, inserted] = edge_key_map.try_emplace(k2, 0);
            if (inserted) {
                this->out_offsets_[elem_node_id.value() + 1]++;
                this->in_offsets_[vtx_node_id.value() + 1]++;
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_faces_ed(std::unordered_map<HasseKey, HasseIndex> & edge_key_map,
                 const std::unordered_map<HasseKey, HasseIndex> & key_map,
                 HasseIndex id,
                 const Element & elem)
    {
        auto iid = key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto hasse_face = to_hasse(face_connect);
            auto ke = key(hasse_face);
            auto face_node_id = key_map.at(ke);
            create_edge_ed(edge_key_map, elem_node_id, face_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_face_edges_ed(std::unordered_map<HasseKey, HasseIndex> & edge_key_map,
                      const std::unordered_map<HasseKey, HasseIndex> & key_map,
                      HasseIndex /*id*/,
                      const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto i : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[i]);
            auto hasse_face = to_hasse(face_connect);
            auto kf = key(hasse_face);
            auto face_node_id = key_map.at(kf);

            for (std::size_t j = 0; j < ELEMENT_TYPE::FACE_EDGES[i].size(); ++j) {
                auto edge = ELEMENT_TYPE::FACE_EDGES[i][j];
                auto edge_connect =
                    utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[edge]);
                auto hasse_edge = to_hasse(edge_connect);
                auto ke = key(hasse_edge);
                auto edge_node_id = key_map.at(ke);
                create_edge_ed(edge_key_map, face_node_id, edge_node_id);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edges_ed(std::unordered_map<HasseKey, HasseIndex> & edge_key_map,
                 const std::unordered_map<HasseKey, HasseIndex> & key_map,
                 HasseIndex id,
                 const Element & elem)
    {
        auto iid = key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto hasse_edge = to_hasse(edge_connect);
            auto ke = key(hasse_edge);
            auto edge_node_id = key_map.at(ke);
            create_edge_ed(edge_key_map, elem_node_id, edge_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edge_vertices_ed(std::unordered_map<HasseKey, HasseIndex> & edge_key_map,
                         const std::unordered_map<HasseKey, HasseIndex> & key_map,
                         HasseIndex /*id*/,
                         const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto hasse_edge = to_hasse(edge_connect);
            auto ke = key(hasse_edge);
            auto edge_node_id = key_map.at(ke);
            for (auto & vtx : edge_connect) {
                auto kv = key(HasseIndex(vtx));
                auto vtx_node_id = key_map.at(kv);
                create_edge_ed(edge_key_map, edge_node_id, vtx_node_id);
            }
        }
    }

    void
    create_edge_ed(std::unordered_map<HasseKey, HasseIndex> & edge_key_map,
                   HasseIndex v1,
                   HasseIndex v2)
    {
        auto k2 = key(v1, v2);
        auto [it, inserted] = edge_key_map.try_emplace(k2, 0);
        if (inserted) {
            this->out_offsets_[v1.value() + 1]++;
            this->in_offsets_[v2.value() + 1]++;
        }
    }

    // Helpers for building the grahc edges

    template <class ELEMENT_TYPE>
    void
    add_faces(const std::unordered_map<HasseKey, HasseIndex> & key_map,
              HasseIndex id,
              const Element & elem)
    {
        auto iid = key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto hasse_face = to_hasse(face_connect);
            auto kf = key(hasse_face);
            auto face_node_id = key_map.at(kf);
            create_edge(elem_node_id, face_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_face_edges(const std::unordered_map<HasseKey, HasseIndex> & key_map,
                   HasseIndex /*id*/,
                   const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto i : make_range(ELEMENT_TYPE::N_FACES)) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[i]);
            auto hasse_face = to_hasse(face_connect);
            auto kf = key(hasse_face);
            auto face_node_id = key_map.at(kf);

            for (std::size_t j = 0; j < ELEMENT_TYPE::FACE_EDGES[i].size(); ++j) {
                auto edge = ELEMENT_TYPE::FACE_EDGES[i][j];
                auto edge_connect =
                    utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[edge]);
                auto hasse_edge = to_hasse(edge_connect);
                auto ke = key(hasse_edge);
                auto edge_node_id = key_map.at(ke);
                create_edge(face_node_id, edge_node_id);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edges(const std::unordered_map<HasseKey, HasseIndex> & key_map,
              HasseIndex id,
              const Element & elem)
    {
        auto iid = key(-(id + 1));
        auto elem_node_id = key_map.at(iid);

        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto hasse_edge = to_hasse(edge_connect);
            auto ke = key(hasse_edge);
            auto edge_node_id = key_map.at(ke);
            create_edge(elem_node_id, edge_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    add_edge_vertices(const std::unordered_map<HasseKey, HasseIndex> & key_map,
                      HasseIndex /*id*/,
                      const Element & elem)
    {
        auto elem_connect = elem.indices();
        for (auto j : make_range(ELEMENT_TYPE::N_EDGES)) {
            auto edge_connect = utils::edge_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto hasse_edge = to_hasse(edge_connect);
            auto ke = key(hasse_edge);
            auto edge_node_id = key_map.at(ke);
            for (auto & vtx : edge_connect) {
                auto kv = key(HasseIndex(vtx));
                auto vtx_node_id = key_map.at(kv);
                create_edge(edge_node_id, vtx_node_id);
            }
        }
    }

    void
    add_vertices(const std::unordered_map<HasseKey, HasseIndex> & key_map,
                 HasseIndex elem_node_id,
                 const Element & elem)
    {
        auto connect = elem.indices();
        for (auto j : make_range(Line2::N_VERTICES)) {
            auto vtx = connect[Line2::EDGE_VERTICES[j]];
            auto kv = key(HasseIndex(vtx));
            auto vtx_node_id = key_map.at(kv);
            create_edge(elem_node_id, vtx_node_id);
        }
    }

    void
    create_edge(HasseIndex v1, HasseIndex v2)
    {
        // edge from v1 -> v2
        {
            auto ofst = this->out_inc_[v1.value()];
            bool already_in = false;
            for (auto i : make_range(ofst)) {
                auto idx = this->out_offsets_[v1.value()] + i;
                if (this->out_adjacency_[idx] == v2) {
                    already_in = true;
                    break;
                }
            }
            if (not already_in) {
                auto idx = this->out_offsets_[v1.value()] + ofst;
                this->out_adjacency_[idx] = v2;
                this->out_inc_[v1.value()]++;
            }
        }
        // edge from v2 -> v1
        {
            auto ofst = this->in_inc_[v2.value()];
            bool already_in = false;
            for (auto i : make_range(ofst)) {
                auto idx = this->in_offsets_[v2.value()] + i;
                if (this->in_adjacency_[idx] == v1) {
                    already_in = true;
                    break;
                }
            }
            if (not already_in) {
                auto idx = this->in_offsets_[v2.value()] + ofst;
                this->in_adjacency_[idx] = v1;
                this->in_inc_[v2.value()]++;
            }
        }
    }

    TRange<HasseIndex> vertex_rng_ = { std::numeric_limits<HasseIndex>::max(),
                                       std::numeric_limits<HasseIndex>::min() };
    TRange<HasseIndex> edge_rng_ = { std::numeric_limits<HasseIndex>::max(),
                                     std::numeric_limits<HasseIndex>::min() };
    TRange<HasseIndex> face_rng_ = { std::numeric_limits<HasseIndex>::max(),
                                     std::numeric_limits<HasseIndex>::min() };
    TRange<HasseIndex> cell_rng_ = { std::numeric_limits<HasseIndex>::max(),
                                     std::numeric_limits<HasseIndex>::min() };

    std::vector<u64> out_offsets_;
    std::vector<HasseIndex> out_adjacency_;
    std::vector<u64> out_inc_;

    std::vector<u64> in_offsets_;
    std::vector<HasseIndex> in_adjacency_;
    std::vector<u64> in_inc_;
};

} // namespace krado
