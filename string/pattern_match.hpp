#pragma once
#include <string>
#include <vector>
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
                    // 从第二个字符开始计算 pi 值。
                    for (int i = 1; i < n; i++)
                    {
                        // j 表示当前已匹配的前缀的长度，同时也指向该前缀的下一个字符。
                        // 我们尝试扩展前一个字符的最长公共前后缀 pi[i-1]。
                        int j = pi[i - 1];
                        // 当 j > 0 (表示当前有已匹配的前缀) 并且 s[j] (前缀的下一个字符) 与当前字符 s[i] 不匹配时，
                        // 我们需要缩短这个前缀。我们通过回溯，令 j = pi[j-1]，
                        // 这相当于找到了一个更短的、同样是 s[0...i-1] 前后缀的子串。
                        while (j > 0 && s[j] != s[i])
                        {
                            j = pi[j - 1];
                        }
                        // 如果 s[j] 和 s[i] 匹配，说明我们可以将公共前后缀的长度增加 1。
                        // 如果 s[j] 和 s[i] 不匹配 (并且 j 已经等于 0)，说明没有公共前后缀，长度为 0。
                        // 这个三元运算符处理了以上两种情况。
                        pi[i] = (s[j] == s[i]) ? j + 1 : 0;
                    }
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
                    // 循环计算 next 数组的值，从 next[1] 开始。
                    for (int i = 1; i <= n; i++)
                    {
                        // j 指向可以与 s[i-1] 比较的字符位置。
                        int j = nxt[i - 1];
                        // 当 j 不为哨兵值，且前缀的下一个字符 s[j] 与当前字符 s[i-1] 不匹配时，
                        // 继续回溯 j，寻找更短的前缀。
                        while (j >= 0 && s[j] != s[i - 1])
                        {
                            j = nxt[j];
                        }
                        // j+1 是新的最长公共前后缀长度。
                        nxt[i] = j + 1;
                    }
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
                    std::vector<int> res;// 存储匹配结果的起始位置。
                    // i 是文本串的指针，j 是模式串的指针。
                    for (int i = 0, j = 0; i < n;)
                    {
                        // 情况一：当前字符匹配。
                        if (text[i] == pattern[j])
                        {
                            // 移动两个指针，继续比较下一个字符。
                            ++i, ++j;
                            // 如果模式串指针 j 到达末尾，说明找到了一个完整的匹配。
                            if (j == m)
                            {
                                // 记录匹配的起始位置 (i - m)。
                                res.push_back(i - m);
                                // 利用 next 数组，将模式串指针 j 移动到下一个可能匹配的位置，
                                // 以便继续查找后续的匹配项（处理模式串重叠的情况）。
                                j = nxt[j];
                            }
                        }
                        // 情况二：当前字符不匹配。
                        else
                        {
                            // 利用 next 数组，将模式串指针 j 回溯到合适的位置。
                            // 文本串指针 i 不动，这正是 KMP 算法的精髓。
                            j = nxt[j];
                            // 如果回溯后 j 变为 -1，说明模式串的第一个字符就不匹配。
                            if (j < 0)
                            {
                                // 将文本串指针和模式串指针都向前移动一位，开始新一轮的比较。
                                ++i, ++j;
                            }
                        }
                    }
                    return res;
                }

            }
        }
    }
}