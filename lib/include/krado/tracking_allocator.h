#pragma once

#include <memory>
#include <iostream>
#include "krado/log.h"
#include "krado/utils.h"

namespace krado {

template <typename T>
class TrackingAllocator {
public:
    using value_type = T;

    TrackingAllocator() = default;

    template <typename U>
    constexpr TrackingAllocator(const TrackingAllocator<U> &) noexcept
    {
    }

    [[nodiscard]] T *
    allocate(std::size_t n)
    {
        Log::debug("alloc: requested {} elements ({} bytes)",
                   utils::human_number(n),
                   utils::human_number(n * sizeof(T)));
        return std::allocator<T> {}.allocate(n);
    }

    void
    deallocate(T * p, std::size_t n) noexcept
    {
        Log::debug("free: released {} elements ({} bytes)",
                   utils::human_number(n),
                   utils::human_number(n * sizeof(T)));
        std::allocator<T> {}.deallocate(p, n);
    }
};

template <typename T, typename U>
bool
operator==(const TrackingAllocator<T> &, const TrackingAllocator<U> &)
{
    return true;
}

template <typename T, typename U>
bool
operator!=(const TrackingAllocator<T> &, const TrackingAllocator<U> &)
{
    return false;
}

} // namespace krado
