// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/range.h"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graph_traits.hpp"
#include <cstddef>
#include <limits>
#include <set>
#include <map>
#include <iostream>
#include <unordered_map>
#include <algorithm>

namespace krado {

class HasseDiagram {
public:
    enum NodeType { Vertex, Edge, Face, Cell };

    struct NodeProps {
        NodeType type;
    };

private:
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, NodeProps>;

public:
    using Node = boost::graph_traits<Graph>::vertex_descriptor;

    const Range &
    vertices() const
    {
        return this->vertex_rng_;
    }

    const Range &
    edges() const
    {
        return this->edge_rng_;
    }

    const Range &
    faces() const
    {
        return this->face_rng_;
    }

    const Range &
    cells() const
    {
        return this->cell_rng_;
    }

    std::size_t
    size() const
    {
        return boost::num_vertices(this->grph_);
    }

    void
    add_node(gidx_t id, NodeType type)
    {
        auto nd = boost::add_vertex(this->grph_);
        this->grph_[nd].type = type;

        if (type == NodeType::Vertex)
            this->vertex_rng_.expand(id);
        else if (type == NodeType::Edge)
            this->edge_rng_.expand(id);
        else if (type == NodeType::Face)
            this->face_rng_.expand(id);
        else if (type == NodeType::Cell)
            this->cell_rng_.expand(id);
    }

    void
    add_edge(gidx_t parent_id, gidx_t child_id)
    {
        auto edge_result = boost::edge(parent_id, child_id, this->grph_);
        if (!edge_result.second)
            boost::add_edge(parent_id, child_id, this->grph_);
    }

    NodeType
    node_type(gidx_t id) const
    {
        return this->grph_[id].type;
    }

    void
    clear()
    {
        this->grph_ = Graph();
        this->vertex_rng_ = { std::numeric_limits<gidx_t>::max(),
                              std::numeric_limits<gidx_t>::min() };
        this->edge_rng_ = { std::numeric_limits<gidx_t>::max(),
                            std::numeric_limits<gidx_t>::min() };
        this->face_rng_ = { std::numeric_limits<gidx_t>::max(),
                            std::numeric_limits<gidx_t>::min() };
        this->cell_rng_ = { std::numeric_limits<gidx_t>::max(),
                            std::numeric_limits<gidx_t>::min() };
    }

    std::vector<gidx_t>
    get_out_vertices(Node v1) const
    {
        auto range = boost::adjacent_vertices(v1, this->grph_);
        std::vector<gidx_t> vtxs;
        for (auto it = range.first; it != range.second; ++it)
            vtxs.push_back(*it);
        return vtxs;
    }

    std::vector<gidx_t>
    get_in_vertices(Node v1) const
    {
        auto range = boost::in_edges(v1, this->grph_);
        std::vector<gidx_t> vtxs;
        for (auto it = range.first; it != range.second; ++it)
            vtxs.push_back(boost::source(*it, this->grph_));
        return vtxs;
    }

    void
    print()
    {
        for (std::size_t i = 0; i < num_vertices(this->grph_); ++i) {
            std::cout << "Node " << i << " (): ";
            for (auto v : get_out_vertices(i))
                std::cout << v << " ";
            std::cout << std::endl;
        }
        std::cerr << "== support:" << std::endl;
        for (std::size_t i = 0; i < num_vertices(this->grph_); ++i) {
            std::cout << "Node " << i << " (): ";
            for (auto v : get_in_vertices(i))
                std::cout << v << " ";
            std::cout << std::endl;
        }
    }

    void
    reserve(std::size_t n_cells, std::size_t n_pts)
    {
        this->grph_.m_vertices.reserve(3 * n_cells + n_pts);
    }

private:
    Range vertex_rng_ = { std::numeric_limits<gidx_t>::max(), std::numeric_limits<gidx_t>::min() };
    Range edge_rng_ = { std::numeric_limits<gidx_t>::max(), std::numeric_limits<gidx_t>::min() };
    Range face_rng_ = { std::numeric_limits<gidx_t>::max(), std::numeric_limits<gidx_t>::min() };
    Range cell_rng_ = { std::numeric_limits<gidx_t>::max(), std::numeric_limits<gidx_t>::min() };

    Graph grph_;
};

} // namespace krado
