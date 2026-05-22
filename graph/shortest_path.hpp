#pragma once
#include <algorithm>
#include <string>
#include <exception>
#include <limits>
#include <sstream>
#include "weighted_graph.hpp"
#include "../utils.hpp"
namespace DSA
{
    namespace Graph
    {
        namespace ShortestPath
        {
            using GraphWeighted::WeightedAdjoint;
            using GraphWeighted::WeightedEdge;
            using GraphWeighted::WGraph;
            using DSA::Utils::Infinity;

            namespace AllPairs
            {
                /**
                 * @brief 使用 Floyd-Warshall 算法计算所有节点对之间的最短路径。
                 * @tparam T 边权重的类型，默认为 int。
                 * @param g 输入的加权图。
                 * @param dis 输出参数，一个二维向量（邻接矩阵），存储任意两点间的最短距离。
                 * @return bool 如果图中存在负权环，则返回 true；否则返回 false。
                 *
                 * @details
                 * Floyd-Warshall 是一种动态规划算法，时间复杂度为 O(n^3)，n 是节点数。
                 * 它可以处理带负权的边，并且可以检测出负权环的存在。
                 * 算法核心思想：对于每一对顶点 i 和 j，检查是否存在一个顶点 k，使得从 i到k再到j的路径比已知的 i到j 的路径更短。
                 */
                template <typename T = int>
                bool Floyd(const WGraph<T> &g, std::vector<std::vector<T>> &dis)
                {
                    int n = g.number_of_node; // 获取图中的节点数量。

                    // 初始化距离矩阵 'dis'。
                    // 所有距离都设为无穷大，表示初始时节点间不连通。
                    dis = std::vector<std::vector<T>>(n + 1, std::vector<T>(n + 1, Infinity<T>()));
                    // 任何节点到其自身的距离为 0。
                    for (int i = 1; i <= n; i++)
                        dis[i][i] = 0;
                    // 根据图的邻接表，初始化距离矩阵的直接路径。
                    for (int i = 1; i <= n; i++)
                        for (auto e : g.adj[i])
                        {
                            int j = e.adjvex;
                            // 如果图中存在平行边（两条或更多连接相同两个节点的边），取权重最小的那条。
                            dis[i][j] = std::min(dis[i][j], e.weight);
                        }
                    // 核心的三重循环。
                    for (int k = 1; k <= n; k++)                                            // k 是作为中间节点的“中转站”
                        for (int i = 1; i <= n; i++)                                        // i 是路径的起点
                            for (int j = 1; j <= n; j++)                                    // j 是路径的终点
                                if (dis[i][k] < Infinity<T>() && dis[k][j] < Infinity<T>()) // 如果从 i 到 k 和从 k 到 j 的路径都存在（距离不是无穷大）
                                    dis[i][j] = std::min(dis[i][j], dis[i][k] + dis[k][j]); // 更新 dis[i][j]：取 (原来的 i到j 的距离) 和 (经由k中转的距离) 中的较小者;这就是 Floyd 算法的松弛操作。
                    // 检测负权环
                    bool reach_negative_ring = false;
                    for (int i = 1; i <= n; i++)
                        reach_negative_ring = reach_negative_ring || dis[i][i] < 0; // 如果一个节点到其自身的距离变为负数，说明从这个节点出发可以到达一个负权环。
                    return reach_negative_ring;
                }
            }
            namespace SingleSource
            {
                /**
                 * @brief 使用 Bellman-Ford 算法计算从单个源点 s 到所有其他节点的最短路径。
                 * @tparam T 边权重的类型，默认为 int。
                 * @param g 输入的加权图。
                 * @param s 源节点的索引（从 1 开始）。
                 * @param dis 输出参数，一个向量，存储源点 s 到各点的最短距离。
                 * @return bool 如果图中存在从源点 s 可达的负权环，则返回 true；否则返回 false。
                 *
                 * @details
                 * Bellman-Ford 算法能处理带负权重的边。时间复杂度为 O(n*m)，n是节点数, m是边数。
                 * 算法思想：对图中的所有边进行 n-1 轮松弛操作。因为一条不含环的最短路径最多有 n-1 条边。
                 * 如果在 n-1 轮后仍能进行松弛，则说明图中存在负权环。
                 */
                template <typename T = int>
                bool BellmanFord(const WGraph<T> &g, int s, std::vector<T> &dis)
                {
                    int n = g.number_of_node; // 获取节点数。

                    // 初始化距离向量 'dis'。所有距离设为无穷大。
                    dis = std::vector<T>(n + 1, Infinity<T>());
                    if (s < 1 || s > n)
                        return false;
                    // 源点到自身的距离为 0。
                    dis[s] = 0;
                    bool relaxed = false; // 标记在一轮循环中是否发生了松弛操作。

                    // 主循环，最多执行 n 轮。
                    for (int i = 1; i <= n; i++)
                    {
                        relaxed = false;
                        // 遍历图中的每一条边。
                        for (auto e : g.E)
                        {
                            // 对边 (u, v) 进行松弛操作。
                            // 如果从源点到 u 的距离已知，并且通过 u 到达 v 的路径更短...
                            if (dis[e.u] < Infinity<T>() && dis[e.u] + e.w < dis[e.v])
                                dis[e.v] = dis[e.u] + e.w, relaxed = true; // ...则更新到 v 的最短距离，标记发生了松弛。

                            // 如果是无向图，还需要对反向边 (v, u) 进行松弛。
                            if (!g.is_directed)
                                if (dis[e.v] < Infinity<T>() && dis[e.v] + e.w < dis[e.u])
                                    dis[e.u] = dis[e.v] + e.w, relaxed = true;
                        }
                        // 优化：如果在某一轮中没有发生任何松弛操作，说明最短路径已经找到，可以提前退出。
                        if (!relaxed)
                            break;
                    }
                    // 在 n-1 轮之后，`relaxed` 仍然为 true，说明在第 n 轮循环中仍然发生了松弛。
                    // 这只有在图中存在负权环时才会发生。
                    bool reach_negative_ring = relaxed;
                    return reach_negative_ring;
                }
                /**
                 * @brief 使用队列优化的 Bellman-Ford 算法 (SPFA) 计算单源最短路径。
                 * @tparam T 边权重的类型，默认为 int。
                 * @param g 输入的加权图。
                 * @param s 源节点的索引（从 1 开始）。
                 * @param dis 输出参数，一个向量，存储源点 s 到各点的最短距离。
                 * @return bool 如果图中存在从源点 s 可达的负权环，则返回 true；否则返回 false。
                 *
                 * @details
                 * SPFA (Shortest Path Faster Algorithm) 是对 Bellman-Ford 的优化。
                 * 它不盲目地对所有边进行松弛，而是维护一个队列，只将被成功松弛的节点的邻居加入队列中。
                 * 在随机数据上平均时间复杂度优于 Bellman-Ford，约为 O(m)，但最坏情况下仍为 O(n*m)。
                 */
                template <typename T = int>
                bool BellmanFord_Queue(const WGraph<T> &g, int s, std::vector<T> &dis)
                {
                    int n = g.number_of_node;
                    dis = std::vector<T>(n + 1, Infinity<T>());
                    if (s < 1 || s > n)
                        return false;
                    dis[s] = 0;
                    // cnt[i] 记录从源点 s 到节点 i 的最短路径所包含的边数。
                    std::vector<int> cnt(n + 1);
                    // q 是一个先进先出的队列，存放待处理的节点。
                    std::queue<int> q;
                    // inq[i] 标记节点 i 当前是否在队列中，以避免重复添加。
                    std::vector<int> inq(n + 1); // avoid using std::vector<bool> which never store bool
                    // 算法从源点 s 开始。
                    q.push(s);
                    inq[s] = true;
                    bool reach_negative_ring = false;
                    while (!q.empty())
                    {
                        // 取出队首节点 u。
                        int u = q.front();
                        q.pop();
                        inq[u] = false;
                        // 遍历从 u 出发的所有边，进行松弛。
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            // 如果通过 u 可以找到一条到 v 的更短路径...
                            if (dis[u] + e.weight < dis[v])
                            {
                                // ...更新到 v 的距离。
                                dis[v] = dis[u] + e.weight;
                                // 更新到 v 的路径边数。
                                cnt[v] = cnt[u] + 1;
                                // 如果 v 不在队列中，则将其入队。
                                if (!inq[v])
                                {
                                    inq[v] = true;
                                    q.push(v);
                                }
                                // 负权环检测：如果到 v 的最短路径边数 >= n，
                                // 说明这条路径上必然有环（鸽巢原理），且该环为负权环。
                                if (cnt[v] >= n)
                                {
                                    reach_negative_ring = true;
                                    break;
                                }
                            }
                        }
                        if (reach_negative_ring)
                            break;
                    }
                    return reach_negative_ring;
                }

                /**
                 * @brief 使用 Dijkstra 算法计算单源最短路径。
                 * @tparam T 边权重的类型，默认为 int。
                 * @param g 输入的加权图。图中不能有负权边。
                 * @param s 源节点的索引（从 1 开始）。
                 * @param dis 输出参数，一个向量，存储源点 s 到各点的最短距离。
                 * @return bool 如果图中存在负权边，则返回 true (表示算法不适用)；否则返回 false。
                 *
                 * @details
                 * Dijkstra 是一种贪心算法，用于解决无负权边的单源最短路径问题。
                 * 时间复杂度通常为 O(m * log n)，其中 m 是边数，n 是节点数（使用优先队列实现）。
                 * 核心思想：维护一个已找到最短路径的节点集合，每次从未处理的节点中选择距离源点最近的一个加入集合，并更新其邻居的距离。
                 * 这个实现使用了优先队列（最小堆）和“懒惰删除”技巧。
                 */
                template <typename T = int>
                bool Dijkstra(const WGraph<T> &g, int s, std::vector<T> &dis)
                {
                    int n = g.number_of_node;
                    dis = std::vector<T>(n + 1, Infinity<T>());
                    if (s < 1 || s > n)
                        return false;
                    dis[s] = 0;

                    // Dijkstra 算法的前提是图中没有负权边。
                    // 此处检查是否存在负权边。
                    bool has_negative_edge = false;
                    for (auto e : g.E)
                        if (e.w < 0)
                            return has_negative_edge = true; // 如果找到负权边，算法无法保证正确性，直接返回。

                    // 定义用于优先队列的节点结构
                    struct lazy_node
                    {
                        int node_index;// 节点索引
                        T lazy_dis; // 从源点到该节点的“懒惰”距离（即存入堆时的距离）
                        // 重载小于运算符，使得 std::priority_queue 成为一个最小堆， 距离小的节点优先级更高。
                        bool operator<(const lazy_node &U) const { return lazy_dis > U.lazy_dis; }
                    };
                    // 创建一个最小优先队列（最小堆）
                    std::priority_queue<lazy_node> heap;
                    // 将源点 s 加入优先队列。
                    heap.push({s, dis[s]});
                    while (!heap.empty())
                    {
                        // 从优先队列中取出当前距离源点最近的节点 u。
                        int u = heap.top().node_index;
                        heap.pop();
                        // 遍历 u 的所有邻居 v。
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            // 松弛操作：如果通过 u 能找到到 v 的更短路径...
                            if (dis[u] + e.weight < dis[v])
                            {
                                // ...更新到 v 的最短距离。
                                dis[v] = dis[u] + e.weight;
                                // 将更新后的 v（及其新距离）加入优先队列。
                                // 注意：这里没有删除堆中可能存在的旧的、更长的 (v, old_dis) 条目。
                                heap.push({v, dis[v]});
                            }
                        }
                        // “懒惰删除”：这是一个使用标准库时的技巧。
                        // 在取出下一个节点之前，检查堆顶元素的距离是否是其最新距离。
                        // 如果堆顶节点的 stored_dis 与其在 dis 数组中的最新距离不符，
                        // 说明这个堆顶条目是过时的（已经被更短的路径更新过），直接丢弃。
                        // 重复此过程，直到堆顶是有效条目或堆为空。
                        while (!heap.empty() && dis[heap.top().node_index] != heap.top().lazy_dis)
                            heap.pop();
                    }

                    return has_negative_edge;
                }
            }

        }
    }
}