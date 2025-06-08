/*
 * @Author: Ning Zhang
 * @Date: 2025-06-08 15:51:55
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2025-06-08 15:55:28
 */

#pragma once

#include "../../include/core/index_notation/index_attributes.h"

namespace MetaWaveCompiler
{
    namespace index_notation
    {

        // is xxx

        bool is_core(IndexAttribute index_value)
        {
            return static_cast<uint64_t>(index_value) & static_cast<uint64_t>(IndexAttribute::is_core);
        }

        bool is_act(IndexAttribute index_value)
        {
            return static_cast<uint64_t>(index_value) & static_cast<uint64_t>(IndexAttribute::is_act);
        }

        bool is_virtual(IndexAttribute index_value)
        {
            return static_cast<uint64_t>(index_value) & static_cast<uint64_t>(IndexAttribute::is_virtual);
        }

        bool is_alpha(IndexAttribute index_value)
        {
            return static_cast<uint64_t>(index_value) & static_cast<uint64_t>(IndexAttribute::is_alpha);
        }

        bool is_beta(IndexAttribute index_value)
        {
            return static_cast<uint64_t>(index_value) & static_cast<uint64_t>(IndexAttribute::is_beta);
        }

        bool is_barred(IndexAttribute index_value)
        {
            return static_cast<uint64_t>(index_value) & static_cast<uint64_t>(IndexAttribute::is_barred);
        }

        bool is_unbarred(IndexAttribute index_value)
        {
            return static_cast<uint64_t>(index_value) & static_cast<uint64_t>(IndexAttribute::is_unbarred);
        }

        // has xxx

        bool has_core_act_virtual(IndexAttribute index_value)
        {
            return is_core(index_value) || is_act(index_value) || is_virtual(index_value);
        }

        bool has_alpha_beta(IndexAttribute index_value)
        {
            return is_alpha(index_value) || is_beta(index_value);
        }

        bool has_barred_unbarred(IndexAttribute index_value)
        {
            return is_barred(index_value) || is_unbarred(index_value);
        }

    }; // namespace index_notation
}; // namespace MetaWaveCompiler

namespace MetaWaveCompiler
{
    namespace index_notation
    {
        bool _check_core_act_virtual(IndexAttribute index_attribute)
        {
            if (is_core(index_attribute))
            {
                if (is_act(index_attribute) || is_virtual(index_attribute))
                {
                    return false;
                }
            }
            else
            {
                if (is_act(index_attribute) && is_virtual(index_attribute))
                {
                    return false;
                }
            }
            return true;
        }

        bool _check_alpha_beta(IndexAttribute index_attribute)
        {
            if (is_alpha(index_attribute))
            {
                if (is_beta(index_attribute))
                {
                    return false;
                }
            }
            return true;
        }

        bool _check_barred_unbarred(IndexAttribute index_attribute)
        {
            if (is_barred(index_attribute))
            {
                if (is_unbarred(index_attribute))
                {
                    return false;
                }
            }
            return true;
        }

        bool is_valid_index_attribute(IndexAttribute index_attribute)
        {
            return _check_core_act_virtual(index_attribute) && _check_alpha_beta(index_attribute) && _check_barred_unbarred(index_attribute);
        }
    };
}; // namespace MetaWaveCompiler