// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "fmt/core.h"
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

/// Shape identifier
class ShapeID {
public:
    explicit constexpr ShapeID() : value_(-1) {}
    constexpr ShapeID(int32 id) : value_(id) {}
    constexpr ShapeID(std::size_t id) : value_(id) {}

    constexpr ShapeID &
    operator=(std::size_t id)
    {
        this->value_ = id;
        return *this;
    }

    constexpr int32
    value() const
    {
        return this->value_;
    }

    constexpr bool
    operator==(int32 other) const
    {
        return this->value_ == other;
    }

    constexpr bool
    operator!=(int32 other) const
    {
        return this->value_ != other;
    }

    constexpr bool
    operator<(ShapeID other) const
    {
        return this->value_ < other.value_;
    }

    constexpr bool
    operator<(int32 other) const
    {
        return this->value_ < other;
    }

private:
    int32 value_;
};

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

inline std::ostream &
operator<<(std::ostream & stream, const krado::ShapeID & id)
{
    stream << id.value();
    return stream;
}

template <>
struct fmt::formatter<krado::ShapeID> {
    constexpr auto
    parse(format_parse_context & ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const krado::ShapeID & obj, FormatContext & ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", obj.value());
    }
};
