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

/// Class that represents an element
class Element {
private:
    static constexpr u8 MAX_INDICES = 8;

    template <std::size_t N>
    Element(ElementType et, const std::array<Index, N> & vtx_ids) : elem_type_(et), n_ids_(N)
    {
        std::memcpy(this->vtx_id_.data(), vtx_ids.data(), this->n_ids_ * sizeof(Index));
    }

public:
    /// Build an element
    ///
    /// @param type Element type
    /// @param vtx_ids Vertex IDs composing the element
    Element(ElementType type, const std::vector<Index> & vtx_ids);

    /// Get element type
    ///
    /// @return Element type
    [[nodiscard]] ElementType type() const;

    /// Get number of vertices
    ///
    /// @return Number of vertices
    [[nodiscard]] u8 num_vertices() const;

    /// Get vertex ID fom local index
    ///
    /// @param idx Local vertex index
    /// @return Vertex ID
    [[nodiscard]] Index index(u8 idx) const;

    /// Get vertex IDs
    ///
    /// @return Vertex IDs
    [[nodiscard]] Span<const Index> indices() const;

    /// Shift element indices by offset
    ///
    /// @param ofst Offset by which we shift the indices
    void shift(Index ofst);

private:
    /// Element type
    ElementType elem_type_ = ElementType::INVALID;
    /// Number of valid indices in `vtx_id_`
    u8 n_ids_ = 0;
    /// Global index into `points` array
    std::array<Index, MAX_INDICES> vtx_id_;

public:
    [[nodiscard]] static std::string type(ElementType type);
    [[nodiscard]] static Element Point(Index id);
    [[nodiscard]] static Element Line2(const std::array<Index, 2> & ids);
    [[nodiscard]] static Element Tri3(const std::array<Index, 3> & ids);
    [[nodiscard]] static Element Quad4(const std::array<Index, 4> & ids);
    [[nodiscard]] static Element Tetra4(const std::array<Index, 4> & ids);
    [[nodiscard]] static Element Pyramid5(const std::array<Index, 5> & ids);
    [[nodiscard]] static Element Prism6(const std::array<Index, 6> & ids);
    [[nodiscard]] static Element Hex8(const std::array<Index, 8> & ids);

    friend class Mesh;
};

bool operator==(const Element & a, const Element & b);

class Line2 {
public:
    static constexpr ElementType TYPE = ElementType::LINE2;
    static constexpr u8 N_VERTICES = 2;
    static const std::array<u8, 2> EDGE_VERTICES;
};

class Tri3 {
public:
    static constexpr ElementType TYPE = ElementType::TRI3;
    static constexpr u8 N_VERTICES = 3;
    static constexpr u8 N_EDGES = 3;
    static const std::vector<u8> EDGES;
    static const std::vector<std::array<u8, 2>> EDGE_VERTICES;

    /// Quality of element
    ///
    /// @param elem
    /// @return Quality [0..1]
    static double gamma(Point pa, Point pb, Point pc);

    static double eta(Point pa, Point pb, Point pc);

    /// Compute circum center in (u, v) parameter space
    ///
    /// @param p1 Point one
    /// @param p2 Point two
    /// @param p3 Point three
    /// @return Circum center, nullopt if points are colinear
    static Optional<UVParam> circum_center(UVParam p1, UVParam p2, UVParam p3);

    /// Compute circum center
    ///
    /// @param p1 Point one
    /// @param p2 Point two
    /// @param p3 Point three
    /// @return Circum center
    static Optional<Point> circum_center(Point p1, Point p2, Point p3);

    /// Compute circum radius using quality norm
    static double circum_radius_quality(Point pa, Point pb, Point pc);

    /// Compute circum radius using euclidian norm
    static double circum_radius_euclidian(Point pa, Point pb, Point pc, double lc);
};

class Quad4 {
public:
    static constexpr ElementType TYPE = ElementType::QUAD4;
    static constexpr u8 N_VERTICES = 4;
    static constexpr u8 N_EDGES = 4;
    static const std::vector<u8> EDGES;
    static const std::vector<std::array<u8, 2>> EDGE_VERTICES;
};

class Tetra4 {
public:
    static constexpr ElementType TYPE = ElementType::TETRA4;
    static constexpr u8 N_VERTICES = 4;
    static constexpr u8 N_EDGES = 6;
    static constexpr u8 N_FACES = 4;
    static const std::vector<u8> EDGES;
    static const std::vector<std::array<u8, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<u8>> FACE_EDGES;
    static const std::vector<std::vector<u8>> FACE_VERTICES;
};

class Pyramid5 {
public:
    static constexpr ElementType TYPE = ElementType::PYRAMID5;
    static constexpr u8 N_VERTICES = 5;
    static constexpr u8 N_EDGES = 8;
    static constexpr u8 N_FACES = 5;
    static const std::vector<u8> EDGES;
    static const std::vector<std::array<u8, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<u8>> FACE_EDGES;
    static const std::vector<std::vector<u8>> FACE_VERTICES;
};

class Prism6 {
public:
    static constexpr ElementType TYPE = ElementType::PRISM6;
    static constexpr u8 N_VERTICES = 6;
    static constexpr u8 N_EDGES = 9;
    static constexpr u8 N_FACES = 5;
    static const std::vector<u8> EDGES;
    static const std::vector<std::array<u8, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<u8>> FACE_EDGES;
    static const std::vector<std::vector<u8>> FACE_VERTICES;
};

class Hex8 {
public:
    static constexpr ElementType TYPE = ElementType::HEX8;
    static constexpr u8 N_VERTICES = 8;
    static constexpr u8 N_EDGES = 12;
    static constexpr u8 N_FACES = 6;
    static const std::vector<u8> EDGES;
    static const std::vector<std::array<u8, 2>> EDGE_VERTICES;
    static const std::vector<std::vector<u8>> FACE_EDGES;
    static const std::vector<std::vector<u8>> FACE_VERTICES;
};

//

template <ElementType ET>
struct ElementSelector;

template <>
struct ElementSelector<ElementType::LINE2> {
    static constexpr u8 N_VERTICES = Line2::N_VERTICES;
};

template <>
struct ElementSelector<ElementType::TRI3> {
    static constexpr u8 N_VERTICES = Tri3::N_VERTICES;
};

template <>
struct ElementSelector<ElementType::QUAD4> {
    static constexpr u8 N_VERTICES = Quad4::N_VERTICES;
};

template <>
struct ElementSelector<ElementType::TETRA4> {
    static constexpr u8 N_VERTICES = Tetra4::N_VERTICES;
    static constexpr u8 N_FACES = Tetra4::N_FACES;
};

template <>
struct ElementSelector<ElementType::HEX8> {
    static constexpr u8 N_VERTICES = Hex8::N_VERTICES;
    static constexpr u8 N_FACES = Hex8::N_FACES;
};

template <>
struct ElementSelector<ElementType::PRISM6> {
    static constexpr u8 N_VERTICES = Prism6::N_VERTICES;
    static constexpr u8 N_FACES = Prism6::N_FACES;
};

template <>
struct ElementSelector<ElementType::PYRAMID5> {
    static constexpr u8 N_VERTICES = Pyramid5::N_VERTICES;
    static constexpr u8 N_FACES = Pyramid5::N_FACES;
};

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Element & el)
{
    stream << "(" << krado::Element::type(el.type()) << ",";
    for (auto & id : el.indices())
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
        return fmt::format_to(ctx.out(), "{}", krado::Element::type(obj));
    }
};
