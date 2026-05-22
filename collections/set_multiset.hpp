#pragma once
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <random>
#include <iostream>
#include "../tree/binary_tree/binary_tree_basic.hpp"
#include "../tree/binary_tree/binary_search_tree/AVL_balanced_tree.hpp"
#include "../tree/binary_tree/binary_search_tree/RB_balanced_tree.hpp"
namespace DSA
{
    namespace Collections
    {
        namespace SetOrMultiset
        {
            using DSA::Tree::BinaryTree::BinarySearchTree::BinarySearchTreeLinked;
            using DSA::Tree::BinaryTree::BinarySearchTree::AVLTree::AVLTreeLinked;
            using DSA::Tree::BinaryTree::BinarySearchTree::RBTree::RBTreeLinked;
            template <typename Key, typename Compare = std::less<Key>, template <typename, typename, typename> class Implement = RBTreeLinked>
            struct Set final
            {
                using key_type = Key;
                using value_type = key_type;
                using key_compare = Compare;
                using value_compare = key_compare;

                using Base = Implement<value_type, key_type, key_compare>;
                using size_type = Base::size_type;
                using pointer = Base::pointer;
                using const_pointer = Base::const_pointer;
                using reference = Base::reference;
                using const_reference = Base::const_reference;
                using const_iterator = Base::const_iterator;
                using iterator = const_iterator;
                using difference_type = Base::difference_type;
                using reverse_iterator = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;
                Set() = default;
                explicit Set(const value_compare &comp) : impl{comp} {};
                explicit Set(const Set &s) : impl(s.impl) {}
                ~Set() = default;
                Set &operator=(const Set &s) { impl = s.impl; }

                // iterators:
                iterator begin() { return impl.begin(); }
                const_iterator begin() const { return impl.begin(); }
                iterator end() { return impl.end(); }
                const_iterator end() const { return impl.end(); }

                reverse_iterator rbegin() { return reverse_iterator(end()); }
                const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
                reverse_iterator rend() { return reverse_iterator(begin()); };
                const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

                const_iterator cbegin() const { return begin(); }
                const_iterator cend() const { return end(); }
                const_reverse_iterator crbegin() const { return rbegin(); }
                const_reverse_iterator crend() const { return rend(); }

                // capacity:
                bool empty() const { return impl.empty(); }
                size_type size() const { return impl.size(); }

                std::pair<iterator, bool> insert(const value_type &v) { return impl.insert_unique(v); }

                iterator erase(const_iterator position) { return impl.erase_unique(position); }
                size_type erase(const key_type &k) { return impl.erase_unique(k); }
                // iterator erase(const_iterator first, const_iterator last);
                void clear() { impl.clear(); }

                void swap(Set &s)
                {
                    if (this != std::addressof(s))
                        std::swap(impl, s.impl);
                }

                // observers:
                key_compare key_comp() const { return impl.comparer(); };
                value_compare value_comp() const { return impl.comparer(); };

                // Set operations:
                iterator find(const key_type &k) { return impl.find(k); }
                const_iterator find(const key_type &k) const { return impl.find(k); };
                size_type count(const key_type &k) const { return impl.count_unique(k); };

                iterator lower_bound(const key_type &k) { return impl.lower_bound(k); }
                const_iterator lower_bound(const key_type &k) const { return impl.lower_bound(k); };

                iterator upper_bound(const key_type &k) { return impl.upper_bound(k); }
                const_iterator upper_bound(const key_type &k) const { return impl.upper_bound(k); };
                std::pair<iterator, iterator> equal_range(const key_type &k) { return impl.equal_range_unique(k); }
                std::pair<const_iterator, const_iterator> equal_range(const key_type &k) const { return impl.equal_range_unique(k); }

                bool checker() { return impl.checker_unique(); }

            protected:
                Base impl;
            };
            template <typename Key, typename Compare = std::less<Key>, template <typename, typename, typename> class Implement = RBTreeLinked>
            struct MultiSet final
            {
                using key_type = Key;
                using value_type = key_type;
                using key_compare = Compare;
                using value_compare = key_compare;

                using Base = Implement<value_type, key_type, key_compare>;
                using size_type = Base::size_type;
                using pointer = Base::pointer;
                using const_pointer = Base::const_pointer;
                using reference = Base::reference;
                using const_reference = Base::const_reference;
                using const_iterator = Base::const_iterator;
                using iterator = const_iterator;
                using difference_type = Base::difference_type;
                using reverse_iterator = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;
                MultiSet() = default;
                explicit MultiSet(const value_compare &comp) : impl{comp} {};
                MultiSet(const MultiSet &s) : impl(s.impl) {}
                ~MultiSet() = default;

                MultiSet &operator=(const MultiSet &s) { impl = s.impl; }

                // iterators:
                iterator begin() { return impl.begin(); }
                const_iterator begin() const { return impl.begin(); }
                iterator end() { return impl.end(); }
                const_iterator end() const { return impl.end(); }

                reverse_iterator rbegin() { return reverse_iterator(end()); }
                const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
                reverse_iterator rend() { return reverse_iterator(begin()); };
                const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

                const_iterator cbegin() const { return begin(); }
                const_iterator cend() const { return end(); }
                const_reverse_iterator crbegin() const { return rbegin(); }
                const_reverse_iterator crend() const { return rend(); }

                // capacity:
                bool empty() const { return impl.empty(); }
                size_type size() const { return impl.size(); }

                iterator insert(const value_type &v) { return impl.insert_multi(v); }

                iterator erase(const_iterator position) { return impl.erase_multi(position); }
                size_type erase(const key_type &k) { return impl.erase_multi(k); }
                // iterator erase(const_iterator first, const_iterator last);
                void clear() { impl.clear(); }

                void swap(MultiSet &s)
                {
                    if (this != std::addressof(s))
                        std::swap(impl, s.impl);
                }

                // observers:
                key_compare key_comp() const { return impl.comparer(); };
                value_compare value_comp() const { return impl.comparer(); };

                // MultiSet operations:
                iterator find(const key_type &k) { return impl.find(k); }
                const_iterator find(const key_type &k) const { return impl.find(k); };
                size_type count(const key_type &k) const { return impl.count_multi(k); };

                // bool contains(const key_type &x) const; // C++20
                // template <class K>
                // bool contains(const K &x) const; // C++20

                iterator lower_bound(const key_type &k) { return impl.lower_bound(k); }
                const_iterator lower_bound(const key_type &k) const { return impl.lower_bound(k); };

                iterator upper_bound(const key_type &k) { return impl.upper_bound(k); }
                const_iterator upper_bound(const key_type &k) const { return impl.upper_bound(k); };
                std::pair<iterator, iterator> equal_range(const key_type &k) { return impl.equal_range_multi(k); }
                std::pair<const_iterator, const_iterator> equal_range(const key_type &k) const { return impl.equal_range_multi(k); }

                bool checker() { return impl.checker_multi(); }

            protected:
                Base impl;
            };

        }
    }
}