// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/exception.h"
#include <utility>

namespace krado {

template <typename T>
class WeakPtr;

/// Reference counted pointer.  It works like `std::shared_ptr<T>`
///
/// @tparam T C++ type we point to
template <typename T>
class Ptr {
private:
    struct ControlBlock {
        T * ptr;
        std::size_t strong;
        std::size_t weak;

        explicit ControlBlock(T * p) : ptr(p), strong(1), weak(0) {}
    };

    ControlBlock * ctrl_;

public:
    Ptr() : ctrl_(nullptr) {}

    // Construct from `nullptr`
    Ptr(std::nullptr_t) : ctrl_(nullptr) {}

    // Cross-type constructor (Ptr<U> -> Ptr<T>)
    template <typename U>
    Ptr(const Ptr<U> & other, T * casted) : ctrl_(nullptr)
    {
        if (casted) {
            this->ctrl_ = reinterpret_cast<ControlBlock *>(other.ctrl_);
            if (this->ctrl_)
                ++this->ctrl_->strong;
        }
    }

    // Copy constructor
    Ptr(const Ptr & other) : ctrl_(other.ctrl_)
    {
        if (this->ctrl_)
            ++this->ctrl_->strong;
    }

    // Move constructor
    Ptr(Ptr && other) noexcept : ctrl_(other.ctrl_) { other.ctrl_ = nullptr; }

    // Converting copy constructor
    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    Ptr(const Ptr<U> & other) : ctrl_(nullptr)
    {
        if (other.ctrl_) {
            this->ctrl_ = reinterpret_cast<ControlBlock *>(other.ctrl_);
            ++this->ctrl_->strong;
        }
    }

    // Copy assignment
    Ptr &
    operator=(const Ptr & other)
    {
        if (this != &other) {
            release();
            this->ctrl_ = reinterpret_cast<ControlBlock *>(other.ctrl_);
            if (this->ctrl_)
                ++this->ctrl_->strong;
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
            this->ctrl_ = reinterpret_cast<ControlBlock *>(other.ctrl_);
            if (this->ctrl_)
                ++this->ctrl_->strong;
        }
        return *this;
    }

    // Assignment from `nullptr`
    Ptr &
    operator=(std::nullptr_t)
    {
        release();
        this->ctrl_ = nullptr;
        return *this;
    }

    // Move assignment
    Ptr &
    operator=(Ptr && other) noexcept
    {
        if (this != &other) {
            release();
            this->ctrl_ = other.ctrl_;
            other.ctrl_ = nullptr;
        }
        return *this;
    }

    ~Ptr() { release(); }

    explicit
    operator bool() const
    {
        return get() != nullptr;
    }

    /// Dereference the pointer
    T &
    operator*() const
    {
        if (this->ctrl_)
            return *this->ctrl_->ptr;
        else
            throw Exception("Access into a null pointer");
    }

    // Access the pointer
    T *
    operator->() const
    {
        return this->ctrl_->ptr;
    }

    // Compare two Ptr<T> of the same type
    bool
    operator==(const Ptr & other) const
    {
        return get() == other.get();
    }

    bool
    operator!=(const Ptr & other) const
    {
        return get() != other.get();
    }

    // Compare two Ptr<U> of different types
    template <typename U>
    bool
    operator==(const Ptr<U> & other) const
    {
        return get() == other.get();
    }

    template <typename U>
    bool
    operator!=(const Ptr<U> & other) const
    {
        return get() != other.get();
    }

    // Compare against nullptr
    bool
    operator==(std::nullptr_t) const
    {
        return get() == nullptr;
    }

    bool
    operator!=(std::nullptr_t) const
    {
        return get() != nullptr;
    }

    // Operator <
    bool
    operator<(std::nullptr_t) const
    {
        return get() < nullptr;
    }

    bool
    operator<(const Ptr & other) const
    {
        return get() < other.get();
    }

    template <typename U>
    bool
    operator<(const Ptr<U> & other) const
    {
        return get() < other.get();
    }

    // Get the pointer
    [[nodiscard]] T *
    get() const
    {
        return this->ctrl_ ? this->ctrl_->ptr : nullptr;
    }

    // Get the reference count
    [[nodiscard]] int
    ref_count() const
    {
        return this->ctrl_ ? this->ctrl_->strong : 0;
    }

    [[nodiscard]] int
    weak_count() const
    {
        return this->ctrl_ ? this->ctrl_->weak : 0;
    }

    // Is this a null pointer?
    [[nodiscard]] bool
    is_null() const
    {
        return this->ctrl_ == nullptr;
    }

    explicit Ptr(T * ptr) : ctrl_(new ControlBlock(ptr)) {}

private:
    void
    release()
    {
        if (this->ctrl_ && --this->ctrl_->strong == 0) {
            delete this->ctrl_->ptr;
            if (this->ctrl_->weak == 0)
                delete this->ctrl_;
        }
    }

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
    static Ptr<T>
    cast(const Ptr<U> & other)
    {
        T * casted = static_cast<T *>(other.get());
        return Ptr<T>(other, casted);
    }

    template <typename U>
    static Ptr<T>
    downcast(const Ptr<U> & other)
    {
        T * casted = dynamic_cast<T *>(other.get());
        if (casted)
            return Ptr<T>(other, casted);
        return nullptr;
    }

    template <typename U>
    friend class Ptr;

    template <typename U>
    friend class WeakPtr;
};

template <typename T>
class WeakPtr {
private:
    typename Ptr<T>::ControlBlock * cb_ = nullptr;

    void
    release()
    {
        if (!this->cb_)
            return;

        // If this was the last weak reference and the object is dead, clean up control block
        if (this->cb_->weak-- == 1) {
            if (this->cb_->strong == 0) {
                delete this->cb_;
            }
        }
        this->cb_ = nullptr;
    }

public:
    WeakPtr() noexcept = default;

    // Construct from `nullptr`
    WeakPtr(std::nullptr_t) : cb_(nullptr) {}

    WeakPtr(const Ptr<T> & ptr) noexcept : cb_(ptr.ctrl_)
    {
        if (this->cb_) {
            this->cb_->weak++;
        }
    }

    WeakPtr(const WeakPtr & other) noexcept : cb_(other.cb_)
    {
        if (this->cb_) {
            this->cb_->weak++;
        }
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    WeakPtr(const WeakPtr<U> & other) : cb_(nullptr)
    {
        if (other.cb_) {
            this->cb_ = reinterpret_cast<Ptr<T>::ControlBlock *>(other.cb_);
            this->cb_->weak++;
        }
    }

    WeakPtr(WeakPtr && other) noexcept : cb_(other.cb_) { other.cb_ = nullptr; }

    WeakPtr &
    operator=(const WeakPtr & other) noexcept
    {
        if (this != &other) {
            release();
            this->cb_ = other.cb_;
            if (this->cb_) {
                this->cb_->weak++;
            }
        }
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T *>::value>>
    WeakPtr &
    operator=(const WeakPtr<U> & other)
    {
        if (reinterpret_cast<const void *>(this) != reinterpret_cast<const void *>(&other)) {
            release();
            this->cb_ = reinterpret_cast<Ptr<T>::ControlBlock *>(other.cb_);
            if (this->cb_)
                this->cb_->weak++;
        }
        return *this;
    }

    WeakPtr &
    operator=(WeakPtr && other) noexcept
    {
        if (this != &other) {
            release();
            this->cb_ = other.cb_;
            other.cb_ = nullptr;
        }
        return *this;
    }

    WeakPtr &
    operator=(const Ptr<T> & ptr) noexcept
    {
        release();
        this->cb_ = ptr.cb_;
        if (this->cb_) {
            this->cb_->weak++;
        }
        return *this;
    }

    ~WeakPtr() { release(); }

    explicit
    operator bool() const
    {
        return get() != nullptr;
    }

    /// Dereference the pointer
    T &
    operator*() const
    {
        if (this->cb_)
            return *this->cb_->ptr;
        else
            throw Exception("Access into a null pointer");
    }

    // Access the pointer
    T *
    operator->() const
    {
        return this->cb_->ptr;
    }

    // Compare two WeakPtr<T> of the same type
    bool
    operator==(const WeakPtr & other) const
    {
        return get() == other.get();
    }

    // Compare against nullptr
    bool
    operator==(std::nullptr_t) const
    {
        return get() == nullptr;
    }

    // Compare two Ptr<U> of different types
    template <typename U>
    bool
    operator==(const WeakPtr<U> & other) const
    {
        return get() == other.get();
    }

    // Get the pointer
    [[nodiscard]] T *
    get() const
    {
        return this->cb_ ? this->cb_->ptr : nullptr;
    }

    [[nodiscard]] bool
    expired() const noexcept
    {
        return !this->cb_ || this->cb_->strong == 0;
    }

    // Safely promote to Ptr<T> if object is still alive
    Ptr<T>
    lock() const noexcept
    {
        if (this->cb_ && this->cb_->strong > 0) {
            this->cb_->strong++;
            Ptr<T> p;
            p.ctrl_ = this->cb_;
            return p;
        }
        return Ptr<T>();
    }

    // Is this a null pointer?
    [[nodiscard]] bool
    is_null() const
    {
        return this->cb_ == nullptr;
    }

    template <typename U>
    friend class WeakPtr;
};

template <typename T, typename U>
Ptr<T>
static_ptr_cast(const Ptr<U> & other)
{
    T * casted = static_cast<T *>(other.get());
    return Ptr<T>(other, casted);
}

template <typename T, typename U>
Ptr<T>
dynamic_ptr_cast(const Ptr<U> & other)
{
    T * casted = dynamic_cast<T *>(other.get());
    if (casted)
        return Ptr<T>(other, casted);
    return nullptr;
}

template <typename T>
WeakPtr<T>
weak_ptr(const Ptr<T> & other)
{
    return WeakPtr<T>(other);
}

} // namespace krado
