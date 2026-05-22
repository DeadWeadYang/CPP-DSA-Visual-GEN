#pragma once
#include <vector>
#include <functional>
#include <algorithm>

namespace DSA
{
    namespace Tree
    {
        namespace Heap
        {
            namespace BinaryHeap
            {
                /**
                 * @brief 一个通用的二叉堆实现。
                 * @tparam T 存储在堆中元素的类型。
                 * @tparam Compare 比较元素的函数对象（仿函数）。默认为 std::less<T>，这会创建一个最大堆。
                 * @details
                 * 二叉堆是一个基于数组实现的完全二叉树，它满足堆属性：
                 * - 在最大堆中，任意节点的值都大于或等于其子节点的值。
                 * - 在最小堆中，任意节点的值都小于或等于其子节点的值。
                 * 这个实现通过模板参数 Compare 支持最大堆和最小堆。
                 */
                template <typename T, typename Compare = std::less<T>>
                struct BinaryHeap
                {
                    using value_type = T;
                    size_t size() const { return size_r; }
                    bool empty() const { return !size_r; }
                    const T &top() const { return data[0]; }
                    void clear()
                    {
                        size_r = 0;
                        data.clear();
                    }
                    /**
                     * @brief向堆中添加一个新元素。
                     * @param d 要添加的元素。
                     * @details
                     * 元素被添加到数组的末尾，然后通过 `adjustUp` 操作（上浮）
                     * 将其移动到正确的位置，以维持堆的属性。
                     */
                    void push(const T &d)
                    {
                        // 如果底层vector容量足够，直接在逻辑末尾放置元素，否则扩展向量。
                        if (size_r == data.size())
                            data.push_back(d);
                        else
                            data[size_r] = d;
                        // 对新加入的元素执行上浮操作，并增加堆的逻辑大小。
                        adjustUp(size_r++);
                        // equivalent to std::push_heap
                    }
                    /**
                     * @brief 移除堆顶元素。
                     * @details
                     * 堆顶元素与最后一个元素交换，然后逻辑大小减一。
                     * 之后对新的堆顶元素执行 `adjustDown` 操作（下沉），
                     * 以恢复堆的属性。
                     */
                    void pop()
                    {
                        // 将堆顶元素与最后一个元素交换，然后将堆的逻辑大小减一。
                        std::swap(data[0], data[--size_r]);
                        // 对新的根节点执行下沉操作。
                        adjustDown(0);
                    }
                    // 释放底层向量中未使用的容量，减少内存占用。
                    void shrink_to_fit()
                    {
                        data.resize(size_r);
                        data.shrink_to_fit();
                    }
                    // 构造函数，允许传入一个自定义的比较器。
                    explicit BinaryHeap(const Compare &cmp = Compare{}) : comp{cmp} {}

                private:
                    std::vector<T> data; // 使用 std::vector 作为堆的底层存储。
                    size_t size_r = 0;   // 堆的逻辑大小，可能小于 `data.size()`。
                    Compare comp;        // 用于比较元素的函数对象。

                    /**
                     * @brief 上浮操作（sift-up）。
                     * @param pos 开始上浮的元素索引。
                     * @details 从 pos 位置开始，将元素与其父节点比较，如果当前元素比父节点“大”
                     * （根据比较器 comp），则交换它们，直到到达根节点或满足堆属性。
                     */
                    void adjustUp(int pos)
                    {
                        int parent = (pos - 1) / 2; // 计算父节点的索引。
                        // 当未到达根节点且当前节点比父节点“大”时循环。
                        while (pos > 0 && comp(data[parent], data[pos]))
                        {
                            std::swap(data[parent], data[pos]);
                            pos = parent;
                            parent = (pos - 1) / 2;
                        }
                    }

                    /**
                     * @brief 下沉操作（sift-down）。
                     * @param pos 开始下沉的元素索引。
                     * @details 从 pos 位置开始，将元素与其“更大”的子节点比较，如果子节点比当前元素“大”
                     * （根据比较器 comp），则交换它们，直到该元素成为叶子节点或满足堆属性。
                     */
                    void adjustDown(int pos)
                    {
                        int child = pos * 2 + 2; // 先指向右子节点。
                        // 当节点至少有右子节点时循环。
                        while (child < size_r)
                        {
                            // 比较左右子节点，让 child 指向“更大”的那个。
                            // comp(right, left) 为 true (例如 right < left)，则 child 指向左子节点。
                            if (comp(data[child], data[child - 1]))
                                --child;
                            // 如果“更大”的子节点并不比当前节点“大”，则堆属性已满足，可以返回。
                            if (!comp(data[pos], data[child]))
                                return;
                            // 交换当前节点和其“更大”的子节点。
                            std::swap(data[pos], data[child]);
                            // 继续向下调整。
                            pos = child;
                            child = pos * 2 + 2;
                        }
                        // 处理没有右子节点的特殊情况。
                        --child; // child 指向左子节点。
                        // 如果左子节点存在，且比当前节点“大”，则交换。
                        if (child < size_r && comp(data[pos], data[child]))
                            std::swap(data[child], data[pos]);
                        return;
                    }
                };
                template <typename T>
                using BinaryHeapMax = BinaryHeap<T>;
                template <typename T>
                using BinaryHeapMin = BinaryHeap<T, std::greater<T>>;
            }
        }
    }
}