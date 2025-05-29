/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 15:53:58
 * @Last Modified by:   Ning Zhang
 * @Last Modified time: 2024-12-21 15:53:58
 */

namespace MetaWaveCompiler
{
    namespace util
    {

        class Uncopyable
        {
        protected:
            Uncopyable() = default;
            ~Uncopyable() = default;

        private:
            Uncopyable(const Uncopyable &) = delete;
            Uncopyable &operator=(const Uncopyable &) = delete;
        };

    } // namespace util
} // namespace MetaWaveCompiler