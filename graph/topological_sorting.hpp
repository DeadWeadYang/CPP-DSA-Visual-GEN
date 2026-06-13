#pragma once
#include <algorithm>
#include <exception>
#include <sstream>
#include "graph_basic.hpp"
#include "weighted_graph.hpp"
#include "graph_vis.hpp"
#include "../vis_trace.hpp"
namespace DSA
{
    namespace Graph
    {
        namespace TopologicalSorting
        {
            using GraphBasic::Edge;
            using GraphBasic::Graph;

            using GraphWeighted::WeightedAdjoint;
            using GraphWeighted::WeightedEdge;
            using GraphWeighted::WGraph;
            namespace TopoSort
            {
                /**
                 * @brief 使用 Kahn 算法对有向无环图 (DAG) 进行拓扑排序。
                 * @param g 输入的图（应为有向图）。
                 * @param order 输出参数，一个向量，存储拓扑排序的结果。
                 * @return bool 如果图不是有向无环图（即存在环），则返回 true；否则返回 false。
                 *
                 * @details
                 * Kahn 算法是一种基于入度的拓扑排序算法。
                 * 1. 计算所有节点的入度。
                 * 2. 将所有入度为 0 的节点加入一个队列。
                 * 3. 当队列不为空时，出队一个节点 u，将其加入拓扑序列。
                 * 4. 遍历 u 的所有邻居 v，将 v 的入度减 1。如果 v 的入度变为 0，则将 v 入队。
                 * 5. 如果最终拓扑序列的节点数不等于图的总节点数，说明图中存在环。
                 */
                bool Kahn(const Graph &g, std::vector<int> &order)
                {
                    order.clear();
                    bool not_DAG = false;
                    // 拓扑排序只对有向图有意义。
                    if (!g.is_directed)
                        return not_DAG = true;
                    int n = g.number_of_node;
                    // ind 向量用于存储每个节点的入度（incoming degree）。
                    std::vector<int> ind(n + 1);
                    // 遍历所有边，计算每个节点的初始入度。
                    for (auto e : g.E)
                        ++ind[e.v];
                    // q 是一个队列，用于存放当前入度为 0 的节点。
                    std::queue<int> q;
                    DSA_VIS_DECL(auto as_string = [](const auto &x)
                    {
                        std::ostringstream oss;
                        oss << x;
                        return oss.str();
                    };)
                    DSA_VIS_ONLY(Vis::InitBasicGraph(g));
                    /*VIS*/ DSA_VIS_STATE_INIT_VECTOR_LABELED("INDEG", n, "INDEG[v] current indegree", 1, "column = graph node v", false);
                    for (int i = 1; i <= n; ++i)
                    {
                        DSA_VIS_ONLY(Vis::SetFlag("INDEG", i, as_string(ind[i])));
                        /*VIS*/ DSA_VIS_STATE_HISTORY("INDEG", 0, i - 1, ind[i], "init", false);
                    }
                    DSA_VIS_ONLY(Vis::SeqClear("QUEUE"));
                    DSA_VIS_ONLY(Vis::SeqClear("ORDER"));
                    // 找到所有初始入度为 0 的节点并入队。
                    for (int i = 1; i <= n; i++)
                        if (!ind[i])
                        {
                            q.push(i);
                            DSA_VIS_ONLY(Vis::SeqPush("QUEUE", i));
                            DSA_VIS_ONLY(Vis::SetNodeColor("G", i, "green"));
                        }
                    /*VIS*/ DSA_VIS_STEP("Kahn 初始化完成：入度 0 的节点全部入队");
                    // 主循环，直到队列为空。
                    while (!q.empty())
                    {
                        int u = q.front(); // 从队列中取出一个入度为 0 的节点。
                        q.pop();
                        DSA_VIS_ONLY(Vis::SeqPop("QUEUE"));
                        order.push_back(u); // 将该节点加入拓扑序列。
                        DSA_VIS_ONLY(Vis::SeqPush("ORDER", u));
                        DSA_VIS_ONLY(Vis::ClearFocus("INDEG"));
                        DSA_VIS_ONLY(Vis::FocusCell("INDEG", u, "write"));
                        /*VIS*/ DSA_VIS_STEP("Kahn：出队节点 " + as_string(u) + "，加入拓扑序第 " + as_string(order.size()));
                        DSA_VIS_ONLY(Vis::MarkNode("G", u, "blue"));

                        // 遍历 u 的所有邻接点 v。
                        for (auto v : g.adj[u])
                        {
                            DSA_VIS_ONLY(Vis::MarkEdge("G", u, v, "", "#2563eb"));
                            // 将 v 的入度减 1，因为 u 已经被处理。
                            --ind[v];
                            DSA_VIS_ONLY(Vis::ClearFocus("INDEG"));
                            DSA_VIS_ONLY(Vis::FocusCell("INDEG", v, "write"));
                            DSA_VIS_ONLY(Vis::SetFlag("INDEG", v, as_string(ind[v])));
                            /*VIS*/ DSA_VIS_STATE_HISTORY("INDEG", 0, v - 1, ind[v], "remove " + as_string(u) + "->" + as_string(v), false);
                            // 如果 v 的入度变为 0，说明它的所有前驱节点都已处理完毕，可以入队了。
                            if (!ind[v])
                            {
                                q.push(v);
                                DSA_VIS_ONLY(Vis::SeqPush("QUEUE", v));
                                DSA_VIS_ONLY(Vis::SetNodeColor("G", v, "green"));
                                /*VIS*/ DSA_VIS_STEP("Kahn：处理边 " + as_string(u) + "->" + as_string(v) + " 后，节点 " + as_string(v) + " 入度为 0，入队");
                            }
                            else
                            {
                                /*VIS*/ DSA_VIS_STEP("Kahn：处理边 " + as_string(u) + "->" + as_string(v) + " 后，入度变为 " + as_string(ind[v]));
                            }
                            DSA_VIS_ONLY(Vis::UnmarkEdge("G", u, v));
                        }
                        DSA_VIS_ONLY(Vis::UnmarkNode("G", u));
                        DSA_VIS_ONLY(Vis::SetNodeColor("G", u, "gray"));
                    }
                    // 检查是否存在环：如果排序后的节点数不等于图的总节点数，说明有节点未被访问，即存在环。
                    not_DAG = (order.size() != n);
                    DSA_VIS_ONLY(Vis::ClearFocus("INDEG"));
                    /*VIS*/ DSA_VIS_STEP(not_DAG ? "Kahn 结束：仍有节点未入拓扑序，图中存在环" : "Kahn 结束：得到完整拓扑序");
                    return not_DAG;
                }
                /**
                 * @brief 验证给定的序列是否是图的一个有效拓扑排序。
                 * @param g 输入的图。
                 * @param order 待验证的节点序列。
                 * @return bool 如果 order 是 g 的一个有效拓扑排序，则返回 true；否则返回 false。
                 *
                 * @details
                 * 一个序列是有效的拓扑排序，当且仅当对于图中的任意一条边 (u, v)，
                 * u 在序列中都出现在 v 的前面。
                 */
                bool TopoValidate(const Graph &g, const std::vector<int> &order)
                {
                    // 拓扑排序只对有向图有意义。
                    if (!g.is_directed)
                        return false;
                    // idx 向量用于记录每个节点在 order 序列中的位置。
                    std::vector<int> idx(g.number_of_node + 1);
                    int n = 0;
                    // 遍历待验证的序列，构建位置映射。
                    for (auto u : order)
                    {
                        // 检查序列中的节点是否有效。
                        if (u > g.number_of_node || u < 1)
                            return false;
                        // 记录节点 u 的位置。
                        idx[u] = ++n;
                    }
                    // 如果序列中的节点数与图的节点数不符，则无效。
                    if (n != g.number_of_node)
                        return false;
                    // 遍历图中的每一条边 (u, v)。
                    for (auto e : g.E)
                        if (idx[e.u] > idx[e.v]) // 如果 u 的位置在 v 的后面，则违反了拓扑排序的定义。
                            return false;
                    // 如果所有边都满足条件，则该序列是有效的。
                    return true;
                }
            }
            // 命名空间，用于处理“活动在边上”(Activity on Edge, AOE) 的网络问题。
            // 这是拓扑排序的一个重要应用，通常用于关键路径分析。
            namespace AOE
            {

                /**
                 * @brief 验证一个加权图是否是有效的 AOE 网络。
                 * @tparam T 边权重的类型。
                 * @param g 输入的加权图。
                 * @param topo_order 输出参数，如果图有效，则存储其拓扑排序。
                 * @return bool 如果是有效的 AOE 网络，返回 true；否则返回 false。
                 *
                 * @details
                 * 一个有效的 AOE 网络必须满足：
                 * 1. 是一个有向无环图 (DAG)。
                 * 2. 有且仅有一个源点（入度为0的节点）。
                 * 3. 有且仅有一个汇点（出度为0的节点）。
                 * 4. 所有边的权重（代表活动持续时间）必须为非负数。
                 */
                template <typename T = int>
                bool validateAOE(const WGraph<T> &g, std::vector<int> &topo_order)
                {
                    int n = g.number_of_node, begin_v = 0, end_v = 0;

                    // ind 和 outd 分别存储每个节点的入度和出度。
                    std::vector<int> ind(n + 1), outd(n + 1);
                    for (auto e : g.E)
                    {
                        ++ind[e.v], ++outd[e.u];

                        // 活动持续时间不能为负。
                        if (e.w < 0)
                            return false;
                    }

                    // 检查源点和汇点的数量。
                    for (int i = 1; i <= n; i++)
                    {
                        if (!ind[i]) // 入度为0的是源点。
                        {
                            if (!begin_v) // 找到第一个源点。
                                begin_v = i;
                            else // 如果已经有一个源点，说明源点不唯一，无效。
                                return false;
                        }
                        if (!outd[i]) // 出度为0的是汇点。
                        {
                            if (!end_v) // 找到第一个汇点。
                                end_v = i;
                            else // 如果已经有一个汇点，说明汇点不唯一，无效。
                                return false;
                        }
                    }
                    // 必须同时存在源点和汇点。
                    if (!begin_v || !end_v)
                        return false;

                    // 最后，使用 Kahn 算法检查图是否是 DAG，同时得到一个拓扑排序
                    // `g.ignoreWeight()` 将加权图临时视为无权图来执行拓扑排序。
                    // Kahn 返回 true 表示有环（不是DAG），所以用 ! 来反转结果。
                    return !TopoSort::Kahn(g.ignoreWeight(), topo_order);
                }

                /**
                 * @brief 对 AOE 网络进行分析，计算关键路径。
                 * @tparam T 边权重的类型。
                 * @param g 输入的 AOE 网络图。
                 * @param critical_path 输出参数，关键路径的长度（即项目完成的最短时间）。
                 * @param critical_event 输出参数，关键事件（即关键路径上的节点）的列表。
                 * @param earliest 输出参数，每个事件的最早发生时间。
                 * @param latest 输出参数，每个事件的最迟发生时间。
                 * @return bool 如果分析成功，返回 true；如果图不是有效的 AOE 网络，返回 false。
                 */
                template <typename T = int>
                bool resolve(const WGraph<T> &g, T &critical_path, std::vector<int> &critical_event, std::vector<int> &earliest, std::vector<int> &latest)
                {
                    std::vector<int> order; // 用于存储拓扑排序结果。
                    int n = g.number_of_node;
                    // 首先验证输入图是否为有效的 AOE 网络。
                    if (!validateAOE(g, order))
                        return false;

                    // --- 计算所有事件的最早发生时间 (earliest) ---
                    // 按照拓扑排序正向递推。
                    earliest = std::vector<T>(n + 1);
                    for (auto u : order)// 遍历拓扑序列中的每个事件 u。
                    {
                        for (auto e : g.adj[u])// 遍历由 u 出发的所有活动。
                        {
                            int v = e.adjvex;// 活动指向的下一个事件 v。
                            // v 的最早发生时间，取决于其所有前驱活动的最晚完成时间。
                            earliest[v] = std::max(earliest[v], earliest[u] + e.weight);
                        }
                    }
                    // 关键路径的长度就是汇点（最后一个事件）的最早发生时间。
                    critical_path = earliest[order[n - 1]];
                    
                    // --- 计算所有事件的最迟发生时间 (latest) ---
                    // 按照拓扑排序逆向递推。
                    latest = std::vector<T>(n + 1, critical_path);// 初始化所有事件的最迟时间为项目总时间。
                    for (int i = n; i >= 1; i--)// 逆向遍历拓扑序列。
                    {
                        int u = order[i - 1];
                        for (auto e : g.adj[u]) // 遍历由 u 出发的所有活动。
                        {
                            int v = e.adjvex;
                            // u 的最迟发生时间，不能晚于其所有后继活动的最迟开始时间。
                            latest[u] = std::min(latest[u], latest[v] - e.weight);
                        }
                    }
                    
                    // --- 找出关键事件 ---
                    // 关键事件是指那些最早发生时间和最迟发生时间相等的事件。
                    critical_event.clear();
                    for (int i = 1; i <= n; i++)
                        if (earliest[i] == latest[i])
                            critical_event.push_back(i);
                    return true;
                }
            }
        }
    }
}
