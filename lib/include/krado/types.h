#pragma once

#include <cstddef>

namespace krado {

/// Local indexing type
using lidx_t = unsigned int;
/// Global indexing type
using gidx_t = std::size_t;
/// Marker type
using marker_t = int;

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

} // namespace krado
