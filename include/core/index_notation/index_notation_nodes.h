/*
 * @Author: Ning Zhang
 * @Date: 2025-06-08 15:56:48
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2025-06-08 16:21:09
 */

#pragma once

#include "../../util/type.h"
#include "index_notation_nodes_abc.h"

// ------------------------------------------------------------------------------
// IndexExprNode nodes
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
// literal nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

    using namespace MetaWaveCompiler::util;

    struct LiteralNode : public IndexExprNode
    {
      // ----------------------------------
      // constructor and destructor
      // ----------------------------------

      template <typename T>
      LiteralNode(T val) : IndexExprNode(type<T>())
      {
        this->val = malloc(sizeof(T));
        *static_cast<T *>(this->val) = val;
      }

      ~LiteralNode() { free(val); }

      // ----------------------------------
      // accept
      // ----------------------------------

      void accept(IndexExprVisitorStrict *v) const { v->visit(this); }

      // ----------------------------------
      // function
      // ----------------------------------

      template <typename T>
      T getVal() const
      {
        taco_iassert(getDataType() == type<T>())
            << "Attempting to get data of wrong type";
        return *static_cast<T *>(val);
      }

      Datatype getDataType() const { return dataType; }

    private:
      Datatype dataType;
      void *val;
    };

  }; // namespace index_notation
}; // namespace MetaWaveCompiler

// ------------------------------------------------------------------------------
// UnaryExprNode nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};

// ------------------------------------------------------------------------------
// BinaryExprNode nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};

// ------------------------------------------------------------------------------
// Negate nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};

// ------------------------------------------------------------------------------
// AddNode nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};

// ------------------------------------------------------------------------------
// SubNode nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};

// ------------------------------------------------------------------------------
// MulNode nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};

// ------------------------------------------------------------------------------
// IndexStmtNode nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};

// ------------------------------------------------------------------------------
// Assignment nodes
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
  namespace index_notation
  {

  };
};
