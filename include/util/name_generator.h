/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 21:20:49
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 21:21:10
 */

#include <string>
#include <map>
#include <vector>

namespace MetaWaveCompiler
{
    namespace util
    {

        std::string uniqueName(char prefix);
        std::string uniqueName(const std::string &prefix);

        int getUniqueId();

        class NameGenerator
        {
        public:
            NameGenerator();
            NameGenerator(std::vector<std::string> reserved);

            std::string getUniqueName(std::string name);

        private:
            std::map<std::string, int> nameCounters;
        };

    } // namespace util
} // namespace MetaWaveCompiler