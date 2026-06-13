#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../tree/binary_tree/Huffman_tree.hpp"
#include "input_utils.hpp"

#include <map>

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    auto weights = parse_int_list(json_string_value(json, "weights", "5 9 12 13 16 45"));
    std::map<int, int> items;
    for (int i = 0; i < static_cast<int>(weights.size()); ++i)
        items[i + 1] = weights[i];

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("huffman");
    DSA::Tree::BinaryTree::HuffmanTree::HuffmanTree<int> tree;
    tree.build(items);
    DSA_VIS_END();
    return 0;
}
