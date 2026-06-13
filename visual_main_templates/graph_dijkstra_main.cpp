#define DSA_VIS_ENABLE
#include "../vis_trace.hpp"
#include "../graph/weighted_graph.hpp"
#include "../graph/shortest_path.hpp"
#include "input_utils.hpp"

#include <vector>

int main(int argc, char **argv)
{
    using namespace DSA::VisualTemplates;
    using WGraph = DSA::Graph::GraphWeighted::WGraph<int>;
    auto input_path = arg_or_default(argc, argv, 1, "input.json");
    auto trace_path = arg_or_default(argc, argv, 2, "trace.output.jsonl");
    auto json = read_file(input_path);
    int n = json_int_value(json, "n", 5);
    int source = json_int_value(json, "source", 1);
    bool directed = json_bool_value(json, "directed", false);
    auto edges = parse_weighted_edges(json_string_value(json, "edges", "1 2 2\n1 3 5\n2 3 1\n2 4 2\n3 5 3\n4 5 1"));

    DSA_VIS_BEGIN(trace_path);
    DSA_VIS_CTX("graph_dijkstra");
    WGraph g(n, edges, directed);
    std::vector<int> dis;
    DSA::Graph::ShortestPath::SingleSource::Dijkstra<int>(g, source, dis);
    DSA_VIS_END();
    return 0;
}
