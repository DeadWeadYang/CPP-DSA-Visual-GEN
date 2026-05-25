#pragma once
#include <algorithm>
#include <vector>
#include <queue>
#include <functional>
#include <map>
#include <tuple>
#include <string>
#include <sstream>
#include <iostream>
#include "binary_tree_basic.hpp"
#include "../../vis_trace.hpp"

namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {
            namespace HuffmanTree
            {
                /**
                 * @brief 霍夫曼树节点所存储的值的结构体。
                 * @tparam T 权重的类型，必须是算术类型。
                 */
                template <typename T>
                struct HuffmanTreeNodeValue
                {
                    // 编译时断言，确保模板参数 T 是算术类型（如 int, float, double）
                    static_assert(std::is_arithmetic<T>::value, "HuffmanTree: the type of weight should be arithmetic");
                    int label;                        // 节点的标签，对于叶子节点，通常是字符的 ASCII 码。
                    T weight;                         // 节点的权重，通常是字符的出现频率。
                    HuffmanTreeNodeValue() = default; // 默认构造函数。
                    // 通过一个键值对 (label, weight) 构造。
                    HuffmanTreeNodeValue(const std::pair<int, T> &lwp) : label(lwp.first), weight(lwp.second) {};
                    // 重载比较运算符，用于优先队列的排序，以权重为基准。
                    bool operator<(const HuffmanTreeNodeValue &U) const { return weight < U.weight; }
                    bool operator>(const HuffmanTreeNodeValue &U) const { return weight > U.weight; }
                };
                /**
                 * @brief 霍夫曼树的节点类型。
                 * @tparam T 权重的类型。
                 * @details 继承自通用的链式二叉树节点，并特化其存储值的类型。
                 */
                template <typename T>
                struct HuffmanTreeNode : BinaryTreeNodeLinked<HuffmanTreeNodeValue<T>, HuffmanTreeNode<T>>
                {
                    using BinaryTreeNodeLinked<HuffmanTreeNodeValue<T>, HuffmanTreeNode<T>>::BinaryTreeNodeLinked;
                };

                /**
                 * @brief 霍夫曼树的主类。
                 * @tparam T 权重的类型。
                 * @details 实现了霍夫曼树的构建、霍夫曼编码的生成和带权路径长度 (WPL) 的计算。
                 */
                template <typename T>
                struct HuffmanTree : BinaryTreeLinked<HuffmanTreeNodeValue<T>, HuffmanTreeNode<T>>
                {
                    // 使用 using 关键字为类型创建别名，使代码更简洁。
                    using value_type = HuffmanTreeNodeValue<T>;
                    using Node = HuffmanTreeNode<T>;
                    using BinaryTreeLinked<value_type, Node>::BinaryTreeLinked;
                    // 定义一个用于优先队列的比较器结构体。
                    struct NodeCompare
                    {
                        // 重载 operator()，使得优先队列成为一个最小堆（权重小的节点优先级高）。
                        bool operator()(Node *pleft, Node *pright) { return pleft->value() > pright->value(); }
                    };
                    // 获取树的带权路径长度 (WPL)。
                    T getWeightedPathLength() const { return WPL; }
                    // 获取生成的霍夫曼编码表。
                    const std::map<int, std::string> &getHuffmanCode() const { return HuffmanCode; }
                    /**
                     * @brief 根据给定的权重集构建霍夫曼树。
                     * @param weights 一个从标签到权重的映射。
                     * @details 这是实现霍夫曼树的核心算法。
                     */
                    void build(std::map<int, T> weights)
                    {
                        // 如果树非空，先清空。
                        if (!this->empty())
                            this->clear();
                        // 创建一个最小优先队列（最小堆），用于存储树节点。
                        std::priority_queue<Node *, std::vector<Node *>, NodeCompare> heap;
#ifdef DSA_VIS_ENABLE
                        constexpr const char *kVisObj = "H";
                        const void *vis_super_root = static_cast<const void *>(this);
                        auto vis_node_text = [](const Node *node) -> std::string {
                            if (!node)
                                return std::string();
                            const auto &v = node->value();
                            std::ostringstream oss;
                            if (v.label == 0)
                                oss << "#:" << v.weight;
                            else
                                oss << "L" << v.label << ":" << v.weight;
                            return oss.str();
                        };
                        DSA_VIS_TREE_INIT(kVisObj);                                         /*VIS*/
                        DSA_VIS_TREE_NEW_NODE(kVisObj, vis_super_root, "forest", false);    /*VIS*/
                        DSA_VIS_TREE_SET_ROOT(kVisObj, vis_super_root, false);               /*VIS*/
                        DSA_VIS_TREE_HIDE_NODE(kVisObj, vis_super_root, false);              /*VIS*/
#endif
                        DSA_VIS_MSG(std::string("Huffman 构建开始：叶子数=") + std::to_string(weights.size()), true); /*VIS*/
                        for (auto const &kv : weights)
                            // 为每个带权重的标签创建一个叶子节点，并将其推入最小堆。
                        {
                            Node *leaf = this->createNodeInternal(kv.first, kv.second);
                            heap.push(leaf);
#ifdef DSA_VIS_ENABLE
                            DSA_VIS_TREE_NEW_NODE(kVisObj, leaf, vis_node_text(leaf), false);      /*VIS*/
                            DSA_VIS_TREE_ADD_CHILD(kVisObj, vis_super_root, leaf, false);           /*VIS*/
#endif
                            DSA_VIS_MSG(std::string("加入叶子：label=") + std::to_string(kv.first) + ", w=" + std::to_string(kv.second), false); /*VIS*/
                        }
                        // 循环合并节点，直到堆中只剩下一个节点（即树的根节点）。
                        while (heap.size() > 1)
                        {
                            // 从堆中取出两个权重最小的节点。
                            Node *pleft = heap.top();
                            heap.pop();
                            Node *pright = heap.top();
                            heap.pop();
                            DSA_VIS_MSG("取出当前最小的两棵树并合并", true); /*VIS*/
#ifdef DSA_VIS_ENABLE
                            DSA_VIS_TREE_MARK(kVisObj, pleft, false);   /*VIS*/
                            DSA_VIS_TREE_MARK(kVisObj, pright, false);  /*VIS*/
#endif
                            // 创建一个新的内部节点作为它们的父节点。
                            // 新节点的权重是两个子节点权重之和，标签可任意（此处为0）。
                            Node *pparent = this->createNodeInternal(0, pleft->value().weight + pright->value().weight);
#ifdef DSA_VIS_ENABLE
                            DSA_VIS_TREE_NEW_NODE(kVisObj, pparent, vis_node_text(pparent), false); /*VIS*/
#endif
                            DSA_VIS_MSG(std::string("新父节点权重=") + std::to_string(pparent->value().weight), false); /*VIS*/
                            // 建立亲子关系。
                            pparent->left() = pleft;
                            pparent->right() = pright;
                            pleft->parent = pright->parent = pparent;
#ifdef DSA_VIS_ENABLE
                            DSA_VIS_TREE_REMOVE_NODE(kVisObj, pleft, false);                 /*VIS*/
                            DSA_VIS_TREE_REMOVE_NODE(kVisObj, pright, false);                /*VIS*/
                            DSA_VIS_TREE_ADD_CHILD(kVisObj, pparent, pleft, false);          /*VIS*/
                            DSA_VIS_TREE_ADD_CHILD(kVisObj, pparent, pright, false);         /*VIS*/
                            DSA_VIS_TREE_ADD_CHILD(kVisObj, vis_super_root, pparent, true);  /*VIS*/
                            DSA_VIS_TREE_UNMARK(kVisObj, pleft, false);                      /*VIS*/
                            DSA_VIS_TREE_UNMARK(kVisObj, pright, false);                     /*VIS*/
#endif

                            // 将新创建的父节点推回堆中。
                            heap.push(pparent);
                        }
                        // 当循环结束时，堆中唯一的元素就是霍夫曼树的根。
                        if (!heap.empty())
                        {
                            // `release` 方法将堆中的根节点设置为树的根节点。
                            Node *final_root = heap.top();
                            this->release(final_root);
#ifdef DSA_VIS_ENABLE
                            DSA_VIS_TREE_SHOW_NODE(kVisObj, vis_super_root, false);              /*VIS*/
                            DSA_VIS_TREE_SET_NOTE(kVisObj, vis_super_root, "Huffman", false);    /*VIS*/
                            DSA_VIS_TREE_SET_ROOT(kVisObj, vis_super_root, false);               /*VIS*/
                            DSA_VIS_TREE_SET_CHILD(kVisObj, vis_super_root, 0, final_root, false); /*VIS*/
                            DSA_VIS_TREE_HIDE_NODE(kVisObj, vis_super_root, false);              /*VIS*/
#endif
                            heap.pop();
                        }
                        // 计算带权路径长度 (WPL)。
                        WPL = this->empty() ? 0 : calculateWPL(this->root(), 0);
                        DSA_VIS_MSG(std::string("Huffman 构建结束，WPL=") + std::to_string(WPL), true); /*VIS*/

                        // 计算霍夫曼编码。
                        HuffmanCode.clear();
                        if (!this->empty())
                            calculateHuffmanCode(this->root(), "");
                        DSA_VIS_MSG(std::string("Huffman 编码生成完成，编码数=") + std::to_string(HuffmanCode.size()), false); /*VIS*/
                    }

                protected:
                    // 内部辅助函数，用于创建一个新的霍夫曼树节点。
                    
                    Node *createNodeInternal(int label, T weight) { 
                        return BinaryTreeLinked<value_type, Node>::createNodeInternal(value_type({label, weight}));
                    }

                private:
                    T WPL;                                  // 存储带权路径长度 (Weighted Path Length)。
                    std::map<int, std::string> HuffmanCode; // 存储从标签到霍夫曼编码的映射。

                    /**
                     * @brief 递归计算树的带权路径长度 (WPL)。
                     * @param p 当前节点指针。
                     * @param pathLength 从根到当前节点的路径长度（深度）。
                     * @return T 子树的 WPL。
                     */
                    T calculateWPL(Node *p, int pathLength)
                    {
                        if (!p)
                            return 0; // 空节点，贡献为 0。
                        // 如果是叶子节点，其 WPL = 自身权重 * 深度。
                        if (!p->left() && !p->right())
                            return p->value().weight * pathLength;
                        // 如果是内部节点，返回左右子树 WPL 之和。
                        return calculateWPL(p->left(), pathLength + 1) + calculateWPL(p->right(), pathLength + 1);
                    }
                    /**
                     * @brief 递归生成霍夫曼编码。
                     * @param p 当前节点指针。
                     * @param hcode 从根到当前节点的路径所形成的编码。
                     */
                    void calculateHuffmanCode(Node *p, std::string hcode)
                    {
                        if (!p)
                            return;// 空节点，返回。
                        
                        // 如果是叶子节点，记录其对应的霍夫曼编码。
                        if (!p->left() && !p->right())
                        {
                            // 特殊情况：如果树只有一个节点，其编码通常定义为 "0"。
                            HuffmanCode[p->value().label] = hcode.empty() ? "0" : hcode;
                            return;
                        }
                        // 递归遍历左子树，路径编码加 "0"。
                        calculateHuffmanCode(p->left(), hcode + "0");
                        // 递归遍历右子树，路径编码加 "1"。
                        calculateHuffmanCode(p->right(), hcode + "1");
                    }
                };

            }
        }
    }
}
