#pragma once
#include <memory>
#include <exception>
#include <string>
#include <sstream>
namespace DSA
{
    namespace Collections
    {
        namespace ArrayLike
        {
            namespace detail
            {
                // 前向声明，因为 VectorBase 和 迭代器 相互引用
                template <typename T>
                struct VectorBase;

                template <typename T>
                class ConstArrayLikeIterator;

                /**
                 * @brief 可变的随机访问迭代器。
                 *
                 * 这是一个符合C++标准库要求的随机访问迭代器，底层用原生指针实现。
                 * 它提供了所有必要的运算符重载，使其能与STL算法无缝协作。
                 */
                template <typename T>
                class ArrayLikeIterator
                {

                public:
                    using value_type = T;
                    using reference = T &;
                    using pointer = T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::random_access_iterator_tag;

                    ArrayLikeIterator() : it_ptr_(nullptr) {}
                    explicit ArrayLikeIterator(pointer p) : it_ptr_(p) {}

                    reference operator*() const { return *it_ptr_; }
                    pointer operator->() const { return it_ptr_; }

                    ArrayLikeIterator &operator++()
                    {
                        ++it_ptr_;
                        return *(this);
                    }
                    ArrayLikeIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++it_ptr_;
                        return tmp;
                    }
                    ArrayLikeIterator &operator--()
                    {
                        --it_ptr_;
                        return *this;
                    }
                    ArrayLikeIterator operator--(int)
                    {
                        auto tmp = *this;
                        --it_ptr_;
                        return tmp;
                    }

                    // --- 随机访问算术运算 ---
                    ArrayLikeIterator &operator+=(difference_type n)
                    {
                        it_ptr_ += n;
                        return *this;
                    }
                    friend ArrayLikeIterator operator+(ArrayLikeIterator it, difference_type n)
                    {
                        it += n;
                        return it;
                    }
                    ArrayLikeIterator &operator-=(difference_type n)
                    {
                        it_ptr_ -= n;
                        return *this;
                    }

                    // 计算两个迭代器之间的距离
                    friend ArrayLikeIterator operator-(ArrayLikeIterator it, difference_type n)
                    {
                        it -= n;
                        return it;
                    }
                    friend difference_type operator-(const ArrayLikeIterator &lhs, const ArrayLikeIterator &rhs) { return lhs.it_ptr_ - rhs.it_ptr_; }
                    bool operator<(const ArrayLikeIterator &other) const { return it_ptr_ < other.it_ptr_; }
                    bool operator<=(const ArrayLikeIterator &other) const { return it_ptr_ <= other.it_ptr_; }
                    bool operator>(const ArrayLikeIterator &other) const { return it_ptr_ > other.it_ptr_; }
                    bool operator>=(const ArrayLikeIterator &other) const { return it_ptr_ >= other.it_ptr_; }
                    bool operator==(const ArrayLikeIterator &other) const { return it_ptr_ == other.it_ptr_; }
                    bool operator!=(const ArrayLikeIterator &other) const { return it_ptr_ != other.it_ptr_; }

                    // Allow conversion to const_iterator
                    friend class ConstArrayLikeIterator<T>;

                protected:
                    pointer it_ptr_;
                };

                /**
                 * @brief 常量（只读）的随机访问迭代器。
                 *
                 * 功能与 ArrayLikeIterator 类似，但提供对元素的只读访问。
                 */
                template <typename T>
                class ConstArrayLikeIterator
                {
                public:
                    using value_type = T;
                    using reference = const T &;
                    using pointer = const T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::random_access_iterator_tag;

                    ConstArrayLikeIterator() : it_ptr_(nullptr) {}
                    explicit ConstArrayLikeIterator(pointer p) : it_ptr_(p) {}

                    // 允许从可变迭代器隐式转换为常量迭代器，这是标准容器的常见行为
                    ConstArrayLikeIterator(const ArrayLikeIterator<T> &other) : it_ptr_(other.it_ptr_) {}

                    reference operator*() const { return *it_ptr_; }
                    pointer operator->() const { return it_ptr_; }

                    ConstArrayLikeIterator &operator++()
                    {
                        ++it_ptr_;
                        ;
                        return *this;
                    }
                    ConstArrayLikeIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ConstArrayLikeIterator &operator--()
                    {
                        --it_ptr_;
                        return *this;
                    }
                    ConstArrayLikeIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    ConstArrayLikeIterator &operator+=(difference_type n)
                    {
                        it_ptr_ += n;
                        return *this;
                    }
                    friend ConstArrayLikeIterator operator+(ConstArrayLikeIterator it, difference_type n)
                    {
                        it += n;
                        return it;
                    }
                    ConstArrayLikeIterator &operator-=(difference_type n)
                    {
                        it_ptr_ -= n;
                        return *this;
                    }
                    friend ConstArrayLikeIterator operator-(ConstArrayLikeIterator it, difference_type n)
                    {
                        it -= n;
                        return it;
                    }
                    friend difference_type operator-(const ConstArrayLikeIterator &lhs, const ConstArrayLikeIterator &rhs) { return lhs.it_ptr_ - rhs.it_ptr_; }
                    bool operator<(const ConstArrayLikeIterator &other) const { return it_ptr_ < other.it_ptr_; }
                    bool operator<=(const ConstArrayLikeIterator &other) const { return it_ptr_ <= other.it_ptr_; }
                    bool operator>(const ConstArrayLikeIterator &other) const { return it_ptr_ > other.it_ptr_; }
                    bool operator>=(const ConstArrayLikeIterator &other) const { return it_ptr_ >= other.it_ptr_; }
                    bool operator==(const ConstArrayLikeIterator &other) const { return it_ptr_ == other.it_ptr_; }
                    bool operator!=(const ConstArrayLikeIterator &other) const { return it_ptr_ != other.it_ptr_; }

                protected:
                    pointer it_ptr_;
                };
                /**
                 * @brief Vector 的内存管理基类。
                 *
                 * 这个类的唯一职责是分配和释放原始内存（raw memory）。
                 * 它不关心内存中对象的构造和析构。这种关注点分离的设计使得代码更清晰。
                 * 它是不可拷贝的，但可移动，符合资源管理的“单一所有权”原则。
                 */
                template <typename T>
                struct VectorMemoryBase
                {

                    using value_type = T;
                    using reference = T &;
                    using const_reference = const T &;
                    using pointer = T *;
                    using const_pointer = const T *;
                    using size_type = size_t;
                    using difference_type = std::ptrdiff_t;
                    using allocator_type = std::allocator<T>;

                    VectorMemoryBase() = default;
                    explicit VectorMemoryBase(size_type n)
                    {
                        if (n)
                        {
                            p_start = alloc.allocate(n); // 分配n个T大小的未初始化内存
                            p_end_of_storage = p_start + n;
                        }
                    }
                    ~VectorMemoryBase()
                    {
                        if (p_start)
                        {
                            alloc.deallocate(p_start, p_end_of_storage - p_start); // 释放内存
                        }
                    }
                    // 删除拷贝构造和拷贝赋值，防止内存被重复释放
                    VectorMemoryBase(const VectorMemoryBase &) = delete;
                    VectorMemoryBase &operator=(const VectorMemoryBase &) = delete;
                    // 实现移动构造函数，用于高效地转移内存所有权
                    VectorMemoryBase(VectorMemoryBase &&other) noexcept
                        : p_start(other.p_start), p_end_of_storage(other.p_end_of_storage), alloc(std::move(other.alloc))
                    {
                        // 将源对象的指针置空，防止其析构函数释放已转移的内存
                        other.p_start = other.p_end_of_storage = nullptr;
                    }

                protected:
                    T *p_start = nullptr;          // 指向已分配内存的起始位置
                    T *p_end_of_storage = nullptr; // 指向已分配内存的末尾之后的位置
                    allocator_type alloc;
                };
                template <typename T>
                struct VectorGuard;
                /**
                 * @brief Vector 的对象生命周期管理基类。
                 *
                 * 继承自 VectorMemoryBase，这个类负责在已分配的原始内存上构造和析构对象。
                 * 它引入了 p_finish 指针来追踪已构造对象的末尾。
                 */
                template <typename T>
                struct VectorBase : VectorMemoryBase<T>
                {

                    using Base = VectorMemoryBase<T>;
                    using value_type = Base::value_type;
                    using reference = Base::reference;
                    using const_reference = Base::const_reference;
                    using pointer = Base::pointer;
                    using const_pointer = Base::const_pointer;
                    using size_type = Base::size_type;
                    using difference_type = Base::difference_type;
                    using allocator_type = Base::allocator_type;
                    using traits = std::allocator_traits<allocator_type>; // 使用 traits 来与分配器交互，这是现代C++的推荐做法
                    friend struct VectorGuard<T>;

                    VectorBase() { this->p_finish = this->p_start; }
                    explicit VectorBase(size_type n) : VectorMemoryBase<T>(n)
                    {
                        this->p_finish = this->p_start;
                    }
                    // 清空容器，调用所有元素的析构函数，但不释放内存
                    void clear()
                    {
                        while (this->p_finish != this->p_start)
                            destroy_at_end();
                    }
                    // 析构时确保所有已构造的对象都被销毁
                    ~VectorBase() { clear(); }
                    // 高效的交换操作，只交换指针和分配器状态
                    void swap(VectorBase &other) noexcept
                    {
                        std::swap(this->p_start, other.p_start);
                        std::swap(this->p_finish, other.p_finish);
                        std::swap(this->p_end_of_storage, other.p_end_of_storage);
                        // Allocator aware swap
                        if constexpr (traits::propagate_on_container_swap::value)
                        {
                            std::swap(this->alloc, other.alloc);
                        }
                    }

                protected:
                    T *p_finish = nullptr; // 指向最后一个已构造元素的末尾之后的位置

                    // --- 在 p_finish 位置构造或销毁元素的底层接口 ---
                    void construct_at_end(const T &v) { construct_at_end_impl(v); }
                    void construct_at_end(T &&v) { construct_at_end_impl(std::move(v)); }
                    void destroy_at_end()
                    {
                        --this->p_finish;
                        traits::destroy(this->alloc, p_finish);
                    }

                    // 从一个迭代器范围构造元素
                    void default_construct_at_end() { construct_at_end_impl(); }
                    void construct_at_end_range(T *first, T *last)
                    {
                        // 使用 std::move_if_noexcept 可以在元素支持 noexcept 移动时进行优化，否则进行拷贝
                        for (; first != last; ++first)
                            construct_at_end_impl(std::move_if_noexcept(*first));
                    }
                    void construct_at_end_multi(size_type n, const T &v)
                    {
                        for (; n > 0; --n)
                            construct_at_end_impl(v);
                    }
                    void default_construct_at_end_multi(size_type n)
                    {
                        for (; n > 0; --n)
                            construct_at_end_impl();
                    }

                private:
                    // 使用可变参数模板和完美转发，将参数原封不动地传递给元素的构造函数
                    template <typename... Args>
                    void construct_at_end_impl(Args &&...args)
                    {
                        traits::construct(this->alloc, this->p_finish, std::forward<Args>(args)...);
                        ++this->p_finish; // 构造成功后立刻移动 p_finish 指针
                    }
                };

                /**
                 * @brief RAII 守护类，用于保证强异常安全。
                 *
                 * 在可能抛出异常的操作（如连续插入多个元素）中，如果某个元素的构造函数抛出异常，
                 * 这个守护类的析构函数会自动被调用，它会销毁所有在这个失败操作中已经成功构造的元素，
                 * 从而使 Vector 恢复到操作开始前的状态，避免了内存泄漏和状态不一致。
                 */
                template <typename T>
                struct VectorGuard
                {
                    using pointer = VectorBase<T>::pointer;
                    VectorBase<T> *guarded_instance; // 指向被守护的 VectorBase 实例
                    pointer guarded_pos = nullptr;   // 记录操作开始时的 p_finish 位置
                    bool commited = false;           // 标记操作是否成功提交
                    explicit VectorGuard(VectorBase<T> *base_instance)
                    {
                        guarded_instance = base_instance;
                        guarded_pos = base_instance->p_finish;
                    }
                    // 如果操作成功，调用此函数，防止析构函数执行回滚操作
                    void commit() { commited = true; }
                    ~VectorGuard()
                    {
                        // 如果操作没有被提交（即中途失败），则执行回滚
                        if (!commited)
                        {
                            while (guarded_instance->p_finish != guarded_pos)
                                guarded_instance->destroy_at_end();
                        }
                    }
                };
            }
            /**
             * @brief 最终的用户接口类 Vector。
             *
             * 继承自 VectorBase，实现了完整的 std::vector 接口，包括容量管理、
             * 元素访问、修改器（push_back, insert, erase等）。
             */
            template <typename T>
            struct Vector final
                : detail::VectorBase<T>
            {
                static constexpr double grow_factor = 2.0; // 容量增长因子
                using Base = detail::VectorBase<T>;
                using allocator_type = Base::allocator_type;
                using value_type = Base::value_type;
                using reference = Base::reference;
                using const_reference = Base::const_reference;
                using pointer = Base::pointer;
                using const_pointer = Base::const_pointer;
                using iterator = detail::ArrayLikeIterator<T>;
                using const_iterator = detail::ConstArrayLikeIterator<T>;
                using size_type = Base::size_type;
                using difference_type = Base::difference_type;
                using reverse_iterator = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;
                using Guard = detail::VectorGuard<T>;
                Vector() = default;
                explicit Vector(size_type n)
                    : detail::VectorBase<T>(n)
                {
                    this->default_construct_at_end_multi(n);
                }
                explicit Vector(size_type n, const T &v)
                    : detail::VectorBase<T>(n)
                {
                    this->construct_at_end_multi(n, v);
                }
                template <class InputIt>
                explicit Vector(InputIt first, InputIt last)
                {
                    // 委托给 insert 实现
                    insert(end(), first, last);
                }
                void clear() { Base::clear(); }
                ~Vector() = default;
                size_type size() const { return this->p_finish - this->p_start; }
                bool empty() const { return this->p_finish == this->p_start; }
                size_type capacity() const { return this->p_end_of_storage - this->p_start; }
                pointer data() { return this->p_start; }
                const_pointer data() const { return this->p_start; }
                reference at(size_type index)
                {
                    if (index >= size())
                    {
                        std::ostringstream ss;
                        ss << "Vector::at(index): index (" << index << ") >= this->size() (" << size() << ") ";
                        throw std::out_of_range(ss.str());
                    }
                    return this->p_start[index];
                }
                const_reference at(size_type index) const
                {
                    if (index >= size())
                    {
                        std::ostringstream ss;
                        ss << "Vector::at(index): index (" << index << ") >= this->size() (" << size() << ") ";
                        throw std::out_of_range(ss.str());
                    }
                    return this->p_start[index];
                }
                reference operator[](size_type index) { return this->p_start[index]; }
                const_reference operator[](size_type index) const { return this->p_start[index]; }
                reference front() { return *(this->p_start); }
                const_reference front() const { return *(this->p_start); }
                reference back() { return *(this->p_finish - 1); }
                const_reference back() const { return *(this->p_finish - 1); }
                void assign(size_type n, const T &v)
                {
                    // 使用 "拷贝再交换" (copy-and-swap) 惯用法，提供强异常保证
                    Vector tmp(n, v);
                    this->swap(tmp);
                }
                template <class InputIt>
                void assign(InputIt first, InputIt last)
                {
                    Vector tmp(first, last);
                    this->swap(tmp);
                }

                iterator begin() { return iterator{this->p_start}; }
                const_iterator begin() const { return const_iterator{this->p_start}; }
                iterator end() { return iterator{this->p_finish}; }
                const_iterator end() const { return const_iterator{this->p_finish}; }
                reverse_iterator rbegin() { return reverse_iterator{end()}; }
                const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
                reverse_iterator rend() { return reverse_iterator{begin()}; }
                const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
                const_iterator cbegin() const { return begin(); }
                const_iterator cend() const { return end(); };
                const_reverse_iterator crbegin() const { return rbegin(); };
                const_reverse_iterator crend() const { return rend(); };

                void reserve(size_type least_cap)
                {
                    if (least_cap > capacity())
                        reallocate(least_cap);
                }
                void shrink_to_fit()
                {
                    if (size() < capacity())
                        reallocate(size());
                }
                void resize(size_type n)
                {
                    if (capacity() < n)
                        reallocate(calc_new_capacity(n));
                    if (size() < n)
                    {
                        Guard guard_tmp(this); // 使用 RAII 守护确保异常安全
                        this->default_construct_at_end_multi(n - size());
                        guard_tmp.commit();
                    }
                    while (size() > n)
                        this->destroy_at_end();
                }
                void resize(size_type n, const T &v)
                {
                    if (capacity() < n)
                        reallocate(calc_new_capacity(n));
                    if (size() < n)
                    {
                        Guard guard_tmp(this); // 使用 RAII 守护确保异常安全
                        this->construct_at_end_multi(n - size(), v);
                        guard_tmp.commit();
                    }
                    while (size() > n)
                        this->destroy_at_end();
                }
                void push_back(const T &v)
                {
                    if (size() == capacity())
                    {
                        size_type new_cap = calc_new_capacity(size() + 1);
                        reallocate(new_cap);
                    }
                    this->construct_at_end(v);
                }
                void pop_back() { this->destroy_at_end(); }
                iterator insert(const_iterator pos, const T &v)
                {
                    size_type pos_index = pos - cbegin();
                    if (size() == capacity())
                    {
                        size_type new_cap = calc_new_capacity(size() + 1);
                        reallocate(new_cap);
                    }
                    this->construct_at_end(v); // 在末尾构造一个临时元素
                    // 使用 std::rotate 将末尾的元素旋转到正确的位置，这是一种高效的移动方式
                    std::rotate(begin() + pos_index, end() - 1, end());
                    return begin() + pos_index;
                }
                iterator insert(const_iterator pos, size_type n, const T &v)
                {
                    size_type pos_index = pos - cbegin();
                    if (!n)
                        return begin() + pos_index;
                    // 检查剩余容量是否足够
                    if (capacity() - size() < n)
                    {
                        size_type new_cap = calc_new_capacity(size() + n);
                        // 创建一个临时的 Base 对象来执行重新分配和插入，以保证异常安全
                        Base tmp(new_cap);
                        tmp.construct_at_end_range(this->p_start, this->p_start + pos_index);
                        tmp.construct_at_end_multi(n, v);
                        tmp.construct_at_end_range(this->p_start + pos_index, this->p_finish);
                        this->Base::swap(tmp);// 与临时对象交换，旧内存随 tmp 的析构而释放
                    }
                    else
                    {
                        Guard guard_tmp(this);
                        this->construct_at_end_multi(n, v);
                        guard_tmp.commit();
                        std::rotate(begin() + pos_index, end() - n, end());
                    }
                    return begin() + pos_index;
                }
                template <typename InputIt>
                iterator insert(const_iterator pos, InputIt first, InputIt last)
                {
                    // 使用迭代器萃取 (iterator_traits) 来获取迭代器的类型
                    using Category = typename std::iterator_traits<InputIt>::iterator_category;
                    size_type pos_index = pos - cbegin();
                    if (first == last)
                        return begin() + pos_index;
                    // 使用 "标签分发" (tag dispatching)，根据迭代器类型调用不同的重载版本以进行优化
                    insert_dispatch(pos_index, first, last, Category());
                    return begin() + pos_index;
                }
                iterator erase(const_iterator pos)
                {
                    iterator nonconst_pos = begin() + (pos - cbegin());
                    // 使用 std::move 将删除点之后的元素向前移动，覆盖被删除的元素
                    std::move(nonconst_pos + 1, end(), nonconst_pos);
                    pop_back(); // 销毁最后一个（现在是多余的）元素
                    return nonconst_pos;
                }
                iterator erase(const_iterator first, const_iterator last)
                {
                    iterator nonconst_first = begin() + (first - cbegin());
                    iterator nonconst_last = begin() + (last - cbegin());
                    if (first == last)
                        return nonconst_last;
                    size_type pos_index = first - cbegin();
                    // 将范围之后的元素向前移动
                    iterator new_end = std::move(nonconst_last, end(), nonconst_first);
                    // 销毁所有移动后末尾多余的元素
                    while (end() != new_end)
                        pop_back();
                    return nonconst_first;
                }

                void swap(Vector &other)
                {
                    if (this != std::addressof(other))
                        this->Base::swap(other);
                }

            protected:
                // 内存重新分配函数
                // 假设 new_cap >= size()
                // assuming new_cap>=size()
                void reallocate(size_type new_cap)
                {
                    // 1. 创建一个新的内存基座
                    Base vec_base(new_cap);
                    // 2. 将现有元素移动（或拷贝）到新内存中
                    //    construct_at_end_range 内部使用 std::move_if_noexcept 实现了优化
                    vec_base.construct_at_end_range(this->p_start, this->p_finish);
                    // 3. 交换，this 现在拥有了新内存和元素，vec_base 持有了旧的
                    this->Base::swap(vec_base);
                    // 4. 函数结束时，vec_base 被析构，从而安全地释放了旧内存
                }

                // --- insert_dispatch 的重载版本 ---
                
                // 为前向迭代器（以及更强的，如双向、随机访问迭代器）提供的优化版本
                template <typename ForwardIt>
                void insert_dispatch(size_type pos_index, ForwardIt first, ForwardIt last,
                                     std::forward_iterator_tag)
                {
                    auto n = std::distance(first, last);// 可以安全地计算距离
                    if (!n)
                        return;

                    if (capacity() - size() >= n)
                    {
                        Guard guard_tmp(this);
                        for (auto it = first; it != last; ++it)
                        {
                            this->construct_at_end(*it);
                        }
                        guard_tmp.commit();
                        std::rotate(begin() + pos_index, end() - n, end());
                    }
                    else
                    {
                        size_type new_cap = calc_new_capacity(size() + n);
                        Base tmp(new_cap);
                        tmp.construct_at_end_range(this->p_start, this->p_start + pos_index);
                        for (auto it = first; it != last; ++it)
                            tmp.construct_at_end(*it);
                        tmp.construct_at_end_range(this->p_start + pos_index, this->p_finish);
                        this->Base::swap(tmp);
                    }
                }

                
                
                // 为最弱的输入迭代器提供的安全版本
                // 输入迭代器只能遍历一次，且不能预先计算距离
                template <typename InputIt>
                void insert_dispatch(size_type pos_index, InputIt first, InputIt last,
                                     std::input_iterator_tag)
                {
                    // 创建一个临时 vector 来存储所有最终元素
                    Vector tmp;
                    tmp.reserve(size());// 预先分配一些容量以减少重分配
                    tmp.construct_at_end_range(this->p_start, this->p_start + pos_index);
                    for (; first != last; ++first)
                        tmp.push_back(*first);
                    for (auto it = begin() + pos_index; it != end(); ++it)
                    {
                        tmp.push_back(std::move_if_noexcept(*it));
                    }
                    this->swap(tmp);
                }
                
                // 计算新容量的策略
                size_type calc_new_capacity(size_type new_size)
                {
                    
                    // 常见的指数增长策略，确保 push_back 的摊还时间复杂度为 O(1)
                    size_type new_cap = capacity() * grow_factor;
                    return std::max(new_cap, new_size);
                }
            };

        }
    }

} // namespace DSA
