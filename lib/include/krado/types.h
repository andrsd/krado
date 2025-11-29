// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace krado {

/// Local indexing type
using lidx_t = unsigned int;
/// Global indexing type
using gidx_t = std::size_t;
/// Marker type
using marker_t = int;
///
template <typename T>
using Optional = std::optional<T>;

/// 32-bit integer
using int32 = std::int32_t;

/// 64-bit integer
using int64 = std::int64_t;

enum class ElementType {
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
    HEX8
};

/// Side set entry
struct side_set_entry_t {
    /// Element ID
    gidx_t elem;
    /// Local side number
    std::size_t side;

    side_set_entry_t(gidx_t elem, std::size_t side) : elem(elem), side(side) {}
};

/// Equality operator for side set entry
inline bool
operator==(const side_set_entry_t & lhs, const side_set_entry_t & rhs)
{
    return lhs.elem == rhs.elem && lhs.side == rhs.side;
}

} // namespace krado
