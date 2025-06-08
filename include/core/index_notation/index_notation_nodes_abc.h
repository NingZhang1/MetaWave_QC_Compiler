/*
 * @Author: Ning Zhang
 * @Date: 2025-06-08 15:56:31
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2025-06-08 16:20:58
 */

#pragma once

#include <memory>
#include <vector>

#include "../../util/CRTP/intrusive_ptr.h"
#include "../../util/CRTP/manageable.h"
#include "../../util/CRTP/uncopyable.h"

namespace MetaWaveCompiler
{
  namespace index_notation
  {

    class IndexExprVisitorStrict;
    class IndexStmtVisitorStrict;

    struct IndexExprNode : public util::Manageable<IndexExprNode>,
                           private util::Uncopyable
    {
    public:
      IndexExprNode() = default;
      // IndexExprNode(Datatype type);
      virtual ~IndexExprNode() = default;
      virtual void accept(IndexExprVisitorStrict *) const = 0;
    };

    struct IndexStmtNode : public util::Manageable<IndexStmtNode>,
                           private util::Uncopyable
    {
    public:
      IndexStmtNode() = default;
      // IndexStmtNode(Type type);
      virtual ~IndexStmtNode() = default;
      virtual void accept(IndexStmtVisitorStrict *) const = 0;
    };

  }; // namespace index_notation
}; // namespace MetaWaveCompiler