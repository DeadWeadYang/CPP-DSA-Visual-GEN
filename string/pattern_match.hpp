#pragma once
#include <string>
#include <vector>
#include "string_vis.hpp"
namespace DSA
{
    namespace StringAlgorithm
    {
        namespace PatternMatch
        {
            namespace KMP
            {

                /*
                pi[i] is the length of the longest proper prefix of the substring  s[0 ... i] which is also a suffix of this substring.
                A proper prefix of a string is a prefix that is not equal to the string itself.
                By definition,  pi[0] = 0 .
                */
                /**
                 * @brief 计算 KMP 算法的 π (pi) 函数，也称为前缀函数。
                 * @param s 输入字符串（通常是模式串）。
                 * @return std::vector<int> 一个向量，其中 pi[i] 是 s[0...i] 的最长公共前后缀的长度。
                 * @details
                 * π 函数是 KMP 算法的核心，它预处理模式串，以确定在发生不匹配时，模式串应该向右移动多远。
                 * 这避免了在文本串中回溯指针，从而提高了效率。
                 */
                std::vector<int> KMP_pi(const std::string &s)
                {
                    int n = s.length();
                    std::vector<int> pi(n);// 创建 pi 数组。pi[0] 默认为 0。
                    DSA_VIS_ONLY(Vis::InitPattern("KMP_PI_STR", s));
                    DSA_VIS_ONLY(Vis::InitPatternTable("PI", n, "PI[i] longest proper prefix-suffix length", "column = pattern index i", 0));
                    if (n > 0)
                    {
                        DSA_VIS_ONLY(Vis::SetTableValue("PI", 0, 0, "init"));
                        DSA_VIS_ONLY(Vis::FocusTableCell("PI", 0, "write"));
                        DSA_VIS_STEP("KMP pi 初始化：pi[0]=0");
                    }
                    // 从第二个字符开始计算 pi 值。
                    for (int i = 1; i < n; i++)
                    {
                        // j 表示当前已匹配的前缀的长度，同时也指向该前缀的下一个字符。
                        // 我们尝试扩展前一个字符的最长公共前后缀 pi[i-1]。
                        int j = pi[i - 1];
                        DSA_VIS_ONLY(Vis::ComparePattern("KMP_PI_STR", i, j, s[j] == s[i]));
                        DSA_VIS_STEP("KMP pi：计算 i=" + std::to_string(i) + "，先尝试用 j=pi[i-1]=" + std::to_string(j) + " 扩展");
                        // 当 j > 0 (表示当前有已匹配的前缀) 并且 s[j] (前缀的下一个字符) 与当前字符 s[i] 不匹配时，
                        // 我们需要缩短这个前缀。我们通过回溯，令 j = pi[j-1]，
                        // 这相当于找到了一个更短的、同样是 s[0...i-1] 前后缀的子串。
                        while (j > 0 && s[j] != s[i])
                        {
                            int old_j = j;
                            j = pi[j - 1];
                            DSA_VIS_ONLY(Vis::Fallback("KMP_PI_STR", old_j, j));
                            DSA_VIS_ONLY(Vis::FocusTableCell("PI", old_j - 1, "read"));
                            DSA_VIS_STEP("KMP pi：s[" + std::to_string(old_j) + "] 与 s[" + std::to_string(i) + "] 失配，j 回退到 pi[" + std::to_string(old_j - 1) + "]=" + std::to_string(j));
                            DSA_VIS_ONLY(Vis::ComparePattern("KMP_PI_STR", i, j, s[j] == s[i]));
                        }
                        // 如果 s[j] 和 s[i] 匹配，说明我们可以将公共前后缀的长度增加 1。
                        // 如果 s[j] 和 s[i] 不匹配 (并且 j 已经等于 0)，说明没有公共前后缀，长度为 0。
                        // 这个三元运算符处理了以上两种情况。
                        pi[i] = (s[j] == s[i]) ? j + 1 : 0;
                        DSA_VIS_ONLY(Vis::SetTableValue("PI", i, pi[i], "set"));
                        DSA_VIS_ONLY(Vis::FocusTableCell("PI", i, "write"));
                        DSA_VIS_STEP("KMP pi：得到 pi[" + std::to_string(i) + "]=" + std::to_string(pi[i]));
                    }
                    DSA_VIS_STATE_CLEAR_FOCUS("PI", false);
                    return pi;// 返回计算好的 pi 数组。
                }
                
                /**
                 * @brief 计算 KMP 算法的 next 数组。
                 * @param s 输入字符串（通常是模式串）。
                 * @return std::vector<int> next 数组。
                 * @details
                 * next 数组是 pi 函数的一种常见变体，常用于 KMP 算法的实现中。
                 * next[i] 表示长度为 i 的前缀的“最长真前后缀”的长度。
                 * 它与 pi 数组的关系通常是 next[i+1] = pi[i]。
                 * next[0] 通常被设为 -1，作为一个特殊的标记。
                 */
                // pi[i]=next[i+1]
                std::vector<int> KMP_next(const std::string &s)
                {
                    int n = s.length();
                    // next 数组大小为 n+1，因为 next[i] 对应长度为 i 的前缀。
                    std::vector<int> nxt(n + 1);
                    nxt[0] = -1;// -1 是一个哨兵值，表示无法再回溯。
                    DSA_VIS_ONLY(Vis::InitPattern("KMP_NEXT_STR", s));
                    DSA_VIS_ONLY(Vis::InitPatternTable("NEXT", n + 1, "NEXT[k] fallback length for prefix length k", "column = prefix length k", 0));
                    DSA_VIS_ONLY(Vis::SetTableValue("NEXT", 0, -1, "sentinel"));
                    DSA_VIS_ONLY(Vis::FocusTableCell("NEXT", 0, "write"));
                    DSA_VIS_STEP("KMP next 初始化：next[0] = -1 作为哨兵");
                    // 循环计算 next 数组的值，从 next[1] 开始。
                    for (int i = 1; i <= n; i++)
                    {
                        // j 指向可以与 s[i-1] 比较的字符位置。
                        int j = nxt[i - 1];
                        if (j >= 0)
                        {
                            DSA_VIS_ONLY(Vis::ComparePattern("KMP_NEXT_STR", i - 1, j, s[j] == s[i - 1]));
                        }
                        DSA_VIS_ONLY(Vis::FocusTableCell("NEXT", i - 1, "read"));
                        DSA_VIS_STEP("KMP next：计算 next[" + std::to_string(i) + "]，从 next[" + std::to_string(i - 1) + "]=" + std::to_string(j) + " 开始");
                        // 当 j 不为哨兵值，且前缀的下一个字符 s[j] 与当前字符 s[i-1] 不匹配时，
                        // 继续回溯 j，寻找更短的前缀。
                        while (j >= 0 && s[j] != s[i - 1])
                        {
                            int old_j = j;
                            j = nxt[j];
                            DSA_VIS_ONLY(Vis::Fallback("KMP_NEXT_STR", old_j, j));
                            DSA_VIS_ONLY(Vis::FocusTableCell("NEXT", old_j, "read"));
                            DSA_VIS_STEP("KMP next：s[" + std::to_string(old_j) + "] 与 s[" + std::to_string(i - 1) + "] 失配，j 回退到 next[" + std::to_string(old_j) + "]=" + std::to_string(j));
                            if (j >= 0)
                            {
                                DSA_VIS_ONLY(Vis::ComparePattern("KMP_NEXT_STR", i - 1, j, s[j] == s[i - 1]));
                            }
                        }
                        // j+1 是新的最长公共前后缀长度。
                        nxt[i] = j + 1;
                        DSA_VIS_ONLY(Vis::SetTableValue("NEXT", i, nxt[i], "set"));
                        DSA_VIS_ONLY(Vis::FocusTableCell("NEXT", i, "write"));
                        DSA_VIS_STEP("KMP next：得到 next[" + std::to_string(i) + "]=" + std::to_string(nxt[i]));
                    }
                    DSA_VIS_STATE_CLEAR_FOCUS("NEXT", false);
                    return nxt;
                }
                /**
                 * @brief 使用 KMP 算法在文本串中查找所有模式串的出现。
                 * @param text 文本串。
                 * @param pattern 模式串。
                 * @return std::vector<int> 一个向量，包含所有匹配项在文本串中的起始索引。
                 */
                std::vector<int> KMP_match(const std::string &text, const std::string &pattern)
                {
                    int n = text.length(), m = pattern.length();
                    // 首先预处理模式串，计算 next 数组。
                    auto nxt = KMP_next(pattern);
                    DSA_VIS_ONLY(Vis::Clear("KMP_NEXT_STR"));
                    std::vector<int> res;// 存储匹配结果的起始位置。
                    DSA_VIS_ONLY(Vis::InitCompare("KMP_MATCH", text, pattern));
                    DSA_VIS_STATE_CLEAR_FOCUS("NEXT", false);
                    DSA_VIS_STEP("KMP 匹配初始化：预处理阶段已得到 next 表，现在开始扫描文本串");
                    // i 是文本串的指针，j 是模式串的指针。
                    for (int i = 0, j = 0; i < n;)
                    {
                        // 情况一：当前字符匹配。
                        DSA_VIS_ONLY(Vis::AlignByIndices("KMP_MATCH", i, j));
                        DSA_VIS_ONLY(Vis::Compare("KMP_MATCH", i, j, text[i] == pattern[j]));
                        if (text[i] == pattern[j])
                        {
                            DSA_VIS_STEP("KMP 匹配：text[" + std::to_string(i) + "]='" + std::string(1, text[i]) + "' 与 pattern[" + std::to_string(j) + "] 匹配，两个指针右移");
                            // 移动两个指针，继续比较下一个字符。
                            ++i, ++j;
                            // 如果模式串指针 j 到达末尾，说明找到了一个完整的匹配。
                            if (j == m)
                            {
                                // 记录匹配的起始位置 (i - m)。
                                res.push_back(i - m);
                                DSA_VIS_ONLY(Vis::Accept("KMP_MATCH", i - m, m));
                                DSA_VIS_STEP("KMP 匹配：找到完整匹配，起点为 " + std::to_string(i - m));
                                // 利用 next 数组，将模式串指针 j 移动到下一个可能匹配的位置，
                                // 以便继续查找后续的匹配项（处理模式串重叠的情况）。
                                int old_j = j;
                                j = nxt[j];
                                DSA_VIS_ONLY(Vis::Fallback("KMP_MATCH", old_j, j));
                                DSA_VIS_ONLY(Vis::FocusTableCell("NEXT", old_j, "read"));
                                DSA_VIS_STEP("KMP 匹配：为处理重叠匹配，j 从 m 回退到 next[m]=" + std::to_string(j));
                            }
                        }
                        // 情况二：当前字符不匹配。
                        else
                        {
                            DSA_VIS_STEP("KMP 匹配：text[" + std::to_string(i) + "]='" + std::string(1, text[i]) + "' 与 pattern[" + std::to_string(j) + "]='" + std::string(1, pattern[j]) + "' 失配，文本指针 i 不回退");
                            // 利用 next 数组，将模式串指针 j 回溯到合适的位置。
                            // 文本串指针 i 不动，这正是 KMP 算法的精髓。
                            int old_j = j;
                            j = nxt[j];
                            DSA_VIS_ONLY(Vis::Fallback("KMP_MATCH", old_j, j));
                            DSA_VIS_ONLY(Vis::FocusTableCell("NEXT", old_j, "read"));
                            DSA_VIS_STEP("KMP 匹配：j 回退到 next[" + std::to_string(old_j) + "]=" + std::to_string(j));
                            // 如果回溯后 j 变为 -1，说明模式串的第一个字符就不匹配。
                            if (j < 0)
                            {
                                // 将文本串指针和模式串指针都向前移动一位，开始新一轮的比较。
                                ++i, ++j;
                                DSA_VIS_ONLY(Vis::AlignByIndices("KMP_MATCH", i, j));
                                DSA_VIS_STEP("KMP 匹配：j 为 -1，说明当前起点无法匹配，i 前进到 " + std::to_string(i) + "，j 归零");
                            }
                        }
                    }
                    DSA_VIS_ONLY(Vis::Clear("KMP_MATCH"));
                    DSA_VIS_STATE_CLEAR_FOCUS("NEXT", false);
                    DSA_VIS_STEP("KMP 匹配结束：共找到 " + std::to_string(res.size()) + " 个匹配位置");
                    return res;
                }

            }
        }
    }
}
