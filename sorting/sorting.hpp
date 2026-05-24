#pragma once
#include <algorithm>
#include <random>
#include <type_traits> // for std::decay
#include <utility>     // for std::declval
#include <vector>
#include <exception>
#include <iostream>
#include <sstream>
#include "../vis_trace.hpp"
namespace DSA
{
    namespace Sorting
    {
        // always use [first,last)

        // 所有排序算法的接口都遵循C++标准库的习惯，使用左闭右开区间 [first, last)

        // C++14 风格的 using 别名，用于获取迭代器所指向元素的类型
        template <typename It>
        using iterator_value_type_t = typename std::iterator_traits<It>::value_type;

        /**
         * @brief 选择排序
         * @tparam RandIt 随机访问迭代器类型
         * @param first 待排序区间的起始迭代器
         * @param last 待排序区间的结束迭代器（不包含）
         *
         * 工作原理：
         * 每次从未排序的部分中找到最小（或最大）的元素，
         * 然后放到已排序部分的末尾。
         *
         * 时间复杂度：O(N^2)
         * 空间复杂度：O(1)
         * 不稳定排序
         */
        template <typename RandIt>
        void SelectionSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", 0, n, false);
            /*VIS*/ DSA_VIS_MSG(std::string("选择排序开始：n=") + std::to_string(n), true);
            // 外层循环：每次确定一个位置（从0到n-2）的最终元素
            for (int i = 0; i < n - 1; i++)
            {
                // `ith` 用于记录当前找到的最小元素的索引
                int ith = i;
                // 内层循环：在未排序部分 arr[i+1...n-1] 中寻找最小的元素
                for (int j = i + 1; j < n; j++)
                {
                    if (arr[j] < arr[ith])
                        ith = j; // 如果发现更小的，就更新最小元素的索引
                }
                // 将找到的最小元素与当前位置i的元素交换
                if (ith != i)
                {
                    /*VIS*/ DSA_VIS_MSG(std::string("第 ") + std::to_string(i) + " 轮：将最小值下标 " + std::to_string(ith) + " 交换到位置 " + std::to_string(i), false);
                    /*VIS*/ DSA_VIS_ARR_MARK("A", i, false);
                    /*VIS*/ DSA_VIS_ARR_MARK("A", ith, false);
                    std::swap(arr[ith], arr[i]); /*VIS*/ DSA_VIS_ARR_SWAP("A", i, ith, true);
                    /*VIS*/ DSA_VIS_ARR_UNMARK("A", i, false);
                    /*VIS*/ DSA_VIS_ARR_UNMARK("A", ith, false);
                }
            }
            /*VIS*/ DSA_VIS_MSG("选择排序结束", false);
        }

        /**
         * @brief 冒泡排序
         * @tparam RandIt 随机访问迭代器类型
         * @param first 待排序区间的起始迭代器
         * @param last 待排序区间的结束迭代器（不包含）
         *
         * 工作原理：
         * 重复地遍历要排序的数列，一次比较两个元素，如果它们的顺序错误就把它们交换过来。
         *
         * 时间复杂度：O(N^2)
         * 空间复杂度：O(1)
         * 稳定排序
         */
        template <typename RandIt>
        void BubbleSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", 0, n, false);
            /*VIS*/ DSA_VIS_MSG(std::string("冒泡排序开始：n=") + std::to_string(n), true);
            bool flag = true; // 如果在一轮遍历中没有发生任何交换，说明数组已经有序，可以提前终止。
            while (flag)
            {
                flag = false;
                // 每一轮内层循环都会将一个未排序部分的最大值“冒泡”到正确的位置
                for (int i = 0; i < n - 1; i++)
                {
                    if (arr[i + 1] < arr[i])
                    {
                        flag = true; // 发生了交换，说明可能还未完全有序
                        /*VIS*/ DSA_VIS_MSG(std::string("交换逆序对：") + std::to_string(i) + " <-> " + std::to_string(i + 1), false);
                        /*VIS*/ DSA_VIS_ARR_MARK("A", i, false);
                        /*VIS*/ DSA_VIS_ARR_MARK("A", i + 1, false);
                        std::swap(arr[i], arr[i + 1]); /*VIS*/ DSA_VIS_ARR_SWAP("A", i, i + 1, true);
                        /*VIS*/ DSA_VIS_ARR_UNMARK("A", i, false);
                        /*VIS*/ DSA_VIS_ARR_UNMARK("A", i + 1, false);
                    }
                }
            }
            /*VIS*/ DSA_VIS_MSG("冒泡排序结束", false);
        }

        /**
         * @brief 插入排序
         * @tparam RandIt 随机访问迭代器类型
         * @param first 待排序区间的起始迭代器
         * @param last 待排序区间的结束迭代器（不包含）
         *
         * 工作原理：
         * 构建有序序列，对于未排序数据，在已排序序列中从后向前扫描，找到相应位置并插入。
         *
         * 时间复杂度：O(N^2)
         * 空间复杂度：O(1)
         * 稳定排序
         */
        template <typename RandIt>
        void InsertionSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", 0, n, false);
            /*VIS*/ DSA_VIS_MSG(std::string("插入排序开始：n=") + std::to_string(n), true);
            // 外层循环：从第二个元素开始（i=1），逐个将其插入到前面已排序的子数组中
            for (int i = 1, j; i < n; i++)
            {
                // 暂存当前需要被插入的元素
                auto tmp = arr[i];
                /*VIS*/ DSA_VIS_MSG(std::string("处理下标 ") + std::to_string(i) + "，向有序区插入", false);
                // 内层循环：从已排序部分的末尾向前查找插入位置
                // 当 j>=0 (防止越界) 且 tmp 小于当前比较的元素 arr[j] 时
                for (j = i - 1; j >= 0 && tmp < arr[j]; j--)
                {
                    arr[j + 1] = arr[j]; /*VIS*/ DSA_VIS_ARR_SET("A", j + 1, arr[j + 1], true); // 将比tmp大的元素向后移动一位
                }
                // 循环结束时，j+1 就是tmp的正确插入位置
                arr[j + 1] = tmp; /*VIS*/ DSA_VIS_ARR_SET("A", j + 1, arr[j + 1], (j + 1 != i));
            }
            /*VIS*/ DSA_VIS_MSG("插入排序结束", false);
        }

        // --- 快速排序的辅助组件 ---

        // Pivot（主元）选择策略：总是选择区间的第一个元素
        struct AlwaysFirstSampler
        {
            template <typename RandIt>
            RandIt operator()(RandIt first, RandIt last)
            {
                return first;
            }
        };
        // Pivot（主元）选择策略：随机选择区间中的一个元素
        struct RandomSampler
        {
            std::mt19937 rng{std::random_device{}()};
            template <typename RandIt>
            RandIt operator()(RandIt first, RandIt last)
            {
                auto n = std::distance(first, last);
                std::uniform_int_distribution<decltype(n)> dist(0, n - 1);
                return (first + dist(rng));
            }
        };
        // Pivot（主元）选择策略：三数取中法
        // 选取区间的第一个、中间、最后一个元素，返回这三者中值大小居中的那个元素的迭代器。
        // 这可以有效避免在“部分有序”的数组上选择到最差的pivot，从而降低快排退化为O(N^2)的风险。
        struct MedianOfThreeSampler
        {
            template <typename RandIt>
            RandIt operator()(RandIt first, RandIt last)
            {
                auto n = std::distance(first, last);
                auto mid = n / 2;
                auto x = (first), y = (first + mid), z = (last - 1);
                // 下面三步通过交换，保证 *x <= *y <= *z
                if (*y < *x)
                    std::swap(x, y);
                if (*z < *x)
                    std::swap(x, z);
                if (*z < *y)
                    std::swap(y, z);
                // 此时 y 指向的元素就是三者的中位数
                return y;
            }
        };

        /**
         * @brief Hoare分区变体实现
         * @param sampler 用于选择主元(pivot)的策略对象
         * @return 一个分割点迭代器 p。使得 [first, p) 中的元素都小于等于pivot，
         *         而 [p, last) 中的元素都大于等于pivot。
         *         注意：此处的实现返回的分割点不是 pivot 最终的位置，而是下一个子区间的起始点。
         */
        template <typename RandIt, typename Sampler>
        RandIt Partition(RandIt first, RandIt last, Sampler &sampler, RandIt base_first)
        {
            int low = 0, high = std::distance(first, last) - 1;
            if (high <= 0)
                return first;
            auto arr = first;
            int base_offset = static_cast<int>(first - base_first);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", base_offset, base_offset + high + 1, false);
            // 1. 通过采样器选择一个主元(pivot)的值
            auto pivot_it = sampler(first, last);
            auto pivot = *pivot_it;
            /*VIS*/ DSA_VIS_ARR_MARK("A", base_offset + static_cast<int>(pivot_it - first), false);
            /*VIS*/ DSA_VIS_MSG(std::string("Partition：pivot 下标=") + std::to_string(base_offset + static_cast<int>(pivot_it - first)), false);
            // 2. 双指针分区过程
            while (true)
            {
                // 从左向右找到第一个不小于pivot的元素
                while (arr[low] < pivot) // 越界前一定能找到，因此不需要判断是否越界
                    ++low;
                // 从右向左找到第一个不大于pivot的元素
                while (pivot < arr[high]) // 同理，不需要判断是否越界
                    --high;
                // 如果 low 和 high 指针交错，说明分区完成
                if (low < high)
                {
                    /*VIS*/ DSA_VIS_MSG(std::string("Partition 交换：") + std::to_string(base_offset + low) + " <-> " + std::to_string(base_offset + high), false);
                    /*VIS*/ DSA_VIS_ARR_MARK("A", base_offset + low, false);
                    /*VIS*/ DSA_VIS_ARR_MARK("A", base_offset + high, false);
                    std::swap(arr[low++], arr[high--]); /*VIS*/ DSA_VIS_ARR_SWAP("A", base_offset + low - 1, base_offset + high + 1, true);
                    /*VIS*/ DSA_VIS_ARR_UNMARK("A", base_offset + low - 1, false);
                    /*VIS*/ DSA_VIS_ARR_UNMARK("A", base_offset + high + 1, false);
                }
                else
                    break;
            }
            /*VIS*/ DSA_VIS_ARR_UNMARK("A", base_offset + static_cast<int>(pivot_it - first), false);
            /*VIS*/ DSA_VIS_MSG("Partition 完成", false);
            // 这是一个非常规的返回值。
            // 传统的Hoare分区会返回一个索引，但这里返回的是一个迭代器。
            // `std::max(1, low)` 是为了防止当pivot是数组中最小元素时，low可能为0，
            // 导致 `arr+low` 成为区间的起始点 `first`，
            // 从而在 `QuickSort(p, last, sampler)` 中造成无限递归。
            // `arr + std::max(1, low)` 确保返回的分割点至少是 `first + 1`，
            // 强制让递归能够继续进行下去。
            return arr + std::max(1, low); // important
        }
        /**
         * @brief 快速排序（递归实现）
         * @tparam RandIt 随机访问迭代器类型
         * @tparam Sampler 主元选择策略类型
         * @param sampler 主元选择策略对象
         *
         * 工作原理：
         * 1. 选一个"主元" (pivot)。
         * 2. "分区"操作：将数组重排，所有小于pivot的元素移到它前面，所有大于pivot的元素移到它后面。
         * 3. 递归地对pivot前后的两个子数组进行快速排序。
         *
         * 时间复杂度：平均O(N log N)，最坏O(N^2) (取决于主元选择)
         * 空间复杂度：O(log N) (递归栈深度)
         * 不稳定排序
         */
        template <typename RandIt, typename Sampler>
        void QuickSort(RandIt first, RandIt last, Sampler &sampler, RandIt base_first)
        {
            // std::sort(first,last);
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            int base_offset = static_cast<int>(first - base_first);
            /*VIS*/ DSA_VIS_MSG(std::string("QuickSort 处理区间 [") + std::to_string(base_offset) + "," + std::to_string(base_offset + n) + ")", false);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", base_offset, base_offset + n, true);
            // p 是由Partition返回的分割点，它是右侧子区间的起始。
            auto p = Partition<RandIt, Sampler>(first, last, sampler, base_first);
            QuickSort(first, p, sampler, base_first);
            QuickSort(p, last, sampler, base_first);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", base_offset, base_offset + n, false);
            if (base_offset == 0)
                /*VIS*/ DSA_VIS_MSG("QuickSort 完成", false);
        }
        // 快速排序的默认版本，使用“三数取中”策略，因为它通常表现最好。
        template <typename RandIt>
        void QuickSort(RandIt first, RandIt last)
        {
            MedianOfThreeSampler sampler{};
            return QuickSort<RandIt, MedianOfThreeSampler>(first, last, sampler, first);
        }
        /**
         * @brief 堆排序
         * @tparam RandIt 随机访问迭代器类型
         *
         * 工作原理：
         * 1. 将输入数组构建成一个最大堆（Max Heap）。
         * 2. 循环地将堆顶元素（即当前最大值）与堆的最后一个元素交换，
         *    然后缩小堆的大小并重新调整堆，使其满足最大堆性质。
         *
         * 时间复杂度：O(N log N)
         * 空间复杂度：O(1)
         * 不稳定排序
         */
        template <typename RandIt>
        void HeapSort(RandIt first, RandIt last)
        {
            // std::make_heap(first,last); std::sort_heap(first,last);
            // std::make_heap(first,last); while(first!=last) std::pop_heap(first,last--);
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", 0, n, false);
            /*VIS*/ DSA_VIS_MSG(std::string("堆排序开始：n=") + std::to_string(n), true);
            // shift_down 函数（也叫 heapify ）：
            // 作用是让以 root_pos 为根的子树重新符合最大堆的性质。
            // 它假设左右子树已经是最大堆。
            auto shift_down = [&](int root_pos, const int end_pos)
            {
                // 对于0-based索引的数组，节点i的左孩子是2i+1，右孩子是2i+2
                int child_pos = root_pos * 2 + 2;
                while (child_pos < end_pos)
                {
                    // 找出左右孩子中较大的那一个
                    if ((arr[child_pos] < arr[child_pos - 1]))
                        --child_pos;
                    // 如果根节点比最大的孩子还大，则无需调整，堆性质已满足
                    if ((arr[child_pos] < arr[root_pos]))
                        return;
                    // 否则，将根与较大孩子交换
                    std::swap(arr[child_pos], arr[root_pos]); /*VIS*/ DSA_VIS_ARR_SWAP("A", root_pos, child_pos, true);
                    // 继续向下调整
                    root_pos = child_pos;
                    child_pos = root_pos * 2 + 2;
                }
                // 处理最后一个可能的左孩子（当循环因为 child_pos >= end_pos 退出时）
                --child_pos;
                if (child_pos < end_pos && (arr[root_pos] < arr[child_pos]))
                {
                    std::swap(arr[child_pos], arr[root_pos]); /*VIS*/ DSA_VIS_ARR_SWAP("A", root_pos, child_pos, true);
                }
                return;
            }; //[root_pos,end_pos)

            // 1. 构建最大堆 (Build-Max-Heap)
            // 从最后一个非叶子节点开始，向前逐个调用shift_down。
            // 最后一个非叶子节点的索引是 n/2 - 1。
            // making heap
            for (int i = n / 2 - 1; i >= 0; i--)
                shift_down(i, n);
            /*VIS*/ DSA_VIS_MSG("建堆完成，开始弹出最大值", true);
            // popping to sort

            // 2. 排序过程
            // 循环 n-1 次，每次将堆顶（最大值）换到数组末尾，然后调整剩余的堆。
            for (int i = n - 1; i > 0; i--)
            {
                /*VIS*/ DSA_VIS_MSG(std::string("将堆顶交换到下标 ") + std::to_string(i), false);
                // 将当前最大值 arr[0] 和堆的末尾元素 arr[i] 交换
                std::swap(arr[0], arr[i]); /*VIS*/ DSA_VIS_ARR_SWAP("A", 0, i, true);
                // 堆的大小减一（通过end_pos=i），并对新的堆顶进行shift_down操作
                shift_down(0, i);
            }
            /*VIS*/ DSA_VIS_MSG("堆排序结束", false);
        }

        /**
         * @brief 归并排序（递归实现的核心）
         * @tparam RandIt 随机访问迭代器类型
         * @param first 待排序区间的起始迭代器
         * @param last 待排序区间的结束迭代器（不包含）
         * @param bfirst 指向一个预先分配好的缓冲区的起始迭代器。该缓冲区的大小必须至少为 `std::distance(first, last)`。
         *
         * @note 这是归并排序的内部实现。它依赖于一个外部传入的缓冲区来避免在递归中反复分配内存，从而提高性能。
         */
        template <typename RandIt>
        void MergeSort(RandIt first, RandIt last, RandIt bfirst, RandIt abase, RandIt bbase)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            /*VIS*/ DSA_VIS_ARR_INIT("B", bfirst, bfirst + n);
            int base_offset = static_cast<int>(first - abase);
            /*VIS*/ DSA_VIS_MSG(std::string("归并排序处理区间 [") + std::to_string(base_offset) + "," + std::to_string(base_offset + n) + ")", false);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", base_offset, base_offset + n, true);
            int mid = n / 2;

            // 注意：这里递归调用时，原数组被分裂，但缓冲区也相应地被分裂使用
            // 排序左半部分 [first, first + mid)，使用缓冲区的前半部分 [bfirst, bfirst + mid)
            MergeSort(first, first + mid, bfirst, abase, bbase);
            // 排序右半部分 [first + mid, last)，使用缓冲区的后半部分 [bfirst + mid, bfirst + n)
            MergeSort(first + mid, last, bfirst + mid, abase, bbase);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", base_offset, base_offset + n, false);
            /*VIS*/ DSA_VIS_MSG("合并两个有序子区间", true);
            auto arr = first;
            // merging
            {
                // std::vector<iterator_value_type_t<RandIt>> buf;
                // buf.reserve(n);
                auto blast = bfirst; // blast 是用于写入缓冲区的迭代器
                int l = 0, r = mid;  // l 指向左半部分的当前元素索引，r 指向右半部分的当前元素索引

                // 比较左右两部分，将较小的元素依次放入缓冲区
                while (l < mid && r < n)
                {
                    if (arr[r] < arr[l])
                    {
                        auto written_value = arr[r++];
                        *(blast++) = written_value; /*VIS*/ DSA_VIS_ARR_SET("B", static_cast<int>((blast - bbase) - 1), written_value, true);
                    }
                    else
                    {
                        auto written_value = arr[l++];
                        *(blast++) = written_value; /*VIS*/ DSA_VIS_ARR_SET("B", static_cast<int>((blast - bbase) - 1), written_value, true);
                    }
                }
                // 将某一半数组中剩余的元素（如果有的话）复制到缓冲区末尾
                // 在 while 循环结束后，必然有一个子数组已经耗尽，另一个子数组可能还有剩余元素
                if (l < mid)
                {
                    for (; l < mid; ++l)
                    {
                        *(blast++) = arr[l]; /*VIS*/ DSA_VIS_ARR_SET("B", static_cast<int>((blast - bbase) - 1), arr[l], true);
                    }
                }
                else
                {
                    for (; r < n; ++r)
                    {
                        *(blast++) = arr[r]; /*VIS*/ DSA_VIS_ARR_SET("B", static_cast<int>((blast - bbase) - 1), arr[r], true);
                    }
                }
                for (int i = 0; i < n; ++i)
                {
                    arr[i] = bfirst[i]; /*VIS*/ DSA_VIS_ARR_SET("A", base_offset + i, arr[i], true);
                }
            }
            if (base_offset == 0)
                /*VIS*/ DSA_VIS_MSG("归并排序完成", false);
        }
        /**
         * @brief 归并排序（对外接口）
         * @tparam RandIt 随机访问迭代器类型
         * @param first 待排序区间的起始迭代器
         * @param last 待排序区间的结束迭代器（不包含）
         *
         * 工作原理：（分治法）
         * 1. 分解（Divide）：将数组递归地分成两半。
         * 2. 解决（Conquer）：递归地对两个子数组进行归并排序。
         * 3. 合并（Combine）：将两个已排序的子数组合并成一个有序的数组。
         *
         * 时间复杂度：O(N log N)
         * 空间复杂度：O(N) (一次性分配一个辅助数组以提高效率)
         * 稳定排序
         */
        template <typename RandIt>
        void MergeSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            std::vector<iterator_value_type_t<RandIt>> buf(n);
            MergeSort(first, last, buf.begin(), first, buf.begin());
        }

        // --- 基数排序的辅助组件 ---

        /**
         * @brief 有符号整数的基数排序适配器
         * 作用：将有符号整数映射到无符号整数，以便进行基数排序。
         * 如果直接对有符号数按位排序，负数的处理会很复杂（例如 -1 的二进制表示是全1）。
         * 通过与最小的负数（如-128 for int8_t）进行异或操作，
         * 可以将 [-128, 127] 的范围线性映射到 [0, 255] 的无符号范围，同时保持其原有的大小关系。
         * 例如，-128 -> 0, -127 -> 1, ..., 0 -> 128, ..., 127 -> 255。
         * 这样就可以像处理无符号数一样对它们进行基数排序了。
         */
        template <typename T>
        struct SignedIntRadixAdaper
        {
            // 编译时断言，确保这个适配器只用于有符号整型
            static_assert(std::is_signed_v<T> && std::is_integral_v<T>,
                          "SignedIntRadixAdaper is designed for signed integer");
            static constexpr size_t max_key_size = 256;           // 基数是256，因为我们每次处理一个字节(8位)
            static constexpr std::make_unsigned_t<T> mask = 0xff; // 掩码，用于取出当前字节

            // 最大的迭代编号，这里直接返回整数类型的字节数
            template <typename RandIt>
            static size_t end_key_index(RandIt first, RandIt last)
            {
                using TT = iterator_value_type_t<RandIt>;
                static_assert(std::is_signed_v<TT> && std::is_integral_v<TT>,
                              "SignedIntRadixAdaper is designed for signed integer");
                return sizeof(TT);
            }
            // 核心转换逻辑：将有符号数映射到保持顺序的无符号数
            static std::make_unsigned_t<T> convert(T value)
            {
                return value ^ (std::numeric_limits<T>::min());
            }
            // 提取整数 value 的第 index 个字节
            size_t operator()(T value, size_t index)
            {
                return (convert(value) >> (8 * index)) & mask;
            }
        };

        /**
         * @brief 无符号整数的基数排序适配器
         * 作用：直接提取无符号整数的特定字节作为排序的键。
         */
        template <typename T>
        struct UnsignedIntRadixAdaper
        {
            static_assert(std::is_unsigned_v<T> && std::is_integral_v<T>,
                          "UnsignedIntRadixAdaper is designed for unsigned integer");
            static constexpr size_t max_key_size = 256;
            static constexpr T mask = 0xff;
            template <typename RandIt>
            static size_t end_key_index(RandIt first, RandIt last)
            {
                using TT = iterator_value_type_t<RandIt>;
                {
                    static_assert(std::is_unsigned_v<T> && std::is_integral_v<T>,
                                  "UnsignedIntRadixAdaper is designed for unsigned integer");
                    return sizeof(TT);
                }
            }
            size_t operator()(T value, size_t index)
            {
                return ((value) >> (8 * index)) & mask;
            }
        };

        /**
         * @brief LSD（最低位优先）基数排序
         * @tparam RandIt 随机访问迭代器
         * @tparam Adapter 用于从元素中提取键的适配器
         *
         * 工作原理：
         * 从最低有效位（或字节）开始，对所有数字进行排序。
         * 然后，移动到次低有效位，再次对所有数字进行排序。
         * 重复此过程，直到处理完最高有效位。
         * 关键在于：每一轮排序都必须是“稳定”的，即相等键值的元素在排序后保持其原始相对顺序。
         * 这里使用“计数排序”作为每一轮的稳定排序算法。
         */
        template <typename RandIt, typename Adapter>
        void RadixSortLSD(RandIt first, RandIt last, Adapter &adapter)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            using value_type = iterator_value_type_t<RandIt>;
            auto arr = first;
            std::vector<value_type> buf(n);                  // 辅助数组，用于存放每轮排序的结果
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            /*VIS*/ DSA_VIS_ARR_INIT("B", buf.begin(), buf.end());
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", 0, n, false);
            /*VIS*/ DSA_VIS_MSG(std::string("基数排序开始：n=") + std::to_string(n), true);
            constexpr int max_key = Adapter::max_key_size;   // 对于整形，通常是256，因为按字节排序
            std::array<size_t, max_key> counts{};            // 计数数组
            int key_n = Adapter::end_key_index(first, last); // 比较的轮数，对于整形为总共要比较的字节数（例如int是4）

            // 从最低位字节 (key_id=0) 到最高位字节进行循环
            for (int key_id = 0; key_id < key_n; key_id++)
            {
                /*VIS*/ DSA_VIS_MSG(std::string("处理第 ") + std::to_string(key_id) + " 个字节", true);
                // --- 以下是计数排序的实现 ---
                // 1. 计数：统计当前字节值为 0-255 的元素各有多少个
                std::fill(counts.begin(), counts.end(), 0);
                for (int i = 0; i < n; i++)
                {
                    counts[adapter(arr[i], key_id)]++;
                }

                // 2. 累加：计算每个桶的右边界。counts[i] 表示小于等于i的元素总数
                for (int i = 1; i < max_key; ++i)
                {
                    counts[i] += counts[i - 1];
                }

                // 3. 放置：从后向前遍历原数组，以保证排序的稳定性。
                //    根据 counts 数组中记录的位置，将元素放入辅助数组 buf 中。
                for (int i = n - 1; i >= 0; i--)
                {
                    buf[--counts[adapter(arr[i], key_id)]] = arr[i]; /*VIS*/ DSA_VIS_ARR_SET("B", static_cast<int>(counts[adapter(arr[i], key_id)]), arr[i], true);
                }

                // 4. 复制：将本轮排序后的结果从 buf 复制回原数组
                for (int i = 0; i < n; ++i)
                {
                    first[i] = buf[i]; /*VIS*/ DSA_VIS_ARR_SET("A", i, first[i], true);
                }
            }
            /*VIS*/ DSA_VIS_MSG("基数排序结束", false);
        }

        /**
         * @brief 整数基数排序（自动选择有符号/无符号适配器）
         */
        template <typename RandIt>
        void IntRadixSort(RandIt first, RandIt last)
        {
            using value_type = iterator_value_type_t<RandIt>;
            static_assert(std::is_integral_v<value_type>, "IntRadixSort is designed for integer");
            if constexpr (std::is_signed_v<value_type>)
            {
                SignedIntRadixAdaper<value_type> adapter{};
                RadixSortLSD(first, last, adapter);
            }
            else
            {
                UnsignedIntRadixAdaper<value_type> adapter{};
                RadixSortLSD(first, last, adapter);
            }
        }

        // --- 希尔排序的辅助组件 ---

        /**
         * @brief Knuth 间隔序列生成器 (h = 3*h + 1)
         * 例如: 1, 4, 13, 40, ...
         */
        // O( N^(3/2) ) worse case
        struct KnuthGapGenerator
        {
            explicit KnuthGapGenerator(size_t n)
            {
                // 计算出小于 n/3 的最大间隔
                for (cur_h = 1; cur_h < n; cur_h = cur_h * 3 + 1)
                    ;
                cur_h /= 3;
            }
            // 每次调用，返回一个递减的间隔值
            size_t operator()()
            {
                size_t gap = cur_h;
                cur_h /= 3;
                return gap;
            }

        private:
            size_t cur_h;
        };

        /**
         * @brief Pratt (2^p * 3^q) 间隔序列生成器 (也称 Sedgewick 序列的一种)
         * 序列是形如 1, 2, 3, 4, 6, 8, 9, 12, ... 的数。
         * 它的最坏情况时间复杂度是 O(N log^2 N)。
         */
        /**
         *  O(N log^2 N) worse case
         */
        struct PrattGapGenerator
        {

            explicit PrattGapGenerator(size_t n)
            {
                if (n < 2)
                    return;
                gaps.push_back(1);
                size_t p2_idx = 0; // 指向下一个要乘以2的基数
                size_t p3_idx = 0; // 指向下一个要乘以3的基数
                // 生成所有小于n的间隔
                while (true)
                {
                    // 从候选项中选出最小的作为下一个间隔
                    size_t next_val = std::min(gaps[p2_idx] * 2, gaps[p3_idx] * 3);
                    if (next_val >= n)
                        break;
                    // 只有当新生成的间隔比当前最大间隔还大时才加入，以避免重复
                    if (next_val > gaps.back())
                        gaps.push_back(next_val);
                    // 移动对应的指针
                    if (next_val == gaps[p2_idx] * 2)
                        p2_idx++;
                    if (next_val == gaps[p3_idx] * 3)
                        p3_idx++;
                }
                // 从后向前使用这些间隔，所以将索引指向末尾
                cur_idx = gaps.size();
            }

            // 每次调用，返回一个递减的间隔值
            size_t operator()()
            {
                return (cur_idx > 0) ? gaps[--cur_idx] : 0;
            }

        private:
            std::vector<size_t> gaps;
            size_t cur_idx;
        };

        /**
         * @brief 希尔排序
         * @tparam GapGen 间隔序列生成器类型
         * @tparam RandIt 随机访问迭代器类型
         *
         * 工作原理：
         * 它是插入排序的一种更高效的改进版本。
         * 它通过比较相距一定间隔的元素来进行工作，各趟比较所用的距离随着算法的进行而减小，
         * 直到只比较相邻元素的最后一趟排序为止（此时相当于一次插入排序）。
         * 由于开始时间隔较大，元素可以一次性跳过很多位置，这使得它比普通插入排序快。
         *
         * 时间复杂度：依赖于间隔序列，可以达到 O(N log^2 N) 或 O(N^(3/2))
         * 空间复杂度：O(1)
         * 不稳定排序
         */
        template <typename GapGen, typename RandIt>
        void ShellSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            /*VIS*/ DSA_VIS_ARR_INIT("A", first, last);
            /*VIS*/ DSA_VIS_ARR_FOCUS("A", 0, n, false);
            /*VIS*/ DSA_VIS_MSG(std::string("希尔排序开始：n=") + std::to_string(n), true);
            GapGen gap_generator(n);

            // 外层循环：获取递减的间隔(gap)序列
            for (int gap = gap_generator(); gap > 0; gap = gap_generator())
            {
                /*VIS*/ DSA_VIS_MSG(std::string("当前 gap=") + std::to_string(gap), true);

                // 中层循环：对每个以gap为间隔的子序列进行插入排序
                // 这个循环从第gap个元素开始，逐个处理到数组末尾
                for (int i = gap; i < n; ++i)
                {

                    // 下面是标准的插入排序逻辑，但步长是gap而不是1
                    auto tmp = arr[i];
                    int j = i;
                    for (; j >= gap && tmp < arr[j - gap]; j -= gap)
                    {
                        arr[j] = arr[j - gap]; /*VIS*/ DSA_VIS_ARR_SET("A", j, arr[j], true);
                    }
                    arr[j] = tmp; /*VIS*/ DSA_VIS_ARR_SET("A", j, arr[j], (j != i));
                }
            }
            /*VIS*/ DSA_VIS_MSG("希尔排序结束", false);
        }
        // 希尔排序的默认版本，使用 Knuth 间隔序列
        template <typename RandIt>
        void ShellSort(RandIt first, RandIt last)
        {
            ShellSort<KnuthGapGenerator>(first, last);
        }
    }
}
