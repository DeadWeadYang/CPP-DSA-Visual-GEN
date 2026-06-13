#pragma once

#include <string>
#include <vector>
#include "../vis_trace.hpp"

namespace DSA
{
    namespace StringAlgorithm
    {
        namespace Vis
        {
            inline void InitPatternTable(const std::string &obj, int cols, const std::string &title, const std::string &axis, int col_base)
            {
                DSA_VIS_STATE_INIT_VECTOR_LABELED(obj, cols, title, col_base, axis, false);
            }

            inline void SetTableValue(const std::string &obj, int col, int value, const std::string &note)
            {
                DSA_VIS_STATE_SET(obj, 0, col, value, false);
                DSA_VIS_STATE_HISTORY(obj, 0, col, value, note, false);
            }

            inline void FocusTableCell(const std::string &obj, int col, const std::string &role)
            {
                DSA_VIS_STATE_CLEAR_FOCUS(obj, false);
                DSA_VIS_STATE_FOCUS(obj, 0, col, role, false);
            }

            inline void InitCompare(const std::string &obj, const std::string &text, const std::string &pattern)
            {
                DSA_VIS_STR_INIT(obj, text, pattern, false);
            }

            inline void InitPattern(const std::string &obj, const std::string &pattern)
            {
                DSA_VIS_STR_INIT_PATTERN(obj, pattern, false);
            }

            inline void AlignByIndices(const std::string &obj, int text_index, int pattern_index)
            {
                int shift = text_index - pattern_index;
                if (shift < 0)
                    shift = 0;
                DSA_VIS_STR_ALIGN(obj, shift, false);
            }

            inline void Compare(const std::string &obj, int text_index, int pattern_index, bool match)
            {
                DSA_VIS_STR_COMPARE(obj, text_index, pattern_index, match, false);
            }

            inline void ComparePattern(const std::string &obj, int current_index, int prefix_index, bool match)
            {
                DSA_VIS_STR_COMPARE(obj, current_index, prefix_index, match, false);
            }

            inline void Fallback(const std::string &obj, int from_j, int to_j)
            {
                DSA_VIS_STR_FALLBACK(obj, from_j, to_j, false);
            }

            inline void Accept(const std::string &obj, int start, int length)
            {
                DSA_VIS_STR_ACCEPT(obj, start, length, false);
            }

            inline void Clear(const std::string &obj)
            {
                DSA_VIS_STR_CLEAR(obj, false);
            }
        }
    }
}
