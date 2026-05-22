#pragma once
#include "queue_stack.hpp"
#include "list_forwardlist.hpp"
#include "priority_queue.hpp"
#include "set_multiset.hpp"
#include "map_multimap.hpp"
#include "vector.hpp"
namespace DSA
{
    namespace Collections
    {
        using ListOrForwardlist::List;
        using DSA::Tree::Heap::PriorityQueue;
        using SetOrMultiset::Set;
        using SetOrMultiset::MultiSet;
        using MapOrMultimap::Map;
        using MapOrMultimap::MultiMap;
        using Queue=Queue;
        using Stack=Stack;
        using ArrayLike::Vector;
    }
}