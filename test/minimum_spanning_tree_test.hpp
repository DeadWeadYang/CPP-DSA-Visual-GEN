#pragma once
#include "../graph/minimum_spanning_tree.hpp"

namespace DSA{
    namespace Graph{
        namespace SpanningTree{
            namespace MinimumSpanningTree{
                struct DemoMST
                {
                    WGraph<int> g;
                    int actual_ans;
                    void KruskalDemo()
                    {
                        std::vector<int> chosen = {};
                        int ans = 0;
                        Kruskal<int>(g, ans, chosen);
                        if (ans != actual_ans)
                        {
                            std::ostringstream ss;
                            ss << "Kruskal test fail\noutput answer: " << ans << "\nexpected answer: " << actual_ans;
                            throw std::runtime_error(ss.str());
                        }
                    }
                    void PrimDemo()
                    {

                        std::vector<int> chosen = {};
                        int ans = 0;
                        Prim<int>(g, ans, chosen);
                        if (ans != actual_ans)
                        {
                            std::ostringstream ss;
                            ss << "Prim test fail\noutput answer: " << ans << "\nexpected answer: " << actual_ans;
                            throw std::runtime_error(ss.str());
                        }
                    }

                    static void Demo(int n, const std::vector<std::tuple<int, int, int>> &edges, int actual_ans)
                    {
                        auto instance = DemoMST();
                        instance.g = WGraph<int>(n, edges);
                        instance.actual_ans = actual_ans;
                        instance.KruskalDemo();
                        instance.PrimDemo();
                    }
                    static void TestCases()
                    {
                        int case_index = 0;
                        try
                        {
                            ++case_index;
                            Demo(7, {
                                        {1, 2, 9},
                                        {1, 5, 2},
                                        {1, 6, 3},
                                        {2, 3, 5},
                                        {2, 6, 7},
                                        {3, 4, 6},
                                        {3, 7, 3},
                                        {4, 5, 6},
                                        {4, 7, 2},
                                        {5, 6, 3},
                                        {5, 7, 6},
                                        {6, 7, 1},
                                    },
                                 16);
                            std::cout << "MinimumSpanningTree test passed" << std::endl;
                        }
                        catch (const std::exception &ex)
                        {
                            std::cerr << "MinimumSpanningTree test case " << case_index << " fail\n"
                                      << ex.what() << std::endl;
                        }
                    }
                };
            }
        }
    }
}