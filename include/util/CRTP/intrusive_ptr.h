/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 15:52:18
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 15:52:55
 */

#pragma once

#include <iostream>
#include <memory>

namespace MetaWaveCompiler
{
    namespace util
    {

        /// Forward declare acquire and release methods
        /// @{
        // template <typename T>
        // void acquire(const T *);
        // template <typename T>
        // void release(const T *);
        /// @}

        /// This class provides an intrusive pointer, which is a pointer that stores its
        /// reference count in the managed class.  The managed class must therefore have
        /// a reference count field and provide two functions 'acquire' and 'release'
        /// to acquire and release a reference on itself.
        ///
        /// For example:
        /// struct X {
        ///   mutable long ref = 0;
        ///   friend void acquire(const X *x) { ++x->ref; }
        ///   friend void release(const X *x) { if (--x->ref ==0) delete x; }
        /// };

        /// This class provides a smart pointer wrapper around std::shared_ptr
        /// with the same interface as the original intrusive pointer implementation.
        /// Now uses std::shared_ptr's automatic reference counting instead of
        /// manual acquire/release calls.
        template <class T>
        class IntrusivePtr : public std::shared_ptr<T>
        {
        public:
            using base_t = std::shared_ptr<T>;

            /// Allocate an undefined IntrusivePtr
            IntrusivePtr() : base_t{} {}

            /// Allocate an IntrusivePtr with an object
            IntrusivePtr(T *p) : base_t{p} {}

            /// Copy constructor
            IntrusivePtr(const IntrusivePtr &other) : base_t{other} {}

            /// Move constructor
            IntrusivePtr(IntrusivePtr &&other) : base_t{std::move(other)} {}

            /// Copy assignment operator
            IntrusivePtr &operator=(const IntrusivePtr &other)
            {
                base_t::operator=(other);
                return *this;
            }

            /// Copy assignment operator for managed object
            IntrusivePtr &operator=(T *p)
            {
                base_t::reset(p);
                return *this;
            }

            /// Move assignment operator
            IntrusivePtr &operator=(IntrusivePtr &&other)
            {
                base_t::operator=(std::move(other));
                return *this;
            }

            /// Destroy the intrusive ptr (handled automatically by std::shared_ptr)
            virtual ~IntrusivePtr() = default;

            /// Check whether the pointer is defined (ptr is not null).
            bool defined() const { return this->get() != nullptr; }

            // Comparison operators
            friend inline bool operator==(const IntrusivePtr<T> &p1, const IntrusivePtr<T> &p2)
            {
                return p1.get() == p2.get();
            }

            friend inline bool operator!=(const IntrusivePtr<T> &p1, const IntrusivePtr<T> &p2)
            {
                return p1.get() != p2.get();
            }

            friend inline bool operator<(const IntrusivePtr<T> &p1, const IntrusivePtr<T> &p2)
            {
                return p1.get() < p2.get();
            }

            friend inline bool operator>(const IntrusivePtr<T> &p1, const IntrusivePtr<T> &p2)
            {
                return p1.get() > p2.get();
            }

            friend inline bool operator<=(const IntrusivePtr<T> &p1, const IntrusivePtr<T> &p2)
            {
                return p1.get() <= p2.get();
            }

            friend inline bool operator>=(const IntrusivePtr<T> &p1, const IntrusivePtr<T> &p2)
            {
                return p1.get() >= p2.get();
            }
        };

    }; // namespace util
}; // namespace MetaWaveCompiler