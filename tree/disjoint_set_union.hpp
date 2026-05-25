#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "../vis_trace.hpp"

namespace DSA
{
    namespace Tree
    {
        namespace DisjointSetUnion
        {
            /**
             * @brief 并查集 (DSU) 或称联合-查找 (Union-Find) 数据结构。
             * @details
             * 并查集是一种用于处理不相交集合的合并与查询问题的数据结构。
             * 它主要支持两个操作：
             * 1. Find: 确定一个元素属于哪个集合（通过查找该集合的代表元）。
             * 2. Union: 将两个元素所在的集合合并成一个集合。
             * 常用于图论中判断连通性、求解最小生成树（Kruskal算法）等。
             */
            struct DSU
            {
                int number_of_node; // 集合中元素（节点）的总数。
                // parent[i] 存储节点 i 的父节点。根节点的父节点是它自己
                // size[i] 存储以 i 为根的集合的大小（元素数量）。
                // rnk[i] 存储以 i 为根的树的秩（一种高度的近似）。
                std::vector<int> parent, size, rnk;

                /**
                 * @brief 构造函数，初始化一个包含 n 个元素的并查集。
                 * @param n 元素的数量，从 1 到 n。
                 */
                DSU(int n = 0)
                {
                    n = std::max(n, 0); // 确保 n 不为负。
                    number_of_node = n;
                    // 初始化存储数组，大小为 n+1 以支持 1-based 索引。
                    parent = std::vector<int>(n + 1);
                    size = std::vector<int>(n + 1, 1);
                    rnk = std::vector<int>(n + 1);
                    // 最初，每个元素自成一个集合，其父节点就是它自己。
                    for (int i = 1; i <= n; i++)
                        parent[i] = i;
#ifdef DSA_VIS_ENABLE
                    constexpr const char *kVisObj = "U";
                    const void *vis_super_root = static_cast<const void *>(this);
                    DSA_VIS_TREE_INIT(kVisObj);                                 /*VIS*/
                    DSA_VIS_TREE_NEW_NODE(kVisObj, vis_super_root, "DSU", false); /*VIS*/
                    DSA_VIS_TREE_SET_ROOT(kVisObj, vis_super_root, false);      /*VIS*/
                    DSA_VIS_TREE_HIDE_NODE(kVisObj, vis_super_root, false);     /*VIS*/
                    for (int i = 1; i <= number_of_node; ++i)
                    {
                        DSA_VIS_TREE_NEW_NODE(kVisObj, vis_node_ref(i), i, false);                /*VIS*/
                        DSA_VIS_TREE_ADD_CHILD(kVisObj, vis_super_root, vis_node_ref(i), false);  /*VIS*/
                        DSA_VIS_TREE_SET_NOTE(kVisObj, vis_node_ref(i), vis_node_note(i), false); /*VIS*/
                    }
                    DSA_VIS_TREE_SET_NOTE(kVisObj, vis_super_root, "super-root", false); /*VIS*/
#endif
                    DSA_VIS_MSG(std::string("DSU 初始化：n=") + std::to_string(number_of_node), true); /*VIS*/
                }
                /**
                 * @brief 递归实现的 Find 操作，带有路径压缩优化。
                 * @param x 要查找的元素。
                 * @return int x 所在集合的代表元。
                 * @details 在查找过程中，将路径上所有节点的父指针直接指向根节点，
                 *          从而“压缩”路径，加速后续的查找操作。
                 */
                int FindRecursive(int x)
                {
                    // 如果 x 的父节点是它自己，那么 x 就是根（代表元）。
                    // 否则，递归地查找父节点的根，并把 x 的父节点直接设为这个根。
                    if (parent[x] == x)
                        return x;
                    int old_parent = parent[x];
                    int root = FindRecursive(old_parent);
                    if (parent[x] != root)
                    {
                        DSA_VIS_MSG(std::string("路径压缩：") + std::to_string(x) + " -> " + std::to_string(root), false); /*VIS*/
                        parent[x] = root;
                        DSA_VIS_TREE_ADD_CHILD("U", vis_node_ref(root), vis_node_ref(x), false);          /*VIS*/
                        DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(x), vis_node_note(x), false);             /*VIS*/
                    }
                    return root;
                }
                /**
                 * @brief 非递归（迭代）实现的 Find 操作，带有路径压缩优化。
                 * @param x 要查找的元素。
                 * @return int x 所在集合的代表元。
                 * @details 使用两轮循环实现：第一轮找到根节点，第二轮将路径上的所有节点指向根。
                 */
                int FindNonrecursive(int x)
                {
                    int t, p = parent[x];
                    // 第一轮：向上遍历直到找到根节点 p。
                    while (parent[p] != p)
                        p = parent[p];
                    // 第二轮：从 x 开始，再次向上遍历，将路径上所有节点的父节点都设为 p。
                    while (x != p)
                    {
                        t = parent[x];
                        if (parent[x] != p)
                        {
                            parent[x] = p;
                            DSA_VIS_TREE_ADD_CHILD("U", vis_node_ref(p), vis_node_ref(x), false);         /*VIS*/
                            DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(x), vis_node_note(x), false);         /*VIS*/
                        }
                        x = t;
                    }
                    return p;
                }
                // Find 操作的默认实现，调用递归版本。
                int Find(int x) { return FindRecursive(x); }
                // 随机合并两个集合，不使用任何优化策略。可能导致树退化成链状。
                void UnionRandomly(int x, int y)
                {
                    x = Find(x), y = Find(y);
                    if (x == y)
                    {
                        DSA_VIS_MSG(std::string("Union 跳过：") + std::to_string(x) + " 与 " + std::to_string(y) + " 已连通", true); /*VIS*/
                        return;
                    }
                    DSA_VIS_MSG(std::string("UnionRandomly：合并根 ") + std::to_string(x) + " -> " + std::to_string(y), true); /*VIS*/
                    DSA_VIS_TREE_MARK("U", vis_node_ref(x), false); /*VIS*/
                    DSA_VIS_TREE_MARK("U", vis_node_ref(y), false); /*VIS*/
                    parent[x] = y;
                    DSA_VIS_TREE_ADD_CHILD("U", vis_node_ref(y), vis_node_ref(x), true); /*VIS*/
                    size[y] += size[x];
                    rnk[y] = std::max(rnk[y], rnk[x] + 1);
                    DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(x), vis_node_note(x), false); /*VIS*/
                    DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(y), vis_node_note(y), false); /*VIS*/
                    DSA_VIS_TREE_UNMARK("U", vis_node_ref(x), false); /*VIS*/
                    DSA_VIS_TREE_UNMARK("U", vis_node_ref(y), false); /*VIS*/
                }
                /**
                 * @brief 按秩合并 (Union by Rank) 的 Union 操作。
                 * @param x, y 要合并的两个元素。
                 * @details 这是一种优化策略，总是将秩较小的树合并到秩较大的树上，
                 *          有助于保持树的平衡，避免树退化得过高。
                 */
                void UnionByRank(int x, int y)
                {
                    x = Find(x), y = Find(y);// 找到各自的根。
                    if (x == y)
                    {
                        DSA_VIS_MSG(std::string("UnionByRank 跳过：") + std::to_string(x) + " 与 " + std::to_string(y) + " 已连通", true); /*VIS*/
                        return;
                    }
                    DSA_VIS_MSG(std::string("UnionByRank：尝试合并根 ") + std::to_string(x) + " 与 " + std::to_string(y), true); /*VIS*/
                    DSA_VIS_TREE_MARK("U", vis_node_ref(x), false); /*VIS*/
                    DSA_VIS_TREE_MARK("U", vis_node_ref(y), false); /*VIS*/
                    if (rnk[x] > rnk[y])// 确保 x 是秩较小的树的根。
                        std::swap(x, y);
                    DSA_VIS_MSG(std::string("按秩合并：") + std::to_string(x) + " -> " + std::to_string(y), false); /*VIS*/
                    parent[x] = y;// 将秩小的树接到秩大的树下。
                    DSA_VIS_TREE_ADD_CHILD("U", vis_node_ref(y), vis_node_ref(x), true); /*VIS*/
                    size[y] += size[x];
                    // 如果两棵树的秩相同，合并后新树的秩需要加 1。
                    if (rnk[x] == rnk[y])
                        ++rnk[y];
                    DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(x), vis_node_note(x), false); /*VIS*/
                    DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(y), vis_node_note(y), false); /*VIS*/
                    DSA_VIS_TREE_UNMARK("U", vis_node_ref(x), false); /*VIS*/
                    DSA_VIS_TREE_UNMARK("U", vis_node_ref(y), false); /*VIS*/
                }
                
                /**
                 * @brief 按大小的启发式合并 (Union by Size) 的 Union 操作。
                 * @param x, y 要合并的两个元素。
                 * @details 另一种优化策略，总是将节点数较少的集合合并到节点数较多的集合上，效果与按秩合并类似
                 */
                void UnionBySize(int x, int y)
                {
                    x = Find(x), y = Find(y);// 找到各自的根。
                    if (x == y)
                    {
                        DSA_VIS_MSG(std::string("UnionBySize 跳过：") + std::to_string(x) + " 与 " + std::to_string(y) + " 已连通", true); /*VIS*/
                        return;
                    }
                    DSA_VIS_MSG(std::string("UnionBySize：尝试合并根 ") + std::to_string(x) + " 与 " + std::to_string(y), true); /*VIS*/
                    DSA_VIS_TREE_MARK("U", vis_node_ref(x), false); /*VIS*/
                    DSA_VIS_TREE_MARK("U", vis_node_ref(y), false); /*VIS*/
                    if (size[x] > size[y])// 确保 x 是较小集合的根。
                        std::swap(x, y);
                    DSA_VIS_MSG(std::string("按大小合并：") + std::to_string(x) + " -> " + std::to_string(y), false); /*VIS*/
                    parent[x] = y;// 将小集合接到大集合下。
                    DSA_VIS_TREE_ADD_CHILD("U", vis_node_ref(y), vis_node_ref(x), true); /*VIS*/
                    size[y] += size[x];// 更新合并后集合的大小。
                    rnk[y] = std::max(rnk[y], rnk[x] + 1);
                    DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(x), vis_node_note(x), false); /*VIS*/
                    DSA_VIS_TREE_SET_NOTE("U", vis_node_ref(y), vis_node_note(y), false); /*VIS*/
                    DSA_VIS_TREE_UNMARK("U", vis_node_ref(x), false); /*VIS*/
                    DSA_VIS_TREE_UNMARK("U", vis_node_ref(y), false); /*VIS*/
                }
                // Union 操作的默认实现，调用按秩合并版本。
                void Union(int x, int y)
                {
                    UnionByRank(x, y);
                }

            private:
#ifdef DSA_VIS_ENABLE
                const void *vis_node_ref(int x) const
                {
                    if (x < 1 || x > number_of_node)
                        return nullptr;
                    return static_cast<const void *>(&parent[x]);
                }
#endif

                std::string vis_node_note(int x) const
                {
                    if (x < 1 || x > number_of_node)
                        return std::string();
                    return std::string("p=") + std::to_string(parent[x]) +
                           ",s=" + std::to_string(size[x]) +
                           ",r=" + std::to_string(rnk[x]);
                }
            };
        }
    }
}
