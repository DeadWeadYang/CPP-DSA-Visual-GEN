#pragma once
#include "../graph/topological_sorting.hpp"

namespace DSA{
    namespace Graph{
        namespace TopologicalSorting{
            
            struct DemoTopo
            {
                Graph g;
                void KahnDemo()
                {
                    std::vector<int> order;
                    TopoSort::Kahn(g, order);
                    if (!TopoSort::TopoValidate(g, order))
                    {
                        throw std::runtime_error("Kahn test fail: restriction not satisfied");
                    }
                }
                static void Demo(int n, const std::vector<std::pair<int, int>> &edges)
                {
                    auto instance = DemoTopo();
                    instance.g = Graph(n, edges, true);
                    instance.KahnDemo();
                }
                static void TestCases()
                {
                    int case_index = 0;
                    try
                    {
                        ++case_index;
                        Demo(13, {
                                     {1, 2},
                                     {1, 6},
                                     {1, 7},
                                     {3, 1},
                                     {3, 4},
                                     {4, 6},
                                     {6, 5},
                                     {7, 5},
                                     {7, 10},
                                     {8, 7},
                                     {9, 8},
                                     {10, 11},
                                     {10, 12},
                                     {10, 13},
                                     {12, 13},
                                 });
                        std::cout
                            << "TopologicalSort test passed" << std::endl;
                    }
                    catch (const std::exception &ex)
                    {
                        std::cerr << "TopologicalSort test case " << case_index << " fail\n"
                                  << ex.what() << std::endl;
                    }
                }
            };
            struct DemoAOE
            {
                WGraph<int> g;
                int actual_CP;
                std::vector<int> actual_CE;
                void AOEResolveDemo()
                {
                    int cp;
                    std::vector<int> ce, early, late;
                    AOE::resolve(g, cp, ce, early, late);
                    if (cp != actual_CP)
                    {
                        std::ostringstream ss;
                        ss << "TopologicalSorting-AOE test fail\noutput critical path length: " << cp << "\nexpected critical path length: " << actual_CP;
                        throw std::runtime_error(ss.str());
                    }
                    if (ce != actual_CE)
                    {
                        std::ostringstream ss;
                        ss << "TopologicalSorting-AOE test fail\noutput critical events: ";
                        ss << "[ ";
                        for (auto ev : ce)
                            ss << ev << ", ";
                        ss << "]\n";
                        ss << "expected critical events: ";
                        ss << "[ ";
                        for (auto ev : actual_CE)
                            ss << ev << ", ";
                        ss << "]";
                        throw std::runtime_error(ss.str());
                    }
                }
                static void Demo(int n, const std::vector<std::tuple<int, int, int>> &edges, int actual_CP, const std::vector<int> &actual_CE)
                {
                    auto instance = DemoAOE();
                    instance.g = WGraph<int>(n, edges, true);
                    instance.actual_CP = actual_CP;
                    instance.actual_CE = actual_CE;
                    instance.AOEResolveDemo();
                }
                static void TestCases()
                {
                    int case_index = 0;
                    try
                    {
                        ++case_index;
                        Demo(9,
                             {
                                 {1, 2, 6},
                                 {1, 3, 4},
                                 {1, 4, 5},
                                 {2, 5, 1},
                                 {3, 5, 1},
                                 {4, 6, 2},
                                 {5, 7, 9},
                                 {5, 8, 7},
                                 {6, 8, 4},
                                 {7, 9, 2},
                                 {8, 9, 4},
                             },
                             18,
                             {1, 2, 5, 7, 8, 9});
                        std::cout
                            << "TopologicalSorting-AOE test passed" << std::endl;
                    }
                    catch (const std::exception &ex)
                    {
                        std::cerr << "TopologicalSorting-AOE test case " << case_index << " fail\n"
                                  << ex.what() << std::endl;
                    }
                }
            };
        }
    }
}