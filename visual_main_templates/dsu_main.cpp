#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../tree/disjoint_set_union.hpp"
#include "input_utils.hpp"

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    int n = json_int_value(json, "n", 6);
    auto ops = parse_dsu_ops(json_string_value(json, "operations", "union_rank 1 2\nunion_rank 3 4\nunion_rank 2 3\nfind 1\nunion_size 5 6\nunion_size 1 6"));

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("dsu");
    DSA::Tree::DisjointSetUnion::DSU dsu(n);
    for (auto [op, a, b] : ops)
    {
        if (op == "find")
            dsu.Find(a);
        else if (op == "union_size")
            dsu.UnionBySize(a, b);
        else
            dsu.UnionByRank(a, b);
    }
    DSA_VIS_END();
    return 0;
}
