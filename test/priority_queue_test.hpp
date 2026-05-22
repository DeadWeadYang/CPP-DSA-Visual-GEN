#pragma once
#include "../tree/heap/priority_queue.hpp"

namespace  DSA
{
    namespace Tree{
        namespace Heap{
            
            struct DemoPriorityQueue
            {
                unsigned int random_seed;
                void BinaryHeapDemo()
                {
                    std::mt19937 rng(random_seed);
                    std::priority_queue<int> pq;
                    PriorityQueue<int> mpq;
                    if (mpq.size() != 0 || !mpq.empty())
                    {
                        throw std::runtime_error("BinaryHeap test fail: size() should be zero and empty() should be true at begining");
                    }
                    std::uniform_int_distribution<int> dist100(0, 99), dist2(0, 1);
                    int n = dist100(rng);
                    for (int i = 1; i <= n; i++)
                    {
                        try
                        {
                            if (!pq.empty() && dist2(rng))
                            {
                                pq.pop();
                                mpq.pop();
                            }
                            else
                            {
                                int x = dist100(rng);
                                pq.push(x);
                                mpq.push(x);
                            }
                        }
                        catch (const std::exception &ex)
                        {
                            std::ostringstream ss;
                            ss << "BinaryHeap test fail: the " << i << " operation fail, caused by: " << ex.what();
                            throw std::runtime_error(ss.str());
                        }
                        if (pq.size() != mpq.size())
                        {
                            std::ostringstream ss;
                            ss << "BinaryHeap test fail: after " << i << " operations, size() expected to be " << pq.size() << ", but return " << mpq.size();
                            throw std::runtime_error(ss.str());
                        }
                        if (pq.top() != mpq.top())
                        {
                            std::ostringstream ss;
                            ss << "BinaryHeap test fail: after " << i << " operations, top() expected to be " << pq.top() << ", but return " << mpq.top();
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                static void Demo(unsigned int random_seed)
                {
                    auto instance = DemoPriorityQueue();
                    instance.random_seed = random_seed;
                    instance.BinaryHeapDemo();
                }
                static void TestCases()
                {
                    int case_index = 0;
                    try
                    {
                        ++case_index;
                        Demo(0);
                        ++case_index;
                        Demo(998244353);
                        std::cout << "PriorityQueue test passed" << std::endl;
                    }
                    catch (const std::exception &ex)
                    {
                        std::cerr << "PriorityQueue test case " << case_index << " fail\n"
                                  << ex.what() << std::endl;
                    }
                }
            };
        }
    }
} 
