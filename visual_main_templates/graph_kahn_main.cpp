#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../graph/graph_basic.hpp"
#include "../graph/topological_sorting.hpp"
#include "input_utils.hpp"

#include <vector>

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    using Graph = DSA::Graph::GraphBasic::Graph;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    int n = json_int_value(json, "n", 5);
    auto edges = parse_edges(json_string_value(json, "edges", "1 2\n1 3\n2 4\n3 4\n4 5"));

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("graph_kahn");
    Graph g(n, edges, true);
    std::vector<int> order;
    DSA::Graph::TopologicalSorting::TopoSort::Kahn(g, order);
    DSA_VIS_END();
    return 0;
}
