#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../tree/heap/binary_heap.hpp"
#include "input_utils.hpp"

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    auto ops = parse_value_ops(json_string_value(json, "operations", "push 5\npush 1\npush 8\npush 3\npush 7\npop\npush 9\npop"));

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("binary_heap");
    DSA::Tree::Heap::BinaryHeap::BinaryHeapMax<int> heap;
    for (auto [op, value] : ops)
    {
        if (op == "push" || op == "insert")
            heap.push(value);
        else if (op == "pop")
            heap.pop();
    }
    DSA_VIS_END();
    return 0;
}
