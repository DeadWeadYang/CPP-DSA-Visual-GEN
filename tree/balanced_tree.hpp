#pragma once
#include <functional>
#include "binary_tree/binary_tree_basic.hpp"
#include "binary_tree/binary_search_tree/AVL_balanced_tree.hpp"
#include "binary_tree/binary_search_tree/RB_balanced_tree.hpp"
namespace DSA
{
    namespace Tree
    {
        namespace BalancedTree
        {
            namespace BalancedBinaryTree
            {
                using DSA::Tree::BinaryTree::BinarySearchTree::AVLTree::AVLTreeLinked;
                using DSA::Tree::BinaryTree::BinarySearchTree::RBTree::RBTreeLinked;
            }
        }
    }
}