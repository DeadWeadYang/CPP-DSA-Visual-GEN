# Data-Structure-Algorithm

一个以教学和工程实践为目标的 C++ 数据结构与算法模板库。  
代码风格强调:
- 可读性与可扩展性
- 接口尽量贴近 STL 使用习惯
- 在复杂度、健壮性、实现噪声之间做平衡

## 1. 项目定位

这个仓库不是“最短代码”或“刷题脚本”集合，而是:
- 可复用的数据结构内核
- 典型算法实现
- 配套回归测试（与 `std` 或已知结果对照）

适合场景:
- 学习容器/迭代器/树结构实现细节
- 做课程项目、面试复盘、算法实验
- 作为自定义容器/算法的起点

## 2. 目录结构

```text
.
├─ collections/    # 容器
├─ tree/           # 树结构与并查集
├─ graph/          # 图与图算法
├─ sorting/        # 排序算法
├─ string/         # 字符串算法
├─ test/           # 各模块测试
├─ test_all.hpp    # 测试聚合入口
├─ test.cpp        # 测试主程序
└─ utils.hpp       # 通用工具
```

## 3. 已实现模块

### 3.1 Collections

- `List`（双向链表，header 哨兵，双向迭代器）
- `Vector`（动态数组，迭代器与内存/对象生命周期分层）
- `Set` / `MultiSet`（基于树实现，接口风格接近 STL）
- `Map` / `MultiMap`（基于树实现）
- `Queue` / `Stack`（适配器风格封装）
- `HashTable`（独立哈希表实现）

聚合头文件:
- [`collections.hpp`](collections.hpp)

### 3.2 Tree

- 二叉树基础框架（中序迭代器、遍历、复制、交换拓扑）
- 二叉搜索树（`insert/find/erase/lower_bound/upper_bound/equal_range`）
- AVL 平衡树
- 红黑树
- Huffman 树
- 二叉堆与优先队列适配器
- 并查集（DSU）

关键文件:
- [`tree/binary_tree/binary_tree_basic.hpp`](tree/binary_tree/binary_tree_basic.hpp)
- [`tree/binary_tree/binary_search_tree/binary_search_tree.hpp`](tree/binary_tree/binary_search_tree/binary_search_tree.hpp)
- [`tree/binary_tree/binary_search_tree/AVL_balanced_tree.hpp`](tree/binary_tree/binary_search_tree/AVL_balanced_tree.hpp)
- [`tree/binary_tree/binary_search_tree/RB_balanced_tree.hpp`](tree/binary_tree/binary_search_tree/RB_balanced_tree.hpp)

### 3.3 Graph

- 基础图结构
- 加权图结构
- 最短路:
  - Floyd
  - Bellman-Ford
  - SPFA（Bellman-Ford 队列优化）
  - Dijkstra（负边检测）
- 最小生成树:
  - Kruskal
  - Prim
- 拓扑排序（Kahn）
- AOE 关键路径

### 3.4 Sorting

- `SelectionSort`
- `BubbleSort`
- `InsertionSort`
- `QuickSort`（可切换采样策略）
- `HeapSort`
- `MergeSort`
- `IntRadixSort`
- `ShellSort`（含不同 gap 生成器）

### 3.5 String

- KMP 前缀函数
- KMP `next` 数组
- KMP 匹配

## 4. 设计要点

### 4.1 STL 风格接口

容器普遍提供:
- `begin/end`
- `size/empty`
- `insert/erase/find`
- `lower_bound/upper_bound/equal_range`（树容器）

测试中大量使用 `std::set/std::map/std::list/std::priority_queue` 做行为对照。

### 4.2 节点与哨兵设计

链表与树都采用了 header 哨兵，目的是统一边界处理。  
目前节点隐派生值节点（`VNode`）的设计，核心动机是:
- header 节点不持有 `value`
- 避免对 `T` 默认构造的硬依赖
- 尽量不引入额外运行时判断

### 4.3 可增强节点（Augment Policy）

`BinarySearchTree` 支持策略化增强数据:
- 默认增强（空）
- AVL 的子树高度增强
- 可扩展到秩统计等增强字段

## 5. 快速开始

### 5.1 编译并运行全部测试

在仓库根目录执行（示例为 g++）:

```bash
g++ -std=c++20 -O2 -Wall -Wextra -pedantic test.cpp -o dsa_test
./dsa_test
```

Windows + MSVC 示例:

```powershell
cl /std:c++20 /EHsc /O2 test.cpp
.\test.exe
```

测试入口:
- [`test.cpp`](test.cpp)
- [`test_all.hpp`](test_all.hpp)

### 5.2 单独使用某个模块

例如使用排序模块:

```cpp
#include "sorting/sorting.hpp"
#include <vector>

int main() {
    std::vector<int> a{5, 1, 4, 2, 3};
    DSA::Sorting::QuickSort(a.begin(), a.end());
}
```

例如使用 `Set`:

```cpp
#include "collections/set_multiset.hpp"

int main() {
    DSA::Collections::SetOrMultiset::Set<int> s;
    s.insert(3);
    s.insert(1);
    s.insert(2);
}
```

## 6. 当前状态与已知问题

- `collections/deque.hpp` 目前是占位实现，尚未完成。



