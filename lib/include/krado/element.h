// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include <vector>
#include <array>
#include <string>
#include <ostream>

namespace krado {

class Element {
public:
    enum Type { POINT, LINE2, TRI3, QUAD4, TETRA4, PYRAMID5, PRISM6, HEX8 };

    /// Build an element
    ///
    /// @param type Element type
    /// @param vtx_ids Vertex IDs composing the element
    /// @param marker Element marker
    Element(Type type, const std::vector<gidx_t> & vtx_ids);

    /// Get element type
    ///
    /// @return Element type
    Type type() const;

    /// Get number of vertices
    ///
    /// @return Number of vertices
    int num_vertices() const;

    /// Get vertex ID fom local index
    ///
    /// @param idx Local vertex index
    /// @return Vertex ID
    gidx_t vertex_id(int idx) const;

    /// Call operator to access vertex index
    ///
    /// @param idx Vertex index
    /// @return Vertex ID
    gidx_t operator()(int idx) const;

    /// Get vertex IDs
    ///
    /// @return Vertex IDs
    const std::vector<gidx_t> & ids() const;

    /// Set element connectivity. This is good for element renumbering. This cannot be used for
    /// changing element type.
    ///
    /// @param ids Vertex IDs
    void set_ids(const std::vector<gidx_t> & ids);

private:
    Type elem_type;
    std::vector<gidx_t> vtx_id;

public:
    static std::string type(Type type);
    static Element Point(gidx_t id);
    static Element Line2(const std::array<gidx_t, 2> & ids);
    static Element Tri3(const std::array<gidx_t, 3> & ids);
    static Element Quad4(const std::array<gidx_t, 4> & ids);
    static Element Tetra4(const std::array<gidx_t, 4> & ids);
    static Element Pyramid5(const std::array<gidx_t, 5> & ids);
    static Element Prism6(const std::array<gidx_t, 6> & ids);
    static Element Hex8(const std::array<gidx_t, 8> & ids);
};

class Line2 {
public:
    static constexpr Element::Type TYPE = Element::LINE2;
    static constexpr int N_VERTICES = 2;
    static const std::vector<int> EDGE_VERTICES;
};

class Tri3 {
public:
    static constexpr Element::Type TYPE = Element::TRI3;
    static constexpr int N_VERTICES = 3;
    static constexpr int N_EDGES = 3;
    static const std::vector<int> EDGES;
    static const std::vector<std::vector<int>> EDGE_VERTICES;
};

class Quad4 {
public:
    static constexpr Element::Type TYPE = Element::QUAD4;
    static constexpr int N_VERTICES = 4;
    static constexpr int N_EDGES = 4;
    static const std::vector<int> EDGES;
    static const std::vector<std::vector<int>> EDGE_VERTICES;
};

class Tetra4 {
public:
    static constexpr Element::Type TYPE = Element::TETRA4;
    static constexpr int N_VERTICES = 4;
    static constexpr int N_EDGES = 6;
    static constexpr int N_FACES = 4;
    static const std::vector<int> EDGES;
    static const std::vector<std::vector<int>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

class Pyramid5 {
public:
    static constexpr Element::Type TYPE = Element::PYRAMID5;
    static constexpr int N_VERTICES = 5;
    static constexpr int N_EDGES = 8;
    static constexpr int N_FACES = 5;
    static const std::vector<int> EDGES;
    static const std::vector<std::vector<int>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

class Prism6 {
public:
    static constexpr Element::Type TYPE = Element::PRISM6;
    static constexpr int N_VERTICES = 6;
    static constexpr int N_EDGES = 9;
    static constexpr int N_FACES = 5;
    static const std::vector<int> EDGES;
    static const std::vector<std::vector<int>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

class Hex8 {
public:
    static constexpr Element::Type TYPE = Element::HEX8;
    static constexpr int N_VERTICES = 8;
    static constexpr int N_EDGES = 12;
    static constexpr int N_FACES = 6;
    static const std::vector<int> EDGES;
    static const std::vector<std::vector<int>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

//

template <Element::Type ET>
struct ElementSelector;

template <>
struct ElementSelector<Element::Type::HEX8> {
    static constexpr int N_VERTICES = Hex8::N_VERTICES;
    static constexpr int N_FACES = Hex8::N_FACES;
};

template <>
struct ElementSelector<Element::Type::PRISM6> {
    static constexpr int N_VERTICES = Prism6::N_VERTICES;
    static constexpr int N_FACES = Prism6::N_FACES;
};

template <>
struct ElementSelector<Element::Type::PYRAMID5> {
    static constexpr int N_VERTICES = Pyramid5::N_VERTICES;
    static constexpr int N_FACES = Pyramid5::N_FACES;
};

bool operator==(const Element & a, const Element & b);

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Element & el)
{
    stream << "(" << krado::Element::type(el.type()) << ",";
    for (auto & id : el.ids())
        stream << " " << id;
    stream << ")";
    return stream;
}
