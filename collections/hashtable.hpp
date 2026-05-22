#pragma once
#include <algorithm>
#include <forward_list>
#include <iterator>
#include <functional>
#include <cmath>
#include <vector>
#include <algorithm>
#include <type_traits>
#include "hashtable_aux.hpp"
#include "../utils.hpp"
namespace DSA
{
    namespace Hashing
    {
        // 默认的键提取器，假设值本身就是键
        using DSA::Utils::IdentityKeyOfValue;
        struct PrimeReHash
        {
            /**
             * @brief 默认的重哈希策略，使用素数来作为桶的数量。
             *
             * 这种策略旨在通过使用素数大小的桶来减少哈希冲突，因为当桶的数量是素数时，
             * 常见的哈希函数（尤其是基于模运算的）能产生更均匀的分布。
             */

            // 为非常小的桶数量提供一个快速查询表，避免查找素数列表
            static constexpr unsigned char fast_bkt[] = {1, 2, 2, 3, 5, 5, 7, 7, 11, 11, 11, 11, 13, 13};

            // 编译时计算素数列表中的素数数量
            constexpr static auto n_primes = sizeof(detail::prime_list) / sizeof(unsigned long) - 1;

            // 指向素数列表末尾的指针
            constexpr static auto last_prime = detail::prime_list + n_primes;

            /**
             * @brief 计算下一个合适的桶数量。
             * @param element_count 当前元素数量。
             * @param least_bucket_count 期望的最小桶数量。
             * @return size_t 计算出的下一个桶数量，通常是一个素数。
             */
            size_t next_bucket_count(size_t element_count, size_t least_bucket_count)
            {
                // 桶的数量至少要能维持最大加载因子
                least_bucket_count = std::max(least_bucket_count, size_t(std::ceil(element_count / factor)));
                // 如果所需桶数很小，直接从快速查询表返回
                if (least_bucket_count < sizeof(fast_bkt))
                    return (size_t)fast_bkt[least_bucket_count];

                // 否则，在预定义的素数列表中查找第一个不小于 least_bucket_count 的素数
                return size_t(*std::lower_bound(detail::prime_list + 6, last_prime, least_bucket_count));
            }

            /**
             * @brief 判断是否需要重哈希。
             * @param element_count 当前元素数量。
             * @param cur_bucket_count 当前桶数量。
             * @return bool 如果当前加载因子超过了最大加载因子，则返回 true。
             */
            bool need_rehash(size_t element_count, size_t cur_bucket_count)
            {
                // 当加载因子 (元素数 / 桶数) > 预设的 factor 时，需要重哈希
                // 同时避免对已经达到最大素数桶容量的表进行重哈希
                return cur_bucket_count < *last_prime && (element_count / double(cur_bucket_count)) > factor;
            }
            // 获取当前的最大加载因子
            float max_load_factor() const { return factor; }
            // 设置新的最大加载因子
            void max_load_factor(float new_factor) { factor = new_factor; }

        private:
            // 加载因子，当 (元素数量 / 桶数量) > factor 时，触发 rehash
            float factor = 0.75;
        };

        namespace detail
        {
            // 前向声明，因为 HashTableIterator 和 HashTableBase 相互引用
            template <typename T, bool HashCached>
            struct HashTableBase;

            /**
             * @brief 节点特性萃取 (Node Traits)。
             *
             * 这个结构体的作用是抽象化节点(Node)的内部存储。
             * 根据模板参数 `HashCached` 的值，节点可以存储 `T` (值) 或者 `std::pair<size_t, T>` (哈希值, 值)。
             * 这样做可以避免在哈希表操作中重复计算哈希值，从而提高性能。
             */
            template <typename T, bool HashCached = true>
            struct hash_node_traits
            {
                // 使用 std::conditional_t 在编译时决定节点的实际存储类型
                using node_value_type = std::conditional_t<HashCached, std::pair<size_t, T>, T>;
                // 提供统一的接口来获取节点中的值 (T)
                constexpr static const T &value(const node_value_type &ndv)
                {
                    if constexpr (HashCached) // if constexpr 是 C++17 的特性，在编译时进行分支选择
                    {
                        return ndv.second; // 如果缓存了哈希，值在 pair 的第二个元素
                    }
                    else
                    {
                        return ndv; // 否则，节点本身就是值
                    }
                }
                // 非 const 版本
                constexpr static T &value(node_value_type &ndv)
                {
                    if constexpr (HashCached)
                    {
                        return ndv.second;
                    }
                    else
                    {
                        return ndv;
                    }
                }
                // 提供接口获取缓存的哈希码
                constexpr static size_t hashcode(const node_value_type &ndv)
                {
                    // 静态断言，确保只有在开启哈希缓存时才能调用此函数
                    static_assert(HashCached, "hash_node_traits: getting hashcode from node directly cannot work since hashcode is not cached");
                    return ndv.first;
                }
                // 根据值和哈希码创建节点内部存储的 node_value_type
                constexpr static node_value_type node_value(const T &v, size_t hashcode)
                {
                    if constexpr (HashCached)
                    {
                        return std::pair<size_t, T>{hashcode, v};
                    }
                    else
                    {
                        // 如果不缓存哈希，则忽略传入的 hashcode，只存储值
                        return v;
                    }
                }
            };
            // 前向声明 const 版本的迭代器
            template <typename T, bool HashCached>
            struct ConstHashTableIterator;

            /**
             * @brief 哈希表的（全局）可变迭代器。
             *
             * 这是一个前向迭代器，用于遍历整个哈希表中的所有元素。
             * 它内部包装了底层 `std::forward_list` 的迭代器。
             */
            template <typename T, bool HashCached>
            struct HashTableIterator
            {
                using value_type = T;

                using iterator_category = std::forward_iterator_tag;
                using reference = T &;
                using pointer = T *;
                using difference_type = std::ptrdiff_t;
                using value_traits = hash_node_traits<T, HashCached>;
                using list_type = std::forward_list<typename value_traits::node_value_type>;
                using node_iterator = list_type::iterator;
                using const_node_iterator = list_type::const_iterator;

                HashTableIterator() : it(nullptr) {}
                explicit HashTableIterator(node_iterator p) : it(p) {}

                reference operator*() const { return value_traits::value(*it); }
                pointer operator->() const { return &value_traits::value(*it); }

                HashTableIterator &operator++()
                {
                    ++it;
                    return *(this);
                }
                HashTableIterator operator++(int)
                {
                    auto tmp = *this;
                    ++it;
                    return tmp;
                }
                bool operator==(const HashTableIterator &other) const { return it == other.it; }
                bool operator!=(const HashTableIterator &other) const { return it != other.it; }

            protected:
                node_iterator it;
                friend HashTableBase<T, HashCached>;
                friend ConstHashTableIterator<T, HashCached>;
            };
            /**
             * @brief 哈希表的（全局）常量迭代器。
             *
             * 功能与 HashTableIterator 类似，但提供对元素的只读访问。
             */
            template <typename T, bool HashCached>
            struct ConstHashTableIterator
            {
                using value_type = const T;
                using value_traits = hash_node_traits<T, HashCached>;
                using iterator_category = std::forward_iterator_tag;
                using reference = const T &;
                using pointer = const T *;
                using difference_type = std::ptrdiff_t;
                using list_type = std::forward_list<typename value_traits::node_value_type>;
                using node_iterator = list_type::const_iterator;
                using const_node_iterator = list_type::const_iterator;

                ConstHashTableIterator() : it(nullptr) {}
                explicit ConstHashTableIterator(const_node_iterator p) : it(p) {}

                // 允许从可变迭代器构造常量迭代器
                ConstHashTableIterator(const HashTableIterator<T, HashCached> &other) : it(other.it) {}

                reference operator*() const { return value_traits::value(*it); }
                pointer operator->() const { return &value_traits::value(*it); }

                ConstHashTableIterator &operator++()
                {
                    ++it;
                    return *(this);
                }
                ConstHashTableIterator operator++(int)
                {
                    auto tmp = *this;
                    ++it;
                    return tmp;
                }
                bool operator==(const ConstHashTableIterator &other) const { return it == other.it; }
                bool operator!=(const ConstHashTableIterator &other) const { return it != other.it; }

            protected:
                const_node_iterator it;
                friend HashTableBase<T, HashCached>;
            };
            /**
             * @brief 哈希表的基类。
             *
             * 主要作用是统一定义哈希表、迭代器等共用的类型别名 (typedefs/usings) 和一些辅助函数，
             * 以减少主模板类 `HashTable` 中的代码冗余。
             */
            template <typename T, bool HashCached>
            struct HashTableBase
            {

                using value_type = T;
                using reference = T &;
                using const_reference = const T &;
                using pointer = T *;
                using const_pointer = const T *;
                using iterator = HashTableIterator<T, HashCached>;
                using const_iterator = ConstHashTableIterator<T, HashCached>;
                using size_type = size_t;
                using difference_type = std::ptrdiff_t;
                using value_traits = hash_node_traits<T, HashCached>;
                using node_value_type = value_traits::node_value_type;
                using list_type = std::forward_list<node_value_type>;
                using node_iterator = list_type::iterator;
                using const_node_iterator = list_type::const_iterator;

            protected:
                // 辅助函数，用于从我们自定义的迭代器中提取出底层的 forward_list 迭代器
                static node_iterator getNode(iterator it)
                {
                    return it.it;
                }
                static const_node_iterator getNode(const_iterator it)
                {
                    return it.it;
                }
            };
        }
        /**
         * @brief 一个分离链接法（Separate Chaining）实现的哈希表。但是实现上所有桶的链表存储为大链表的一段子链表
         *
         * @tparam T 存储的元素类型 (value_type)。
         * @tparam KeyT 键的类型。
         * @tparam Hash 哈希函数对象类型。
         * @tparam EqualT 判断两个值是否相等的函数对象类型。
         * @tparam KeyOfValue 从值 T 中提取键 KeyT 的函数对象类型。
         * @tparam ReHashPolicy 重哈希策略类型。
         * @tparam HashCached 是否缓存哈希值。
         */
        template <typename T, typename KeyT = T, typename Hash = std::hash<KeyT>, typename EqualT = std::equal_to<T>, typename KeyOfValue = IdentityKeyOfValue<T>, typename ReHashPolicy = PrimeReHash, bool HashCached = true>
        struct HashTable : detail::HashTableBase<T, HashCached>
        {
            using Base = detail::HashTableBase<T, HashCached>;
            using key_type = KeyT;
            using value_type = Base::value_type;
            using reference = Base::reference;
            using const_reference = Base::const_reference;
            using pointer = Base::pointer;
            using const_pointer = Base::const_pointer;
            using iterator = Base::iterator;
            using const_iterator = Base::const_iterator;
            using size_type = Base::size_type;
            using difference_type = Base::difference_type;
            using value_traits = Base::value_traits;
            using node_value_type = Base::node_value_type;
            using list_type = Base::list_type;
            using node_iterator = Base::node_iterator;
            using const_node_iterator = Base::const_node_iterator;
            using hasher = Hash;
            using key_equal = EqualT;
            using hash_table = HashTable<T, KeyT, Hash, EqualT, KeyOfValue, ReHashPolicy, HashCached>;

            /**
             * @brief 局部迭代器 (Local Iterator)，用于遍历单个桶内的元素。
             */
            struct const_local_iterator;
            struct local_iterator
            {
                explicit local_iterator(node_iterator p, size_type at_bucket, const hash_table *c) : it(p), ht(c)
                {
                    this->end = c->overall_list.end();
                    this->at_bucket = at_bucket;
                    this->with_bucket_count = c->bucket_count();
                }
                reference operator*() const { return value_traits::value(*it); }
                pointer operator->() const { return &value_traits::value(*it); }
                local_iterator &operator++()
                {
                    if (it != end)
                    {
                        ++it; // 移动到底层链表的下一个节点
                        if (it != end)
                        {
                            // 检查新节点是否仍然属于当前桶
                            // 如果不属于，则表示当前桶的遍历已经结束，将迭代器置为 end 状态
                            if constexpr (HashCached)
                            {
                                if (hash_bucket(value_traits::hashcode(*it), with_bucket_count) != at_bucket)
                                {
                                    it = end;
                                }
                            }
                            else
                            {
                                if (hash_bucket(ht->hashf(KeyOfValue{}(*it)), with_bucket_count) != at_bucket)
                                {
                                    it = end;
                                }
                            }
                        }
                    }
                    return *(this);
                }
                local_iterator operator++(int)
                {
                    auto tmp = *this;
                    ++(*this);
                    return tmp;
                }
                bool operator==(const local_iterator &other) const { return it == other.it; }
                bool operator!=(const local_iterator &other) const { return it != other.it; }

            protected:
                const hash_table *ht = nullptr;
                node_iterator it, end;                  // 当前迭代器位置和链表末尾
                size_type at_bucket, with_bucket_count; // 记录正在遍历的桶的索引和当时的桶总数
                friend const_local_iterator;
            };

            /**
             * @brief 常量局部迭代器。
             */
            struct const_local_iterator
            {

                explicit const_local_iterator(const_node_iterator p, size_type at_bucket, const hash_table *c) : it(p), ht(c)
                {
                    this->end = c->overall_list.end();
                    this->at_bucket = at_bucket;
                    this->with_bucket_count = c->bucket_count();
                }
                const_local_iterator(local_iterator &other)
                {
                    ht = other.ht;
                    it = other.it;
                    end = other.end;
                    at_bucket = other.at_bucket;
                    with_bucket_count = other.with_bucket_count;
                }
                const_reference operator*() const { return value_traits::value(*it); }
                const_pointer operator->() const { return &value_traits::value(*it); }
                const_local_iterator &operator++()
                {
                    if (it != end)
                    {
                        ++it;
                        if (it != end)
                        {
                            if constexpr (HashCached)
                            {
                                if (hash_bucket(value_traits::hashcode(*it), with_bucket_count) != at_bucket)
                                {
                                    it = end;
                                }
                            }
                            else
                            {
                                if (hash_bucket(ht->hashf(KeyOfValue{}(*it)), with_bucket_count) != at_bucket)
                                {
                                    it = end;
                                }
                            }
                        }
                    }
                    return *(this);
                }
                const_local_iterator operator++(int)
                {
                    auto tmp = *this;
                    ++(*this);
                    return tmp;
                }
                bool operator==(const const_local_iterator &other) const { return it == other.it; }
                bool operator!=(const const_local_iterator &other) const { return it != other.it; }

            protected:
                const hash_table *ht = nullptr;
                const_node_iterator it, end;
                size_type at_bucket, with_bucket_count;
            };
            size_type size() const { return size_r; }
            bool empty() const { return !size_r; }
            // 返回桶的数量
            size_type bucket_count() const { return bucket_before_first.size(); }
            // 返回第 n 个桶中的元素数量
            size_type bucket_size(size_type n) const
            {
                size_type res = 0;
                // 从桶的第一个元素开始遍历，直到遇到链表末尾或下一个桶的元素
                const_node_iterator it = bucket_before_first[n];
                for (++it; it != overall_list.end() && hash_bucket(node_hashcode(*it)) == n; ++res, ++it)
                    ;
                return res;
            }
            // 返回哈希函数对象
            hasher hash_function() const { return hashf; }
            // 返回键值比较函数对象
            key_equal keq_eq() const { return kequal; }
            // 返回最大加载因子
            float max_load_factor() const { return rehash_policy.max_load_factor(); }

            /**
             * @brief 重哈希操作。
             * @param new_bucket_count 期望的新桶数量。如果为0，则由策略自动计算。
             *
             * 不重新创建节点，而是遍历一次现有的 `overall_list`，
             * 并使用 `forward_list::splice_after` 将节点移动到它们在新桶布局中的正确位置。
             * 这避免了内存分配和拷贝，开销很小。
             */
            void rehash(size_type new_bucket_count)
            {
                // 确定最终的新桶数量
                new_bucket_count = std::max(new_bucket_count, rehash_policy.next_bucket_count(size(), new_bucket_count));
                // 创建一个新的桶向量，所有条目都指向链表头的前一个位置 (before_begin)
                std::vector<node_iterator> tmp(new_bucket_count, overall_list.before_begin());
                node_iterator it = overall_list.begin();
                if (it != overall_list.end())
                {
                    // 计算第一个元素在新布局下的桶索引
                    size_t last_bucket = hash_bucket(node_hashcode(*it), new_bucket_count);
                    for (++it; it != overall_list.end(); ++it)
                    {
                        size_t cur_bucket = hash_bucket(node_hashcode(*it), new_bucket_count);
                        if (cur_bucket != last_bucket)
                        {
                            // 当遇到一个属于不同桶的元素时，把之前属于 last_bucket 的整个链表段
                            // 移到 (splice) 新桶表 tmp[last_bucket] 的后面。
                            overall_list.splice_after(tmp[last_bucket], overall_list, overall_list.before_begin(), it);
                            last_bucket = cur_bucket;
                        }
                    }
                    // 把最后一段链表也移动到正确的位置
                    overall_list.splice_after(tmp[last_bucket], overall_list, overall_list.before_begin(), overall_list.end());
                }
                // 用新的桶向量替换旧的
                bucket_before_first.swap(tmp);
            }
            // 设置最大加载因子，并可能触发重哈希
            void max_load_factor(float new_factor)
            {
                rehash_policy.max_load_factor(new_factor);
                if (rehash_policy.need_rehash(size(), bucket_count()))
                    rehash(0); // 传入0让策略自动计算大小
            }
            explicit HashTable(size_type init_buckets_count, const hasher &h = hasher{}, const key_equal &keq = key_equal{}) : hashf(h), kequal(keq)
            {
                rehash_policy.max_load_factor(1.0);
                bucket_before_first.assign(rehash_policy.next_bucket_count(0, init_buckets_count), overall_list.before_begin());
            }
            HashTable()
            {
                rehash_policy.max_load_factor(1.0);
                bucket_before_first.assign(rehash_policy.next_bucket_count(0, 0), overall_list.before_begin());
            }
            void clear()
            {
                overall_list.clear();
                bucket_before_first.clear();
            }

            iterator begin() { return overall_list.begin(); }
            const_iterator begin() const { return overall_list.begin(); }
            const_iterator cbegin() const { return overall_list.cbegin(); }

            iterator end() { return overall_list.end(); }
            const_iterator end() const { return overall_list.end(); }
            const_iterator cend() const { return overall_list.end(); }

            // --- 局部(桶)迭代器 ---
            local_iterator begin(size_type n)
            {
                // 找到桶n的第一个节点的前一个节点。如果桶为空，其指向链表的 before_begin，
                // 此时返回 end()。否则返回该桶第一个节点的迭代器。
                return local_iterator(
                    (
                        bucket_before_first[n] == overall_list.before_begin()
                            ? overall_list.end()
                            : std::next(bucket_before_first[n])),
                    n, this);
            }
            const_local_iterator begin(size_type n) const
            {
                return const_local_iterator(
                    (bucket_before_first[n] == overall_list.before_begin()
                         ? overall_list.end()
                         : std::next(bucket_before_first[n])),
                    n, this);
            }
            const_local_iterator cbegin(size_type n) const { return begin(n); }

            local_iterator end(size_type n) { return local_iterator(overall_list.end(), n, this); }
            const_local_iterator end(size_type n) const { return const_local_iterator(overall_list.end(), n, this); }
            const_local_iterator cend(size_type n) const { return end(n); }

            // 根据键计算桶索引
            size_type bucket(const key_type &k) const
            {
                return hash_bucket(hashf(k));
            }

            /**
             * @brief 插入唯一键值。
             * @return pair<iterator, bool>，其中 iterator 指向插入的元素或已存在的元素，
             *         bool 表示是否成功插入 (true) 或已存在 (false)。
             */
            std::pair<iterator, bool> insert_unique(const T &v)
            {
                node_value_type vnode = node_value(v);
                size_t vhash = node_hashcode(vnode);
                size_type vbucket = hash_bucket(vhash);
                node_iterator it = bucket_before_first[vbucket];

                // 在桶内查找是否已存在
                for (++it; it != overall_list.end(); ++it)
                {
                    size_t cur_hash = node_hashcode(*it);
                    if (hash_bucket(cur_hash) != vbucket) // 已超出当前桶的范围
                        break;
                    if (cur_hash == vhash && vkequal(value_traits::value(*it), v))
                    {
                        return {iterator{it}, false}; // 键已存在
                    }
                }
                // 插入前检查是否需要 rehash
                if (rehash_policy.need_rehash(size() + 1, bucket_count()))
                {
                    rehash(bucket_count() * 2);   // 至少扩大一倍
                    vbucket = hash_bucket(vhash); // rehash 后重新计算桶
                }

                it = overall_list.insert_after(bucket_before_first[vbucket], vnode);

                // 维护 bucket_before_first 数组的正确性
                node_iterator nxt_it = std::next(it);
                if (nxt_it != overall_list.end())
                {
                    size_type nxt_bucket = hash_bucket(node_hashcode(*nxt_it));
                    if (nxt_bucket != vbucket)
                    {
                        // 如果插入的节点 `it` 后面紧邻的节点属于一个新的桶 `nxt_bucket`，
                        // 那么 `it` 就成为了新桶 `nxt_bucket` 的 "前一个" 节点。
                        bucket_before_first[nxt_bucket] = it;
                    }
                }
                ++size_r;
                return {iterator{it}, true};
            }

            /**
             * @brief 插入可重复的键值。
             * @return iterator 指向新插入的元素。
             */
            iterator insert_multi(const T &v)
            {
                if (rehash_policy.need_rehash(size() + 1, bucket_count()))
                    rehash(bucket_count() * 2);
                node_value_type vnode = node_value(v);
                size_t vhash = node_hashcode(vnode);
                size_type vbucket = hash_bucket(vhash);
                node_iterator it = bucket_before_first[vbucket], pre_it;
                size_t cur_hash;
                size_type cur_bucket;

                // 找到合适的插入位置
                for (pre_it = it, ++it; it != overall_list.end(); pre_it = it, ++it)
                {
                    cur_hash = node_hashcode(*it);
                    cur_bucket = hash_bucket(cur_hash);
                    if (cur_bucket != vbucket ||
                        (cur_hash == vhash && vkequal(value_traits::value(*it), v)))
                        break;
                }
                pre_it = overall_list.insert_after(pre_it, vnode);
                // 同样需要维护 bucket_before_first
                if (it != overall_list.end() && cur_bucket != vbucket)
                {
                    bucket_before_first[cur_bucket] = pre_it;
                }
                ++size_r;
                return iterator{pre_it};
            }
            iterator find(const key_type &k) { return iterator{find_node(*this, k)}; }
            const_iterator find(const key_type &k) const { return const_iterator{find_node(*this, k)}; }

            /**
             * @brief 删除指定迭代器位置的元素。
             * @return iterator 指向被删除元素之后元素的迭代器。
             */
            iterator erase(const_iterator pos)
            {
                const_node_iterator posn = getNode(pos); // 获取底层迭代器
                size_t phash = node_hashcode(*posn);
                size_type pbucket = hash_bucket(phash);
                node_iterator it = bucket_before_first[pbucket], pre_it;

                // 找到 posn 的前一个节点 pre_it
                for (pre_it = it, ++it; it != overall_list.end(); pre_it = it, ++it)
                {
                    if (it == posn)
                    {
                        it = overall_list.erase_after(pre_it);
                        --size_r;
                        break;
                    }
                }

                // 维护 bucket_before_first 数组
                if (it != overall_list.end())
                {
                    size_type nbucket = hash_bucket(node_hashcode(*it));
                    if (nbucket != pbucket)
                    {
                        // 如果删除后，pre_it 后面紧邻的节点属于一个新的桶 nbucket,
                        // 那么 pre_it 就成了 nbucket 的前驱节点
                        bucket_before_first[nbucket] = pre_it;
                        // 如果被删除的元素是 pbucket 的第一个元素
                        if (pre_it == bucket_before_first[pbucket])
                        {
                            // pbucket 现在变空了
                            bucket_before_first[pbucket] = overall_list.before_begin();
                        }
                    }
                }
                else // 如果删除的是链表最后一个元素
                {
                    if (pre_it == bucket_before_first[pbucket])
                    {
                        bucket_before_first[pbucket] = overall_list.before_begin();
                    }
                }
                return iterator{it};
            }

            // 根据键删除唯一元素
            size_type erase_unique(const key_type &k)
            {
                // ... 实现与 erase(pos) 类似，但先要查找 ...
                // 找到后调用 erase(iterator) 即可，但这里为了效率直接实现了
                size_type res = 0;
                size_t khash = hashf(k);
                size_type kbucket = hash_bucket(khash);
                node_iterator it = bucket_before_first[kbucket], pre_it;
                for (pre_it = it, ++it; it != overall_list.end(); pre_it = it, ++it)
                {
                    size_t cur_hash = node_hashcode(*it);
                    if (hash_bucket(cur_hash) != kbucket)
                        break;
                    if (cur_hash == khash && kequal(KeyOfValue{}(value_traits::value(*it)), k))
                    {
                        it = overall_list.erase_after(pre_it);
                        ++res;
                        size_r -= res;
                        break;
                    }
                }
                if (res)
                {
                    // 维护 bucket_before_first，逻辑同 erase(pos)
                    if (it != overall_list.end())
                    {
                        size_type nbucket = hash_bucket(node_hashcode(*it));
                        if (nbucket != kbucket)
                        {
                            bucket_before_first[nbucket] = pre_it;
                            if (pre_it == bucket_before_first[kbucket])
                            {
                                bucket_before_first[kbucket] = overall_list.before_begin();
                            }
                        }
                    }
                    else
                    {
                        if (pre_it == bucket_before_first[kbucket])
                        {
                            bucket_before_first[kbucket] = overall_list.before_begin();
                        }
                    }
                }
                return res;
            }
            // 根据键删除所有匹配的元素 (multi)
            size_type erase_multi(const key_type &k)
            {
                // ... 找到第一个匹配元素后，继续向后查找所有匹配元素，然后一次性删除 ...
                size_type res = 0;
                size_t khash = hashf(k);
                size_type kbucket = hash_bucket(khash);
                node_iterator it = bucket_before_first[kbucket], pre_it;
                for (pre_it = it, ++it; it != overall_list.end(); pre_it = it, ++it)
                {
                    size_t cur_hash = node_hashcode(*it);
                    if (hash_bucket(cur_hash) != kbucket)
                        break;
                    if (cur_hash == khash && kequal(KeyOfValue{}(value_traits::value(*it)), k))
                    {
                        // 找到第一个匹配项，继续寻找这个匹配范围的末尾
                        for (++res, ++it; it != overall_list.end(); ++res, ++it)
                        {
                            cur_hash = node_hashcode(*it);
                            if (cur_hash != khash)
                                break;
                            if (!kequal(KeyOfValue{}(value_traits::value(*it)), k))
                                break;
                        }
                        it = overall_list.erase_after(pre_it, it);
                        size_r -= res;
                        break;
                    }
                }
                if (res)
                {

                    // 维护 bucket_before_first，逻辑同 erase(pos)
                    if (it != overall_list.end())
                    {
                        size_type nbucket = hash_bucket(node_hashcode(*it));
                        if (nbucket != kbucket)
                        {
                            bucket_before_first[nbucket] = pre_it;
                            if (pre_it == bucket_before_first[kbucket])
                            {
                                bucket_before_first[kbucket] = overall_list.before_begin();
                            }
                        }
                    }
                    else
                    {
                        if (pre_it == bucket_before_first[kbucket])
                        {
                            bucket_before_first[kbucket] = overall_list.before_begin();
                        }
                    }
                }
                return res;
            }
            // 统计键的个数
            size_type count_unique(const key_type &k) const
            {
                return size_type(find(k) != end());
            }
            size_type count_multi(const key_type &k) const
            {
                auto rg = equal_range_multi(k);
                return std::distance(rg.first, rg.second);
            }
            // 返回键匹配的范围 (unique)
            std::pair<iterator, iterator> equal_range_unique(const key_type &k)
            {
                node_iterator node_it = find_node(*this, k);
                if (node_it == overall_list.end())
                    return {iterator{node_it}, iterator{node_it}};
                else
                    return {iterator{node_it}, iterator{std::next(node_it)}};
            }
            std::pair<const_iterator, const_iterator> equal_range_unique(const key_type &k) const
            {
                const_node_iterator node_it = find_node(*this, k);
                if (node_it == overall_list.end())
                    return {const_iterator{node_it}, const_iterator{node_it}};
                else
                    return {const_iterator{node_it}, const_iterator{std::next(node_it)}};
            }

            // 返回键匹配的范围 (multi)
            std::pair<iterator, iterator> equal_range_multi(const key_type &k)
            {
                std::pair<node_iterator, node_iterator> res = equal_range_multi_node(*this, k);
                return {iterator{res.first}, iterator{res.second}};
            }
            std::pair<const_iterator, const_iterator> equal_range_multi(const key_type &k) const
            {
                std::pair<const_node_iterator, const_node_iterator> res = equal_range_multi_node(*this, k);
                return {const_iterator{res.first}, const_iterator{res.second}};
            }

        protected:
            // 继承 using 声明，让派生类可以使用基类的 protected 成员
            using Base::getNode;
            list_type overall_list;                         // 单一前向链表，存储所有元素
            std::vector<node_iterator> bucket_before_first; // 桶数组，核心设计！
                                                            // bucket_before_first[i] 存储指向第 i 个桶的第一个节点之前的节点的迭代器
                                                            // 这样设计是为了配合 `forward_list::insert_after` 和 `erase_after
            hasher hashf;                                   // 哈希函数对象
            size_type size_r = 0;                           // 元素数量
            ReHashPolicy rehash_policy;                     // 重哈希策略对象
            key_equal kequal;                               // 键值比较函数对象

            // 辅助函数，通过提取键来比较两个值对象 T
            bool vkequal(const T &v1, const T &v2) const
            {
                return kequal(KeyOfValue{}(v1), KeyOfValue{}(v2));
            }
            // 辅助函数，抽象化获取节点哈希码的过程
            size_t node_hashcode(const node_value_type &it) const
            {
                if constexpr (HashCached)
                {
                    return value_traits::hashcode(it);
                }
                else
                {
                    return hashf(KeyOfValue{}(it));
                }
            }
            // 辅助函数，抽象化创建节点值的过程
            node_value_type node_value(const T &v) const
            {
                if constexpr (HashCached)
                {
                    return value_traits::node_value(v, hashf(KeyOfValue{}(v)));
                }
                else
                {
                    return v;
                }
            }
            // 静态辅助函数，将完整的哈希码约束到桶索引
            static size_type hash_bucket(size_t hashcode_to_constrain, size_type constrain_bucket_count)
            {

                // 如果 ReHashPolicy 提供了 always_pow2 标志，并且为 true，则使用位运算代替取模，效率更高
                if constexpr (requires { ReHashPolicy::always_pow2; })
                {
                    if constexpr (ReHashPolicy::always_pow2)
                    {
                        return hashcode_to_constrain & (constrain_bucket_count - 1);
                    }
                }
                // 默认使用取模运算
                return hashcode_to_constrain % constrain_bucket_count;
            }
            // 实例方法版本，使用当前哈希表的桶数量
            size_type hash_bucket(size_t hashcode_to_constrain) const
            {
                return hash_bucket(hashcode_to_constrain, bucket_count());
            }

            /**
             * @brief 查找节点的静态辅助函数模板。
             * @tparam Self 可以是 HashTable 或 const HashTable，通过模板推导实现代码复用。
             * @return 返回找到的底层节点迭代器，或 end()。
             */
            template <typename Self>
            static auto find_node(Self &self, const key_type &k)
            {
                size_t khash = self.hashf(k);
                size_type kbucket = self.hash_bucket(khash);
                auto it = self.bucket_before_first[kbucket];
                for (++it; it != self.overall_list.end(); ++it)
                {
                    size_t cur_hash = self.node_hashcode(*it);
                    if (self.hash_bucket(cur_hash) != kbucket)
                        break;
                    if (cur_hash == khash && self.kequal(KeyOfValue{}(value_traits::value(*it)), k))
                    {
                        return it;
                    }
                }
                return self.overall_list.end();
            }

            /**
             * @brief 查找多个匹配节点的范围的静态辅助函数模板。
             */
            template <typename Self>
            static auto equal_range_multi_node(Self &self, const key_type &k)
            {

                size_t khash = self.hashf(k);
                size_type kbucket = self.hash_bucket(khash);
                auto it = self.bucket_before_first[kbucket];

                // 查找第一个匹配的节点
                for (++it; it != self.overall_list.end(); ++it)
                {
                    size_t cur_hash = self.node_hashcode(*it);
                    if (self.hash_bucket(cur_hash) != kbucket)
                        break;
                    if (cur_hash == khash && self.kequal(KeyOfValue{}(value_traits::value(*it)), k))
                    {
                        auto next_it = std::next(it);
                        for (; next_it != self.overall_list.end(); ++next_it)
                        {

                            // 找到第一个后，继续向后查找，直到键不匹配为止
                            cur_hash = self.node_hashcode(*next_it);
                            if (cur_hash != khash)
                                break;
                            if (!self.kequal(KeyOfValue{}(value_traits::value(*next_it)), k))
                                break;
                        }
                        return std::make_pair(it, next_it);// 返回 [first, last) 范围
                    }
                }
                // 没找到，返回一个空范围
                return std::make_pair(self.overall_list.end(), self.overall_list.end());
            }
        };

    }
}