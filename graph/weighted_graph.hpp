#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include <tuple>
#include "graph_basic.hpp"
namespace DSA
{
    namespace Graph
    {
        namespace GraphWeighted
        {

            using std::pair;
            using std::queue;
            using std::stack;
            using std::tuple;
            using std::vector;

            template <typename T=int>
            struct WeightedEdge
            {
                int u, v;
                T w;
            };
            template <typename T=int>
            struct WeightedAdjoint
            {
                int adjvex;
                T weight;
                int edge_index = 0;
            };

            template <typename T=int>
            struct WGraph
            {
                int number_of_node;
                bool is_directed;
                vector<WeightedEdge<T>> E;

                vector<vector<WeightedAdjoint<T>>> adj;
                WGraph(int n = 0, const vector<tuple<int, int, T>> &edges = {}, bool directed = false)
                {
                    n = std::max(n, 0);
                    number_of_node = n;
                    is_directed = directed;
                    adj = vector<vector<WeightedAdjoint<T>>>(n + 1);
                    int i = 0;
                    for (auto e : edges)
                    {
                        int u, v, w;
                        std::tie(u, v, w) = e;
                        if (u > n || v > n || u < 1 || v < 1)
                            continue;
                        if (directed)
                            addDirectedEdge(u, v, w, i);
                        else
                            addUndirectedEdge(u, v, w, i);
                        E.push_back({u, v, w});
                        ++i;
                    }
                }

                GraphBasic::Graph ignoreWeight() const
                {
                    vector<pair<int, int>> edges;
                    edges.reserve(E.size());
                    std::transform(E.begin(), E.end(), std::back_inserter(edges), [](WeightedEdge<T> e)
                                   { return std::make_pair(e.u, e.v); });
                    return GraphBasic::Graph(number_of_node, edges, is_directed);
                }

            private:
                void addDirectedEdge(int u, int v, T w, int id = 0)
                {
                    adj[u].push_back({v, w, id});
                }
                void addUndirectedEdge(int u, int v, T w, int id = 0)
                {
                    adj[u].push_back({v, w, id});
                    adj[v].push_back({u, w, id});
                }
            };

        }
    }
}