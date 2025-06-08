/*
 * @Author: Ning Zhang
 * @Date: 2025-06-08 15:34:56
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2025-06-08 15:54:03
 */

#pragma once

#include <memory>
#include <vector>

namespace MetaWaveCompiler
{
    namespace index_notation
    {

        enum class IndexAttribute : uint64_t
        {
            none = 0,
            is_core = 1ULL << 0,     // 轨道属性
            is_act = 11ULL << 1,     // 轨道属性
            is_virtual = 11ULL << 2, // 轨道属性
            is_alpha = 1ULL << 3,    // 自旋属性
            is_beta = 1ULL << 4,     // 自旋属性
            is_barred = 1ULL << 5,   // 旋量
            is_unbarred = 1ULL << 6, // 旋量
        };

    }; // namespace index_notation
}; // namespace MetaWaveCompiler

// ------------------------------------------------------------------------------
// this module extract the index attribute from the index value
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
    namespace index_notation
    {
        // is xxx

        bool is_core(IndexAttribute index_value);
        bool is_act(IndexAttribute index_value);
        bool is_virtual(IndexAttribute index_value);
        bool is_alpha(IndexAttribute index_value);
        bool is_beta(IndexAttribute index_value);
        bool is_barred(IndexAttribute index_value);
        bool is_unbarred(IndexAttribute index_value);

        // has xxx

        bool has_core_act_virtual(IndexAttribute index_value);
        bool has_alpha_beta(IndexAttribute index_value);
        bool has_barred_unbarred(IndexAttribute index_value);
    }
}

// ------------------------------------------------------------------------------
// this module check whether the index attribute is valid
// 1. the index attribute can be only be one of core / act / virtual
// 2. the index attribute can be only be one of alpha / beta
// 3. the index attribute can be only be one of barred / unbarred
// ------------------------------------------------------------------------------

namespace MetaWaveCompiler
{
    namespace index_notation
    {
        bool _check_core_act_virtual(IndexAttribute index_attribute);
        bool _check_alpha_beta(IndexAttribute index_attribute);
        bool _check_barred_unbarred(IndexAttribute index_attribute);
        
        bool is_valid_index_attribute(IndexAttribute index_attribute);

    }; // namespace index_notation
}; // namespace MetaWaveCompiler