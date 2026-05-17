// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include <iterator>
#include <ostream>

namespace krado {

class Range {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Index;
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
        Index idx_ = 0;
    };

    Range(Index start, Index end) : start_idx_(start), end_idx_(end) {}

    Iterator
    begin() const
    {
        return Iterator(this->start_idx_);
    }

    Iterator
    end() const
    {
        return Iterator(this->end_idx_);
    }

    [[nodiscard]] Index
    first() const
    {
        return this->start_idx_;
    }

    [[nodiscard]] Index
    last() const
    {
        return this->end_idx_;
    }

    /// Get the number of indices in the range
    [[nodiscard]] Index
    size() const
    {
        return this->end_idx_ - this->start_idx_;
    }

    void
    expand(Index v)
    {
        this->start_idx_ = std::min(this->start_idx_, v);
        this->end_idx_ = std::max(this->end_idx_, v + 1);
    }

private:
    Index start_idx_;
    Index end_idx_;
};

inline bool
operator==(const Range & a, const Range & b)
{
    return (a.first() == b.first()) && (a.last() == b.last());
}

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Range & el)
{
    stream << "[" << el.first() << ", " << el.last() << "]";
    return stream;
}
