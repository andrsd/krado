// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/exception.h"
#include <utility>

namespace krado {

/// Reference counted pointer.  It works like `std::shared_ptr<T>`
///
/// @tparam T C++ type we point to
template <typename T>
class Ptr {
public:
    Ptr() : ptr_(nullptr), ref_count_(nullptr), weak_count_(nullptr) {}

    Ptr(const Ptr & other) :
        ptr_(other.ptr_),
        ref_count_(other.ref_count_),
        weak_count_(other.weak_count_)
    {
        if (this->ptr_)
            ++(*this->ref_count_);
    }

    // Converting copy constructor
    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    Ptr(const Ptr<U> & other) :
        ptr_(other.ptr_),
        ref_count_(other.ref_count_),
        weak_count_(other.weak_count_)
    {
        if (this->ptr_)
            ++(*this->ref_count_);
    }

    Ptr &
    operator=(const Ptr & other)
    {
        if (this != &other) {
            release();
            this->ptr_ = other.ptr_;
            this->ref_count_ = other.ref_count_;
            this->weak_count_ = other.weak_count_;
            if (this->ptr_)
                ++(*this->ref_count_);
        }
        return *this;
    }

    // Converting assignment
    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    Ptr &
    operator=(const Ptr<U> & other)
    {
        if (reinterpret_cast<const void *>(this) != reinterpret_cast<const void *>(&other)) {
            release();
            this->ptr_ = other.ptr_;
            this->ref_count_ = other.ref_count_;
            this->weak_count_ = other.weak_count_;
            if (this->ptr_)
                ++(*this->ref_count_);
        }
        return *this;
    }

    ~Ptr() { release(); }

    operator bool() const { return this->ptr_ != nullptr; }

    /// Dereference the pointer
    T &
    operator*() const
    {
        if (this->ptr_)
            return *this->ptr_;
        else
            throw Exception("Access into a null pointer");
    }

    /// Access the pointer
    T *
    operator->() const
    {
        return this->ptr_;
    }

    /// Compare two pointers
    bool
    operator==(const Ptr<T> & other) const
    {
        return this->ptr_ == other.ptr_;
    }

    bool
    operator!=(const Ptr<T> & other) const
    {
        return this->ptr_ != other.ptr_;
    }

    /// Get the pointer
    T *
    get() const
    {
        return this->ptr_;
    }

    /// Get the reference count
    ///
    /// @return The reference count
    int
    ref_count() const
    {
        return this->ptr_ ? *this->ref_count_ : 0;
    }

    bool
    is_null() const
    {
        return this->ptr_ == nullptr;
    }

private:
    explicit Ptr(T * ptr) :
        ptr_(ptr),
        ref_count_(new std::size_t(1)),
        weak_count_(new std::size_t(0))
    {
    }

    void
    release()
    {
        if (this->ptr_ && --(*this->ref_count_) == 0) {
            delete this->ptr_;
            if (*this->weak_count_ == 0) {
                delete this->ref_count_;
                delete this->weak_count_;
            }
        }
    }

    T * ptr_;
    std::size_t * ref_count_;
    std::size_t * weak_count_;

public:
    /// Allocate a new object and return a `Ptr` to it
    ///
    /// @tparam T C++ type we point to
    /// @tparam ARGS
    /// @param args Arguments passed into a constructor
    template <typename... ARGS>
    static Ptr<T>
    alloc(ARGS &&... args)
    {
        return Ptr<T>(new T(std::forward<ARGS>(args)...));
    }

    template <typename U>
    friend class Ptr;
};

} // namespace krado
