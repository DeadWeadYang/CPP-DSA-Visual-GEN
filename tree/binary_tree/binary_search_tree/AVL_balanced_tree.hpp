#pragma once
#include "binary_search_tree.hpp"
#include "../../tree_vis.hpp"
#include <string>

namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {
            namespace BinarySearchTree
            {
                namespace AVLTree
                {
                    using DSA::Tree::Vis::BTreeFocus;
                    using DSA::Tree::Vis::BTreeUnfocus;
                    // 这是一个“增强策略”，专门为节点添加“高度”信息。
                    // 它是策略模式的一个应用，将AVL树特定的数据(height)和行为(update)与通用BST逻辑解耦。
                    template <typename OtherAugment = DefaultNodeAugmentation>
                    struct SubtreeHeightAugment
                    {
                        // Data结构体通过继承，为节点添加了一个height成员。
                        struct Data : OtherAugment::Data
                        {
                            int height = 0;
                        };
                        // update函数是此策略的核心。它在节点的孩子发生变化后被调用，
                        // 用来根据左右子树的高度重新计算并更新当前节点的高度。
                        static void update(Data *node_data, const Data *left_data, const Data *right_data)
                        {
                            set_height(node_data, std::max(get_height(left_data), get_height(right_data)) + 1);
                            // if constexpr 确保了只有当OtherAugment也需要更新时，才会编译其update调用。
                            if constexpr (requires { OtherAugment::update(node_data, left_data, right_data); })
                            {
                                OtherAugment::update(node_data, left_data, right_data);
                            }
                        }
                        template <typename NodeLike>
                        static void update(NodeLike *node, const NodeLike *left, const NodeLike *right)
                        {
                            update(static_cast<Data *>(node), static_cast<const Data *>(left), static_cast<const Data *>(right));
                            if (node)
                            {
                                /*VIS*/ DSA_VIS_BT_SET_NOTE("T", node, std::string("h=") + std::to_string(get_height(static_cast<const Data *>(node))), false);
                            }
                        }
                        // 一个安全的辅助函数，将空指针（NIL节点）的高度视为0。
                        static int get_height(const Data *node_data) { return node_data ? node_data->height : 0; }
                        static int set_height(Data *node_data, int h)
                        {
                            if (!node_data)
                                return 0;
                            node_data->height = h;
                            return h;
                        }
                        // 这个编译时标志告诉基类BST，在插入或删除后，
                        // 必须沿着路径向上调用update函数，从而确保所有祖先节点的高度都被正确更新。
                        static constexpr bool require_path_update = true;
                    };
                    // AVL树的节点类型，它通过继承和模板参数，将SubtreeHeightAugment策略应用到BST节点上。
                    template <typename T, typename AugmentPolicy = DefaultNodeAugmentation>
                    struct AVLTreeNodeLinked : BinarySearchTreeNodeLinked<T, AVLTreeNodeLinked<T, AugmentPolicy>, SubtreeHeightAugment<AugmentPolicy>>
                    {
                        using node_base = BinarySearchTreeNodeLinked<T, AVLTreeNodeLinked<T, AugmentPolicy>, SubtreeHeightAugment<AugmentPolicy>>;
                        using BinarySearchTreeNodeLinked<T, AVLTreeNodeLinked<T, AugmentPolicy>, SubtreeHeightAugment<AugmentPolicy>>::BinarySearchTreeNodeLinked;
                    };
                    // 重载swap_node，以确保在使用tree_swap时，节点的高度也被正确交换。
                    template <typename T, typename AugmentPolicy>
                    void swap_node(AVLTreeNodeLinked<T, AugmentPolicy> &x, AVLTreeNodeLinked<T, AugmentPolicy> &y)
                    {
                        using node_base = AVLTreeNodeLinked<T, AugmentPolicy>::node_base;
                        swap_node(static_cast<node_base &>(x), static_cast<node_base &>(y));
                        using HeightData = SubtreeHeightAugment<AugmentPolicy>::Data;
                        DSA_VIS_BT_SET_NOTE("T", &x, std::string("h=") + std::to_string(SubtreeHeightAugment<AugmentPolicy>::get_height(static_cast<const HeightData *>(&x))), false); /*VIS*/
                        DSA_VIS_BT_SET_NOTE("T", &y, std::string("h=") + std::to_string(SubtreeHeightAugment<AugmentPolicy>::get_height(static_cast<const HeightData *>(&y))), false); /*VIS*/
                    }
                    // AVL树的主类。
                    template <typename T, typename KeyT = T, typename Compare = std::less<KeyT>, typename KeyOfValue = IdentityKeyOfValue<T>, typename OtherAugment = DefaultNodeAugmentation>
                    struct AVLTreeLinked : BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, SubtreeHeightAugment<OtherAugment>, AVLTreeNodeLinked<T, OtherAugment>>
                    {
                        // static_assert(
                        //     std::is_base_of<RBTreeNodeLinked<T>, NodeType>::value,
                        //     "RBTree only accepts NodeType inheriting RBTreeNodeLinked");
                        using value_type = T;
                        using key_type = KeyT;
                        using AugmentPolicy = SubtreeHeightAugment<OtherAugment>;
                        using NodeType = AVLTreeNodeLinked<T, OtherAugment>;
                        using Base = BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, AugmentPolicy, NodeType>;
                        using iterator = Base::iterator;
                        using const_iterator = Base::const_iterator;
                        using pointer = Base::pointer;
                        using const_pointer = Base::const_pointer;
                        using reference = Base::reference;
                        using const_reference = Base::const_reference;
                        using difference_type = Base::difference_type;
                        using size_type = Base::size_type;
                        using Node = typename Base::Node;
                        using BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, AugmentPolicy, NodeType>::BinarySearchTreeLinked;
                        bool checker_unique() const override
                        {
                            if (!check_avl())
                                return false;
                            return Base::checker_unique();
                        }
                        bool checker_multi() const override
                        {
                            if (!check_avl())
                                return false;
                            return Base::checker_multi();
                        }
                        explicit AVLTreeLinked(const AVLTreeLinked &other) = default;

                        AVLTreeLinked &operator=(const AVLTreeLinked &other) = default;

                        std::unique_ptr<AVLTreeLinked> clone() const
                        {
                            return std::unique_ptr<AVLTreeLinked>(clone_unsafe());
                        }

                        virtual void swap(AVLTreeLinked &other)
                        {
                            this->Base::swap(other);
                        }
                        void swap(Base &other) override
                        {
                            if (typeid(*this) != typeid(other))
                                return;
                            this->Base::swap(other);
                        }

                    protected:
                        static int get_height(Node *node) { return AugmentPolicy::get_height(node); }
                        static Node *set_height(Node *node, int h) { return AugmentPolicy::set_height(node, h), node; }
                        // 计算节点的平衡因子。公式为：右子树高度 - 左子树高度。
                        // > 1: 右子树过高 (Right-heavy)
                        // < -1: 左子树过高 (Left-heavy)
                        // [-1, 1]: 平衡
                        static int get_balance_factor(Node *node) { return node ? (get_height(node->right()) - get_height(node->left())) : 0; }
                        // 从parent节点开始，向上回溯至根，检查并修复所有不满足AVL平衡条件的节点。
                        void height_rebalance(Node *parent)
                        {
                            // balance_factor = right_height - left_heght
                            if (!parent)
                                return;
                            /*VIS*/ DSA_VIS_NOTE("AVL 回溯检查平衡因子");
                            bool to_right;
                            // 循环从修改点的父节点开始，一直向上检查到根（的父节点，即header）。
                            while (parent != this->end_ptr())
                            {
                                int pbf = get_balance_factor(parent); // pbf: parent's balance factor
                                // 如果当前节点是平衡的，其高度已在之前的may_update_path或rotate中被更新。
                                // 我们只需继续向上检查父节点即可。
                                if (std::abs(pbf) <= 1)
                                {
                                    parent = parent->parent;
                                    continue;
                                }
                                /*VIS*/ DSA_VIS_STEP(std::string("AVL 回溯发现失衡节点 parent：bf=") + std::to_string(pbf) + (pbf < 0 ? "，左子树比右子树高至少 2" : "，右子树比左子树高至少 2"));
                                // --- 如果代码执行到这里，说明parent节点不平衡，需要旋转 ---

                                // `node` 是 `parent` 的较高一侧的子节点。
                                Node *node = (pbf < 0 ? parent->left() : parent->right()); // the higher child
                                int nbf = get_balance_factor(node);                        // can only be -1 or 0 or 1
                                DSA_VIS_DECL(bool parent_left_heavy = (pbf < 0);)
                                DSA_VIS_DECL(bool child_left_heavy = (nbf < 0);)
                                DSA_VIS_DECL(bool child_right_heavy = (nbf > 0);)
                                DSA_VIS_DECL(std::string parent_side = parent_left_heavy ? "左" : "右";)
                                DSA_VIS_DECL(std::string child_side = child_left_heavy ? "左" : (child_right_heavy ? "右" : "中");)
                                DSA_VIS_DECL(std::string avl_case = parent_left_heavy ? (child_right_heavy ? "LR" : "LL") : (child_left_heavy ? "RL" : "RR");)
                                DSA_VIS_ONLY(BTreeFocus(parent, node));
                                /*VIS*/ DSA_VIS_STEP("AVL 判定旋转类型：" + avl_case + "。parent 的" + parent_side + "子树过高，较高子节点 node 的 bf=" + std::to_string(nbf) + "（" + child_side + "侧更高/或两侧等高）");

                                // 判断是需要双旋转 (LR, RL) 还是单旋转 (LL, RR)。
                                // 如果parent和node的失衡方向相反 (pbf和nbf异号)，则为双旋转情况。
                                if (nbf * pbf < 0)
                                {
                                    Node *inner = nbf < 0 ? node->left() : node->right();
                                    DSA_VIS_ONLY(BTreeFocus(inner));
                                    /*VIS*/ DSA_VIS_STEP(avl_case + " 型双旋第一步：parent 和 node 的较高方向相反，先对 node " + std::string(nbf < 0 ? "右旋" : "左旋") + "，让内侧孙节点上升，把三角形转成直线");
                                    // 双旋转的第一步：对`node`进行一次反向旋转，将其转换为单旋转的情况。
                                    // here, balance_factor of node can only be -1 or 1
                                    // in this case, the higher child of node and the higher child (aka node) of parent are in different direction
                                    this->rotate(node, to_right = (nbf < 0)); /*VIS*/ (to_right ? DSA_VIS_BT_ROTATE_RIGHT("T", node, false) : DSA_VIS_BT_ROTATE_LEFT("T", node, false));
                                    // 旋转后，`node`指针仍然指向原来的节点，但该节点已成为子节点。
                                    // 必须将其更新为新的父节点(即旋转后的子树新根)，为下一步做准备。
                                    node = node->parent;
                                    /*VIS*/ DSA_VIS_STEP(avl_case + " 型双旋第一步完成：新的 node 成为较高子树根，接下来按单旋处理 parent");
                                }
                                // 单旋转或双旋转的第二步：对`parent`进行旋转。
                                // now the higher child of (new)node and the higher child (aka (new)node) of parent are in the same direction
                                /*VIS*/ DSA_VIS_STEP(avl_case + " 型最终旋转：对失衡节点 parent " + std::string(pbf < 0 ? "右旋" : "左旋") + "，让较高子节点上升为局部子树新根");
                                this->rotate(parent, to_right = (pbf < 0)); /*VIS*/ (to_right ? DSA_VIS_BT_ROTATE_RIGHT("T", parent, false) : DSA_VIS_BT_ROTATE_LEFT("T", parent, false));
                                /*VIS*/ DSA_VIS_STEP("AVL 本轮旋转修复完成：局部子树重新满足 |bf| <= 1，继续向上检查祖先");
                                DSA_VIS_ONLY(BTreeUnfocus(parent, node, parent->left(), parent->right(), node->left(), node->right()));
                                // `parent`指针指向的节点在旋转后已成为子节点。
                                // `parent->parent` 现在指向整个旋转操作完成后的新子树的根
                                node = parent->parent;
                                // `node->parent` 则是我们需要在下一次循环中检查的更高一级的祖先节点。
                                parent = node->parent;
                            }
                        }

                        // insert_hook和erase_hook是基类提供的钩子函数。
                        // 在插入或删除一个节点后，基类会调用这些钩子，从而触发我们的平衡恢复逻辑。
                        void insert_hook(Node *p) override { return height_rebalance(p->parent); }
                        void erase_hook(Node *p) override { return height_rebalance(p->parent); }
                        Node *createNodeInternal(const T &v) override
                        {
                            Node *node = set_height(Base::createNodeInternal(v), 1); /*VIS*/ DSA_VIS_BT_SET_NOTE("T", node, "h=1", false);
                            /*VIS*/ DSA_VIS_NOTE("AVL 新节点高度初始化为 1");
                            return node;
                        }
                        // 检查全树是否满足AVL属性的辅助函数，用于调试和验证。
                        bool check_avl() const
                        {
                            std::function<bool(Node *)> func;
                            func = [&](Node *p) -> bool
                            {
                                if (!p)
                                    return true;
                                if (std::abs(get_balance_factor(p)) > 1)
                                    return false;
                                if (!func(p->left()))
                                    return false;
                                if (!func(p->right()))
                                    return false;
                                return true;
                            };
                            return func(this->root());
                        }
                        AVLTreeLinked *clone_unsafe() const override
                        {
                            return new AVLTreeLinked(*this);
                        }
                    };

                }
            }
        }
    }
}

