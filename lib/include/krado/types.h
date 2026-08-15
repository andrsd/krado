// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <functional>
#include <span>
#include "fmt/core.h"

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
    u8 side;

    SideEntry(Index elem, u8 side) : elem(elem), side(side) {}
};

/// Equality operator for side set entry
inline bool
operator==(const SideEntry & lhs, const SideEntry & rhs)
{
    return lhs.elem == rhs.elem && lhs.side == rhs.side;
}

enum class QuadSplitMode : u8 {
    /// Split into 2 triangles
    SPLIT2,
    /// Split into 4 triangles
    SPLIT4
};

class HasseKey {
public:
    explicit constexpr HasseKey(u64 val) : value_(val) {}

    [[nodiscard]] constexpr u64
    value() const
    {
        return this->value_;
    }

    HasseKey
    operator++(int)
    {
        auto tmp = *this;
        ++(this->value_);
        return tmp;
    }

    HasseKey &
    operator+=(HasseKey other)
    {
        this->value_ += other.value_;
        return *this;
    }

    HasseKey &
    operator+=(u64 other)
    {
        this->value_ += other;
        return *this;
    }

    auto operator<=>(const HasseKey &) const = default;

private:
    u64 value_;
};

class HasseIndex {
public:
    explicit constexpr HasseIndex() : value_(0) {}
    explicit constexpr HasseIndex(u64 val) : value_(val) {}
    explicit constexpr HasseIndex(HasseKey val) : value_(val.value()) {}

    [[nodiscard]] constexpr u64
    value() const
    {
        return this->value_;
    }

    HasseIndex
    operator+(int other) const
    {
        return HasseIndex(this->value_ + other);
    }

    HasseIndex
    operator-(int other) const
    {
        return HasseIndex(this->value_ - other);
    }

    HasseIndex
    operator-() const
    {
        return static_cast<HasseIndex>(-this->value_);
    }

    constexpr bool
    operator<(HasseIndex other) const
    {
        return this->value_ < other.value_;
    }

    constexpr bool
    operator<=(HasseIndex other) const
    {
        return this->value_ <= other.value_;
    }

    constexpr bool
    operator==(HasseIndex other) const
    {
        return this->value_ == other.value_;
    }

    constexpr bool
    operator!=(HasseIndex other) const
    {
        return this->value_ != other.value_;
    }

    HasseIndex
    operator++()
    {
        this->value_++;
        return *this;
    }

    HasseIndex
    operator++(int)
    {
        auto tmp = *this;
        ++(this->value_);
        return tmp;
    }

private:
    u32 value_;
};

} // namespace krado

template <>
struct std::hash<krado::HasseKey> {
    std::size_t
    operator()(const krado::HasseKey & id) const noexcept
    {
        return std::hash<int> {}(id.value());
    }
};
