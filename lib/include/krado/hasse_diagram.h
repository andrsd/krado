#pragma once

#include <set>
#include <map>
#include <iostream>
#include <unordered_map>

namespace krado {

class HasseDiagram {
public:
    struct Node {
        enum Type { Vertex, Edge, Face, Cell } type;
        std::size_t id;
        std::vector<std::size_t> children;
        std::vector<std::size_t> support;
    };

    /// Nodes
    std::unordered_map<std::size_t, Node> nodes;
    /// Vertices: idices into nodes
    std::set<std::size_t> vertices;
    /// Edges: idices into nodes
    std::set<std::size_t> edges;
    /// Faces: idices into nodes
    std::set<std::size_t> faces;
    /// Cells: indices into nodes
    std::set<std::size_t> cells;

    void
    add_node(std::size_t id, Node::Type type)
    {
        this->nodes[id] = { type, id, {}, {} };
        if (type == Node::Vertex)
            this->vertices.insert(id);
        else if (type == Node::Edge)
            this->edges.insert(id);
        else if (type == Node::Face)
            this->faces.insert(id);
        else if (type == Node::Cell)
            this->cells.insert(id);
    }

    void
    add_edge(std::size_t parent_id, std::size_t child_id)
    {
        insert(this->nodes[parent_id].children, child_id);
        insert(this->nodes[child_id].support, parent_id);
    }

    void
    clear()
    {
        this->nodes.clear();
        this->vertices.clear();
        this->edges.clear();
        this->faces.clear();
        this->cells.clear();
    }

    void
    print()
    {
        for (const auto & [id, node] : this->nodes) {
            std::cout << "Node " << id << " (): ";
            for (auto child_id : node.children) {
                std::cout << child_id << " ";
            }
            std::cout << std::endl;
        }
        std::cerr << "== support:" << std::endl;
        for (const auto & [id, node] : this->nodes) {
            std::cout << "Node " << id << " (): ";
            for (auto child_id : node.support) {
                std::cout << child_id << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    void
    insert(std::vector<std::size_t> & vec, std::size_t v)
    {
        if (std::find(vec.begin(), vec.end(), v) == vec.end())
            vec.push_back(v);
    }
};

} // namespace krado
