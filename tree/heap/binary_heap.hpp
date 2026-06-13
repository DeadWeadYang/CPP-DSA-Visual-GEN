#pragma once
#include <vector>
#include <deque>
#include <functional>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstdint>
#include "../../vis_trace.hpp"

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
                        DSA_VIS_ONLY(vis_ensure_init());
                        DSA_VIS_STEP(std::string("堆插入：新元素 ") + vis_to_string(d) + " 放到末尾 idx=" + std::to_string(size_r) + "，堆大小 +1"); /*VIS*/
                        // 如果底层vector容量足够，直接在逻辑末尾放置元素，否则扩展向量。
                        if (size_r == data.size())
                            data.push_back(d);
                        else
                            data[size_r] = d;
                        DSA_VIS_ONLY(vis_add_index(size_r, false));
                        ++size_r;
                        DSA_VIS_ONLY(vis_rebuild_array());
                        // 对新加入的元素执行上浮操作，并增加堆的逻辑大小。
                        adjustUp(static_cast<int>(size_r - 1));
                        DSA_VIS_STEP("堆插入完成：上浮结束"); /*VIS*/
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
                        DSA_VIS_ONLY(vis_ensure_init());
                        if (!size_r)
                            return;
                        DSA_VIS_STEP(std::string("堆弹出堆顶：") + vis_to_string(data[0])); /*VIS*/
                        if (size_r == 1)
                        {
                            DSA_VIS_ONLY(vis_remove_last_index(false));
                            --size_r;
                            DSA_VIS_ONLY(vis_rebuild_array());
                            DSA_VIS_STEP("堆弹出完成：删除唯一元素后堆为空"); /*VIS*/
                            return;
                        }
                        // 将堆顶元素与最后一个元素交换，然后将堆的逻辑大小减一。
                        swap_indices(0, size_r - 1);
                        DSA_VIS_ONLY(vis_remove_last_index(false));
                        --size_r;
                        DSA_VIS_ONLY(vis_rebuild_array());
                        // 对新的根节点执行下沉操作。
                        adjustDown(0);
                        DSA_VIS_STEP("堆弹出完成：下沉结束"); /*VIS*/
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
#ifdef DSA_VIS_ENABLE
                    static constexpr const char *k_vis_arr = "HA";
                    static constexpr const char *k_vis_tree = "HT";
                    bool vis_ready = false;
                    size_t vis_tree_size = 0;

                    std::string vis_to_string(const T &value) const
                    {
                        std::ostringstream oss;
                        oss << value;
                        return oss.str();
                    }
                    std::string vis_index_label(size_t idx) const
                    {
                        return std::string("i=") + std::to_string(idx);
                    }
                    const void *vis_index_ref(size_t idx) const
                    {
                        return reinterpret_cast<const void *>(static_cast<std::uintptr_t>(idx + 1));
                    }
                    void vis_ensure_init()
                    {
                        if (vis_ready)
                            return;
                        DSA_VIS_ARR_INIT(k_vis_arr, data.begin(), data.begin() + size_r);          /*VIS*/
                        DSA_VIS_ARR_REBUILD(k_vis_arr, data.begin(), data.begin() + size_r, false); /*VIS*/
                        DSA_VIS_BT_INIT(k_vis_tree);                                                /*VIS*/
                        vis_tree_size = 0;
                        for (size_t i = 0; i < size_r; ++i)
                            vis_add_index(i, false);
                        vis_ready = true;
                    }
                    void vis_rebuild_array()
                    {
                        DSA_VIS_ARR_REBUILD(k_vis_arr, data.begin(), data.begin() + size_r, false); /*VIS*/
                    }
                    void vis_add_index(size_t idx, bool step)
                    {
                        if (idx < vis_tree_size)
                        {
                            vis_refresh_index(idx, step);
                            return;
                        }
                        for (size_t cur = vis_tree_size; cur <= idx; ++cur)
                        {
                            const void *node = vis_index_ref(cur);
                            DSA_VIS_BT_NEW_NODE(k_vis_tree, node, vis_index_label(cur), false); /*VIS*/
                            if (cur == 0)
                                DSA_VIS_BT_SET_ROOT(k_vis_tree, node, false); /*VIS*/
                            else
                                DSA_VIS_BT_LINK(k_vis_tree, vis_index_ref((cur - 1) / 2), ((cur & 1U) == 0U), node, false); /*VIS*/
                            vis_refresh_index(cur, false);
                        }
                        vis_tree_size = idx + 1;
                        vis_refresh_index(idx, step);
                    }
                    void vis_refresh_index(size_t idx, bool step)
                    {
                        if (idx >= size_r)
                            return;
                        DSA_VIS_BT_SET_NOTE(k_vis_tree, vis_index_ref(idx), std::string("v=") + vis_to_string(data[idx]), step); /*VIS*/
                    }
                    void vis_remove_last_index(bool step)
                    {
                        if (!size_r || vis_tree_size == 0)
                            return;
                        size_t last = size_r - 1;
                        if (last == 0)
                            DSA_VIS_BT_SET_ROOT(k_vis_tree, static_cast<const void *>(nullptr), false); /*VIS*/
                        DSA_VIS_BT_REMOVE_NODE(k_vis_tree, vis_index_ref(last), false);                 /*VIS*/
                        DSA_VIS_BT_DESTROY_NODE(k_vis_tree, vis_index_ref(last), false);                /*VIS*/
                        vis_tree_size = last;
                    }
#endif
                    void swap_indices(size_t i, size_t j)
                    {
                        if (i == j)
                            return;
                        DSA_VIS_ARR_MARK(k_vis_arr, static_cast<int>(i), false); /*VIS*/
                        DSA_VIS_ARR_MARK(k_vis_arr, static_cast<int>(j), false); /*VIS*/
                        std::swap(data[i], data[j]);
                        DSA_VIS_ARR_SWAP(k_vis_arr, static_cast<int>(i), static_cast<int>(j), false); /*VIS*/
                        DSA_VIS_ONLY(vis_refresh_index(i, false));
                        DSA_VIS_ONLY(vis_refresh_index(j, false));
                        DSA_VIS_ARR_UNMARK(k_vis_arr, static_cast<int>(i), false); /*VIS*/
                        DSA_VIS_ARR_UNMARK(k_vis_arr, static_cast<int>(j), false); /*VIS*/
                    }

                    /**
                     * @brief 上浮操作（sift-up）。
                     * @param pos 开始上浮的元素索引。
                     * @details 从 pos 位置开始，将元素与其父节点比较，如果当前元素比父节点“大”
                     * （根据比较器 comp），则交换它们，直到到达根节点或满足堆属性。
                     */
                    void adjustUp(int pos)
                    {
                        int parent = (pos - 1) / 2; // 计算父节点的索引。
                        DSA_VIS_STEP(std::string("开始上浮：idx=") + std::to_string(pos)); /*VIS*/
                        // 当未到达根节点且当前节点比父节点“大”时循环。
                        while (pos > 0 && comp(data[parent], data[pos]))
                        {
                            swap_indices(static_cast<size_t>(parent), static_cast<size_t>(pos));
                            DSA_VIS_STEP(std::string("上浮交换：idx ") + std::to_string(pos) + " 与父节点 " + std::to_string(parent));
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
                        DSA_VIS_STEP(std::string("开始下沉：idx=") + std::to_string(pos)); /*VIS*/
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
                            swap_indices(static_cast<size_t>(pos), static_cast<size_t>(child));
                            DSA_VIS_STEP(std::string("下沉交换：idx ") + std::to_string(pos) + " 与孩子 " + std::to_string(child));
                            // 继续向下调整。
                            pos = child;
                            child = pos * 2 + 2;
                        }
                        // 处理没有右子节点的特殊情况。
                        --child; // child 指向左子节点。
                        // 如果左子节点存在，且比当前节点“大”，则交换。
                        if (child < size_r && comp(data[pos], data[child]))
                        {
                            swap_indices(static_cast<size_t>(child), static_cast<size_t>(pos));
                            DSA_VIS_STEP(std::string("下沉交换：idx ") + std::to_string(pos) + " 与左孩子 " + std::to_string(child));
                        }
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
