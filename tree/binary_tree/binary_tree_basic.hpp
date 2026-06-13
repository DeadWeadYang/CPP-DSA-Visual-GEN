#pragma once
#include <type_traits>
#include <functional>
#include <queue>
#include "../../utils.hpp"
#include "../../vis_trace.hpp"
namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {
            // 注意：由于存在哨兵节点，需要存在一层不构造value，不同于传统的NodeBase基类，这里用CRTP实现了隐派生类来管理value
            // 注意：为了减少重复代码，只使用非const的节点指针，并且保证节点指针不暴露
            // 这是一个使用CRTP（奇特重现模板模式）的节点基类。
            // NodeType必须是继承自BinaryTreeNodeLinked<T, NodeType>的类，这通过static_assert来保证。
            // 这种设计允许派生节点（如红黑树节点）添加额外属性（如颜色），同时能被基类代码（如遍历、查找）统一处理。
            /*
                Node info that depend on tree structure should be added in derived NodeType, rather than fit into the value
            */
            template <typename T, typename NodeType>
            struct BinaryTreeNodeLinked
            {
                using value_type = T;
                using Node = NodeType;
                using CNode = const Node;
                // T value;
                Node *parent = nullptr;
                Node *children[2] = {nullptr, nullptr};
                Node *&left() { return children[0]; }
                Node *&right() { return children[1]; }
                Node *left() const { return children[0]; }
                Node *right() const { return children[1]; }

                template <typename U, typename N>
                struct ValueNodeImpl : N
                {
                    U value_;
                    explicit ValueNodeImpl(const U &v) : N(), value_(v)
                    {
                        static_assert(std::is_base_of<BinaryTreeNodeLinked<U, N>, N>::value,
                                      "NodeType must inherit from BinaryTreeNodeLinked<T, NodeType>");
                    }
                };
                using VNode = ValueNodeImpl<T, NodeType>;

                // CRTP的核心运用
                T &value()
                {
                    // 零开销：直接 static_cast，不做运行时检查
                    return static_cast<VNode &>(static_cast<Node &>(*this)).value_;
                }
                const T &value() const
                {
                    return static_cast<const VNode &>(static_cast<const Node &>(*this)).value_;
                }

                explicit BinaryTreeNodeLinked()
                {
                    static_assert(
                        std::is_base_of<BinaryTreeNodeLinked<T, NodeType>, NodeType>::value,
                        "NodeType must inherit from BinaryTreeNodeLinked<T, NodeType>");
                }
                virtual ~BinaryTreeNodeLinked() = default;
            };
            // 一个辅助函数，仅交换两个节点的指针成员，用于 tree_swap 的内部实现。
            template <typename T, typename NodeType>
            void swap_node(BinaryTreeNodeLinked<T, NodeType> &x, BinaryTreeNodeLinked<T, NodeType> &y)
            {
                DSA_VIS_BT_SWAP_TOPOLOGY("T", static_cast<NodeType *>(&x), static_cast<NodeType *>(&y), false); /*VIS*/
                std::swap(x.parent, y.parent);
                std::swap(x.children, y.children);
            }
            // 类型萃取，用于编译时检查一个类型是否是我们定义的二叉树节点。
            template <typename NodeType>
            struct is_binary_tree_node_linked
                : std::is_base_of<BinaryTreeNodeLinked<typename NodeType::value_type, NodeType>, NodeType>
            {
            };
            template <typename T, typename NodeType>
            struct is_binary_tree_node_linked_T
                : std::is_base_of<BinaryTreeNodeLinked<T, NodeType>, NodeType>
            {
            };
            // 从指定节点开始，查找其所在子树的最左节点。
            template <typename NodeType>
            NodeType *binary_leftmost(NodeType *node)
            {
                static_assert(is_binary_tree_node_linked<NodeType>::value,
                              "binary_leftmost() only accepts NodeType inheriting BinaryTreeNodeLinked");
                while (node && node->left())
                    node = node->left();
                return node;
            }
            // 从指定节点开始，查找其所在子树的最右节点。
            template <typename NodeType>
            NodeType *binary_rightmost(NodeType *node)
            {
                static_assert(is_binary_tree_node_linked<NodeType>::value,
                              "binary_rightmost() only accepts NodeType inheriting BinaryTreeNodeLinked");
                while (node && node->right())
                    node = node->right();
                return node;
            }

            namespace detail
            {
                // 查找一个节点在中序遍历中的后继节点。
                // 这个函数的实现逻辑与哨兵头节点（header）设计紧密耦合。
                // 哨兵节点设计: header->left()==root(), header->right()==leftmost(), header->parent==rightmost(), root()->parent==header
                template <typename NodeType>
                NodeType *successor(NodeType *node)
                {
                    static_assert(is_binary_tree_node_linked<NodeType>::value,
                                  "successor() only accepts NodeType inheriting BinaryTreeNodeLinked");
                    // below is for header->left()==root(),header->right()==leftmost(),header->parent==rightmost(),root()->parent==header
                    // 规则1: 如果节点有右子树，其后继是右子树中的最左节点。
                    if (node->right())
                    {
                        node = node->right();
                        while (node->left())
                            node = node->left();
                        return node;
                    }
                    // 规则2: 如果节点没有右子树，需要向上回溯parent节点。
                    NodeType *p = node->parent;
                    // 持续向上，直到“node”是其parent“p”的左孩子。
                    // 这个循环的终点是当 node 成为 root，此时 p 成为 header，
                    // 此时 node == p->left() (即 root == header->left())，循环终止。
                    // 对于 rightmost() 节点，这个循环最终会返回 header，这是正确的行为，因为 end() 是最后一个元素的后继。
                    // The header is the "super-root", all paths eventually lead to it.
                    while (node != p->left())
                    {
                        node = p;
                        p = p->parent;
                    }

                    // If we are the max element, p will be the header. That is the correct successor (end()).
                    // If node is the header, this will not execute correctly, but ++end() is UB.
                    return p;
                }

                // 查找一个节点在中序遍历中的前驱节点。
                // 同样与独特的哨兵设计紧密耦合
                template <typename NodeType>
                NodeType *predecessor(NodeType *node)
                {
                    static_assert(is_binary_tree_node_linked<NodeType>::value,
                                  "predecessor() only accepts NodeType inheriting BinaryTreeNodeLinked");
                    // If 'node' is the header, its predecessor is the maximum element.
                    // 规则1: 如果节点有左子树，其前驱是左子树中的最右节点。
                    // 特殊情况: 如果 node 是 header (end()迭代器)，它的左子树是 root，
                    // 这个逻辑会正确地找到整棵树的最右节点 (rightmost)，即 end() 的前驱。
                    if (node->left())
                    {
                        node = node->left();
                        while (node->right())
                            node = node->right();
                        return node;
                    }
                    // 规则2: 如果节点没有左子树，需要向上回溯parent节点。
                    NodeType *p = node->parent;
                    // 持续向上，直到“node”是其parent“p”的右孩子。
                    // 对于 leftmost() 节点，这个循环会一直走到 p 成为 header，node 成为 root，
                    // 此时 node == p->left()，循环继续，最终导致未定义行为，符合 --begin() 的预期。
                    while (node == p->left())
                    {
                        node = p;
                        p = p->parent;
                    }
                    // above is for header->left()==root(),header->right()==leftmost(),header->parent==rightmost(),root()->parent==header
                    return p;
                }

                template <typename T, typename NodeType>
                struct BinaryTreeLinked;

                template <typename T, typename NodeType>
                class ConstInOrderIterator;

                template <typename T, typename NodeType>
                class InOrderIterator
                {

                public:
                    using value_type = T;
                    using reference = T &;
                    using pointer = T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;

                    InOrderIterator() : node_ptr_(nullptr) {}
                    explicit InOrderIterator(NodeType *p) : node_ptr_(p) {}

                    reference operator*() const { return node_ptr_->value(); }
                    pointer operator->() const { return &(node_ptr_->value()); }

                    InOrderIterator &operator++()
                    {
                        node_ptr_ = successor<NodeType>(node_ptr_);
                        return *this;
                    }
                    InOrderIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    InOrderIterator &operator--()
                    {
                        node_ptr_ = predecessor<NodeType>(node_ptr_);
                        return *this;
                    }
                    InOrderIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const InOrderIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const InOrderIterator &other) const { return !(*this == other); }

                    // Allow conversion to const_iterator
                    friend class ConstInOrderIterator<T, NodeType>;
                    // NodeType *get_node() const { return node_ptr_; } // Needed for const_iterator conversion

                protected:
                    NodeType *node_ptr_;

                    friend struct BinaryTreeLinked<T, NodeType>;
                };

                template <typename T, typename NodeType>
                class ConstInOrderIterator
                {
                public:
                    using value_type = T;
                    using reference = const T &;
                    using pointer = const T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;

                    ConstInOrderIterator() : node_ptr_(nullptr) {}
                    explicit ConstInOrderIterator(NodeType *p) : node_ptr_(p) {}
                    ConstInOrderIterator(const InOrderIterator<T, NodeType> &other) : node_ptr_(other.node_ptr_) {}

                    reference operator*() const { return node_ptr_->value(); }
                    pointer operator->() const { return &(node_ptr_->value()); }

                    ConstInOrderIterator &operator++()
                    {
                        node_ptr_ = successor<NodeType>(node_ptr_);
                        return *this;
                    }
                    ConstInOrderIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ConstInOrderIterator &operator--()
                    {
                        node_ptr_ = predecessor<NodeType>(node_ptr_);
                        return *this;
                    }
                    ConstInOrderIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const ConstInOrderIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const ConstInOrderIterator &other) const { return !(*this == other); }

                protected:
                    NodeType *node_ptr_;
                    friend struct BinaryTreeLinked<T, NodeType>;
                };

            }

            // 一个基础的、满足CRTP约束的节点类型
            template <typename T>
            struct BinaryTreeNodeLinkedBasic : BinaryTreeNodeLinked<T, BinaryTreeNodeLinkedBasic<T>>
            {
                using BinaryTreeNodeLinked<T, BinaryTreeNodeLinkedBasic<T>>::BinaryTreeNodeLinked;
                ~BinaryTreeNodeLinkedBasic() = default;
            };

            // 强调：为了减少重复代码，只使用非const的节点指针，并且保证节点指针不暴露
            using DSA::Utils::ICloneable;
            template <typename T, typename NodeType = BinaryTreeNodeLinkedBasic<T>>
            struct BinaryTreeLinked : ICloneable
            {

                static_assert(
                    is_binary_tree_node_linked_T<T, NodeType>::value,
                    "BinaryTreeLinked: NodeType must inherit from BinaryTreeNodeLinked<T, NodeType>");

                using Node = NodeType;
                using CNode = const Node;
                using value_type = T;
                using size_type = std::size_t;
                using pointer = value_type *;
                using const_pointer = const value_type *;
                using reference = value_type &;
                using const_reference = const value_type &;
                using difference_type = std::ptrdiff_t;

                using iterator = detail::InOrderIterator<T, NodeType>;
                using const_iterator = detail::ConstInOrderIterator<T, NodeType>;

                BinaryTreeLinked()
                {
                    header = createNodeInternal();
                    root() = nullptr;
                    leftmost() = header;
                    rightmost() = header;
                }
                void tree_copy_recursive(Node *&node, Node *parent, Node *fromp)
                {
                    if (!fromp)
                    {
                        node = nullptr;
                        return;
                    }
                    node = copyNodeInternal(*fromp);
                    node->parent = parent;
                    tree_copy_recursive(node->left(), node, fromp->left());
                    tree_copy_recursive(node->right(), node, fromp->right());
                }
                explicit BinaryTreeLinked(const BinaryTreeLinked &other) : BinaryTreeLinked()
                {
                    BinaryTreeLinked tmp;
                    tree_copy_recursive(tmp.root(), tmp.header, other.root());
                    this->swap(tmp);
                    if (root())
                    {
                        leftmost() = binary_leftmost(root());
                        rightmost() = binary_rightmost(root());
                    }
                }
                BinaryTreeLinked &operator=(const BinaryTreeLinked &other)
                {
                    BinaryTreeLinked tmp(other);
                    this->swap(tmp);
                    return *this;
                }
                bool empty() const { return root() == nullptr; }
                virtual void clear()
                {
                    destroyRecursive(root());
                    // 要重置所有状态指针
                    root() = nullptr;
                    leftmost() = header;
                    rightmost() = header;
                }
                virtual ~BinaryTreeLinked()
                {
                    clear();
                    destroyNodeInternal(end_ptr());
                }

                void preOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    preOrderRecursive(root(), f); // 递归实现
                }
                void inOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    inOrderRecursive(root(), f);
                }
                void postOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    postOrderRecursive(root(), f);
                }
                void levelOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    if (empty())
                        return;
                    std::queue<CNode *> q;
                    q.push(root());
                    while (!q.empty())
                    {
                        CNode *p = q.front();
                        q.pop();
                        f(p->value());
                        if (p->left())
                            q.push(p->left());
                        if (p->right())
                            q.push(p->right());
                    }
                }

                iterator begin() { return iterator(leftmost()); }
                const_iterator begin() const { return const_iterator(leftmost()); }
                const_iterator cbegin() const { return begin(); }
                iterator end() { return iterator(this->end_ptr()); }
                const_iterator end() const { return const_iterator(this->end_ptr()); }
                const_iterator cend() const { return end(); }
                virtual void swap(BinaryTreeLinked &other)
                {
                    using std::swap;
                    swap(header, other.header);
                }

                std::unique_ptr<BinaryTreeLinked> clone() const
                {
                    return std::unique_ptr<BinaryTreeLinked>(clone_unsafe());
                }

            protected:
                Node *header = nullptr; // 哨兵头节点，其三个指针被用来存储树的元信息

                // root 是 header 的左孩子。
                Node *&root() { return end_ptr()->left(); }
                Node *root() const { return end_ptr()->left(); }

                // end_ptr() 只是 header 的别名，方便理解。
                Node *end_ptr() const { return header; }
                Node *begin_ptr() const { return leftmost(); }

                Node *release(Node *new_root = nullptr)
                {
                    Node *old_root = root();
                    if (!new_root)
                    {
                        root() = nullptr;
                        leftmost() = rightmost() = end_ptr();
                    }
                    else
                    {
                        root() = new_root;
                        root()->parent = end_ptr();
                        leftmost() = binary_leftmost(root());
                        rightmost() = binary_rightmost(root());
                    }
                    return old_root;
                }
                // leftmost 是 header 的右孩子。
                Node *&leftmost() { return end_ptr()->right(); }
                // rightmost 是 header 的parent节点。
                // 这里单纯是只剩下parent节点可以用，目前功能不依赖 rightmost ，可以选择不实现这个部分
                Node *&rightmost() { return end_ptr()->parent; }
                Node *leftmost() const { return end_ptr()->right(); }
                Node *rightmost() const { return end_ptr()->parent; }
                void destroyRecursive(Node *p)
                {
                    if (p)
                    {
                        destroyRecursive(p->left());
                        destroyRecursive(p->right());
                        destroyNodeInternal(p);
                    }
                }
                void preOrderRecursive(CNode *p, const std::function<void(const T &)> &f) const
                {
                    if (p)
                    {
                        f(p->value());
                        preOrderRecursive(p->left(), f);
                        preOrderRecursive(p->right(), f);
                    }
                }
                void inOrderRecursive(CNode *p, const std::function<void(const T &)> &f) const
                {
                    if (p)
                    {
                        inOrderRecursive(p->left(), f);
                        f(p->value());
                        inOrderRecursive(p->right(), f);
                    }
                }
                void postOrderRecursive(CNode *p, const std::function<void(const T &)> &f) const
                {
                    if (p)
                    {
                        postOrderRecursive(p->left(), f);
                        postOrderRecursive(p->right(), f);
                        f(p->value());
                    }
                }
                virtual void destroyNodeInternal(Node *p)
                {
                    delete p;
                }
                virtual Node *createNodeInternal()
                {
                    return new Node{};
                }
                virtual Node *createNodeInternal(const T &v)
                {
                    return new typename Node::VNode{v};
                }
                // header shall not be copied
                virtual Node *copyNodeInternal(const Node &node)
                {

                    using VNode = typename Node::VNode;
                    return new VNode(static_cast<const VNode &>(node));
                }

                static void link_to(const bool link_right, Node *node, Node *p)
                {
                    node && (node->parent = p);
                    p->children[link_right] = node;
                }
                void link_root(Node *p)
                {
                    root() = p;
                    p->parent = end_ptr();
                }
                void solo_root(Node *p)
                {
                    link_root(p);
                    leftmost() = rightmost() = p;
                }
                void remove_root()
                {
                    root() = nullptr;
                    leftmost() = rightmost() = end_ptr();
                }
                bool only_root()
                {
                    return leftmost() == root() && rightmost() == root();
                }

                // 用于交换两个节点在树中的拓扑位置，而不是交换它们的值。
                // 这样做是为了在不使迭代器失效（仍然指向同一个节点对象）的情况下，改变树的逻辑顺序。
                // 这是实现简化子类 erase 逻辑的核心。
                /*
                    to keep iterator consistent, instead of swapping the node value, we swap the tree info
                */
                void tree_swap(Node *x, Node *y)
                {
                    if (x == y)
                        return;
                    bool xright = is_right(x);
                    bool yright = is_right(y);
                    bool link_right;
                    // case 1: x 和 y 是直接亲子关系
                    if (x->parent == y || y->parent == x)
                    {
                        // 注意swap_node需要放在if分支里
                        if (x->parent == y)
                        {
                            swap_node(*x, *y); // 会导致临时的自引用，下一句立刻修复
                            y->parent = x;
                            x->children[xright] = y;
                        }
                        else
                        {
                            swap_node(*x, *y); // 会导致临时的自引用，下一句立刻修复
                            x->parent = y;
                            y->children[yright] = x;
                        }
                    }
                    else
                    {
                        // case 2: x 和 y 没有直接的亲子关系
                        swap_node(*x, *y);
                    }

                    // 更新 parent 和 x,y 的关系
                    if (x == root() || y == root())
                    {
                        if (x == root())
                        {
                            link_root(y);
                            link_to(link_right = yright, x, x->parent);
                        }
                        else
                        {
                            link_root(x);
                            link_to(link_right = xright, y, y->parent);
                        }
                    }
                    else
                    {
                        link_to(link_right = yright, x, x->parent);
                        link_to(link_right = xright, y, y->parent);
                    }
                    // 更新 x,y 和子节点的的关系
                    link_to(link_right = false, x->left(), x);
                    link_to(link_right = true, x->right(), x);
                    link_to(link_right = false, y->left(), y);
                    link_to(link_right = true, y->right(), y);

                    // 如果交换的节点是 leftmost 或 rightmost，则更新 header 中的缓存指针
                    if (x == leftmost() || y == leftmost())
                    {
                        if (x == leftmost())
                        {
                            leftmost() = y;
                        }
                        else
                        {
                            leftmost() = x;
                        }
                    }
                    if (x == rightmost() || y == rightmost())
                    {
                        if (x == rightmost())
                        {
                            rightmost() = y;
                        }
                        else
                        {
                            rightmost() = x;
                        }
                    }
                }
                static Node *prev(Node *p)
                {
                    return detail::predecessor<Node>(p);
                }
                static Node *next(Node *p)
                {
                    return detail::successor<Node>(p);
                }
                static bool is_right(Node *p)
                {
                    return p == p->parent->right();
                }
                static Node *getNode(const_iterator iter)
                {
                    return const_cast<Node *>(iter.node_ptr_);
                }
                BinaryTreeLinked *clone_unsafe() const override
                {
                    return new BinaryTreeLinked(*this);
                }
            };

        }
    }
}
