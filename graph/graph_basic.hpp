#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <tuple>
#include <sstream>
#include "../vis_trace.hpp"

namespace DSA
{
	namespace Graph
	{

		namespace GraphBasic
		{

			struct Edge
			{
				int u, v;
			};

			struct Graph
			{
				int number_of_node;
				bool is_directed;
				std::vector<Edge> E;
				std::vector<std::vector<int>> adj;
				Graph(int n = 0, const std::vector<std::pair<int, int>> &edges = {}, bool directed = false)
				{
					n = std::max(n, 0);
					number_of_node = n;
					is_directed = directed;
					adj = std::vector<std::vector<int>>(n + 1);
					for (auto e : edges)
					{
						int u = e.first, v = e.second;
						if (u > n || v > n || u < 1 || v < 1)
							continue;
						if (directed)
							AddDirectedEdge(u, v);
						else
							AddUndirectedEdge(u, v);
						E.push_back({u, v});
					}
				}

			private:
				void AddDirectedEdge(int u, int v)
				{
					adj[u].push_back(v);
				}
				void AddUndirectedEdge(int u, int v)
				{
					adj[u].push_back(v);
					adj[v].push_back(u);
				}
			};
			struct BasicDemoDFS
			{
				Graph g;
				int visited;
				std::vector<int> vis;
				std::stack<int> vis_stack;
				void DFS(int u)
				{
					auto as_string = [](const auto &x)
					{
						std::ostringstream oss;
						oss << x;
						return oss.str();
					};
					vis_stack.push(u);
					/*VIS*/ DSA_VIS_MSG("DFS 入栈节点 " + as_string(u), false);
					vis[u] = ++visited;
					/*VIS*/ DSA_VIS_G_MARK_NODE("G", DSA_VIS_NODE(u), false);
					/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(u), "blue", false);
					/*VIS*/ DSA_VIS_G_SET_NODE_VALUE("G", DSA_VIS_NODE(u), as_string(u) + "\nord=" + as_string(vis[u]), false);
					/*VIS*/ DSA_VIS_MSG("DFS 访问节点 " + as_string(u), true);

					for (auto v : g.adj[u])
						if (!vis[v])
						{
							/*VIS*/ DSA_VIS_MSG("DFS 沿边探索 " + as_string(u) + " -> " + as_string(v), false);
							/*VIS*/ DSA_VIS_G_MARK_EDGE("G", DSA_VIS_NODE(u), DSA_VIS_NODE(v), false);
							/*VIS*/ DSA_VIS_G_SET_EDGE_STYLE("G", DSA_VIS_NODE(u), DSA_VIS_NODE(v), "#2563eb", 3, "", false);
							DFS(v);
							/*VIS*/ DSA_VIS_G_UNMARK_EDGE("G", DSA_VIS_NODE(u), DSA_VIS_NODE(v), false);
						}

					vis_stack.pop();
					/*VIS*/ DSA_VIS_MSG("DFS 回溯离开节点 " + as_string(u), false);
					/*VIS*/ DSA_VIS_G_UNMARK_NODE("G", DSA_VIS_NODE(u), false);
					/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(u), "gray", false);
				}
				static void Demo(int n, const std::vector<std::pair<int, int>> &edges, bool directed = false)
				{
					auto instance = BasicDemoDFS();
					instance.g = Graph(n, edges, directed);
					instance.vis_stack = std::stack<int>();
					instance.vis = std::vector<int>(n + 1, 0);
					instance.visited = 0;
					/*VIS*/ DSA_VIS_G_INIT("G", directed);
					for (int i = 1; i <= n; ++i)
					{
						/*VIS*/ DSA_VIS_G_NEW_NODE("G", DSA_VIS_NODE(i), i, false);
						/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(i), "black", false);
					}
					for (auto e : instance.g.E)
					{
						/*VIS*/ DSA_VIS_G_NEW_EDGE("G", DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "", false);
						/*VIS*/ DSA_VIS_G_SET_EDGE_STYLE("G", DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "#64748b", 2, "", false);
					}
					/*VIS*/ DSA_VIS_MSG("DFS Demo 开始", true);
					instance.DFS(1);
					/*VIS*/ DSA_VIS_MSG("DFS Demo 结束", false);
				}
			};

			struct BasicDemoBFS
			{
				Graph g;
				int visited;
				std::vector<int> vis;
				std::queue<int> vis_queue;
				void BFS(int u)
				{
					auto as_string = [](const auto &x)
					{
						std::ostringstream oss;
						oss << x;
						return oss.str();
					};
					vis_queue.push(u);
					vis[u] = ++visited;
					/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(u), "green", false);
					/*VIS*/ DSA_VIS_G_SET_NODE_VALUE("G", DSA_VIS_NODE(u), as_string(u) + "\nord=" + as_string(vis[u]), false);
					/*VIS*/ DSA_VIS_MSG("BFS 起点入队 " + as_string(u), true);

					while (!vis_queue.empty())
					{
						u = vis_queue.front();
						vis_queue.pop();
						/*VIS*/ DSA_VIS_MSG("BFS 出队节点 " + as_string(u), true);
						/*VIS*/ DSA_VIS_G_MARK_NODE("G", DSA_VIS_NODE(u), false);
						/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(u), "blue", false);
						for (auto v : g.adj[u])
							if (!vis[v])
							{
								/*VIS*/ DSA_VIS_MSG("BFS 扫描边 " + as_string(u) + " -> " + as_string(v), false);
								/*VIS*/ DSA_VIS_G_MARK_EDGE("G", DSA_VIS_NODE(u), DSA_VIS_NODE(v), false);
								/*VIS*/ DSA_VIS_G_SET_EDGE_STYLE("G", DSA_VIS_NODE(u), DSA_VIS_NODE(v), "#2563eb", 3, "", false);
								vis_queue.push(v);
								vis[v] = ++visited;
								/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(v), "green", false);
								/*VIS*/ DSA_VIS_G_SET_NODE_VALUE("G", DSA_VIS_NODE(v), as_string(v) + "\nord=" + as_string(vis[v]), false);
								/*VIS*/ DSA_VIS_MSG("BFS 发现并入队 " + as_string(v), true);
								/*VIS*/ DSA_VIS_G_UNMARK_EDGE("G", DSA_VIS_NODE(u), DSA_VIS_NODE(v), false);
							}
						/*VIS*/ DSA_VIS_G_UNMARK_NODE("G", DSA_VIS_NODE(u), false);
						/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(u), "gray", false);
					}
				}
				static void Demo(int n, const std::vector<std::pair<int, int>> &edges, bool directed = false)
				{
					auto instance = BasicDemoBFS();
					instance.g = Graph(n, edges, directed);
					instance.vis_queue = std::queue<int>();
					instance.vis = std::vector<int>(n + 1, 0);
					instance.visited = 0;
					/*VIS*/ DSA_VIS_G_INIT("G", directed);
					for (int i = 1; i <= n; ++i)
					{
						/*VIS*/ DSA_VIS_G_NEW_NODE("G", DSA_VIS_NODE(i), i, false);
						/*VIS*/ DSA_VIS_G_SET_NODE_COLOR("G", DSA_VIS_NODE(i), "black", false);
					}
					for (auto e : instance.g.E)
					{
						/*VIS*/ DSA_VIS_G_NEW_EDGE("G", DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "", false);
						/*VIS*/ DSA_VIS_G_SET_EDGE_STYLE("G", DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "#64748b", 2, "", false);
					}
					/*VIS*/ DSA_VIS_MSG("BFS Demo 开始", true);
					instance.BFS(1);
					/*VIS*/ DSA_VIS_MSG("BFS Demo 结束", false);
				}
			};

		}
	}
}
