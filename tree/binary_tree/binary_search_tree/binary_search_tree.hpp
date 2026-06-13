#pragma once
#include <functional>
#include <algorithm>
#include <type_traits>
#include <concepts>
#include "../binary_tree_basic.hpp"
#include "../../../vis_trace.hpp"
#include "../../../utils.hpp"
namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {
            namespace BinarySearchTree
            {
                using DSA::Utils::IdentityKeyOfValue;
                // 默认的节点增强策略。这是一个空策略，用于普通BST。
                // 通过替换这个策略，可以实现更高级的树，如红黑树或顺序统计树。
                struct DefaultNodeAugmentation
                {
                    // 节点可以携带的额外数据，默认为空。
                    struct Data
                    {
                    };
                    // 一个编译时标志，指示在插入/删除后是否需要从节点向上更新到根。
                    // 对于需要动态维护子树信息的树应设为 true。
                    static constexpr bool require_path_update = false;
                    // 一个编译时标志，用于指示是否实现了子树大小的快速计算。
                    static constexpr bool subtree_size_implemented = false;
                };
                // BST的节点，继承自基础二叉树节点，并应用了“增强策略”。
                // AugmentPolicy: 这是一个策略模式的应用，允许我们“注入”额外的数据和行为到节点中。
                template <typename T, typename NodeType, typename AugmentPolicy = DefaultNodeAugmentation>
                struct BinarySearchTreeNodeLinked : AugmentPolicy::Data, BinaryTreeNodeLinked<T, NodeType>
                {

                    using augment_policy = AugmentPolicy;
                    using node_base = BinaryTreeNodeLinked<T, NodeType>;
                    using BinaryTreeNodeLinked<T, NodeType>::BinaryTreeNodeLinked;
                    virtual ~BinarySearchTreeNodeLinked()
                    {
                        static_assert(
                            std::is_base_of<BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy>, NodeType>::value,
                            "NodeType must inherit from BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy>");
                    }
                };
                template <typename AugmentData>
                void swap_augment_data(AugmentData &x, AugmentData &y)
                {
                    std::swap(x, y);
                }
                // 重载基础树的 swap_node，以确保在交换节点拓扑时，也交换增强数据。
                template <typename T, typename NodeType, typename AugmentPolicy>
                void swap_node(BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy> &x, BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy> &y)
                {
                    using node_base = BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy>::node_base;
                    // 首先调用基类的swap_node来交换parent和children指针。
                    swap_node(static_cast<node_base &>(x), static_cast<node_base &>(y));
                    // 然后交换派生类中由AugmentPolicy::Data带来的额外数据。
                    using AugmentData = AugmentPolicy::Data;
                    swap_augment_data(static_cast<AugmentData &>(x), static_cast<AugmentData &>(y));
                }
                // 一个基础的、满足CRTP和增强策略的BST节点类型。
                template <typename T, typename AugmentPolicy = DefaultNodeAugmentation>
                struct BinarySearchTreeNodeLinkedBasic : BinarySearchTreeNodeLinked<T, BinarySearchTreeNodeLinkedBasic<T, AugmentPolicy>, AugmentPolicy>
                {
                    using BinarySearchTreeNodeLinked<T, BinarySearchTreeNodeLinkedBasic<T, AugmentPolicy>, AugmentPolicy>::BinarySearchTreeNodeLinked;
                };

                // KeyOfValue: 一个函数对象，用于从值类型T中提取出键类型KeyT。
                // 例如，对于std::map<Key, Value>，T是std::pair<const Key, Value>，KeyOfValue会提取出其中的Key。
                // 对于std::set<Key>，T就是Key，KeyOfValue直接返回自身即可（通过IdentityKeyOfValue实现）。
                template <typename T, typename KeyT = T, typename Compare = std::less<KeyT>, typename KeyOfValue = IdentityKeyOfValue<T>, typename AugmentPolicy = DefaultNodeAugmentation, typename NodeType = BinarySearchTreeNodeLinkedBasic<T, AugmentPolicy>>
                struct BinarySearchTreeLinked : BinaryTreeLinked<T, NodeType>
                {
                    using Base = BinaryTreeLinked<T, NodeType>;
                    using value_type = T;
                    using key_type = KeyT;
                    using size_type = Base::size_type;
                    using extracted_key_t =
                        std::invoke_result_t<KeyOfValue, const value_type &>;

                    // 静态断言，确保KeyOfValue的返回类型与模板参数KeyT匹配。
                    static_assert(
                        std::is_same<const key_type &, extracted_key_t>::value,
                        "KeyOfValue::operator()(Value) must return Key");
                    // 静态断言，确保节点类型符合要求。
                    static_assert(
                        std::is_base_of<BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy>, NodeType>::value,
                        "NodeType must inherit from BinarySearchTreeNodeLinked<T,NodeType,AugmentPolicy>");

                    using iterator = Base::iterator;
                    using const_iterator = Base::const_iterator;
                    using pointer = Base::pointer;
                    using const_pointer = Base::const_pointer;
                    using reference = Base::reference;
                    using const_reference = Base::const_reference;
                    using difference_type = Base::difference_type;
                    using Node = typename Base::Node;
                    using CNode = const Node;
                    using BinaryTreeLinked<T, NodeType>::BinaryTreeLinked;
                    explicit BinarySearchTreeLinked(const Compare &cmp = Compare{}) : BinaryTreeLinked<T, NodeType>(), comp(cmp), size_r(0) {}

                    explicit BinarySearchTreeLinked(const BinarySearchTreeLinked &other) : BinaryTreeLinked<T, NodeType>(other)
                    {
                        comp = other.comp;
                        size_r = other.size_r;
                    }
                    void clear() override
                    {
                        Base::clear();
                        size_r = 0;
                    }
                    BinarySearchTreeLinked &operator=(const BinarySearchTreeLinked &other)
                    {
                        this->Base::operator=(other);
                        comp = other.comp;
                        size_r = other.size_r;
                        return *this;
                    }
                    // swap函数，除了交换基类部分(header指针)，也需要交换派生类自己的成员(比较器和大小)。
                    virtual void swap(BinarySearchTreeLinked &other) 
                    {
                        using std::swap;
                        this->Base::swap(other);
                        swap(comp, other.comp);
                        swap(size_r, other.size_r);
                    }
                    void swap(Base&other)override{
                        if(typeid(*this)!=typeid(other))return;
                        this->swap(static_cast<BinarySearchTreeLinked&>(other));
                    }

                    // 检查树是否满足唯一键的BST的属性。用于调试。
                    virtual bool checker_unique() const
                    {
                        return check_unique(this->root(), nullptr, nullptr);
                    }
                    // 检查树是否满足多重键的BST的属性。用于调试。
                    virtual bool checker_multi() const
                    {
                        return check_multi(this->root(), nullptr, nullptr);
                    }

                    size_type size() const { return size_r; }
                    iterator find(const key_type &k)
                    {
                        return iterator{find_pointer(k)};
                    }
                    const_iterator find(const key_type &k) const
                    {
                        return const_iterator{find_pointer(k)};
                    }
                    size_type count_unique(const key_type &k) const { return count_unique_pointer(k); }
                    size_type count_multi(const key_type &k) const { return count_multi_pointer(k); }
                    iterator lower_bound(const key_type &k)
                    {
                        return iterator{lower_bound_pointer(k)};
                    }
                    const_iterator lower_bound(const key_type &k) const
                    {
                        return const_iterator{lower_bound_pointer(k)};
                    }
                    iterator upper_bound(const key_type &k)
                    {
                        return iterator{upper_bound_pointer(k)};
                    }
                    const_iterator upper_bound(const key_type &k) const
                    {
                        return const_iterator{upper_bound_pointer(k)};
                    }
                    // 插入唯一键。返回一个pair，包含指向元素或已存在元素的迭代器，以及一个指示插入是否成功的bool值。
                    // 这与std::map::insert的行为一致。
                    std::pair<iterator, bool> insert_unique(const T &v)
                    {
                        std::pair<Node *, bool> res = insert_unique_pointer(v);
                        return {iterator{res.first}, res.second};
                    }

                    // 插入可重复键。总是插入新元素，并返回指向新元素的迭代器。
                    // 这与std::multimap::insert的行为一致。
                    iterator insert_multi(const T &v)
                    {
                        return iterator{insert_multi_pointer(v)};
                    }
                    iterator erase(const_iterator iter)
                    {
                        if (iter == this->cend())
                            return this->end();
                        Node *p = this->getNode(iter);
                        return iterator{erase_pointer(p)};
                    }
                    // 返回一个迭代器对，表示键值为k的元素范围[first, last)。
                    size_type erase_unique(const key_type &k)
                    {
                        return erase_unique_pointer(k);
                    }
                    size_type erase_multi(const key_type &k)
                    {
                        return erase_multi_pointer(k);
                    }
                    std::pair<iterator, iterator> equal_range_unique(const key_type &k)
                    {
                        std::pair<Node *, Node *> res = equal_range_unique_pointer(k);
                        return {iterator{res.first}, iterator{res.second}};
                    }
                    std::pair<const_iterator, const_iterator> equal_range_unique(const key_type &k) const
                    {
                        std::pair<Node *, Node *> res = equal_range_unique_pointer(k);
                        return {const_iterator{res.first}, const_iterator{res.second}};
                    }
                    std::pair<iterator, iterator> equal_range_multi(const key_type &k)
                    {
                        std::pair<Node *, Node *> res = equal_range_multi_pointer(k);
                        return {iterator{res.first}, iterator{res.second}};
                    }
                    std::pair<const_iterator, const_iterator> equal_range_multi(const key_type &k) const
                    {
                        std::pair<Node *, Node *> res = equal_range_multi_pointer(k);
                        return {const_iterator{res.first}, const_iterator{res.second}};
                    }
                    Compare &comparer() { return comp; }
                    const Compare &comparer() const { return comp; }

                    std::unique_ptr<BinarySearchTreeLinked> clone() const
                    {
                        return std::unique_ptr<BinarySearchTreeLinked>(clone_unsafe());
                    }

                protected:
                    Compare comp{};
                    size_type size_r = 0;
                    // skey是一组静态辅助函数，用于通过KeyOfValue从不同来源（节点、指针、值）中方便地提取键。
                    static const key_type &skey(Node *node) { return KeyOfValue{}(node->value()); }
                    static const key_type &skey(const T &v) { return KeyOfValue{}(v); }
                    // 查找的指针实现：标准的BST查找。
                    virtual Node *find_pointer(const key_type &k) const
                    {
                        Node *p = this->root();
                        while (p)
                        {
                            const key_type &pk = skey(p);
                            if (this->comp(k, pk))
                                p = p->left();
                            else if (this->comp(pk, k))
                                p = p->right();
                            else
                                return p;
                        }
                        // 如果没有找到，返回end()迭代器对应的指针。
                        return p ? p : this->end_ptr();
                    }
                    // lower_bound的指针实现：查找第一个不小于k的元素。
                    virtual Node *lower_bound_pointer(const key_type &k, Node *node, Node *res) const
                    {
                        while (node)
                        {
                            // 如果当前节点的键不小于k (node->key >= k)
                            if (!comp(skey(node), k))
                            {
                                // 那么它是一个潜在的答案，记录下来，然后去左子树寻找更精确（更小）的答案。
                                res = node;
                                node = node->left();
                            }
                            else // 如果 node->key < k ，答案一定在右子树。
                                node = node->right();
                        }
                        return res;
                    }
                    Node *lower_bound_pointer(const key_type &k) const
                    {
                        return lower_bound_pointer(k, this->root(), this->end_ptr());
                    }
                    // upper_bound的指针实现：查找第一个大于k的元素。
                    virtual Node *upper_bound_pointer(const key_type &k, Node *node, Node *res) const
                    {
                        while (node)
                        {
                            // 如果 k < node->key
                            if (comp(k, skey(node)))
                            {
                                // 那么它是一个潜在的答案，记录下来，然后去左子树寻找更精确（更小）的答案。
                                res = node;
                                node = node->left();
                            }
                            else // 如果 node->key <= k ，答案一定在右子树。
                                node = node->right();
                        }
                        return res;
                    }
                    Node *upper_bound_pointer(const key_type &k) const
                    {
                        return upper_bound_pointer(k, this->root(), this->end_ptr());
                    }
                    virtual std::pair<Node *, Node *> equal_range_unique_pointer(const key_type &k) const
                    {
                        Node *node = this->root(), *res = this->end_ptr();
                        while (node)
                        {
                            if (comp(k, skey(node)))
                            {
                                res = node;
                                node = node->left();
                            }
                            else if (comp(skey(node), k))
                            {
                                node = node->right();
                            }
                            else // 对于unique tree，范围是 [node, successor(node)] ；successor(node)可以快速计算为node右子树的最左节点，如果右子树不存在，则为之前记录的res。
                                return {node, (node->right() ? binary_leftmost(node->right()) : res)};
                        }
                        return {res, res};
                    }
                    virtual std::pair<Node *, Node *> equal_range_multi_pointer(const key_type &k) const
                    {
                        // 对于允许重复键的树，equal_range就是[lower_bound, upper_bound]。
                        // 这里做了一个小优化：首先大致定位到k所在的区域，然后再在该区域内精确计算lower_bound和upper_bound。
                        Node *node = this->root(), *res = this->end_ptr();
                        while (node)
                        {
                            if (comp(k, skey(node)))
                            {
                                res = node;
                                node = node->left();
                            }
                            else if (comp(skey(node), k))
                            {
                                node = node->right();
                            }
                            else
                                return {lower_bound_pointer(k, node->left(), node),
                                        upper_bound_pointer(k, node->right(), res)};
                        }
                        return {res, res};
                    }
                    size_type count_unique_pointer(const key_type &k) const
                    {
                        return size_t(find_pointer(k) != this->end_ptr());
                    }
                    size_type count_multi_pointer(const key_type &k) const
                    {
                        if (this->empty())
                            return 0;
                        Node *node = this->root();
                        //     // 如果节点增强策略实现了子树大小统计，这里可以有一个O(logN)的实现。
                        // if constexpr ((AugmentPolicy::subtree_size_implemented) && (requires { node.size; }))
                        // {
                        //     size_type res=0;
                        //     throw std::logic_error("fast count not implementd yet");
                        // }
                        // else
                        // {
                        // 默认实现：计算equal_range返回的范围内的元素个数。复杂度为O(logN + count)。
                        auto rg = equal_range_multi_pointer(k);
                        return std::distance(iterator{rg.first}, iterator{rg.second});
                        // }
                    }
                    virtual std::pair<Node *, bool> insert_unique_pointer(const T &v)
                    {
                        if (this->empty())
                        {
                            /*VIS*/ DSA_VIS_STEP("BST 插入（unique）：空树，创建第一个节点作为根");
                            Node *root_node = insert_root(v);
                            /*VIS*/ DSA_VIS_STEP("BST 插入（unique）：根节点创建完成");
                            return {root_node, true};
                        }
                        const key_type &vk = skey(v);
                        Node *node = this->root(), *p = nullptr;
                        bool insert_right = true;
                        while (node)
                        {
                            p = node;
                            const key_type &pk = skey(p);
                            if (insert_right = this->comp(pk, vk))
                                node = p->right();
                            else if (this->comp(vk, pk))
                                node = p->left();
                            else
                            {
                                /*VIS*/ DSA_VIS_STEP("BST 插入（unique）：发现重复键，跳过插入");
                                return {p, false};
                            }
                        }
                        /*VIS*/ DSA_VIS_NOTE("BST 插入（unique）：已找到插入位置，准备挂接新节点");
                        node = insert_to(insert_right, v, p);
                        // 如果需要，向上更新路径上的增强数据
                        may_update_path(node);
                        /*VIS*/ DSA_VIS_STEP(insert_right ? "BST 插入（unique）：新节点已挂到父节点右侧" : "BST 插入（unique）：新节点已挂到父节点左侧");
                        // 为派生类（如红黑树）提供插入后的处理钩子（如重新平衡）
                        insert_hook(node);
                        return {node, true};
                    }
                    virtual Node *insert_multi_pointer(const T &v)
                    {
                        if (this->empty())
                        {
                            /*VIS*/ DSA_VIS_STEP("BST 插入（multi）：空树，创建第一个节点作为根");
                            Node *root_node = insert_root(v);
                            /*VIS*/ DSA_VIS_STEP("BST 插入（multi）：根节点创建完成");
                            return root_node;
                        }
                        const key_type &vk = skey(v);
                        Node *node = this->root(), *p = nullptr;
                        bool insert_right = true;
                        while (node)
                        {
                            p = node;
                            const key_type &pk = skey(p);
                            // 关键区别：当 v->key >= p->key 时，都往右走。
                            // comp(vk, pk) 为 false 意味着 vk >= pk。
                            // 这确保了所有等价的键都会被插入到已有键的右侧，从而聚在一起。
                            if (insert_right = !this->comp(vk, pk))
                                node = p->right();
                            else
                                node = p->left();
                        }
                        /*VIS*/ DSA_VIS_NOTE("BST 插入（multi）：已找到插入位置，准备挂接新节点");
                        node = insert_to(insert_right, v, p);
                        // 如果需要，向上更新路径上的增强数据
                        may_update_path(node);
                        /*VIS*/ DSA_VIS_STEP(insert_right ? "BST 插入（multi）：新节点已挂到父节点右侧" : "BST 插入（multi）：新节点已挂到父节点左侧");
                        // 为派生类（如红黑树）提供插入后的处理钩子（如重新平衡）
                        insert_hook(node);
                        return node;
                    }
                    Node *insert_root(const T &v)
                    {
                        size_r = 1;
                        /*VIS*/ DSA_VIS_NOTE("BST 建立根节点");
                        Node *p = this->createNodeInternal(v); /*VIS*/ DSA_VIS_BT_NEW_NODE("T", p, p->value(), false);
                        this->solo_root(p); /*VIS*/ DSA_VIS_BT_SET_ROOT("T", p, false);
                        return p;
                    }
                    Node *insert_to(const bool insert_right, const T &v, Node *p)
                    {
                        ++size_r;
                        /*VIS*/ DSA_VIS_NOTE(insert_right ? "BST 插入到父节点右子树" : "BST 插入到父节点左子树");
                        Node *node = this->createNodeInternal(v); /*VIS*/ DSA_VIS_BT_NEW_NODE("T", node, node->value(), false);
                        this->link_to(insert_right, node, p); /*VIS*/ DSA_VIS_BT_LINK("T", p, insert_right, node, false);
                        // 插入后，需要检查并更新缓存的leftmost和rightmost指针
                        if (insert_right)
                        {
                            if (p == this->rightmost())
                                this->rightmost() = node;
                        }
                        else
                        {
                            if (p == this->leftmost())
                                this->leftmost() = node;
                        }
                        return node;
                    }
                    Node *erase_pointer(Node *p)
                    {
                        if (!p || p == this->end_ptr())
                            return this->end_ptr();
                        /*VIS*/ DSA_VIS_STEP("BST 删除：定位目标节点");
                        /*VIS*/ DSA_VIS_BT_MARK("T", p, false);
                        // 核心删除逻辑在tree_remove中，它负责处理拓扑关系
                        Node *np = tree_remove(p);
                        /*VIS*/ DSA_VIS_NOTE("BST 删除：完成拓扑移除，开始回溯更新");
                        // 向上更新路径
                        may_update_path(p->parent);
                        // 为派生类提供删除后的处理钩子
                        /*VIS*/ DSA_VIS_BT_UNMARK("T", p, false);
                        /*VIS*/ DSA_VIS_BT_DESTROY_NODE("T", p, false);
                        erase_hook(p);
                        /*VIS*/ DSA_VIS_NOTE("BST 删除完成");
                        // 释放节点内存
                        this->destroyNodeInternal(p);
                        // 返回指向被删除元素下一个元素的指针
                        return np;
                    }
                    size_type erase_unique_pointer(const key_type &k)
                    {
                        Node *p = find_pointer(k);
                        if (p == this->end_ptr())
                            return 0;
                        erase_pointer(p);
                        return 1;
                    }
                    size_type erase_multi_pointer(const key_type &k)
                    {
                        size_type res = 0;
                        // 找到所有等价键的范围，然后依次删除
                        for (auto pr = equal_range_multi_pointer(k);
                             pr.first != pr.second; ++res)
                        {
                             // erase_pointer会返回下一个元素的指针，正好可以用于下一次循环
                            pr.first = erase_pointer(pr.first);
                        }
                        return res;
                    }
                    // 一个辅助函数，根据增强策略决定是否需要更新从p到根的路径。
                    // 使用if constexpr可以在编译时移除这个循环，当策略不需要更新时，没有运行时开销。
                    void may_update_path(Node *p)
                    {

                        if constexpr (AugmentPolicy::require_path_update)
                        {

                            while (p && p != this->end_ptr())
                            {
                                // 如果策略定义了update函数，就调用它。
                                if constexpr (requires { AugmentPolicy::update(p, p->left(), p->right()); })
                                {
                                    AugmentPolicy::update(p, p->left(), p->right());
                                }
                                p = p->parent;
                            }
                        }
                    }
                    // 从树中移除节点p的拓扑结构，是删除操作最核心、最复杂的部分。
                    Node *tree_remove(Node *p)
                    {
                        if (!p || p == this->end_ptr())
                            return this->end_ptr();
                        --size_r;
                        // res 是 p 在中序遍历中的后继者，也是删除操作后迭代器应该指向的位置。
                        Node *res = this->next(p);
                        if (this->only_root())
                        {
                            /*VIS*/ DSA_VIS_NOTE("BST 删除：树仅有根节点，直接清空");
                            /*VIS*/ DSA_VIS_BT_REMOVE_NODE("T", p, false);
                            this->remove_root(); /*VIS*/ DSA_VIS_BT_SET_ROOT("T", static_cast<Node *>(nullptr), false);
                            return res;
                        }
                        // 更新缓存的leftmost和rightmost
                        if (p == this->leftmost())
                            this->leftmost() = res;
                        if (p == this->rightmost())
                            this->rightmost() = this->prev(p);
                        // 情况1：p有两个孩子。这是最复杂的情况。
                        if (p->left() && p->right())
                        {
                            /*VIS*/ DSA_VIS_STEP("BST 删除：目标有两个孩子，先与中序后继交换拓扑");
                            // 不移动节点的值，而是交换p和其后继者res的“拓扑位置”。
                            // 调用tree_swap后，p节点被移动到了原本res所在的位置。
                            // 因为res是后继者，它保证了最多只有一个右孩子。
                            // 这样，复杂的“双孩子删除”问题就转化为了简单的“单孩子或零孩子删除”问题。
                            this->tree_swap(p, res);
                        }
                        // 情况2：p现在最多只有一个孩子 (原始的p只有一个或零个孩子，或者p是交换后的节点)。
                        // now p only has at most one child
                        Node *t = p->left() ? p->left() : p->right();
                        bool link_right = this->is_right(p);
                        /*VIS*/ DSA_VIS_BT_REMOVE_NODE("T", p, false);
                        // --- 以下是为派生类（如红黑树）的 erase_hook 传递信息的技巧 ---
                        // 在断开p的连接之前，我们临时重用p的子指针来存储“删除上下文”。
                        // p的原始子指针之一现在指向它的替代者t。
                        // 另一个子指针（由link_right决定）指向p自己，这可以被erase_hook用来判断p是左是右。
                        // erase_hook(p)被调用时，它可以通过检查p的这两个子指针，得知替代者是谁，以及删除位置的颜色信息等。
                        p->children[link_right] = p; // record minimum info for rebalance
                        p->children[!link_right] = t;
                        // --- 技巧结束 ---

                        if (p == this->root())
                        {
                            /*VIS*/ DSA_VIS_STEP(t ? "BST 删除：删除当前根节点，由唯一子树接替为新根" : "BST 删除：删除当前根节点，树变为空");
                            // 如果删除的是根，让t成为新根，或者清空树
                            if (t)
                            {
                                this->link_root(t); /*VIS*/ DSA_VIS_BT_SET_ROOT("T", t, false);
                            }
                            else
                            {
                                this->remove_root(); /*VIS*/ DSA_VIS_BT_SET_ROOT("T", static_cast<Node *>(nullptr), false);
                            }
                        }
                        else
                        {
                            /*VIS*/ DSA_VIS_STEP(t ? "BST 删除：父节点绕过目标节点，连接目标的唯一子树" : "BST 删除：父节点绕过目标节点，删除叶子位置");
                            // 将p的parent节点直接连接到t，绕过p。
                            Node *old_parent = p->parent;
                            this->link_to(link_right, t, p->parent); /*VIS*/ DSA_VIS_BT_LINK("T", old_parent, link_right, t, false);
                        }
                        // 返回p的后继者，作为erase的返回值。
                        return res;
                    }
                    // 左旋操作。
                    void rotate_left(Node *node)
                    {
                        /*VIS*/ DSA_VIS_MSG("BST 左旋", false);
                        Node *nparent = node->parent;
                        Node *nright = node->right();
                        bool link_right;
                        // 步骤1: nright的左孩子成为node的右孩子
                        this->link_to(link_right = true, nright->left(), node);
                        // 步骤2: node成为nright的左孩子
                        this->link_to(link_right = false, node, nright);
                        // 步骤3: nright取代node的位置
                        if (node == this->root())
                        {
                            this->link_root(nright);
                        }
                        else
                        {
                            link_right = (node == nparent->right());
                            this->link_to(link_right, nright, nparent);
                        }
                        // 步骤4: 如果有增强数据，更新受影响的节点（先node后nright）
                        if constexpr (requires { AugmentPolicy::update(node, node->left(), node->right()); })
                        {
                            AugmentPolicy::update(node, node->left(), node->right());
                            AugmentPolicy::update(nright, nright->left(), nright->right());
                        }
                    }
                    
                    // 右旋操作。
                    void rotate_right(Node *node)
                    {
                        /*VIS*/ DSA_VIS_MSG("BST 右旋", false);
                        Node *nparent = node->parent;
                        Node *nleft = node->left();
                        bool link_right;
                        // 步骤1: nleft的右孩子成为node的左孩子
                        this->link_to(link_right = false, nleft->right(), node);
                        // 步骤2: node成为nleft的右孩子
                        this->link_to(link_right = true, node, nleft);
                        // 步骤3: nleft取代node的位置
                        if (node == this->root())
                        {
                            this->link_root(nleft);
                        }
                        else
                        {
                            link_right = (node == nparent->right());
                            this->link_to(link_right, nleft, nparent);
                        }

                        // 步骤4: 如果有增强数据，更新受影响的节点（先node后nleft）
                        if constexpr (requires { AugmentPolicy::update(node, node->left(), node->right()); })
                        {
                            AugmentPolicy::update(node, node->left(), node->right());
                            AugmentPolicy::update(nleft, nleft->left(), nleft->right());
                        }
                    }
                    void rotate(Node *node, const bool to_right)
                    {
                        return to_right ? rotate_right(node) : rotate_left(node);
                    }
                    // 插入钩子函数。基类中为空。派生类（如红黑树）可以重写此函数，在插入后进行再平衡。
                    virtual void insert_hook(Node *p) {}
                    // 删除钩子函数。基类中为空。派生类可以重写此函数，在删除后进行再平衡。
                    // tree_remove中的指针设置就是为了给这个函数提供足够的信息
                    virtual void erase_hook(Node *p) {}
                    
                    // 递归检查函数，用于验证树是否满足可重复键的BST性质。
                    bool check_multi(Node *node, Node *liml, Node *limr) const
                    {
                        if (!node)
                            return true;
                        if (liml && comp(skey(node), skey(liml)))
                            return false;
                        if (limr && comp(skey(limr), skey(node)))
                            return false;
                        if (!check_multi(node->left(), liml, node))
                            return false;
                        if (!check_multi(node->right(), node, limr))
                            return false;
                        return true;
                    }
                    // 递归检查函数，用于验证树是否满足唯一键的BST性质。
                    bool check_unique(Node *node, Node *liml, Node *limr) const
                    {
                        if (!node)
                            return true;
                        if (liml && !comp(skey(liml), skey(node)))
                            return false;
                        if (limr && !comp(skey(node), skey(limr)))
                            return false;
                        if (!check_unique(node->left(), liml, node))
                            return false;
                        if (!check_unique(node->right(), node, limr))
                            return false;
                        return true;
                    }
                    BinarySearchTreeLinked *clone_unsafe() const override
                    {
                        return new BinarySearchTreeLinked(*this);
                    }
                };
            }
        }
    }
}
