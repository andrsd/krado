// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>
#include <cstddef>
#include <type_traits>

namespace krado {

/// This behaves like std::unique_ptr
template <typename T>
class Qtr {
    static_assert(sizeof(T), "Qtr<T>: T must be complete at destruction time.");
    static_assert(!std::is_void<T>::value, "Can't delete incomplete type");

public:
    constexpr Qtr() noexcept : ptr_(nullptr) {}
    constexpr Qtr(std::nullptr_t) noexcept : Qtr() {}
    explicit Qtr(T * ptr) noexcept : ptr_(ptr) {}

    Qtr(const Qtr &) = delete;
    Qtr & operator=(const Qtr &) = delete;

    Qtr(Qtr && other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }

    template <class U, class = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    Qtr(Qtr<U> && other) noexcept : ptr_(other.release())
    {
    }

    Qtr &
    operator=(Qtr && other) noexcept
    {
        if (this != &other) {
            reset();
            this->ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    template <class U, class = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    Qtr &
    operator=(Qtr<U> && other) noexcept
    {
        reset(other.release());
        return *this;
    }

    ~Qtr() { reset(); }

    T *
    get() const noexcept
    {
        return this->ptr_;
    }

    bool
    is_null() const noexcept
    {
        return this->ptr_ == nullptr;
    }

    explicit
    operator bool() const noexcept
    {
        return !is_null();
    }

    bool
    operator==(std::nullptr_t) const noexcept
    {
        return is_null();
    }

    bool
    operator!=(std::nullptr_t) const noexcept
    {
        return !is_null();
    }

    T &
    operator*() const
    {
        return *this->ptr_;
    }

    T *
    operator->() const noexcept
    {
        return this->ptr_;
    }

    T *
    release() noexcept
    {
        T * tmp = this->ptr_;
        this->ptr_ = nullptr;
        return tmp;
    }

    void
    reset(T * p = nullptr) noexcept
    {
        if (this->ptr_ != p) {
            if (this->ptr_)
                delete this->ptr_;
            this->ptr_ = p;
        }
    }

    void
    swap(Qtr & other) noexcept
    {
        std::swap(this->ptr_, other.ptr_);
    }

private:
    T * ptr_;

public:
    template <typename... Args>
    static Qtr<T>
    alloc(Args &&... args)
    {
        return Qtr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename U>
    friend class Qtr;
};

} // namespace krado
