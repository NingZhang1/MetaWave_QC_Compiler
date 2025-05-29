/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 21:26:32
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 22:19:29
 */

#pragma once

#include <functional>

namespace MetaWaveCompiler
{
    namespace util
    {

        /**
         * @brief 获取 std::function 对象中存储的函数指针地址
         *
         * @tparam T 函数返回值类型
         * @tparam U 函数参数类型包（可变参数模板）
         * @tparam Fnptr 函数指针类型，默认为 T(*)(U...)
         *
         * @param f 要获取地址的 std::function 对象
         *
         * @return Fnptr 返回函数指针。如果 std::function 不包含函数指针，则返回 nullptr
         *
         * @note 此函数仅适用于存储普通函数指针的 std::function 对象，
         *       不适用于存储成员函数指针、lambda 表达式或其他可调用对象的情况
         */
        template <typename T, typename... U, typename Fnptr = T (*)(U...)>
        Fnptr functorAddress(std::function<T(U...)> f)
        {
            if (f.target_type() == typeid(T (*)(U...))) // NOTE: lambda 表达式不储存为函数指针
            {
                return *f.template target<T (*)(U...)>();
            }
            return nullptr;
        }

        /**
         * @brief 比较两个 std::function 对象是否指向相同的函数
         *
         * @tparam T 第一个函数的返回值类型
         * @tparam U 第一个函数的参数类型包
         * @tparam R 第二个函数的返回值类型
         * @tparam A 第二个函数的参数类型包
         *
         * @param f 第一个 std::function 对象
         * @param g 第二个 std::function 对象
         *
         * @return bool 如果两个函数指向相同的函数指针则返回 true，否则返回 false
         *
         * @note 此函数首先检查第一个函数指针是否有效（非空），
         *       然后比较两个函数指针是否指向相同的地址。
         *       此函数仅适用于比较普通函数指针，不适用于比较成员函数指针、
         *       lambda 表达式或其他可调用对象。
         */
        template <typename T, typename... U, typename R, typename... A>
        bool targetPtrEqual(std::function<T(U...)> f, std::function<R(A...)> g)
        {
            return functorAddress(f) != nullptr && functorAddress(g) != nullptr && functorAddress(f) == functorAddress(g);
        }

    } // namespace util
} // namespace MetaWaveCompiler