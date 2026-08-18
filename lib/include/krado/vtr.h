// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ref.h"
#include <memory>

namespace krado {

/// View pointer
///
/// This is a wrapper around a raw pointer
template <typename T>
class Vtr {
public:
    Vtr() : ptr_(nullptr) {}
    Vtr(std::nullptr_t) : ptr_(nullptr) {}
    Vtr(T * ptr) : ptr_(ptr) {}
    Vtr(Ref<T> ref) : ptr_(&ref.get()) {}

    T &
    operator*() const
    {
        return *this->ptr_;
    }

    T *
    operator->() const
    {
        return this->ptr_;
    }

    T *
    get() const
    {
        return this->ptr_;
    }

    // Is this a null pointer?
    [[nodiscard]] bool
    is_null() const
    {
        return this->ptr_ == nullptr;
    }

    bool
    operator==(const Vtr & other) const noexcept
    {
        return this->ptr_ == other.ptr_;
    }

    template <typename U>
    bool
    operator<(const Vtr<U> & other) const noexcept
    {
        return this->ptr_ < other.ptr_;
    }

private:
    T * ptr_;
};

template <typename T>
class Vtr<const T> {
public:
    Vtr() : ptr_(nullptr) {}
    Vtr(std::nullptr_t) : ptr_(nullptr) {}
    Vtr(const T * ptr) : ptr_(ptr) {}
    Vtr(Vtr<T> ptr) : ptr_(ptr.get()) {}
    Vtr(Ref<T> ref) : ptr_(&ref.get()) {}
    Vtr(Ref<const T> ref) : ptr_(&ref.get()) {}

    const T &
    operator*() const
    {
        return *this->ptr_;
    }

    const T *
    operator->() const
    {
        return this->ptr_;
    }

    const T *
    get() const
    {
        return this->ptr_;
    }

    // Is this a null pointer?
    [[nodiscard]] bool
    is_null() const
    {
        return this->ptr_ == nullptr;
    }

    bool
    operator==(const Vtr & other) const noexcept
    {
        return this->ptr_ == other.ptr_;
    }

    template <typename U>
    bool
    operator<(const Vtr<U> & other) const noexcept
    {
        return this->ptr_ < other.ptr_;
    }

private:
    const T * ptr_;
};

// Helper functions
template <typename T>
Vtr<T>
vtr(T * t) noexcept
{
    return Vtr<T>(t);
}

template <typename T>
Vtr<const T>
cvtr(const T * t) noexcept
{
    return Vtr<const T>(t);
}

template <typename T>
Vtr<const T>
cvtr(Vtr<T> t) noexcept
{
    return Vtr<const T>(t);
}

template <typename T>
inline bool
operator==(const Ref<const T> & a, const Vtr<const T> & b)
{
    return &a.get() == b.get();
}

template <typename T>
inline bool
operator==(const Vtr<const T> & a, const Ref<const T> & b)
{
    return a.get() == &b.get();
}

} // namespace krado
