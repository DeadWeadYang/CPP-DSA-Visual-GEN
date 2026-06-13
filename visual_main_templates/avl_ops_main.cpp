#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../tree/binary_tree/binary_search_tree/AVL_balanced_tree.hpp"
#include "input_utils.hpp"

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    using Tree = DSA::Tree::BinaryTree::BinarySearchTree::AVLTree::AVLTreeLinked<int>;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    auto ops = parse_value_ops(json_string_value(json, "operations", "insert 3\ninsert 2\ninsert 1\ninsert 4\ninsert 5\nerase 2"));

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("avl_ops");
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
