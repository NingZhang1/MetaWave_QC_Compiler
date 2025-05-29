/*
 * @Author: Ning Zhang
 * @Date: 2024-12-21 16:19:20
 * @Last Modified by: Ning Zhang
 * @Last Modified time: 2024-12-21 21:25:05
 */

#include "../include/util/strings.h"

#include <iostream>

using namespace std;

namespace MetaWaveCompiler
{
    namespace util
    {

        /**
         * @brief 将字符串按照指定的分隔符分割成字符串数组
         *
         * @param str 要分割的源字符串
         * @param delim 分隔符
         * @param keepDelim 是否在结果中保留分隔符
         * @return vector<string> 分割后的字符串数组
         *
         * @details 该函数将输入字符串按照指定的分隔符进行分割。
         *          如果 keepDelim 为 true，分隔符会被添加到每个分割后的子字符串的开头。
         *          空字符串（长度为0的子串）会被忽略。
         */
        vector<string> split(const string &str, const string &delim, bool keepDelim)
        {
            vector<string> results;
            size_t prev = 0;
            size_t next = 0;

            while ((next = str.find(delim, prev)) != std::string::npos)
            {
                if (next - prev != 0)
                {
                    std::string substr = ((keepDelim) ? delim : "") + str.substr(prev, next - prev);
                    results.push_back(substr);
                }
                prev = next + delim.size();
            }

            if (prev < str.size())
            {
                string substr = ((keepDelim) ? delim : "") + str.substr(prev);
                results.push_back(substr);
            }

            return results;
        }

        /**
         * @brief 重复指定字符串n次
         *
         * @param text 要重复的字符串
         * @param n 重复次数
         * @return std::string 重复后的字符串
         *
         * @details 将输入字符串重复n次并返回连接后的结果。
         *          如果 n 为 0，返回空字符串。
         */
        std::string repeat(std::string text, size_t n)
        {
            string str;
            for (size_t i = 0; i < n; i++)
            {
                str += text;
            }
            return str;
        }

        /**
         * @brief 使用指定字符填充文本两侧以达到指定长度
         *
         * @param text 要填充的文本
         * @param fill 填充字符
         * @param n 目标总长度
         * @return string 填充后的字符串
         *
         * @details 在文本两侧填充指定字符，使最终字符串达到指定长度。
         *          文本两侧会各添加一个空格，剩余空间由填充字符平均分配。
         *          如果需要填充奇数个字符，额外的一个字符会被添加到右侧。
         */
        string fill(string text, char fill, size_t n)
        {
            size_t numfills = n - (text.size() + 2);
            size_t prefix = numfills / 2;
            size_t suffix = numfills / 2 + (numfills % 2);
            return string(prefix, fill) + " " + text + " " + string(suffix, fill);
        }

    } // namespace util
} // namespace MetaWaveCompiler
