# CPP-DSA Visual GEN

CPP-DSA 是 DSA Visual 项目的 C++ 算法与数据结构子仓库。它保留普通模板库的使用方式，同时在关键算法流程中加入可关闭的可视化 hook，用于生成 JSONL trace，供前端播放器渲染动画和同步代码追踪。

它在总装仓库中的位置通常是：

```text
DSAVisual/
  CPP-DSA/              本仓库，C++ 算法、hook、main 模板
  JSAV/                 播放器和 JSAV 扩展
  dist/                 可直接部署的前端发布包
  doc/                  完整开发与部署文档
```

## 项目定位

这个仓库有两种使用方式：

| 模式 | 用途 |
| --- | --- |
| 普通算法库 | 不定义 `DSA_VIS_ENABLE`，hook 会退化为空操作，算法按普通 C++ 模板库使用 |
| 可视化 trace 生成器 | 定义 `DSA_VIS_ENABLE`，算法执行时写出 JSONL trace，用于 DSA Visual 播放器 |

核心链路：

```text
input.json
  -> visual_main_templates/*.cpp
  -> C++ algorithm + hook
  -> trace.jsonl
  -> DSA Visual JS player
```

## 目录结构

```text
CPP-DSA/
  collections/             容器
  tree/                    树结构、堆、并查集、Huffman
  graph/                   图结构与图算法
  sorting/                 排序算法与排序可视化辅助层
  string/                  字符串算法
  visual_main_templates/   给后端 trace service 编译运行的 main 模板
  test/                    普通算法正确性测试
  vis_trace.hpp            JSONL trace logger 与 hook API
  test.cpp                 普通测试入口
  test_all.hpp             测试聚合入口
  collections.hpp          容器聚合头文件
  utils.hpp                通用工具
```

## 已覆盖模块

### Collections

- `List`
- `Vector`
- `Set` / `MultiSet`
- `Map` / `MultiMap`
- `Queue` / `Stack`
- `HashTable`

### Tree

- 二叉树基础框架
- 二叉搜索树
- AVL 树
- 红黑树
- Huffman 树
- 二叉堆与优先队列适配器
- 并查集

树结构的可视化 hook 已覆盖节点创建、连接、删除、旋转、颜色、标记和说明文本。二叉树播放器侧采用 ghost root 策略，因此 C++ 侧应尽量通过 hook 描述拓扑变化，而不是依赖播放器做 `sync` 合并。

### Graph

- Floyd
- Bellman-Ford
- SPFA
- Dijkstra
- Kruskal
- Prim
- Kahn 拓扑排序
- AOE 关键路径

图算法的可视化重点不是复刻所有辅助容器内部状态，而是突出算法核心对象：当前点、当前边、候选/确认关系、状态表和历史更新。最短路、生成树等算法会结合图动画与状态表。

### Sorting

- Selection sort
- Bubble sort
- Insertion sort
- Quick sort
- Heap sort
- Merge sort
- Integer radix sort
- Shell sort

排序 hook 通过 `sorting/sorting_vis.hpp` 集中封装，算法主流程中尽量只保留解释性动作，避免让可视化代码污染模板接口。

### String

- KMP 前缀函数
- KMP `next` 数组
- KMP 匹配

字符串可视化会区分预处理阶段和匹配阶段，并用比较、回退、接受匹配等事件描述过程。

## 普通算法库使用

不定义 `DSA_VIS_ENABLE` 时，所有可视化宏都是空操作。

编译全部测试：

```sh
g++ -std=c++20 -O2 -Wall -Wextra -pedantic test.cpp -o dsa_test
./dsa_test
```

Windows PowerShell 示例：

```powershell
g++ -std=c++20 -O2 -Wall -Wextra -pedantic test.cpp -o dsa_test.exe
.\dsa_test.exe
```

单独使用排序：

```cpp
#include "sorting/sorting.hpp"
#include <vector>

int main() {
    std::vector<int> values{5, 1, 4, 2, 3};
    DSA::Sorting::QuickSort(values.begin(), values.end());
}
```

单独使用集合：

```cpp
#include "collections/set_multiset.hpp"

int main() {
    DSA::Collections::SetOrMultiset::Set<int> s;
    s.insert(3);
    s.insert(1);
    s.insert(2);
}
```

## 生成可视化 trace

可视化入口模板位于 `visual_main_templates/`。这些文件会：

1. 读取输入 JSON。
2. 开启 `DSA_VIS_ENABLE`。
3. 调用对应算法。
4. 输出 JSONL trace。

例如编译 quick sort trace 模板：

```sh
g++ -std=c++20 -I. visual_main_templates/sorting_quick_main.cpp -o sorting_quick
```

准备输入：

```json
{
  "values": "7 2 6 3 1 5 4"
}
```

运行：

```sh
./sorting_quick input.json trace.jsonl
```

Windows PowerShell 示例：

```powershell
g++ -std=c++20 -I. visual_main_templates/sorting_quick_main.cpp -o sorting_quick.exe
.\sorting_quick.exe input.json trace.jsonl
```

生成的 `trace.jsonl` 可以交给 DSA Visual 播放器：

```js
DSAVisual.mount("#player", {
  traceText,
  assetsBase: "/dsa-visual/assets/",
  sourceBase: "/sources/",
  codeTrace: true
});
```

## Main 模板

当前 `visual_main_templates/` 包含：

```text
sorting_selection_main.cpp
sorting_bubble_main.cpp
sorting_insertion_main.cpp
sorting_quick_main.cpp
sorting_heap_main.cpp
sorting_merge_main.cpp
sorting_radix_main.cpp
sorting_shell_main.cpp
bst_ops_main.cpp
avl_ops_main.cpp
rb_ops_main.cpp
binary_heap_main.cpp
dsu_main.cpp
huffman_main.cpp
graph_dijkstra_main.cpp
graph_floyd_main.cpp
graph_prim_main.cpp
graph_kahn_main.cpp
kmp_match_main.cpp
```

这些模板是给后端服务按 `algorithmId` 白名单编译运行的，不建议让用户直接提交任意 C++ 文件路径。

## Hook 设计约定

可视化 hook 定义在 `vis_trace.hpp`，并按模块拆出轻量辅助层，例如：

```text
sorting/sorting_vis.hpp
graph/graph_vis.hpp
tree/tree_vis.hpp
string/string_vis.hpp
```

主要约定：

- `DSA_VIS_ENABLE` 未定义时，hook 必须退化为空操作，不能改变算法结果和控制流。
- 可解释动作应配套 `DSA_VIS_STEP(...)` 或模块级说明，让播放器 step 和教学文本自然对应。
- 算法主流程中不要堆大段纯可视化代码，固定组合应下沉到 `vis` 辅助命名空间。
- 辅助数据结构不一定逐步可视化，应围绕教学目标呈现。例如 Dijkstra 的 lazy-delete priority queue 不展示过期堆顶清理细节，只展示候选加入、当前 top、距离表变化。
- 状态表适合表达动态规划、最短路、生成树等算法的核心变化，可以用 matrix/vector/table hook 记录当前值、焦点和历史更新。
- 树结构增删节点应打具体拓扑 hook，避免依赖大范围 `sync`。

常用宏示例：

```cpp
DSA_VIS_BEGIN(trace_path);
DSA_VIS_CTX("sorting_quick");
DSA_VIS_STEP("选择当前区间的基准元素");
DSA_VIS_END();
```

数组：

```cpp
DSA_VIS_ARR_INIT("A", first, last);
DSA_VIS_ARR_FOCUS("A", l, r, false);
DSA_VIS_ARR_SWAP("A", i, j, false);
DSA_VIS_STEP("交换两个位于错误分区的元素");
```

状态表：

```cpp
DSA_VIS_STATE_INIT_MATRIX_LABELED("dist", n, n, "距离表", 1, 1, "顶点", false);
DSA_VIS_STATE_SET("dist", i, j, value, false);
DSA_VIS_STATE_HISTORY("dist", i, j, value, "通过中间点更新", false);
```

图：

```cpp
DSA_VIS_G_INIT("G", directed);
DSA_VIS_G_MARK_NODE("G", u, false);
DSA_VIS_G_MARK_EDGE("G", u, v, false);
```

树：

```cpp
DSA_VIS_BT_NEW_NODE("T", node, value, false);
DSA_VIS_BT_LINK("T", parent, true, child, false);
DSA_VIS_BT_ROTATE_LEFT("T", pivot, false);
```

更完整的 hook 说明在外层 DSAVisual 文档中维护：

```text
doc/dsa-visual/cpp-hooks.html
doc/dsa-visual/cpp-extend-algorithm.html
doc/dsa-visual/development-guide.html
```

## 新增算法建议流程

1. 先保证算法在不定义 `DSA_VIS_ENABLE` 时能通过普通测试。
2. 分析哪些动作值得教学解释，给这些动作添加说明文本和精确 hook。
3. 如果 hook 组合较固定，放到模块的 `vis` 辅助文件中。
4. 在 `visual_main_templates/` 增加一个 main 模板，输入统一来自 JSON。
5. 运行模板生成 `trace.jsonl`，用播放器检查动画和代码追踪。
6. 如果需要新的播放器事件，再同步扩展 JS 解析器和文档。

## 输入模板约定

后端 trace service 通常会调用：

```text
main input.json trace.jsonl
```

模板通过 `visual_main_templates/input_utils.hpp` 读取输入。面向新手的输入表单由前端 catalog 决定，C++ main 模板只负责把 JSON 转为算法需要的数据。

## 已知状态

- `collections/deque.hpp` 仍是占位实现。
- `test.cpp` 用于验证普通算法正确性，不用于验证 hook 视觉效果。
- 可视化 trace 需要通过 `visual_main_templates/` 或单独编写的小 main 验证。
