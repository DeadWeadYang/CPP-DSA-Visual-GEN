#pragma once
#include <algorithm>
#include <string>
#include <exception>
#include <limits>
#include <queue>
#include <sstream>
#include "weighted_graph.hpp"
#include "graph_vis.hpp"
#include "../utils.hpp"
#include "../vis_trace.hpp"
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
                    DSA_VIS_DECL(auto as_string = [](const auto &x)
                    {
                        std::ostringstream oss;
                        oss << x;
                        return oss.str();
                    };)
                    DSA_VIS_ONLY({
                        DSA_VIS_G_INIT("G", g.is_directed);
                        for (int i = 1; i <= n; ++i)
                        {
                            DSA_VIS_G_NEW_NODE("G", DSA_VIS_NODE(i), i, false);
                            DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(i), "black", false);
                        }
                        for (auto e : g.E)
                        {
                            DSA_VIS_G_NEW_EDGE("G", DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), e.w, false);
                            DSA_VIS_G_SET_EDGE_STYLE("G", DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "#64748b", 2, "", false);
                        }
                    });
                    /*VIS*/ DSA_VIS_G_LAYOUT("G", false);
                    /*VIS*/ DSA_VIS_STATE_INIT_MATRIX_LABELED("DIS", n, n, "DIS[i][j] shortest distance from i to j", 1, 1, "row = source i, column = target j", false);
                    DSA_VIS_ONLY({
                        for (int ii = 1; ii <= n; ++ii)
                            for (int jj = 1; jj <= n; ++jj)
                            {
                                std::string cell = (dis[ii][jj] >= Infinity<T>()) ? std::string("INF") : as_string(dis[ii][jj]);
                                DSA_VIS_STATE_SET("DIS", ii - 1, jj - 1, cell, false);
                                DSA_VIS_STATE_HISTORY("DIS", ii - 1, jj - 1, cell, "init", false);
                            }
                    });
                    /*VIS*/ DSA_VIS_STEP("Floyd 初始化完成：距离矩阵记录所有直接边和自身距离");
                    // 核心的三重循环。
                    for (int k = 1; k <= n; k++)                                            // k 是作为中间节点的“中转站”
                    {
                        /*VIS*/ DSA_VIS_G_MARK_NODE("G", DSA_VIS_NODE(k), false);
                        /*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(k), "blue", false);
                        /*VIS*/ DSA_VIS_STEP("Floyd：枚举中转点 k=" + as_string(k));
                        for (int i = 1; i <= n; i++)                                        // i 是路径的起点
                            for (int j = 1; j <= n; j++)                                    // j 是路径的终点
                                if (dis[i][k] < Infinity<T>() && dis[k][j] < Infinity<T>()) // 如果从 i 到 k 和从 k 到 j 的路径都存在（距离不是无穷大）
                                {
                                    /*VIS*/ DSA_VIS_STATE_CLEAR_FOCUS("DIS", false);
                                    /*VIS*/ DSA_VIS_STATE_FOCUS("DIS", i - 1, j - 1, "write", false);
                                    /*VIS*/ DSA_VIS_STATE_FOCUS("DIS", i - 1, k - 1, "read", false);
                                    /*VIS*/ DSA_VIS_STATE_FOCUS("DIS", k - 1, j - 1, "candidate", false);
                                    T old_dis = dis[i][j];
                                    dis[i][j] = std::min(dis[i][j], dis[i][k] + dis[k][j]); // 更新 dis[i][j]：取 (原来的 i到j 的距离) 和 (经由k中转的距离) 中的较小者;这就是 Floyd 算法的松弛操作。
                                    if (dis[i][j] != old_dis)
                                    {
                                        DSA_VIS_DECL(std::string new_dis = as_string(dis[i][j]);)
                                        DSA_VIS_ONLY(DSA_VIS_STATE_SET("DIS", i - 1, j - 1, new_dis, false));
                                        DSA_VIS_ONLY(DSA_VIS_STATE_HISTORY("DIS", i - 1, j - 1, new_dis, "via k=" + as_string(k), false));
                                        /*VIS*/ DSA_VIS_STEP(
                                            "Floyd 更新 dist[" + as_string(i) + "][" + as_string(j) + "]："
                                                + (old_dis >= Infinity<T>() ? std::string("INF") : as_string(old_dis))
                                                + " -> " + new_dis + "，因为经过 k=" + as_string(k) + " 更短");
                                    }
                                    else
                                    {
                                        /*VIS*/ DSA_VIS_STEP(
                                            "Floyd 比较 dist[" + as_string(i) + "][" + as_string(j) + "]：经过 k=" + as_string(k) + " 没有更短");
                                    }
                                }
                        /*VIS*/ DSA_VIS_G_UNMARK_NODE("G", DSA_VIS_NODE(k), false);
                        /*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(k), "gray", false);
                    }
                    // 检测负权环
                    bool reach_negative_ring = false;
                    for (int i = 1; i <= n; i++)
                        reach_negative_ring = reach_negative_ring || dis[i][i] < 0; // 如果一个节点到其自身的距离变为负数，说明从这个节点出发可以到达一个负权环。
                    /*VIS*/ DSA_VIS_STATE_CLEAR_FOCUS("DIS", false);
                    /*VIS*/ DSA_VIS_STEP(reach_negative_ring ? "Floyd 结束：检测到负权环" : "Floyd 结束：距离矩阵已经稳定");
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
                    DSA_VIS_ONLY(Vis::InitWeightedGraph(g));
                    DSA_VIS_ONLY(Vis::InitDistanceState("DIST", dis, n));
                    DSA_VIS_ONLY(Vis::SetNodeColor("G", s, "blue"));
                    DSA_VIS_ONLY(Vis::SetNodeLabel("G", s, Vis::ToString(s) + "\nd=0"));
                    /*VIS*/ DSA_VIS_STEP("Bellman-Ford 初始化完成：源点 " + Vis::ToString(s) + " 距离为 0");

                    // 主循环，最多执行 n 轮。
                    for (int i = 1; i <= n; i++)
                    {
                        relaxed = false;
                        /*VIS*/ DSA_VIS_STEP("Bellman-Ford 第 " + Vis::ToString(i) + " 轮：扫描所有边并尝试松弛");
                        // 遍历图中的每一条边。
                        for (auto e : g.E)
                        {
                            auto relax_edge = [&](int from, int to)
                            {
                                DSA_VIS_ONLY(Vis::FocusRelaxation("DIST", from, to));
                                // 如果从源点到 from 的距离已知，并且通过 from 到达 to 的路径更短...
                                if (dis[from] < Infinity<T>() && dis[from] + e.w < dis[to])
                                {
                                    T old_dis = dis[to];
                                    dis[to] = dis[from] + e.w;
                                    relaxed = true;
                                    DSA_VIS_ONLY(Vis::SetDistance("DIST", to, dis[to], "via " + Vis::ToString(from)));
                                    DSA_VIS_ONLY(Vis::StyleEdge("G", e.u, e.v, "#16a34a", 3));
                                    DSA_VIS_ONLY(Vis::SetNodeLabel("G", to, Vis::ToString(to) + "\nd=" + Vis::ToString(dis[to])));
                                    DSA_VIS_ONLY(Vis::SetNodeColor("G", to, "green"));
                                    /*VIS*/ DSA_VIS_STEP(
                                        "Bellman-Ford 松弛边 " + Vis::ToString(from) + "->" + Vis::ToString(to) + "：dist[" + Vis::ToString(to) + "] "
                                            + Vis::DistanceText(old_dis) + " -> " + Vis::ToString(dis[to]));
                                    return;
                                }
                                /*VIS*/ DSA_VIS_STEP("Bellman-Ford 检查边 " + Vis::ToString(from) + "->" + Vis::ToString(to) + "：未改善最短路");
                            };
                            DSA_VIS_ONLY(Vis::MarkEdge("G", e.u, e.v, e.w, "#2563eb"));
                            // 对边 (u, v) 进行松弛操作。
                            relax_edge(e.u, e.v);

                            // 如果是无向图，还需要对反向边 (v, u) 进行松弛。
                            if (!g.is_directed)
                                relax_edge(e.v, e.u);
                            DSA_VIS_ONLY(Vis::UnmarkEdge("G", e.u, e.v));
                            DSA_VIS_ONLY(Vis::StyleEdge("G", e.u, e.v, "#64748b", 2));
                        }
                        // 优化：如果在某一轮中没有发生任何松弛操作，说明最短路径已经找到，可以提前退出。
                        if (!relaxed)
                        {
                            /*VIS*/ DSA_VIS_STEP("Bellman-Ford 本轮没有任何更新，提前结束");
                            break;
                        }
                    }
                    // 在 n-1 轮之后，`relaxed` 仍然为 true，说明在第 n 轮循环中仍然发生了松弛。
                    // 这只有在图中存在负权环时才会发生。
                    bool reach_negative_ring = relaxed;
                    DSA_VIS_ONLY(Vis::ClearFocus("DIST"));
                    /*VIS*/ DSA_VIS_STEP(reach_negative_ring ? "Bellman-Ford 检测到负权环" : "Bellman-Ford 结束：最短距离已稳定");
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
                    DSA_VIS_ONLY(Vis::InitWeightedGraph(g));
                    DSA_VIS_ONLY(Vis::InitDistanceState("DIST", dis, n));
                    DSA_VIS_ONLY(Vis::InitValueState("CNT", cnt, n));
                    DSA_VIS_ONLY(Vis::SeqClear("QUEUE"));
                    DSA_VIS_ONLY(Vis::SeqPush("QUEUE", s));
                    DSA_VIS_ONLY(Vis::SetNodeColor("G", s, "blue"));
                    DSA_VIS_ONLY(Vis::SetNodeLabel("G", s, Vis::ToString(s) + "\nd=0"));
                    /*VIS*/ DSA_VIS_STEP("SPFA 初始化完成：源点 " + Vis::ToString(s) + " 入队，距离为 0");
                    bool reach_negative_ring = false;
                    while (!q.empty())
                    {
                        // 取出队首节点 u。
                        int u = q.front();
                        q.pop();
                        inq[u] = false;
                        DSA_VIS_ONLY(Vis::SeqPop("QUEUE"));
                        DSA_VIS_ONLY(Vis::MarkNode("G", u, "blue"));
                        /*VIS*/ DSA_VIS_STEP("SPFA 出队节点 " + Vis::ToString(u) + "，扫描它的出边");
                        // 遍历从 u 出发的所有边，进行松弛。
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            DSA_VIS_ONLY(Vis::MarkEdge("G", u, v, e.weight, "#2563eb"));
                            DSA_VIS_ONLY(Vis::FocusRelaxation("DIST", u, v));
                            // 如果通过 u 可以找到一条到 v 的更短路径...
                            if (dis[u] + e.weight < dis[v])
                            {
                                // ...更新到 v 的距离。
                                T old_dis = dis[v];
                                dis[v] = dis[u] + e.weight;
                                // 更新到 v 的路径边数。
                                cnt[v] = cnt[u] + 1;
                                // 如果 v 不在队列中，则将其入队。
                                if (!inq[v])
                                {
                                    inq[v] = true;
                                    q.push(v);
                                    DSA_VIS_ONLY(Vis::SeqPush("QUEUE", v));
                                }
                                DSA_VIS_ONLY(Vis::SetDistance("DIST", v, dis[v], "via " + Vis::ToString(u)));
                                DSA_VIS_ONLY(Vis::SetValue("CNT", v, cnt[v], "via " + Vis::ToString(u)));
                                DSA_VIS_ONLY(Vis::StyleEdge("G", u, v, "#16a34a", 3));
                                DSA_VIS_ONLY(Vis::SetNodeLabel("G", v, Vis::ToString(v) + "\nd=" + Vis::ToString(dis[v])));
                                DSA_VIS_ONLY(Vis::SetNodeColor("G", v, "green"));
                                /*VIS*/ DSA_VIS_STEP(
                                    "SPFA 松弛边 " + Vis::ToString(u) + "->" + Vis::ToString(v) + "：dist[" + Vis::ToString(v) + "] "
                                        + Vis::DistanceText(old_dis) + " -> " + Vis::ToString(dis[v]) + "，必要时加入候选队列");
                                // 负权环检测：如果到 v 的最短路径边数 >= n，
                                // 说明这条路径上必然有环（鸽巢原理），且该环为负权环。
                                if (cnt[v] >= n)
                                {
                                    reach_negative_ring = true;
                                    /*VIS*/ DSA_VIS_STEP("SPFA：节点 " + Vis::ToString(v) + " 的路径边数达到 " + Vis::ToString(cnt[v]) + "，检测到负权环");
                                    break;
                                }
                            }
                            else
                            {
                                DSA_VIS_ONLY(Vis::StyleEdge("G", u, v, "#94a3b8", 2, "6,4"));
                                /*VIS*/ DSA_VIS_STEP("SPFA 检查边 " + Vis::ToString(u) + "->" + Vis::ToString(v) + "：未改善最短路");
                            }
                            DSA_VIS_ONLY(Vis::UnmarkEdge("G", u, v));
                            DSA_VIS_ONLY(Vis::StyleEdge("G", u, v, "#64748b", 2));
                        }
                        DSA_VIS_ONLY(Vis::UnmarkNode("G", u));
                        DSA_VIS_ONLY(Vis::SetNodeColor("G", u, "gray"));
                        if (reach_negative_ring)
                            break;
                    }
                    DSA_VIS_ONLY(Vis::ClearFocus("DIST"));
                    /*VIS*/ DSA_VIS_STEP(reach_negative_ring ? "SPFA 检测到负权环" : "SPFA 结束：候选队列为空，距离已稳定");
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
                    DSA_VIS_DECL(auto as_string = [](const auto &x)
                    {
                        std::ostringstream oss;
                        oss << x;
                        return oss.str();
                    };)

                    // Dijkstra 算法的前提是图中没有负权边。
                    // 此处检查是否存在负权边。
                    bool has_negative_edge = false;
                    for (auto e : g.E)
                        if (e.w < 0)
                            return has_negative_edge = true; // 如果找到负权边，算法无法保证正确性，直接返回。
                    DSA_VIS_ONLY(Vis::InitWeightedGraph(g));
                    DSA_VIS_ONLY(Vis::InitDistanceState("DIST", dis, n));
                    DSA_VIS_ONLY(Vis::InitFlagState("FIXED", n));

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
                    std::vector<char> fixed(n + 1, false);
                    DSA_VIS_ONLY(Vis::SetNodeColor("G", s, "blue"));
                    DSA_VIS_ONLY(Vis::SetNodeLabel("G", s, as_string(s) + "\nd=0"));
                    /*VIS*/ DSA_VIS_STEP("Dijkstra 初始化完成：源点 " + as_string(s) + " 距离为 0，加入候选集合（代码用堆维护）");
                    /*VIS*/ DSA_VIS_G_MARK_NODE("G", DSA_VIS_NODE(s), false);
                    while (!heap.empty())
                    {
                        // 从优先队列中取出当前距离源点最近的节点 u。
                        auto cur = heap.top();
                        heap.pop();
                        int u = cur.node_index;
                        DSA_VIS_ONLY(Vis::ClearFocus("DIST"));
                        DSA_VIS_ONLY(Vis::FocusCell("DIST", u, "candidate"));
                        if (cur.lazy_dis != dis[u])
                        {
                            continue;
                        }
                        if (fixed[u])
                        {
                            continue;
                        }
                        fixed[u] = true;
                        DSA_VIS_ONLY(Vis::SetFlag("FIXED", u, "Y"));
                        DSA_VIS_ONLY(Vis::ClearFocus("DIST"));
                        DSA_VIS_ONLY(Vis::FocusCell("DIST", u, "write"));
                        /*VIS*/ DSA_VIS_STEP("Dijkstra：选取当前最小未确定点 u=" + as_string(u) + "，距离确定为 " + as_string(dis[u]));
                        DSA_VIS_ONLY(Vis::MarkNode("G", u, "blue"));
                        // 遍历 u 的所有邻居 v。
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            if (fixed[v])
                            {
                                /*VIS*/ DSA_VIS_STEP("Dijkstra：邻点 " + as_string(v) + " 已确定，跳过边 " + as_string(u) + "->" + as_string(v));
                                continue;
                            }
                            DSA_VIS_ONLY(Vis::MarkEdge("G", u, v, e.weight, "#2563eb"));
                            DSA_VIS_ONLY(Vis::ClearFocus("DIST"));
                            DSA_VIS_ONLY(Vis::FocusCell("DIST", u, "read"));
                            DSA_VIS_ONLY(Vis::FocusCell("DIST", v, "write"));
                            // 松弛操作：如果通过 u 能找到到 v 的更短路径...
                            if (dis[u] + e.weight < dis[v])
                            {
                                T old_dis = dis[v];
                                // ...更新到 v 的最短距离。
                                dis[v] = dis[u] + e.weight;
                                // 将更新后的 v（及其新距离）加入优先队列。
                                // 注意：这里没有删除堆中可能存在的旧的、更长的 (v, old_dis) 条目。
                                heap.push({v, dis[v]});
                                DSA_VIS_ONLY(Vis::SetDistance("DIST", v, dis[v], "via " + as_string(u)));
                                DSA_VIS_ONLY(Vis::MarkEdge("G", u, v, e.weight, "#16a34a"));
                                DSA_VIS_ONLY(Vis::SetNodeLabel("G", v, as_string(v) + "\\nd=" + as_string(dis[v])));
                                DSA_VIS_ONLY(Vis::SetNodeColor("G", v, "green"));
                                /*VIS*/ DSA_VIS_STEP(
                                    "Dijkstra 松弛边 " + as_string(u) + "->" + as_string(v) + "：dist[" + as_string(v) + "] "
                                        + (old_dis >= Infinity<T>() ? std::string("INF") : as_string(old_dis)) + " -> " + as_string(dis[v]) + "，节点 " + as_string(v) + " 加入候选集合");
                            }
                            else
                            {
                                DSA_VIS_ONLY(Vis::MarkEdge("G", u, v, e.weight, "#94a3b8", 2, "6,4"));
                                /*VIS*/ DSA_VIS_STEP("Dijkstra 检查边 " + as_string(u) + "->" + as_string(v) + "：未改善最短路");
                            }
                            DSA_VIS_ONLY(Vis::UnmarkEdge("G", u, v));
                        }
                        DSA_VIS_ONLY(Vis::UnmarkNode("G", u));
                        DSA_VIS_ONLY(Vis::SetNodeColor("G", u, "gray"));
                        // “懒惰删除”：这是一个使用标准库时的技巧。
                        // 在取出下一个节点之前，检查堆顶元素的距离是否是其最新距离。
                        // 如果堆顶节点的 stored_dis 与其在 dis 数组中的最新距离不符，
                        // 说明这个堆顶条目是过时的（已经被更短的路径更新过），直接丢弃。
                        // 重复此过程，直到堆顶是有效条目或堆为空。
                        while (!heap.empty() && dis[heap.top().node_index] != heap.top().lazy_dis)
                        {
                            heap.pop();
                        }
                    }
                    DSA_VIS_ONLY(Vis::ClearFocus("DIST"));
                    /*VIS*/ DSA_VIS_STEP("Dijkstra 结束：所有可达点的最短距离已确定");
                    return has_negative_edge;
                }
            }

        }
    }
}

