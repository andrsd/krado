// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include <iterator>
#include <ostream>
#include <concepts>

namespace krado {

template <typename T>
class TRange {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type *;

        Iterator() = default;
        explicit Iterator(value_type idx) : idx_(idx) {}

        const value_type &
        operator*() const
        {
            return this->idx_;
        }

        pointer
        operator->() const
        {
            return &this->idx_;
        }

        /// Prefix increment
        Iterator &
        operator++()
        {
            this->idx_++;
            return *this;
        }

        /// Postfix increment
        Iterator
        operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool
        operator==(const Iterator & a, const Iterator & b)
        {
            return a.idx_ == b.idx_;
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return a.idx_ != b.idx_;
        };

    private:
        T idx_ = T(0);
    };

    TRange(T start, T end) : start_idx_(start), end_idx_(end) {}

    [[nodiscard]] Iterator
    begin() const
    {
        return Iterator(this->start_idx_);
    }

    [[nodiscard]] Iterator
    end() const
    {
        return Iterator(this->end_idx_);
    }

    [[nodiscard]] T
    first() const
    {
        return this->start_idx_;
    }

    [[nodiscard]] T
    last() const
    {
        return this->end_idx_;
    }

    /// Get the number of indices in the range
    [[nodiscard]] T
    size() const
    {
        return this->end_idx_ - this->start_idx_;
    }

    void
    expand(T v)
    {
        this->start_idx_ = std::min(this->start_idx_, v);
        this->end_idx_ = std::max(this->end_idx_, v + 1);
    }

    [[nodiscard]] bool
    contains(T idx) const
    {
        return (this->start_idx_ <= idx) and (idx < this->end_idx_);
    }

private:
    T start_idx_;
    T end_idx_;
};

template <typename T>
inline bool
operator==(const TRange<T> & a, const TRange<T> & b)
{
    return (a.first() == b.first()) && (a.last() == b.last());
}

class Range : public TRange<Index> {
public:
    Range(Index start, Index end) : krado::TRange<Index>(start, end) {}
};

/// Create a range from `start` to `end`
///
/// @param start First element
/// @param end Last elements (excluded)
template <typename T, typename U>
inline TRange<T>
make_range(T start, U end)
{
    return { static_cast<T>(start), static_cast<T>(end) };
}

/// Create a range from 0 to `end`
///
/// @param end Last elements (excluded)
template <typename T>
inline TRange<T>
make_range(T end)
{
    return { 0, end };
}

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Range & el)
{
    stream << "[" << el.first() << ", " << el.last() << "]";
    return stream;
}

inline std::ostream &
operator<<(std::ostream & stream, const krado::TRange<krado::HasseIndex> & el)
{
    stream << "[" << el.first().value() << ", " << el.last().value() << "]";
    return stream;
}
