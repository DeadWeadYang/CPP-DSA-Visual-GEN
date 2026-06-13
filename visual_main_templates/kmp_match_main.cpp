#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../string/pattern_match.hpp"
#include "input_utils.hpp"

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    auto text = json_string_value(json, "text", "ababcabcabababd");
    auto pattern = json_string_value(json, "pattern", "ababd");

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("kmp_match");
    DSA::StringAlgorithm::PatternMatch::KMP::KMP_match(text, pattern);
    DSA_VIS_END();
    return 0;
}
