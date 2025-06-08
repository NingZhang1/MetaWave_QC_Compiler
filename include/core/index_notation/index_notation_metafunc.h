/*
 * @Author: Ning Zhang
 * @Date: 2025-06-08 16:17:54
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2025-06-08 16:20:45
 */

#pragma once

#include "../../util/type.h"
#include "index_notation_nodes_abc.h"

namespace MetaWaveCompiler
{
    namespace index_notation
    {
        /// Returns true if expression e is of type E.
        template <typename E>
        inline bool isa(const IndexExprNode *e)
        {
            return e != nullptr && dynamic_cast<const E *>(e) != nullptr;
        }

        /// Casts the expression e to type E.
        template <typename E>
        inline const E *to(const IndexExprNode *e)
        {
            taco_iassert(isa<E>(e)) << "Cannot convert " << typeid(e).name() << " to " << typeid(E).name();
            return static_cast<const E *>(e);
        }

        /// Returns true if statement e is of type S.
        template <typename S>
        inline bool isa(const IndexStmtNode *s)
        {
            return s != nullptr && dynamic_cast<const S *>(s) != nullptr;
        }

        /// Casts the index statement node s to subtype S.
        template <typename SubType>
        inline const SubType *to(const IndexStmtNode *s)
        {
            taco_iassert(isa<SubType>(s)) << "Cannot convert " << typeid(s).name() << " to " << typeid(SubType).name();
            return static_cast<const SubType *>(s);
        }

        template <typename I>
        inline const typename I::Node *getNode(const I &stmt)
        {
            taco_iassert(isa<typename I::Node>(stmt.ptr));
            return static_cast<const typename I::Node *>(stmt.ptr);
        }
    };
};