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
        using value_type = gidx_t;

        explicit Iterator(value_type idx) : idx(idx) {}

        const value_type &
        operator*() const
        {
            return this->idx;
        }

        /// Prefix increment
        Iterator &
        operator++()
        {
            this->idx++;
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
            return a.idx == b.idx;
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return a.idx != b.idx;
        };

    private:
        gidx_t idx;
    };

    Range(gidx_t start, gidx_t end) : start_idx(start), end_idx(end) {}

    Iterator
    begin() const
    {
        return Iterator(this->start_idx);
    }

    Iterator
    end() const
    {
        return Iterator(this->end_idx + 1);
    }

    gidx_t
    first() const
    {
        return this->start_idx;
    }

    gidx_t
    last() const
    {
        return this->end_idx;
    }

    /// Get the number of indices in the range
    gidx_t
    size() const
    {
        return this->end_idx - this->start_idx + 1;
    }

    void
    expand(gidx_t v)
    {
        this->start_idx = std::min(this->start_idx, v);
        this->end_idx = std::max(this->end_idx, v);
    }

private:
    gidx_t start_idx;
    gidx_t end_idx;
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
