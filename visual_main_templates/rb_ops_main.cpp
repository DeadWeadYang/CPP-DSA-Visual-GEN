#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../tree/binary_tree/binary_search_tree/RB_balanced_tree.hpp"
#include "input_utils.hpp"

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    using Tree = DSA::Tree::BinaryTree::BinarySearchTree::RBTree::RBTreeLinked<int>;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    auto ops = parse_value_ops(json_string_value(json, "operations", "insert 10\ninsert 8\ninsert 9\ninsert 12\ninsert 11\nerase 8"));

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("rb_ops");
    Tree tree;
    for (auto [op, value] : ops)
    {
        if (op == "insert" || op == "push")
            tree.insert_unique(value);
        else if (op == "erase" || op == "delete" || op == "remove")
            tree.erase_unique(value);
    }
    DSA_VIS_END();
    return 0;
}
