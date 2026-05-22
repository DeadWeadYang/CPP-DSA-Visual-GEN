#pragma once
#include "../sorting/sorting.hpp"
namespace DSA
{
    namespace Sorting
    {

        struct DemoSorting
        {
            std::vector<int> input;
            std::vector<int> output;
            static void Print(const std::vector<int> &v, std::ostream &os)
            {
                os << "[ ";
                for (auto i : v)
                    os << i << ", ";
                os << "]";
            }
            void SelectionSortDemo()
            {
                auto tmp = input;
                SelectionSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "SelectionSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            void BubbleSortDemo()
            {
                auto tmp = input;
                BubbleSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "BubbleSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            void InsertionSortDemo()
            {
                auto tmp = input;
                InsertionSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "InsertionSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            void QuickSortDemo()
            {
                auto tmp = input;
                QuickSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "QuickSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            void HeapSortDemo()
            {
                auto tmp = input;
                HeapSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "HeapSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            void MergeSortDemo()
            {
                auto tmp = input;
                MergeSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "MergeSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            void IntRadixSortDemo()
            {
                auto tmp = input;
                IntRadixSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "IntRadixSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            void ShellSortDemo()
            {

                auto tmp = input;
                ShellSort(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "ShellSort fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
                tmp = input;
                ShellSort<PrattGapGenerator>(tmp.begin(), tmp.end());
                if (tmp != output)
                {
                    std::ostringstream ss;
                    ss << "ShellSort(PrattGapGenerator) fail, got :";
                    Print(tmp, ss);
                    throw std::runtime_error(ss.str());
                }
            }
            static void FullDemo(const std::vector<int> &input)
            {
                auto instance = DemoSorting();
                instance.input = input;
                instance.output = input;
                std::sort(instance.output.begin(), instance.output.end());
                instance.BubbleSortDemo();
                instance.HeapSortDemo();
                instance.InsertionSortDemo();
                instance.IntRadixSortDemo();
                instance.MergeSortDemo();
                instance.QuickSortDemo();
                instance.SelectionSortDemo();
                instance.ShellSortDemo();
            }
            static std::vector<int> RandomGen(int n, int w, unsigned int seed = 0)
            {
                std::mt19937 rng{seed};
                std::uniform_int_distribution<int> v(-w, w);
                std::vector<int> x(n);
                for (int i = 0; i < n; i++)
                    x[i] = v(rng);
                return x;
            }
            static std::vector<int> OrderedGen(int n, bool reversed = false)
            {
                std::vector<int> x(n);
                for (int i = 0; i < n; i++)
                    x[i] = (reversed ? n - i : i);
                return x;
            }
            static void QuickDemo(const std::vector<int> &input)
            {

                auto instance = DemoSorting();
                instance.input = input;
                instance.output = input;
                std::sort(instance.output.begin(), instance.output.end());
                instance.HeapSortDemo();
                instance.IntRadixSortDemo();
                instance.MergeSortDemo();
                instance.QuickSortDemo();
            }
            static void TestCases()
            {
                int case_index = 0;
                try
                {
                    ++case_index;
                    FullDemo(RandomGen(5, 10));
                    ++case_index;
                    FullDemo(RandomGen(50, 1000));
                    ++case_index;
                    FullDemo(RandomGen(500, 1000000));
                    ++case_index;
                    QuickDemo(RandomGen(50000, 10000000));
                    ++case_index;
                    QuickDemo(RandomGen(500000, 100000000));
                    ++case_index;
                    QuickDemo(OrderedGen(500000));
                    ++case_index;
                    QuickDemo(OrderedGen(500000, true));

                    std::cout
                        << "Sorting test passed" << std::endl;
                }
                catch (const std::exception &ex)
                {
                    std::cerr << "Sorting test case " << case_index << " fail\n"
                              << ex.what() << std::endl;
                }
            }
        };
    }
}