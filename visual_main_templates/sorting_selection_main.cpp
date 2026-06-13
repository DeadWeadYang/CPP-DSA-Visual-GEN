#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../sorting/sorting.hpp"
#include "input_utils.hpp"

#include <vector>

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    auto values = parse_int_list(json_string_value(json, "values", "7 2 6 3 1 5 4"));

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("sorting_selection");
    DSA::Sorting::Vis::PrimaryScope<std::vector<int>::iterator> scope(values.begin(), values.end(), "A");
    DSA::Sorting::SelectionSort(values.begin(), values.end());
    DSA_VIS_END();
    return 0;
}
