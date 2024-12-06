#pragma once

#include "krado/range.h"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graph_traits.hpp"
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
        return this->vertex_rng;
    }

    const Range &
    edges() const
    {
        return this->edge_rng;
    }

    const Range &
    faces() const
    {
        return this->face_rng;
    }

    const Range &
    cells() const
    {
        return this->cell_rng;
    }

    std::size_t
    size() const
    {
        return boost::num_vertices(this->grph);
    }

    void
    add_node(std::size_t id, NodeType type)
    {
        auto nd = boost::add_vertex(this->grph);
        this->grph[nd].type = type;

        if (type == NodeType::Vertex)
            this->vertex_rng.expand(id);
        else if (type == NodeType::Edge)
            this->edge_rng.expand(id);
        else if (type == NodeType::Face)
            this->face_rng.expand(id);
        else if (type == NodeType::Cell)
            this->cell_rng.expand(id);
    }

    void
    add_edge(std::size_t parent_id, std::size_t child_id)
    {
        auto edge_result = boost::edge(parent_id, child_id, this->grph);
        if (!edge_result.second)
            boost::add_edge(parent_id, child_id, this->grph);
    }

    NodeType
    node_type(std::size_t id) const
    {
        return this->grph[id].type;
    }

    void
    clear()
    {
        this->grph = Graph();
        this->vertex_rng = { std::numeric_limits<std::size_t>::max(),
                             std::numeric_limits<std::size_t>::min() };
        this->edge_rng = { std::numeric_limits<std::size_t>::max(),
                           std::numeric_limits<std::size_t>::min() };
        this->face_rng = { std::numeric_limits<std::size_t>::max(),
                           std::numeric_limits<std::size_t>::min() };
        this->cell_rng = { std::numeric_limits<std::size_t>::max(),
                           std::numeric_limits<std::size_t>::min() };
    }

    std::vector<std::size_t>
    get_out_vertices(Node v1) const
    {
        auto range = boost::adjacent_vertices(v1, this->grph);
        std::vector<std::size_t> vtxs;
        for (auto it = range.first; it != range.second; ++it)
            vtxs.push_back(*it);
        return vtxs;
    }

    std::vector<std::size_t>
    get_in_vertices(Node v1) const
    {
        auto range = boost::in_edges(v1, this->grph);
        std::vector<std::size_t> vtxs;
        for (auto it = range.first; it != range.second; ++it)
            vtxs.push_back(boost::source(*it, this->grph));
        return vtxs;
    }

    void
    print()
    {
        for (std::size_t i = 0; i < num_vertices(this->grph); ++i) {
            std::cout << "Node " << i << " (): ";
            for (auto v : get_out_vertices(i))
                std::cout << v << " ";
            std::cout << std::endl;
        }
        std::cerr << "== support:" << std::endl;
        for (std::size_t i = 0; i < num_vertices(this->grph); ++i) {
            std::cout << "Node " << i << " (): ";
            for (auto v : get_in_vertices(i))
                std::cout << v << " ";
            std::cout << std::endl;
        }
    }

private:
    Range vertex_rng = { std::numeric_limits<std::size_t>::max(),
                         std::numeric_limits<std::size_t>::min() };
    Range edge_rng = { std::numeric_limits<std::size_t>::max(),
                       std::numeric_limits<std::size_t>::min() };
    Range face_rng = { std::numeric_limits<std::size_t>::max(),
                       std::numeric_limits<std::size_t>::min() };
    Range cell_rng = { std::numeric_limits<std::size_t>::max(),
                       std::numeric_limits<std::size_t>::min() };

    Graph grph;
};

} // namespace krado
