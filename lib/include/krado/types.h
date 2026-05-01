// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "fmt/core.h"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace krado {

// signed integers
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

// unsigned integers
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

/// Global indexing type
using Index = std::uint32_t;
///
template <typename T>
using Optional = std::optional<T>;

template <typename T>
using Span = std::span<T>;

/// Shape identifier
using ShapeID = std::int32_t;

/// Marker type
using Marker = std::int32_t;

enum class ElementType : u8 {
    /// 0-D element
    POINT,
    /// 1-D element
    LINE2,
    /// Triangle
    TRI3,
    /// Quadrilateral
    QUAD4,
    /// Tetrahedron
    TETRA4,
    /// Pyramid
    PYRAMID5,
    /// Prism
    PRISM6,
    /// Hexahedron
    HEX8,
    /// Invalid
    INVALID
};

/// Side set entry
struct SideEntry {
    /// Element ID
    Index elem;
    /// Local side number
    std::size_t side;

    SideEntry(Index elem, std::size_t side) : elem(elem), side(side) {}
};

/// Equality operator for side set entry
inline bool
operator==(const SideEntry & lhs, const SideEntry & rhs)
{
    return lhs.elem == rhs.elem && lhs.side == rhs.side;
}

enum class QuadSplitMode {
    /// Split into 2 triangles
    SPLIT2,
    /// Split into 4 triangles
    SPLIT4
};

} // namespace krado
