#pragma once
#include <type_traits>
#include <utility>
#include <cstddef>
#include <random>
#include <queue>
#include <exception>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include "binary_heap.hpp"

namespace DSA
{
    namespace Tree
    {
        namespace Heap
        {

            // 1. Detection idiom: attempt to call the required members
            template <typename, typename = void>
            struct is_heap_impl : std::false_type
            {
            };

            template <typename H>
            struct is_heap_impl<H,
                                std::void_t<
                                    // these expressions must be well-formed
                                    decltype(std::declval<H &>().empty()),                                              // bool empty() const
                                    decltype(std::declval<H &>().size()),                                               // size_t size() const
                                    decltype(std::declval<H &>().push(std::declval<typename H::value_type const &>())), // void push(const T&)
                                    decltype(std::declval<H &>().pop()),                                                // void pop()
                                    decltype(std::declval<H &>().top())                                                 // const T& top() const
                                    >> : std::true_type
            {
            };
            template <typename H, typename Cmp>
            constexpr bool has_cmp_ctor_v =
                std::is_constructible<H, Cmp>::value;

            // 2. The priority_queue adapter
            template <
                typename T,
                typename Compare = std::less<T>,
                template <typename, typename> class HeapImpl = BinaryHeap::BinaryHeap // see below
                >

            struct PriorityQueue
            {
                using impl_t = HeapImpl<T, Compare>;
                static_assert(is_heap_impl<impl_t>::value,
                              "HeapImpl<T,Compare> must provide: empty(), size(), push(), pop(), top()");
                static_assert(has_cmp_ctor_v<impl_t, Compare>,
                              "HeapImpl<T,Compare> must be constructible from (Compare)");

            public:
                using value_type = T;
                using size_type = std::size_t;
                using reference = T &;
                using const_reference = T const &;

            private:
                impl_t impl_;

            public:
                // ctor: take comparator, forward to heap if it needs one
                explicit PriorityQueue(const Compare &cmp = Compare{})
                    : impl_{cmp}
                {
                }
                template <typename InputIt>
                PriorityQueue(InputIt first, InputIt last, Compare cmp = Compare{})
                    : PriorityQueue{cmp}
                {
                    for (; first != last; ++first)
                        impl_.push(*first);
                }

                bool empty() const { return impl_.empty(); }
                size_type size() const { return impl_.size(); }
                const_reference top() const { return impl_.top(); }

                void push(const value_type &v) { impl_.push(v); }
                void pop() { impl_.pop(); }
            };

        }
    }
}