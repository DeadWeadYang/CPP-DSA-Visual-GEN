#pragma once
#include <queue>
#include <vector>
namespace DSA
{
    namespace Collections
    {
        template <class T, class Container = std::deque<T>>
        class Queue
        {
        public:
            using container_type = Container;
            using value_type = container_type::value_type;
            using reference = container_type::reference;
            using const_reference = container_type::const_reference;
            using size_type = container_type::size_type;

        protected:
            container_type c;

        public:
            Queue() = default;
            ~Queue() = default;

            Queue(const Queue &q) = default;

            Queue &operator=(const Queue &q) = default;

            explicit Queue(const container_type &_c) : c(_c) {}

            bool empty() const { return c.empty(); }
            size_type size() const { return c.size(); }

            reference front() { return c.front(); }
            const_reference front() const { return c.front(); }
            reference back() { return c.back(); }
            const_reference back() const { return c.back(); }

            void push(const value_type &v) { c.push_back(v); }
            void pop() { c.pop_front(); }

            void swap(queue &q) { std::swap(c, q.c); }
        };

        template <class T, class Container = std::vector<T>>
        class Stack
        {
        public:
            using container_type = Container;
            using value_type = container_type::value_type;
            using reference = container_type::reference;
            using const_reference = container_type::const_reference;
            using size_type = container_type::size_type;

        protected:
            container_type c;

        public:
            Stack() = default;
            ~Stack() = default;

            Stack(const Stack &q) = default;

            stack &operator=(const Stack &q) = default;

            explicit Stack(const container_type &_c) : c(_c) {}

            bool empty() const { return c.empty(); }
            size_type size() const { return c.size(); }
            reference top() { return c.back(); }
            const_reference top() const { return c.back(); }

            void push(const value_type &x) { c.push_back(x); }
            void pop() { c.pop_back(); }

            void swap(Stack &q)
            {
                std::swap(c, q.c);
            }
        };
    }
}