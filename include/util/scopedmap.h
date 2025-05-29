#include <iostream>
#include <list>
#include <map>
#include <ostream>
#include "./collections.h"
#include "error.h"

namespace MetaWaveCompiler
{
    namespace util
    {

        /**
         * @brief 支持作用域管理的映射容器类
         *
         * @details ScopedMap 实现了一个支持多层作用域的键值对存储结构。
         *          它使用栈式的作用域管理方式，支持动态地创建和销毁作用域，
         *          并在多个作用域中查找键值对。主要特点：
         *          - 支持嵌套作用域管理
         *          - 遵循最近作用域优先原则
         *          - 自动的作用域生命周期管理
         *
         * 典型应用场景：
         * - 编译器的符号表管理
         * - 嵌套块作用域中的变量管理
         * - 需要分层管理数据的场景
         *
         * @tparam Key 键类型
         * @tparam Value 值类型
         */
        template <typename Key, typename Value>
        class ScopedMap
        {
        public:
            ScopedMap() { scope(); }

            ~ScopedMap() { unscope(); }

            /**
             * @brief 创建一个新的作用域层级
             *
             * @note 新作用域会被添加到作用域栈的顶部，成为当前活动作用域
             */
            void scope() { scopes.push_front(std::map<Key, Value>()); }

            /**
             * @brief 移除最顶层的作用域
             *
             * @note 此操作会删除当前作用域中的所有键值对
             */
            void unscope() { scopes.pop_front(); }

            /**
             * @brief 在当前作用域中插入键值对
             *
             * @param value 要插入的键值对
             * @note 插入操作只影响当前（最顶层）作用域
             */
            void insert(const std::pair<Key, Value> &value) { scopes.front().insert(value); }

            /**
             * @brief 从最近的作用域(不一定是最顶层)中移除指定键的映射
             *
             * @param key 要移除的键
             * @throw metawave_ierror 如果键在所有作用域中都不存在
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
             * @brief 获取指定键对应的值
             *
             * @param key 要查找的键
             * @return const Value& 找到的值的常量引用
             * @throw metawave_ierror 如果键在所有作用域中都不存在
             * @note 按照从内到外的顺序在作用域中查找，返回最先找到的值
             */
            const Value &get(const Key &key) const
            {
                for (auto &scope : scopes)
                {
                    if (scope.find(key) != scope.end())
                    {
                        return scope.at(key);
                    }
                }
                metawave_ierror << "Not in scope";
                return scopes.front().begin()->second; // silence warnings
            }

            /**
             * @brief 检查指定的键是否存在于任意作用域中
             *
             * @param key 要检查的键
             * @return bool 如果键存在于任意作用域中返回 true，否则返回 false
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
             * @brief 输出运算符重载，用于打印 ScopedMap 的内容
             *
             * @param os 输出流
             * @param smap 要打印的 ScopedMap 对象
             * @return std::ostream& 输出流引用
             */
            friend std::ostream &operator<<(std::ostream &os, ScopedMap<Key, Value> smap)
            {
                os << "ScopedMap:" << std::endl;
                for (auto &scope : util::reverse(smap.scopes))
                {
                    os << "  - ";
                    if (scope.size() > 0)
                    {
                        auto val = *scope.begin();
                        os << val.first << " -> " << val.second << std::endl;
                    }
                    for (auto &val : excludeFirst(scope))
                    {
                        os << "    " << val.first << " -> " << val.second << std::endl;
                    }
                    std::cout << std::endl;
                }
                return os;
            }

        private:
            /** @brief 存储多层作用域的链表，每个作用域是一个映射表 */
            std::list<std::map<Key, Value>> scopes;
        };

    } // namespace util
} // namespace MetaWaveCompiler