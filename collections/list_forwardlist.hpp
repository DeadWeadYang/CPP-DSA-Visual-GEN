#pragma once
#include <memory>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <list>
#include <string>
#include <iostream>
#include <exception>
#include <sstream>
namespace DSA
{
    namespace Collections
    {
        namespace ListOrForwardlist
        {
            namespace detail
            {
                template <typename T>
                struct ListBase;

                template <typename T>
                struct ListNode
                {
                    // T value;
                    ListNode *n_next = nullptr;
                    ListNode *n_prev = nullptr;

                    template <typename U>
                    struct ValueNodeImpl : ListNode<U>
                    {
                        U value_;
                        ValueNodeImpl(const U &v) : ListNode(), value_(v) {}
                    };
                    using VNode = ValueNodeImpl<T>;
                    T &value()
                    {
                        return static_cast<VNode &>(static_cast<ListNode &>(*this)).value_;
                    }
                    const T &value() const
                    {
                        return static_cast<const VNode &>(static_cast<const ListNode &>(*this)).value_;
                    }
                    ListNode() = default;
                    virtual ~ListNode() = default;
                    // 将当前节点（this）链接到节点p的前面
                    void link(ListNode *p)
                    {
                        this->n_next = p;
                        this->n_prev = p->n_prev;
                        p->n_prev->n_next = this;
                        p->n_prev = this;
                    }
                    // 将当前节点（this）链接到节点p的后面
                    void link_back(ListNode *p)
                    {
                        this->n_prev = p;
                        this->n_next = p->n_next;
                        p->n_next->n_prev = this;
                        p->n_next = this;
                    }
                    // 将当前节点从链表中解开
                    void unlink()
                    {
                        this->n_prev->n_next = this->n_next;
                        this->n_next->n_prev = this->n_prev;
                    }
                };

                template <typename T>
                class ConstListIterator;

                template <typename T>
                class ListIterator
                {

                public:
                    using value_type = T;
                    using reference = T &;
                    using pointer = T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;
                    using Node = ListNode<T>;

                    ListIterator() : node_ptr_(nullptr) {}
                    explicit ListIterator(Node *p) : node_ptr_(p) {}

                    reference operator*() const { return node_ptr_->value(); }
                    pointer operator->() const { return &node_ptr_->value(); }

                    ListIterator &operator++()
                    {
                        node_ptr_ = node_ptr_->n_next;
                        return *this;
                    }
                    ListIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ListIterator &operator--()
                    {
                        node_ptr_ = node_ptr_->n_prev;
                        return *this;
                    }
                    ListIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const ListIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const ListIterator &other) const { return !(*this == other); }

                    // Allow conversion to const_iterator
                    friend class ConstListIterator<T>;
                    // ListNode *get_node() const { return node_ptr_; } // Needed for const_iterator conversion

                protected:
                    Node *node_ptr_;

                    friend struct ListBase<T>;
                };

                template <typename T>
                class ConstListIterator
                {
                public:
                    using value_type = T;
                    using reference = const T &;
                    using pointer = const T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;
                    using Node = ListNode<T>;

                    ConstListIterator() : node_ptr_(nullptr) {}
                    explicit ConstListIterator(const Node *p) : node_ptr_(p) {}
                    ConstListIterator(const ListIterator<T> &other) : node_ptr_(other.node_ptr_) {}

                    reference operator*() const { return node_ptr_->value(); }
                    pointer operator->() const { return &node_ptr_->value(); }

                    ConstListIterator &operator++()
                    {
                        node_ptr_ = node_ptr_->n_next;
                        return *this;
                    }
                    ConstListIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ConstListIterator &operator--()
                    {
                        node_ptr_ = node_ptr_->n_prev;
                        return *this;
                    }
                    ConstListIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const ConstListIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const ConstListIterator &other) const { return !(*this == other); }

                protected:
                    const Node *node_ptr_;
                    friend struct ListBase<T>;
                };
                // ListBase 封装了底层的节点管理和指针操作，与上层的 List 类实现关注点分离。
                template <typename T>
                struct ListBase
                {

                    using value_type = T;
                    using reference = T &;
                    using const_reference = const T &;
                    using pointer = T *;
                    using const_pointer = const T *;
                    using iterator = ListIterator<T>;
                    using const_iterator = ConstListIterator<T>;
                    using size_type = size_t;
                    using difference_type = std::ptrdiff_t;
                    using Node = ListNode<T>;
                    ListBase()
                    {
                        // 使用一个哨兵节点（header）这个节点不存储实际数据，它的存在简化了边界条件的处理。
                        // 例如，插入和删除操作无需区分是在链表头部、尾部还是中部。
                        header = createNodeInternal();
                        // 初始时，一个空链表的 header 节点的 next 和 prev 都指向自身，形成一个环。
                        header->n_prev = header->n_next = header;
                    }
                    void clear()
                    {
                        if (begin_ptr() != end_ptr())
                        {
                            destroyNodeInternal(begin_ptr(), end_ptr());
                            // 清空后，重置 header 节点的指针，恢复到空链表状态。
                            header->n_prev = header->n_next = header;
                        }
                    }
                    // RAII：析构时自动清理所有节点内存。
                    ~ListBase()
                    {
                        clear();
                        destroyNodeInternal(end_ptr()); // 最后销毁 header 节点自身。
                    }

                protected:
                    Node *header;
                    // begin_ptr() 指向第一个实际数据节点。
                    Node *&begin_ptr() { return header->n_next; }
                    Node *begin_ptr() const { return header->n_next; }
                    // end_ptr() 始终指向 header 节点。
                    Node *end_ptr() const { return header; }
                    Node *createNodeInternal()
                    {
                        return new Node{};
                    }
                    Node *createNodeInternal(const T &v)
                    {
                        return new typename Node::VNode{v};
                    }
                    void destroyNodeInternal(Node *p) { delete p; }
                    void destroyNodeInternal(Node *first, Node *last)
                    {
                        for (Node *p = first; first != last; p = first)
                        {
                            first = first->n_next;
                            destroyNodeInternal(p);
                        }
                    }
                    // 将 [first, prelast] 所代表的节点范围，链接到 pos 节点之前。
                    // 这是一个核心的 O(1) 操作，是 splice 的基础。
                    // note prelast=prev(last),last=next(prelast) ,that is ,[first,prelast]=[first,last)
                    static void link_range(Node *pos, Node *first, Node *prelast)
                    {
                        first->n_prev = pos->n_prev;
                        prelast->n_next = pos;
                        pos->n_prev->n_next = first;
                        pos->n_prev = prelast;
                    }
                    // 将 [first, prelast] 所代表的节点范围，链接到 pos 节点之后。
                    static void link_range_back(Node *pos, Node *first, Node *prelast)
                    {
                        first->n_prev = pos;
                        prelast->n_next = pos->n_next;
                        pos->n_next->n_prev = prelast;
                        pos->n_next = first;
                    }
                    // 将 [first, prelast] 所代表的节点范围从链表中解开。
                    // 这同样是 O(1) 操作。
                    static void unlink_range(Node *first, Node *prelast)
                    {
                        first->n_prev->n_next = prelast->n_next;
                        prelast->n_next->n_prev = first->n_prev;
                    }
                    // 辅助函数，用于从 const_iterator 中安全地获取底层的非 const 节点指针。
                    // 这是必需的，因为像 insert, erase 等修改操作通常以 const_iterator 作为参数。
                    static Node *getNode(const_iterator iter)
                    {
                        return const_cast<Node *>(iter.node_ptr_);
                    }
                };
            }
            // List 类继承自 ListBase，负责提供用户接口、管理大小和实现复杂算法。
            template <typename T>
            struct List final
                : detail::ListBase<T>
            { // types:
                using Base = detail::ListBase<T>;
                using value_type = Base::value_type;
                using reference = Base::reference;
                using const_reference = Base::const_reference;
                using pointer = Base::pointer;
                using const_pointer = Base::const_pointer;
                using iterator = Base::iterator;
                using const_iterator = Base::const_iterator;
                using size_type = Base::size_type;
                using difference_type = Base::difference_type;
                using reverse_iterator = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;
                using Node = Base::Node;
                List() = default;
                ~List() = default;
                explicit List(const List &other)
                    : detail::ListBase<T>()
                {
                    for (const T &it : other)
                        push_back(it);
                }
                explicit List(List &&other)
                    : detail::ListBase<T>()
                {
                    this->swap(other);
                }
                // 拷贝赋值运算符，采用经典的“拷贝并交换”（Copy-and-Swap）模式，强异常安全。
                List &operator=(const List &other)
                {
                    List tmp(other);
                    this->swap(tmp);
                    return *this;
                }
                List &operator=(List &&other)
                {
                    this->swap(other);
                    return *this;
                }
                explicit List(size_type n, const T &v = T{})
                    : detail::ListBase<T>()
                {
                    for (; n > 0; --n)
                        push_back(v);
                }
                template <typename InputIt>
                explicit List(InputIt first, InputIt last)
                    : detail::ListBase<T>()
                {
                    for (; first != last; ++first)
                        push_back(*first);
                }
                void assign(size_type n, const T &v)
                {
                    List tmp(n, v);
                    this->swap(tmp);
                }
                template <typename InputIt>
                void assign(InputIt first, InputIt last)
                {
                    List tmp(first, last);
                    this->swap(tmp);
                }

                iterator begin() { return iterator{begin_ptr()}; }
                const_iterator begin() const { return const_iterator{begin_ptr()}; }
                iterator end() { return iterator{end_ptr()}; }
                const_iterator end() const { return const_iterator{end_ptr()}; }
                reverse_iterator rbegin() { return reverse_iterator{end()}; }
                const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
                reverse_iterator rend() { return reverse_iterator{begin()}; }
                const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
                const_iterator cbegin() const { return begin(); }
                const_iterator cend() const { return end(); };
                const_reverse_iterator crbegin() const { return rbegin(); };
                const_reverse_iterator crend() const { return rend(); };

                bool empty() const { return begin_ptr() == end_ptr(); }
                void clear()
                {
                    size_r = 0;
                    Base::clear();
                }
                size_type size() const { return size_r; }
                reference front() { return begin_ptr()->value(); }
                const_reference front() const { return begin_ptr()->value(); }
                reference back() { return end_ptr()->n_prev->value(); }
                const_reference back() const { return end_ptr()->n_prev->value(); }

                iterator insert(const_iterator pos, const T &v)
                {
                    return iterator{insert_pointer(getNode(pos), v)};
                }
                // 同样是“拷贝并交换”（Copy-and-Swap）模式，强异常安全
                iterator insert(const_iterator pos, size_type n, const T &v)
                {
                    if (n)
                    {
                        List tmp(n, v);
                        iterator res = tmp.begin();
                        this->splice(pos, tmp);
                        return res;
                    }
                    return pos;
                }
                template <typename InputIt>
                iterator insert(const_iterator pos, InputIt first, InputIt last)
                {
                    if (first != last)
                    {
                        List tmp(first, last);
                        iterator res = tmp.begin();
                        this->splice(pos, tmp);
                        return res;
                    }
                    return pos;
                }
                iterator erase(const_iterator pos) { return iterator{erase_pointer(getNode(pos))}; }
                iterator erase(const_iterator first, const_iterator last)
                {
                    if (first != last)
                        erase_pointer(getNode(first), getNode(std::prev(last)));
                    return iterator{getNode(last)};
                }
                void push_back(const T &v) { insert_pointer(end_ptr(), v); }
                void pop_back() { erase_pointer(end_ptr()->n_prev); }
                void push_front(const T &v) { insert_pointer(begin_ptr(), v); }
                void pop_front() { erase_pointer(end_ptr()->n_next); }
                void splice(const_iterator pos, List &other)
                {
                    splice_pointer(getNode(pos), other);
                }
                void splice(const_iterator pos, List &&other) { splice(pos, other); }
                void splice(const_iterator pos, List &other, const_iterator it)
                {
                    splice_pointer(getNode(pos), other, getNode(it));
                }
                // pos 不能在 [first, last) 范围内。违反此前提将导致未定义行为。
                void splice(const_iterator pos, List &other, const_iterator first, const_iterator last)
                {
                    if (first != last)
                    {
                        if (this != std::addressof(other))
                            // 从other拼接，需要调用O(|[first,last)|)的重载
                            splice_range(pos, first, std::prev(last), this->size_r, other.size_r);
                        else
                            // 当从自身拼接时，不需要更新两个不同 list 的大小,因此可以调用O(1)的重载
                            splice_range(pos, first, std::prev(last), 0);
                    }
                }
                void resize(size_type new_size)
                {
                    while (new_size < size_r)
                        pop_back();
                    if (new_size > size_r)
                        insert(end(), new_size - size_r, T{});
                }
                // swap 实现为 O(1) 操作，仅交换 header 指针和 size 成员。
                void swap(List &other)
                {
                    using std::swap;
                    swap(this->header, other.header);
                    swap(this->size_r, other.size_r);
                }
                template <typename Compare>
                void merge(List &other, Compare comp)
                {
                    if (this != std::addressof(other) && !other.empty())
                    {
                        if (this->empty())
                            this->swap(other);
                        else
                            merge_range(this->begin(), std::prev(this->end()), other.begin(), std::prev(other.end()), this->size_r, other.size_r);
                    }
                }
                template <typename Compare>
                void merge(List &&other, Compare comp) { merge(other, comp); }

                void merge(List &other) { merge(other, std::less<T>{}); }
                void merge(List &&other) { merge(other); }

                // 找出所有满足条件的连续区间，
                // 然后使用 splice 将这些区间移动到一个临时的 to_remove 链表中。
                // 最后 to_remove 析构时，所有被移除的节点被一次性销毁。
                size_type remove(const T &v)
                {
                    List to_remove{};
                    for (iterator i = begin(), ed = end(); i != ed;)
                    {
                        if (*i == v)
                        {
                            iterator j = std::next(i);
                            for (; j != ed && *j == v; ++j)
                                ;
                            to_remove.splice(to_remove.end(), *this, i, j);
                            i = (j == ed) ? j : ++j;
                        }
                        else
                            ++i;
                    }
                    return to_remove.size();
                }
                // 确保谓词 pred 对每个元素按顺序只调用一次。
                template <typename UnaryPredicate>
                size_type remove_if(UnaryPredicate pred)
                {

                    List to_remove{};
                    // 外层循环负责遍历。增量操作在循环体内手动控制。
                    for (iterator i = begin(), ed = end(); i != ed;)
                    {
                        // 1. 对当前元素 i 调用 pred。
                        if (pred(*i))
                        {
                            // 2. 如果 pred(*i) 为 true，启动内层循环（作为工具）向前探查，
                            //    找到所有连续满足 pred 的元素。
                            iterator j = std::next(i);
                            // 3. 内层循环会消耗性地调用 pred，并更新 pred 的内部状态（如果有）
                            for (; j != ed && pred(*j); ++j)
                                ;
                            // 4. 将找到的整个范围 [i, j) 使用 splice 批量移除。
                            to_remove.splice(to_remove.end(), *this, i, j);

                            // 5. 关键的迭代器更新：
                            //    此时，我们已经通过内层循环得知 pred(*j) 为 false（或 j==ed）。
                            //    因此，外层循环无需再对 j 进行检查。
                            //    我们直接跳过 j，从它的下一个元素开始下一轮检查。
                            //    这保证了 pred 在每个元素上只调用一次，对于有状态的谓词是正确且高效的。
                            i = (j == ed) ? j : ++j;
                        }
                        else
                            ++i; // 6. 如果 pred(*i) 为 false，正常前进。
                    }
                    return to_remove.size();
                }
                void reverse()
                {
                    Node *p = begin_ptr();
                    while (p != end_ptr())
                    {
                        // 1. 交换当前节点的 next 和 prev 指针。
                        std::swap(p->n_next, p->n_prev);
                        // 2. 移动到原始的下一个节点。因为指针已交换，原始的 next 现在是 prev。
                        p = p->n_prev;
                    }
                    // 3. 最后，交换 header 节点的指针，完成整个反转。
                    std::swap(p->n_next, p->n_prev);
                }
                template <typename BinaryPredicate>
                size_type unique(BinaryPredicate pred)
                {

                    List to_remove{};
                    for (iterator i = begin(), ed = end(); i != ed;)
                    {
                        iterator j = std::next(i);
                        for (; j != ed && pred(*i, *j); ++j)
                            ;
                        if ((++i) != j)
                        {
                            to_remove.splice(to_remove.end(), *this, i, j);
                            i = j;
                        }
                    }
                    return to_remove.size();
                }
                size_type unique()
                {
                    return unique(std::equal_to<T>{});
                }
                // 采用非递归的自底向上归并排序。是链表排序的最高效算法之一。
                template <typename Compare>
                void sort(Compare comp)
                {
                    // 1. 准备一个大小为机器字长位数的数组，用于存放已排序的子链表。
                    //    sorted_ranges[i] 理论上存放长度为 2^i 的子链表。
                    constexpr int size_t_bits = std::numeric_limits<size_type>::digits;
                    std::pair<iterator, iterator> sorted_ranges[size_t_bits], carry, empty_range; // ranges all in form [first,prelast]
                    // 2. 遍历整个列表，将每个元素视为一个长度为1的已排序子链表（carry）。
                    for (iterator it = this->begin(), ed = this->end(); it != ed;)
                    {
                        carry.first = carry.second = it, ++it;
                        // 3. 尝试将 carry 合并到 sorted_ranges 中。
                        for (int i = 0; i < size_t_bits; ++i)
                        {
                            // 3a. 如果槽位为空，直接放入并结束内层循环。
                            if (sorted_ranges[i] == empty_range)
                            {
                                std::swap(sorted_ranges[i], carry);
                                break;
                            }
                            // 3b. 如果槽位不为空，将槽内的子链表与 carry 合并。
                            iterator first_anchor = std::prev(sorted_ranges[i].first);
                            merge_range(sorted_ranges[i].first, sorted_ranges[i].second, carry.first, carry.second);
                            // 合并后的结果成为新的 carry，继续向上层槽位尝试。
                            carry.first = std::next(first_anchor);
                            carry.second = std::prev(it);
                            sorted_ranges[i] = empty_range; // 清空当前槽位。
                        }
                    }
                    // 4. 合并 sorted_ranges 中所有剩余的子链表，直到只剩一个。
                    for (int i = 0; i < size_t_bits; ++i)
                        if (sorted_ranges[i] != empty_range)
                        {
                            if (carry == empty_range)
                            {
                                std::swap(sorted_ranges[i], carry);
                                continue;
                            }

                            iterator first_anchor = std::prev(sorted_ranges[i].first);
                            merge_range(sorted_ranges[i].first, sorted_ranges[i].second, carry.first, carry.second);
                            carry.first = std::next(first_anchor);
                            carry.second = std::prev(this->end());
                            sorted_ranges[i] = empty_range;
                        }
                    return;
                }
                void sort()
                {
                    sort(std::less<T>{});
                }

            protected:
                size_type size_r = 0;
                using Base::begin_ptr;
                using Base::createNodeInternal;
                using Base::destroyNodeInternal;
                using Base::end_ptr;
                using Base::getNode;
                using Base::link_range;
                using Base::link_range_back;
                using Base::unlink_range;

                Node *insert_pointer(Node *pos, const T &v)
                {
                    Node *node = createNodeInternal(v);
                    node->link(pos);
                    ++size_r;
                    return node;
                }
                Node *erase_pointer(Node *pos)
                {
                    Node *res = pos->n_next;
                    pos->unlink();
                    destroyNodeInternal(pos);
                    --size_r;
                    return res;
                }
                Node *erase_pointer(Node *first, Node *prelast)
                {
                    Node *res = prelast->n_next;
                    unlink_range(first, prelast);
                    for (Node *p = first; p != res; p = first)
                    {
                        first = first->n_next;
                        destroyNodeInternal(p);
                        --size_r;
                    }
                }
                void splice_pointer(Node *pos, List &other)
                {
                    if (this != std::addressof(other) && !other.empty())
                    {
                        Node *first = other.begin_ptr();
                        Node *prelast = other.end_ptr()->n_prev;
                        unlink_range(first, prelast);
                        link_range(pos, first, prelast);
                        this->size_r += other.size_r;
                        other.size_r = 0;
                    }
                }
                void splice_pointer(Node *pos, List &other, Node *it)
                {
                    if (pos != it && pos != it->n_prev && it != other.end_ptr())
                    {
                        it->unlink();
                        it->link(pos);
                        --other.size_r;
                        ++this->size_r;
                    }
                }
                // O(1)的重载
                static void splice_range(const_iterator pos, const_iterator first, const_iterator prelast, size_type &size_change1, size_type &size_change2, size_type change_size)
                {
                    Node *n_first = getNode(first);
                    Node *n_prelast = getNode(prelast);
                    size_change1 += change_size;
                    size_change2 -= change_size;
                    unlink_range(n_first, n_prelast);
                    link_range(getNode(pos), n_first, n_prelast);
                }
                // O(|[first,prelast]|)的重载
                //  note: pos shall not be in [first,prelast]
                static void splice_range(const_iterator pos, const_iterator first, const_iterator prelast, size_type &size_change1, size_type &size_change2)
                {
                    splice_range(pos, first, prelast, size_change1, size_change2, std::distance(first, prelast) + 1);
                }
                static size_type splice_range(const_iterator pos, const_iterator first, const_iterator prelast)
                {
                    size_type res = 0, tmp;
                    splice_range(pos, first, prelast, res, tmp);
                    return res;
                }
                static size_type splice_range(const_iterator pos, const_iterator first, const_iterator prelast, size_type change_size)
                {
                    size_type res = 0, tmp;
                    splice_range(pos, first, prelast, res, tmp, change_size);
                    return res;
                }
                static void splice_range_back(const_iterator pos, const_iterator first, const_iterator prelast, size_type &size_change1, size_type &size_change2, size_type change_size)
                {
                    Node *n_first = getNode(first);
                    Node *n_prelast = getNode(prelast);
                    size_change1 += change_size;
                    size_change2 -= change_size;
                    unlink_range(n_first, n_prelast);
                    link_range_back(getNode(pos), n_first, n_prelast);
                }
                static void splice_range_back(const_iterator pos, const_iterator first, const_iterator prelast, size_type &size_change1, size_type &size_change2)
                {
                    splice_range_back(pos, first, prelast, size_change1, size_change2, std::distance(first, prelast) + 1);
                }
                static size_type splice_range_back(const_iterator pos, const_iterator first, const_iterator prelast)
                {
                    size_type res = 0, tmp;
                    splice_range_back(pos, first, prelast, res, tmp);
                    return res;
                }
                static size_type splice_range_back(const_iterator pos, const_iterator first, const_iterator prelast, size_type change_size)
                {
                    size_type res = 0, tmp;
                    splice_range_back(pos, first, prelast, res, tmp, change_size);
                    return res;
                }

                // merge 辅助函数，是 sort 的核心。
                // 它会尝试将 [first2, prelast2] 合并到 [first1, prelast1] 中。
                // 假设两个范围都已排序。
                // try merge [first2,prelast2] into [first1,prelast1]
                // note: this function assum that both ranges are ordered and no overlap
                template <typename Compare>
                static void merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2, size_type &size_change1, size_type &size_change2, Compare comp)
                {
                    while (true)
                    {
                        // 1. 检查 range2 的头部是否小于 range1 的头部。
                        if (comp(*first2, *first1))
                        {
                            // 2. 如果是，则向前看，找到 range2 中所有小于 *first1 的连续元素。
                            size_type change_size = 1;
                            const_iterator pos2 = first2;
                            const_iterator peek2 = std::next(pos2);
                            while (pos2 != prelast2 && comp(*peek2, *first1))
                            {
                                pos2 = peek2;
                                ++peek2;
                                ++change_size;
                            };
                            size_change1 += change_size;
                            size_change2 -= change_size;
                            Node *first = getNode(first2);
                            Node *prelast = getNode(pos2);
                            // 3. 将这个连续的、更小的子范围，通过一次 link 操作，整体移动到 first1 之前。
                            unlink_range(first, prelast);
                            link_range(getNode(first1), first, prelast);
                            if (pos2 == prelast2)
                                return; // range2 已全部合并完毕。
                            first2 = peek2;
                        }
                        // 4. 如果 range1 已检查到末尾，将 range2 剩余部分全部拼接到后面。
                        if (first1 == prelast1)
                        {
                            splice_range_back(prelast1, first2, prelast2, size_change1, size_change2);
                            return;
                        }
                        // 5. 移动 first1，继续下一轮比较。
                        ++first1;
                    }
                }
                static void merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2, size_type &size_change1, size_type &size_change2)
                {
                    merge_range(first1, prelast1, first2, prelast2, size_change1, size_change2, std::less<T>{});
                }

                template <typename Compare>
                static size_type merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2, Compare comp)
                {
                    size_type res = 0, tmp;
                    merge_range(first1, prelast1, first2, prelast2, res, tmp, comp);
                    return res;
                }
                static size_type merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2)
                {
                    return merge_range(first1, prelast1, first2, prelast2, std::less<T>{});
                }
            };

        }
    }
}