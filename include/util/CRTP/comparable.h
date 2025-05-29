/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 08:42:03
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 22:49:34
 */

#pragma once

namespace MetaWaveCompiler
{
    namespace util
    {

        // NOTE: 不错的写法！

        /// Interface for classes that can be compared to each other. Classes that
        /// implement this interfaces define `==` and `<` to get `!=`, `>`, `<=`, and
        /// `>=` for free.
        template <class T>
        class Comparable
        {
        };

        template <class T>
        bool operator!=(const Comparable<T> &lhs, const Comparable<T> &rhs)
        {
            return !(*static_cast<const T *>(&lhs) == *static_cast<const T *>(&rhs));
        }

        template <class T>
        bool operator>(const Comparable<T> &lhs, const Comparable<T> &rhs)
        {
            return !(*static_cast<const T *>(&lhs) < *static_cast<const T *>(&rhs) || *static_cast<const T *>(&lhs) == *static_cast<const T *>(&rhs));
        }

        template <class T>
        bool operator<=(const Comparable<T> &lhs, const Comparable<T> &rhs)
        {
            return !(*static_cast<const T *>(&lhs) > *static_cast<const T *>(&rhs));
        }

        template <class T>
        bool operator>=(const Comparable<T> &lhs, const Comparable<T> &rhs)
        {
            return !(*static_cast<const T *>(&lhs) < *static_cast<const T *>(&rhs));
        }

    } // namespace util
} // namespace MetaWaveCompiler