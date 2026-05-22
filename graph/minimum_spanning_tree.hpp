#pragma once
#include <algorithm>
#include <string>
#include <sstream>
#include <exception>
#include "weighted_graph.hpp"
#include "../tree/disjoint_set_union.hpp"
#include "../utils.hpp"
namespace DSA
{
    namespace Graph
    {
        namespace SpanningTree
        {

            using DSA::Utils::Infinity;
            using GraphWeighted::WeightedAdjoint;
            using GraphWeighted::WeightedEdge;
            using GraphWeighted::WGraph;
            using Tree::DisjointSetUnion::DSU;

            namespace MinimumSpanningTree
            {
                /**
                 * @brief 使用 Kruskal 算法计算图的最小生成树 (MST)。
                 * @tparam T 边权重的类型，默认为 int。
                 * @param g 输入的加权图。
                 * @param sum 输出参数，MST 的总权重。
                 * @param chosen 输出参数，一个向量，存储构成 MST 的边的索引。
                 * @return bool 如果成功找到一个生成树 (即图是连通的)，则返回 true；否则返回 false。
                 *
                 * @details
                 * Kruskal 是一种贪心算法。其核心思想是：
                 * 1. 将图中所有的边按权重从小到大排序。
                 * 2. 依次遍历排序后的边。
                 * 3. 对于每条边，如果将它加入到当前的生成树中不会形成环，就选择这条边。
                 * 4. 重复步骤 3，直到选择了 n-1 条边（n是节点数）。
                 * 该算法使用并查集 (DSU) 来高效地检测环。
                 */
                template <typename T = int>
                bool Kruskal(const WGraph<T> &g, T &sum, std::vector<T> &chosen)
                {
                    chosen.clear();            // 清空结果向量，以防之前有数据。
                    DSU dsu(g.number_of_node); // 初始化并查集，每个节点自成一个T集合。
                    // 创建一个存储边索引的向量，用于排序，而不直接排序原始的边列表。
                    std::vector<int> sortedE(g.E.size());
                    for (int i = 0; i < sortedE.size(); i++)
                        sortedE[i] = i;
                    // 使用 lambda 表达式按边的权重对索引进行升序排序。
                    std::sort(sortedE.begin(), sortedE.end(), [&](int x, int y)
                              { return g.E[x].w < g.E[y].w; });
                    sum = 0; // 初始化总权重为 0。

                    // 遍历排序后的边。
                    for (auto id : sortedE)
                    {
                        auto e = g.E[id];
                        // 查找边 e 的两个端点 u 和 v 所在的集合的代表元。
                        int pu = dsu.Find(e.u), pv = dsu.Find(e.v);
                        // 如果两个端点已经在同一个集合中，说明加入这条边会形成环，因此跳过。
                        if (pu == pv)
                            continue;
                        // 如果不在同一个集合，则选择这条边。
                        sum += e.w;           // 将边的权重累加到总权重中。
                        dsu.Union(pu, pv);    // 合并两个端点所在的集合。
                        chosen.push_back(id); // 将这条边的索引加入到结果集中。

                        // 优化：如果已经找到了 n-1 条边，那么 MST 就已经构建完成，可以提前结束循环
                        if (chosen.size() == g.number_of_node - 1)
                            break;
                    }
                    // 如果最终选择的边数等于 n-1，说明原图是连通的，成功找到了 MST。
                    return chosen.size() == g.number_of_node - 1;
                }

                /**
                 * @brief 使用 Prim 算法计算图的最小生成树 (MST)。
                 * @tparam T 边权重的类型，默认为 int。
                 * @param g 输入的加权图。
                 * @param sum 输出参数，MST 的总权重。
                 * @param chosen 输出参数，一个向量，存储构成 MST 的边的索引。
                 * @return bool 如果成功找到一个生成树，则返回 true；否则返回 false。
                 *
                 * @details
                 * Prim 算法也是一种贪心算法，它从一个初始节点开始，逐步扩大一棵树。
                 * 1. 维护两个集合：一个包含已在生成树中的节点，另一个包含尚未加入的节点。
                 * 2. 每次选择一条权重最小的边，其一个端点在树内，另一个端点在树外。
                 * 3. 将这条边和它连接的树外节点加入到生成树中。
                 * 4. 重复步骤 2 和 3，直到所有节点都加入树中。
                 * 这个实现使用了优先队列来高效地找到符合条件的最小权重边。
                 */
                template <typename T = int>
                bool Prim(const WGraph<T> &g, T &sum, std::vector<int> &chosen)
                {
                    chosen.clear();
                    // dis[v] 存储将节点 v 连接到当前生成树所需的最小边权重。inf 表示 v 还未被触及。
                    std::vector<T> dis(g.number_of_node + 1, Infinity<T>());
                    // 定义用于优先队列的节点结构。
                    struct lazy_node
                    {
                        int node_id; // 节点索引
                        T lazy_dis;  // 连接到该节点的边的权重
                        // 重载小于运算符，使得 std::priority_queue 成为一个最小堆。
                        bool operator<(const lazy_node &U) const { return lazy_dis > U.lazy_dis; }
                    };
                    std::priority_queue<lazy_node> heap; // 创建最小优先队列。
                    // last_updated_edge[v] 存储连接节点 v 到树的那条最小权边的索引。
                    std::vector<int> last_updated_edge(g.number_of_node + 1, -1);
                    dis[1] = 0;        // 从节点 1 开始算法。
                    heap.push({1, 0}); // 将起始节点加入堆中，连接它的“边”权重为0。
                    while (!heap.empty())
                    {
                        int u = heap.top().node_id; // 从堆中取出离树最近的节点 u。
                        heap.pop();
                        dis[u] = 0; // 标记 u 已加入 MST (将距离设为0)。

                        // 遍历 u 的所有邻居 v。
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            // 如果 v 尚未加入 MST (dis[v]==inf) 或者找到了更短的边来连接 v
                            if (e.weight < dis[v])
                            {
                                // 更新连接 v 的最小边权重和边的索引。
                                heap.push({v, (dis[v] = e.weight)});
                                last_updated_edge[v] = e.edge_index;
                            }
                        }
                        // “懒惰删除”：丢弃堆中过时的条目，即那些节点的距离已经被更短的边更新过。
                        while (!heap.empty() && dis[heap.top().node_id] != heap.top().lazy_dis)
                            heap.pop();
                    }
                    sum = 0; // 初始化总权重。
                    // 遍历 last_updated_edge 数组来构建最终的 MST。
                    for (auto c : last_updated_edge)
                        if (c != -1) // -1 表示该节点是起始节点或不可达。
                        {
                            chosen.push_back(c); // 添加边索引到结果集。
                            sum += g.E[c].w;     // 累加权重。
                        }
                    // 如果最终选择的边数等于 n-1，说明原图是连通的，成功找到了 MST。
                    return chosen.size() == g.number_of_node - 1;
                }

            }
        }
    }
}