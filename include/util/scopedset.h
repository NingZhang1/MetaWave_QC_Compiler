/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 21:46:15
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 22:02:50
 */

#pragma once

#include <list>
#include <set>
#include <ostream>
#include "error.h"

namespace MetaWaveCompiler
{
    namespace util
    {

        /**
         * @brief 支持作用域管理的集合容器类
         *
         * @details ScopedSet 实现了一个支持多层作用域的集合数据结构。
         *          它使用栈式的作用域管理方式，支持动态地创建和销毁作用域，
         *          并在多个作用域中查找元素。主要特点：
         *          - 支持嵌套作用域管理
         *          - 遵循最近作用域优先原则
         *          - 自动的作用域生命周期管理
         *
         * 典型应用场景：
         * - 编译器的符号管理
         * - 作用域内唯一标识符的管理
         * - 需要分层管理唯一元素的场景
         *
         * @tparam Key 元素类型
         */
        template <typename Key>
        class ScopedSet
        {
        public:
            ScopedSet() { scope(); }

            ~ScopedSet() { unscope(); }

            /**
             * @brief 创建一个新的作用域层级
             *
             * @note 新作用域会被添加到作用域栈的顶部，成为当前活动作用域
             */
            void scope() { scopes.push_front(std::set<Key>()); }

            /**
             * @brief 移除最顶层的作用域
             *
             * @note 此操作会删除当前作用域中的所有元素
             */
            void unscope() { scopes.pop_front(); }

            /**
             * @brief 在当前作用域中插入元素
             *
             * @param key 要插入的元素
             * @note 插入操作只影响当前（最顶层）作用域
             */
            void insert(const Key &key) { scopes.front().insert(key); }

            /**
             * @brief 从最近的作用域(不一定是最顶层)中移除指定元素
             *
             * @param key 要移除的元素
             * @throw metawave_ierror 如果元素在所有作用域中都不存在
             */
            void remove(const Key &key)
            {
                for (auto &scope : scopes)
                {
                    const auto it = scope.find(key);
                    if (it != scope.end())
                    {
                        scope.erase(it);
                        return;
                    }
                }
                metawave_ierror << "Not in scope";
            }

            /**
             * @brief 检查指定的元素是否存在于任意作用域中
             *
             * @param key 要检查的元素
             * @return bool 如果元素存在于任意作用域中返回 true，否则返回 false
             */
            bool contains(const Key &key)
            {
                for (auto &scope : scopes)
                {
                    if (scope.find(key) != scope.end())
                    {
                        return true;
                    }
                }
                return false;
            }

            /**
             * @brief 输出运算符重载，用于打印 ScopedSet 的内容
             *
             * @param os 输出流
             * @param sset 要打印的 ScopedSet 对象
             * @return std::ostream& 输出流引用
             */
            friend std::ostream &operator<<(std::ostream &os, ScopedSet<Key> sset)
            {
                os << "ScopedSet:" << std::endl;
                return os;
            }

        private:
            /** @brief 存储多层作用域的链表，每个作用域是一个集合 */
            std::list<std::set<Key>> scopes;
        };

    } // namespace util
} // namespace MetaWaveCompiler