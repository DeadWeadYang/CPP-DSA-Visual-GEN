#pragma once

#include <sstream>
#include <string>
#include "../utils.hpp"
#include "../vis_trace.hpp"

namespace DSA
{
    namespace Graph
    {
        namespace Vis
        {
            template <typename T>
            std::string ToString(const T &x)
            {
                std::ostringstream oss;
                oss << x;
                return oss.str();
            }

            template <typename T>
            std::string DistanceText(const T &x)
            {
                return x >= DSA::Utils::Infinity<T>() ? std::string("INF") : ToString(x);
            }

            template <typename GraphT>
            void InitWeightedGraph(const GraphT &g, const std::string &obj = "G")
            {
                DSA_VIS_G_INIT(obj, g.is_directed);
                for (int i = 1; i <= g.number_of_node; ++i)
                {
                    DSA_VIS_G_NEW_NODE(obj, DSA_VIS_NODE(i), i, false);
                    DSA_VIS_G_SET_NODE_COLOR(obj, DSA_VIS_NODE(i), "black", false);
                }
                for (auto e : g.E)
                {
                    DSA_VIS_G_NEW_EDGE(obj, DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), e.w, false);
                    DSA_VIS_G_SET_EDGE_STYLE(obj, DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "#64748b", 2, "", false);
                }
            }

            template <typename GraphT>
            void InitBasicGraph(const GraphT &g, const std::string &obj = "G")
            {
                DSA_VIS_G_INIT(obj, g.is_directed);
                for (int i = 1; i <= g.number_of_node; ++i)
                {
                    DSA_VIS_G_NEW_NODE(obj, DSA_VIS_NODE(i), i, false);
                    DSA_VIS_G_SET_NODE_COLOR(obj, DSA_VIS_NODE(i), "black", false);
                }
                for (auto e : g.E)
                {
                    DSA_VIS_G_NEW_EDGE(obj, DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "", false);
                    DSA_VIS_G_SET_EDGE_STYLE(obj, DSA_VIS_NODE(e.u), DSA_VIS_NODE(e.v), "#64748b", 2, "", false);
                }
            }

            template <typename T>
            void InitDistanceState(const std::string &obj, const std::vector<T> &dis, int n)
            {
                DSA_VIS_STATE_INIT_VECTOR_LABELED(obj, n, obj + "[v] distance by graph node", 1, "column = graph node v", false);
                for (int i = 1; i <= n; ++i)
                {
                    std::string value = DistanceText(dis[i]);
                    DSA_VIS_STATE_SET(obj, 0, i - 1, value, false);
                    DSA_VIS_STATE_HISTORY(obj, 0, i - 1, value, "init", false);
                }
            }

            inline void InitFlagState(const std::string &obj, int n, const std::string &initial = "N")
            {
                DSA_VIS_STATE_INIT_VECTOR_LABELED(obj, n, obj + "[v] status flag", 1, "column = graph node v", false);
                for (int i = 1; i <= n; ++i)
                    DSA_VIS_STATE_SET(obj, 0, i - 1, initial, false);
            }

            template <typename T>
            void InitValueState(const std::string &obj, const std::vector<T> &values, int n, const std::string &note = "init")
            {
                DSA_VIS_STATE_INIT_VECTOR_LABELED(obj, n, obj + "[v] value by graph node", 1, "column = graph node v", false);
                for (int i = 1; i <= n; ++i)
                {
                    std::string value = ToString(values[i]);
                    DSA_VIS_STATE_SET(obj, 0, i - 1, value, false);
                    DSA_VIS_STATE_HISTORY(obj, 0, i - 1, value, note, false);
                }
            }

            template <typename T>
            void SetDistance(const std::string &obj, int node, const T &value, const std::string &note)
            {
                std::string text = DistanceText(value);
                DSA_VIS_STATE_SET(obj, 0, node - 1, text, false);
                DSA_VIS_STATE_HISTORY(obj, 0, node - 1, text, note, false);
            }

            inline void SetFlag(const std::string &obj, int node, const std::string &value)
            {
                DSA_VIS_STATE_SET(obj, 0, node - 1, value, false);
            }

            template <typename T>
            void SetValue(const std::string &obj, int node, const T &value, const std::string &note = "")
            {
                std::string text = ToString(value);
                DSA_VIS_STATE_SET(obj, 0, node - 1, text, false);
                if (!note.empty())
                    DSA_VIS_STATE_HISTORY(obj, 0, node - 1, text, note, false);
            }

            inline void FocusCell(const std::string &obj, int node, const std::string &role)
            {
                DSA_VIS_STATE_FOCUS(obj, 0, node - 1, role, false);
            }

            inline void ClearFocus(const std::string &obj)
            {
                DSA_VIS_STATE_CLEAR_FOCUS(obj, false);
            }

            inline void FocusRelaxation(const std::string &obj, int from, int to)
            {
                ClearFocus(obj);
                FocusCell(obj, from, "read");
                FocusCell(obj, to, "write");
            }

            inline void MarkNode(const std::string &obj, int node, const std::string &color)
            {
                DSA_VIS_G_MARK_NODE(obj, DSA_VIS_NODE(node), false);
                DSA_VIS_G_SET_NODE_COLOR(obj, DSA_VIS_NODE(node), color, false);
            }

            inline void SetNodeColor(const std::string &obj, int node, const std::string &color)
            {
                DSA_VIS_G_SET_NODE_COLOR(obj, DSA_VIS_NODE(node), color, false);
            }

            inline void UnmarkNode(const std::string &obj, int node)
            {
                DSA_VIS_G_UNMARK_NODE(obj, DSA_VIS_NODE(node), false);
            }

            inline void SetNodeLabel(const std::string &obj, int node, const std::string &label)
            {
                DSA_VIS_G_SET_NODE_VALUE(obj, DSA_VIS_NODE(node), label, false);
            }

            inline void VisitNode(const std::string &obj, int node, int order, const std::string &color)
            {
                MarkNode(obj, node, color);
                SetNodeLabel(obj, node, ToString(node) + "\nord=" + ToString(order));
            }

            template <typename W>
            void MarkEdge(const std::string &obj, int u, int v, const W &label, const std::string &color, int width = 3, const std::string &dash = "")
            {
                DSA_VIS_G_MARK_EDGE(obj, DSA_VIS_NODE(u), DSA_VIS_NODE(v), false);
                DSA_VIS_G_SET_EDGE_LABEL(obj, DSA_VIS_NODE(u), DSA_VIS_NODE(v), label, false);
                DSA_VIS_G_SET_EDGE_STYLE(obj, DSA_VIS_NODE(u), DSA_VIS_NODE(v), color, width, dash, false);
            }

            inline void StyleEdge(const std::string &obj, int u, int v, const std::string &color, int width = 2, const std::string &dash = "")
            {
                DSA_VIS_G_SET_EDGE_STYLE(obj, DSA_VIS_NODE(u), DSA_VIS_NODE(v), color, width, dash, false);
            }

            inline void UnmarkEdge(const std::string &obj, int u, int v)
            {
                DSA_VIS_G_UNMARK_EDGE(obj, DSA_VIS_NODE(u), DSA_VIS_NODE(v), false);
            }

            inline void SeqClear(const std::string &obj)
            {
                DSA_VIS_STATE_SEQ_CLEAR(obj, false);
            }

            template <typename T>
            void SeqPush(const std::string &obj, const T &value)
            {
                DSA_VIS_STATE_SEQ_PUSH(obj, value, false);
            }

            inline void SeqPop(const std::string &obj)
            {
                DSA_VIS_STATE_SEQ_POP(obj, false);
            }
        }
    }
}
