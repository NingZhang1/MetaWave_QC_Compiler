/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 15:52:38
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 15:53:50
 */

#include "intrusive_ptr.h"

namespace MetaWaveCompiler
{
    namespace util
    {

        template <class Data>
        class Manageable
        {
            // friend void acquire(const Data *data) { ++data->ref; }
            // friend void release(const Data *data)
            // {
            //     if (--data->ref == 0)
            //         delete data;
            // }
            // mutable long ref = 0;
        };

    }; // namespace util
}; // namespace MetaWaveCompiler