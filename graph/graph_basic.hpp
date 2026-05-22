#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <tuple>

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

					for (auto v : g.adj[u])
						if (!vis[v])
							DFS(v);

					vis_stack.pop();
				}
				static void Demo(int n, const std::vector<std::pair<int, int>> &edges, bool directed = false)
				{
					auto instance = BasicDemoDFS();
					instance.g = Graph(n, edges, directed);
					instance.vis_stack = std::stack<int>();
					instance.vis = std::vector<int>(n + 1, 0);
					instance.visited = 0;
					instance.DFS(1);
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

					while (!vis_queue.empty())
					{
						u = vis_queue.front();
						vis_queue.pop();
						for (auto v : g.adj[u])
							if (!vis[v])
								vis_queue.push(v);
					}
				}
				static void Demo(int n, const std::vector<std::pair<int, int>> &edges, bool directed = false)
				{
					auto instance = BasicDemoBFS();
					instance.g = Graph(n, edges, directed);
					instance.vis_queue = std::queue<int>();
					instance.vis = std::vector<int>(n + 1, 0);
					instance.visited = 0;
					instance.BFS(1);
				}
			};

		}
	}
}
