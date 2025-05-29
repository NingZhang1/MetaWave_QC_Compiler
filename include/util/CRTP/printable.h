/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 15:53:41
 * @Last Modified by:   Ning Zhang
 * @Last Modified time: 2024-12-21 15:53:41
 */

#include <ostream>

namespace MetaWaveCompiler
{
    namespace util
    {

        class Printable
        {
        public:
            virtual void print(std::ostream &os) const = 0;
            virtual ~Printable() {};
        };

        inline std::ostream &operator<<(std::ostream &os, const Printable &printable)
        {
            printable.print(os);
            return os;
        }

    } // namespace util
} // namespace MetaWaveCompiler