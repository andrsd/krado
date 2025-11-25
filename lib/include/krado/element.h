// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include "krado/utils.h"
#include <fmt/format.h>
#include <vector>
#include <array>
#include <string>
#include <ostream>

namespace krado {

class Element {
public:
    /// Build an element
    ///
    /// @param type Element type
    /// @param vtx_ids Vertex IDs composing the element
    /// @param marker Element marker
    Element(ElementType type, const std::vector<gidx_t> & vtx_ids);

    /// Get element type
    ///
    /// @return Element type
    [[nodiscard]] ElementType type() const;

    /// Get number of vertices
    ///
    /// @return Number of vertices
    [[nodiscard]] int num_vertices() const;

    /// Get vertex ID fom local index
    ///
    /// @param idx Local vertex index
    /// @return Vertex ID
    [[nodiscard]] gidx_t vertex_id(int idx) const;

    /// Call operator to access vertex index
    ///
    /// @param idx Vertex index
    /// @return Vertex ID
    [[nodiscard]] gidx_t operator()(int idx) const;

    /// Get vertex IDs
    ///
    /// @return Vertex IDs
    [[nodiscard]] const std::vector<gidx_t> & ids() const;

    /// Get vertex IDs
    ///
    /// @param idx Local vertex index
    /// @return Vertex IDs
    [[nodiscard]] gidx_t id(int idx) const;

    /// Set element connectivity. This is good for element renumbering. This cannot be used for
    /// changing element type.
    ///
    /// @param ids Vertex IDs
    void set_ids(const std::vector<gidx_t> & ids);

private:
    ElementType elem_type_;
    std::vector<gidx_t> vtx_id_;

public:
    [[nodiscard]] static std::string type(ElementType type);
    [[nodiscard]] static Element Point(gidx_t id);
    [[nodiscard]] static Element Line2(const std::array<gidx_t, 2> & ids);
    [[nodiscard]] static Element Tri3(const std::array<gidx_t, 3> & ids);
    [[nodiscard]] static Element Quad4(const std::array<gidx_t, 4> & ids);
    [[nodiscard]] static Element Tetra4(const std::array<gidx_t, 4> & ids);
    [[nodiscard]] static Element Pyramid5(const std::array<gidx_t, 5> & ids);
    [[nodiscard]] static Element Prism6(const std::array<gidx_t, 6> & ids);
    [[nodiscard]] static Element Hex8(const std::array<gidx_t, 8> & ids);

    friend class Mesh;
};

class Line2 {
public:
    static constexpr ElementType TYPE = ElementType::LINE2;
    static constexpr int N_VERTICES = 2;
    static const std::array<int, 2> EDGE_VERTICES;
};

class Tri3 {
public:
    static constexpr ElementType TYPE = ElementType::TRI3;
    static constexpr int N_VERTICES = 3;
    static constexpr int N_EDGES = 3;
    static const std::vector<int> EDGES;
    static const std::vector<std::array<int, 2>> EDGE_VERTICES;
};

class Quad4 {
public:
    static constexpr ElementType TYPE = ElementType::QUAD4;
    static constexpr int N_VERTICES = 4;
    static constexpr int N_EDGES = 4;
    static const std::vector<int> EDGES;
    static const std::vector<std::array<int, 2>> EDGE_VERTICES;
};

class Tetra4 {
public:
    static constexpr ElementType TYPE = ElementType::TETRA4;
    static constexpr int N_VERTICES = 4;
    static constexpr int N_EDGES = 6;
    static constexpr int N_FACES = 4;
    static const std::vector<int> EDGES;
    static const std::vector<std::array<int, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

class Pyramid5 {
public:
    static constexpr ElementType TYPE = ElementType::PYRAMID5;
    static constexpr int N_VERTICES = 5;
    static constexpr int N_EDGES = 8;
    static constexpr int N_FACES = 5;
    static const std::vector<int> EDGES;
    static const std::vector<std::array<int, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

class Prism6 {
public:
    static constexpr ElementType TYPE = ElementType::PRISM6;
    static constexpr int N_VERTICES = 6;
    static constexpr int N_EDGES = 9;
    static constexpr int N_FACES = 5;
    static const std::vector<int> EDGES;
    static const std::vector<std::array<int, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

class Hex8 {
public:
    static constexpr ElementType TYPE = ElementType::HEX8;
    static constexpr int N_VERTICES = 8;
    static constexpr int N_EDGES = 12;
    static constexpr int N_FACES = 6;
    static const std::vector<int> EDGES;
    static const std::vector<std::array<int, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<int>> FACE_EDGES;
    static const std::vector<std::vector<int>> FACE_VERTICES;
};

//

template <ElementType ET>
struct ElementSelector;

template <>
struct ElementSelector<ElementType::TRI3> {
    static constexpr int N_VERTICES = Tri3::N_VERTICES;
};

template <>
struct ElementSelector<ElementType::QUAD4> {
    static constexpr int N_VERTICES = Quad4::N_VERTICES;
};

template <>
struct ElementSelector<ElementType::TETRA4> {
    static constexpr int N_VERTICES = Tetra4::N_VERTICES;
    static constexpr int N_FACES = Tetra4::N_FACES;
};

template <>
struct ElementSelector<ElementType::HEX8> {
    static constexpr int N_VERTICES = Hex8::N_VERTICES;
    static constexpr int N_FACES = Hex8::N_FACES;
};

template <>
struct ElementSelector<ElementType::PRISM6> {
    static constexpr int N_VERTICES = Prism6::N_VERTICES;
    static constexpr int N_FACES = Prism6::N_FACES;
};

template <>
struct ElementSelector<ElementType::PYRAMID5> {
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

/// Format `ElementType` using fmt library
template <>
struct fmt::formatter<krado::ElementType> {
    constexpr auto
    parse(format_parse_context & ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const krado::ElementType & obj, FormatContext & ctx) const -> decltype(ctx.out())
    {
        switch (obj) {
        case krado::ElementType::POINT:
            return fmt::format_to(ctx.out(), "POINT");
        case krado::ElementType::LINE2:
            return fmt::format_to(ctx.out(), "LINE2");
        case krado::ElementType::TRI3:
            return fmt::format_to(ctx.out(), "TRI3");
        case krado::ElementType::QUAD4:
            return fmt::format_to(ctx.out(), "QUAD4");
        case krado::ElementType::TETRA4:
            return fmt::format_to(ctx.out(), "TETRA4");
        case krado::ElementType::HEX8:
            return fmt::format_to(ctx.out(), "HE8");
        case krado::ElementType::PYRAMID5:
            return fmt::format_to(ctx.out(), "PYRAMID5");
        case krado::ElementType::PRISM6:
            return fmt::format_to(ctx.out(), "PRISM6");
        }
        krado::utils::unreachable();
    }
};
