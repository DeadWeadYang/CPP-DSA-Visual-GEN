#pragma once

#include "../vis_trace.hpp"

namespace DSA
{
    namespace Tree
    {
        namespace Vis
        {
            template <typename NodePtr>
            void BTreeFocusOne(const char *obj, NodePtr node)
            {
                if (node)
                    DSA_VIS_BT_MARK(obj, node, false);
            }

            template <typename... Nodes>
            void BTreeFocus(const char *obj, Nodes... nodes)
            {
                (BTreeFocusOne(obj, nodes), ...);
            }

            template <typename... Nodes>
            void BTreeFocus(Nodes... nodes)
            {
                BTreeFocus("T", nodes...);
            }

            template <typename NodePtr>
            void BTreeUnfocusOne(const char *obj, NodePtr node)
            {
                if (node)
                    DSA_VIS_BT_UNMARK(obj, node, false);
            }

            template <typename... Nodes>
            void BTreeUnfocus(const char *obj, Nodes... nodes)
            {
                (BTreeUnfocusOne(obj, nodes), ...);
            }

            template <typename... Nodes>
            void BTreeUnfocus(Nodes... nodes)
            {
                BTreeUnfocus("T", nodes...);
            }
        }
    }
}
