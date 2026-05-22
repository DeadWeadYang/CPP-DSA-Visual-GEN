#pragma once
#include "../graph/shortest_path.hpp"
namespace DSA
{
    namespace Graph{
        namespace ShortestPath{
            struct DemoShortestPath
            {
                WGraph<int> g;
                int start_v, end_v;
                int actual_ans;
                bool fail_with_negative = false;
                bool cannot_reach = false;
                void FloydDemo()
                {
                    std::vector<std::vector<int>> dis;
                    bool fail_neg = AllPairs::Floyd<int>(g, dis);
                    if (fail_neg != fail_with_negative)
                    {
                        std::ostringstream ss;
                        ss << std::boolalpha;
                        ss << "Floyd test fail\noutput negative ring existence: " << fail_neg << "\nexpected negative ring existence: " << fail_with_negative;
                        throw std::runtime_error(ss.str());
                    }
                    if (fail_with_negative)
                        return;
                    int ans = dis[start_v][end_v];
                    if (cannot_reach)
                    {
                        if (ans != Infinity<int>())
                        {
                            std::ostringstream ss;
                            ss << "Floyd test fail\noutput distance: " << ans << "\nexpected distance: infinity";
                            throw std::runtime_error(ss.str());
                        }
                    }
                    else
                    {
                        if (ans != actual_ans)
                        {
                            std::ostringstream ss;
                            ss << "Floyd test fail\noutput distance: " << ans << "\nexpected distance: " << actual_ans;
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                void BellmanFordDemo()
                {
                    std::vector<int> dis;
                    bool fail_neg = SingleSource::BellmanFord<int>(g, start_v, dis);
                    if (fail_neg != fail_with_negative)
                    {
                        std::ostringstream ss;
                        ss << std::boolalpha;
                        ss << "Bellman-Ford test fail\noutput negative ring existence: " << fail_neg << "\nexpected negative ring existence: " << fail_with_negative;
                        throw std::runtime_error(ss.str());
                    }
                    if (fail_with_negative)
                        return;
                    int ans = dis[end_v];
                    if (cannot_reach)
                    {
                        if (ans != Infinity<int>())
                        {
                            std::ostringstream ss;
                            ss << "Bellman-Ford test fail\noutput distance: " << ans << "\nexpected distance: infinity";
                            throw std::runtime_error(ss.str());
                        }
                    }
                    else
                    {
                        if (ans != actual_ans)
                        {
                            std::ostringstream ss;
                            ss << "Bellman-Ford test fail\noutput distance: " << ans << "\nexpected distance: " << actual_ans;
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                void BellmanFordQueueDemo()
                {
                    std::vector<int> dis;
                    bool fail_neg = SingleSource::BellmanFord_Queue<int>(g, start_v, dis);
                    if (fail_neg != fail_with_negative)
                    {
                        std::ostringstream ss;
                        ss << std::boolalpha;
                        ss << "Bellman-Ford-Queue test fail\noutput negative ring existence: " << fail_neg << "\nexpected negative ring existence: " << fail_with_negative;
                        throw std::runtime_error(ss.str());
                    }
                    if (fail_with_negative)
                        return;
                    int ans = dis[end_v];
                    if (cannot_reach)
                    {
                        if (ans != Infinity<int>())
                        {
                            std::ostringstream ss;
                            ss << "Bellman-Ford-Queue test fail\noutput distance: " << ans << "\nexpected distance: infinity";
                            throw std::runtime_error(ss.str());
                        }
                    }
                    else
                    {
                        if (ans != actual_ans)
                        {
                            std::ostringstream ss;
                            ss << "Bellman-Ford-Queue test fail\noutput distance: " << ans << "\nexpected distance: " << actual_ans;
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                void DijkstraDemo()
                {
                    std::vector<int> dis;
                    bool fail_neg = SingleSource::Dijkstra<int>(g, start_v, dis);
                    if (fail_neg != fail_with_negative)
                    {
                        std::ostringstream ss;
                        ss << std::boolalpha;
                        ss << "Dijkstra test fail\noutput negative edge existence: " << fail_neg << "\nexpected negative edge existence: " << fail_with_negative;
                        throw std::runtime_error(ss.str());
                    }
                    if (fail_with_negative)
                        return;
                    int ans = dis[end_v];
                    if (cannot_reach)
                    {
                        if (ans != Infinity<int>())
                        {
                            std::ostringstream ss;
                            ss << "Dijkstra test fail\noutput distance: " << ans << "\nexpected distance: infinity";
                            throw std::runtime_error(ss.str());
                        }
                    }
                    else
                    {
                        if (ans != actual_ans)
                        {
                            std::ostringstream ss;
                            ss << "Dijkstra test fail\noutput distance: " << ans << "\nexpected distance: " << actual_ans;
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                static void Demo(int n, const std::vector<std::tuple<int, int, int>> &edges, int start_v, int end_v, int actual_ans, bool fail_with_negative = false, bool cannot_reach = false)
                {
                    auto instance = DemoShortestPath();
                    instance.g = WGraph<int>(n, edges);
                    instance.start_v = start_v;
                    instance.end_v = end_v;
                    instance.actual_ans = actual_ans;
                    instance.fail_with_negative = fail_with_negative;
                    instance.cannot_reach = cannot_reach;
                    instance.FloydDemo();
                    instance.BellmanFordDemo();
                    instance.BellmanFordQueueDemo();
                    instance.DijkstraDemo();
                }
                static void TestCases()
                {
                    int case_index = 0;
                    try
                    {
                        ++case_index;
                        Demo(7, {
                                    {2, 4, 2},
                                    {1, 4, 3},
                                    {7, 2, 2},
                                    {3, 4, 3},
                                    {5, 7, 5},
                                    {7, 3, 3},
                                    {6, 1, 1},
                                    {6, 3, 4},
                                    {2, 4, 3},
                                    {5, 6, 3},
                                    {7, 2, 1},
                                },
                             5, 4, 7);
                        ++case_index;
                        Demo(10, {
                                     {5, 6, 308},
                                     {8, 10, 696},
                                     {4, 2, 569},
                                     {8, 6, 471},
                                     {1, 2, 874},
                                     {5, 3, 130},
                                     {4, 5, 804},
                                     {8, 9, 89},
                                     {10, 4, 717},
                                     {10, 9, 41},
                                     {7, 6, 998},
                                     {1, 6, 639},
                                     {7, 9, 650},
                                     {7, 8, 339},
                                     {3, 1, 597},
                                     {9, 1, 622},
                                     {7, 10, 2},
                                     {5, 1, 4},
                                     {1, 4, 372},
                                     {1, 10, 163},
                                 },
                             9, 4, 576);
                        ++case_index;
                        Demo(5, {
                                    {1, 2, 3},
                                    {2, 3, 4},
                                    {3, 4, 1},
                                    {4, 5, -9},
                                    {5, 3, 2},
                                },
                             1, 4, 0, true, false);
                        ++case_index;
                        Demo(5, {
                                    {1, 2, 3},
                                    {2, 3, 4},
                                    {3, 4, 1},
                                },
                             1, 5, 0, false, true);

                        std::cout
                            << "ShortestPath test passed" << std::endl;
                    }
                    catch (const std::exception &ex)
                    {
                        std::cerr << "ShortestPath test case " << case_index << " fail\n"
                                  << ex.what() << std::endl;
                    }
                }
            };
        }
    }
} 