#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <tuple>
#include <sstream>
#include "graph_vis.hpp"
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
					vis_stack.push(u);
					vis[u] = ++visited;
					DSA_VIS_ONLY(Vis::VisitNode("G", u, vis[u], "blue"));
					/*VIS*/ DSA_VIS_STEP("DFS 访问节点 " + Vis::ToString(u) + "，入栈并记录访问序 " + Vis::ToString(vis[u]));

					for (auto v : g.adj[u])
						if (!vis[v])
						{
							DSA_VIS_ONLY(Vis::MarkEdge("G", u, v, "", "#2563eb"));
							/*VIS*/ DSA_VIS_STEP("DFS 沿边 " + Vis::ToString(u) + " -> " + Vis::ToString(v) + " 递归探索未访问节点");
							DFS(v);
							DSA_VIS_ONLY(Vis::UnmarkEdge("G", u, v));
						}

					vis_stack.pop();
					DSA_VIS_ONLY(Vis::UnmarkNode("G", u));
					DSA_VIS_ONLY(Vis::SetNodeColor("G", u, "gray"));
					/*VIS*/ DSA_VIS_STEP("DFS 回溯离开节点 " + Vis::ToString(u));
				}
				static void Demo(int n, const std::vector<std::pair<int, int>> &edges, bool directed = false)
				{
					auto instance = BasicDemoDFS();
					instance.g = Graph(n, edges, directed);
					instance.vis_stack = std::stack<int>();
					instance.vis = std::vector<int>(n + 1, 0);
					instance.visited = 0;
					DSA_VIS_ONLY(Vis::InitBasicGraph(instance.g));
					/*VIS*/ DSA_VIS_STEP("DFS Demo 开始");
					instance.DFS(1);
					/*VIS*/ DSA_VIS_STEP("DFS Demo 结束");
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
					vis_queue.push(u);
					vis[u] = ++visited;
					DSA_VIS_ONLY(Vis::SetNodeColor("G", u, "green"));
					DSA_VIS_ONLY(Vis::SetNodeLabel("G", u, Vis::ToString(u) + "\nord=" + Vis::ToString(vis[u])));
					/*VIS*/ DSA_VIS_STEP("BFS 起点 " + Vis::ToString(u) + " 入队，访问序为 " + Vis::ToString(vis[u]));

					while (!vis_queue.empty())
					{
						u = vis_queue.front();
						vis_queue.pop();
						DSA_VIS_ONLY(Vis::MarkNode("G", u, "blue"));
						/*VIS*/ DSA_VIS_STEP("BFS 出队节点 " + Vis::ToString(u) + "，扫描它的邻边");
						for (auto v : g.adj[u])
							if (!vis[v])
							{
								DSA_VIS_ONLY(Vis::MarkEdge("G", u, v, "", "#2563eb"));
								vis_queue.push(v);
								vis[v] = ++visited;
								DSA_VIS_ONLY(Vis::SetNodeColor("G", v, "green"));
								DSA_VIS_ONLY(Vis::SetNodeLabel("G", v, Vis::ToString(v) + "\nord=" + Vis::ToString(vis[v])));
								/*VIS*/ DSA_VIS_STEP("BFS 通过边 " + Vis::ToString(u) + " -> " + Vis::ToString(v) + " 发现新节点并入队");
								DSA_VIS_ONLY(Vis::UnmarkEdge("G", u, v));
							}
						DSA_VIS_ONLY(Vis::UnmarkNode("G", u));
						DSA_VIS_ONLY(Vis::SetNodeColor("G", u, "gray"));
					}
				}
				static void Demo(int n, const std::vector<std::pair<int, int>> &edges, bool directed = false)
				{
					auto instance = BasicDemoBFS();
					instance.g = Graph(n, edges, directed);
					instance.vis_queue = std::queue<int>();
					instance.vis = std::vector<int>(n + 1, 0);
					instance.visited = 0;
					DSA_VIS_ONLY(Vis::InitBasicGraph(instance.g));
					/*VIS*/ DSA_VIS_STEP("BFS Demo 开始");
					instance.BFS(1);
					/*VIS*/ DSA_VIS_STEP("BFS Demo 结束");
				}
			};

		}
	}
}
