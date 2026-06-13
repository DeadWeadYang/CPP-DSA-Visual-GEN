#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace DSA
{
    namespace Vis
    {
        struct SourceLoc
        {
            const char *file;
            int line;
            const char *func;
        };

        class Logger
        {
        public:
            static Logger &Global()
            {
                static Logger instance;
                return instance;
            }

            void Begin(const std::string &path = "trace.sorting.jsonl")
            {
                End();
                out_.open(path, std::ios::out | std::ios::trunc);
                active_ = out_.is_open();
                seq_ = 0;
                pause_depth_ = 0;
                current_ctx_ = "default";
                initialized_arrays_.clear();
                initialized_btrees_.clear();
                initialized_trees_.clear();
                initialized_graphs_.clear();
                btree_node_ids_.clear();
                tree_node_ids_.clear();
                graph_node_ids_.clear();
                btree_node_seq_ = 0;
                tree_node_seq_ = 0;
                graph_node_seq_ = 0;
            }

            void End()
            {
                if (out_.is_open())
                    out_.close();
                active_ = false;
                seq_ = 0;
                pause_depth_ = 0;
                initialized_arrays_.clear();
                initialized_btrees_.clear();
                initialized_trees_.clear();
                initialized_graphs_.clear();
                btree_node_ids_.clear();
                tree_node_ids_.clear();
                graph_node_ids_.clear();
                btree_node_seq_ = 0;
                tree_node_seq_ = 0;
                graph_node_seq_ = 0;
            }

            bool IsActive() const noexcept { return active_; }
            bool IsPaused() const noexcept { return pause_depth_ > 0; }
            void Pause() noexcept
            {
                if (!active_)
                    return;
                ++pause_depth_;
            }
            void Resume() noexcept
            {
                if (!active_)
                    return;
                if (pause_depth_ > 0)
                    --pause_depth_;
            }

            void SetArrayNames(std::string primary_name, std::string buffer_name = "B")
            {
                primary_name_ = std::move(primary_name);
                buffer_name_ = std::move(buffer_name);
            }

            const std::string &PrimaryArrayName() const { return primary_name_; }
            const std::string &BufferArrayName() const { return buffer_name_; }

            template <typename RandIt>
            void EnsureArrayInit(const std::string &obj, RandIt first, RandIt last, SourceLoc loc)
            {
                if (!active_)
                    return;
                const std::string scoped = ScopedObjKey(obj);
                if (initialized_arrays_.count(scoped))
                    return;
                initialized_arrays_.insert(scoped);
                WriteEvent("array", "init", obj, "{\"values\":" + JsonArray(first, last) + "}", false, loc);
            }

            void Focus(const std::string &obj, int l, int r, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"l\":" << l << ",\"r\":" << r << "}";
                WriteEvent("array", "focus", obj, args.str(), step, loc);
            }

            void Swap(const std::string &obj, int i, int j, SourceLoc loc, bool step = true)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"i\":" << i << ",\"j\":" << j << "}";
                WriteEvent("array", "swap", obj, args.str(), step, loc);
            }

            template <typename T>
            void Set(const std::string &obj, int i, const T &value, SourceLoc loc, bool step = true)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"i\":" << i << ",\"v\":" << JsonValue(value) << "}";
                WriteEvent("array", "set", obj, args.str(), step, loc);
            }

            template <typename RandIt>
            void SyncArray(const std::string &obj, RandIt first, RandIt last, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"values\":" << JsonArray(first, last) << "}";
                WriteEvent("array", "sync", obj, args.str(), step, loc);
            }

            template <typename RandIt>
            void ArrayRebuild(const std::string &obj, RandIt first, RandIt last, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"values\":" << JsonArray(first, last) << "}";
                WriteEvent("array", "rebuild", obj, args.str(), step, loc);
            }

            void Mark(const std::string &obj, int i, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"indices\":[" << i << "]}";
                WriteEvent("array", "mark", obj, args.str(), step, loc);
            }

            void Unmark(const std::string &obj, int i, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"indices\":[" << i << "]}";
                WriteEvent("array", "unmark", obj, args.str(), step, loc);
            }

            void EnsureBTreeInit(const std::string &obj, SourceLoc loc)
            {
                if (!active_)
                    return;
                const std::string scoped = ScopedObjKey(obj);
                if (initialized_btrees_.count(scoped))
                    return;
                initialized_btrees_.insert(scoped);
                WriteEvent("btree", "init", obj, "{\"nodes\":[]}", false, loc);
            }

            void EnsureTreeInit(const std::string &obj, SourceLoc loc)
            {
                if (!active_)
                    return;
                const std::string scoped = ScopedObjKey(obj);
                if (initialized_trees_.count(scoped))
                    return;
                initialized_trees_.insert(scoped);
                WriteEvent("tree", "init", obj, "{\"nodes\":[]}", false, loc);
            }

            template <typename NodePtr, typename V>
            void BTreeNewNode(const std::string &obj, NodePtr node, const V &value, SourceLoc loc, bool step = true)
            {
                if (!active_ || !node)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":\"" << Escape(GetOrCreateBTreeNodeId(static_cast<const void *>(node))) << "\""
                     << ",\"value\":" << JsonValue(value)
                     << ",\"color\":\"black\"}";
                WriteEvent("btree", "new_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeSetRoot(const std::string &obj, NodePtr node, SourceLoc loc, bool step = true)
            {
                if (!active_)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << BTreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("btree", "set_root", obj, args.str(), step, loc);
            }

            template <typename ParentPtr, typename ChildPtr>
            void BTreeLink(const std::string &obj, ParentPtr parent, bool link_right, ChildPtr child, SourceLoc loc, bool step = true)
            {
                if (!active_)
                    return;
                EnsureBTreeInit(obj, loc);
                if (!parent)
                    return;
                std::ostringstream args;
                args << "{\"parent\":" << BTreeNodeRef(static_cast<const void *>(parent))
                     << ",\"side\":\"" << (link_right ? "right" : "left") << "\""
                     << ",\"child\":" << BTreeNodeRef(static_cast<const void *>(child)) << "}";
                WriteEvent("btree", "link", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeRemoveNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = true)
            {
                if (!active_ || !node)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << BTreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("btree", "remove_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeRotateLeft(const std::string &obj, NodePtr pivot, SourceLoc loc, bool step = true)
            {
                if (!active_ || !pivot)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"pivot\":" << BTreeNodeRef(static_cast<const void *>(pivot)) << "}";
                WriteEvent("btree", "rotate_left", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeRotateRight(const std::string &obj, NodePtr pivot, SourceLoc loc, bool step = true)
            {
                if (!active_ || !pivot)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"pivot\":" << BTreeNodeRef(static_cast<const void *>(pivot)) << "}";
                WriteEvent("btree", "rotate_right", obj, args.str(), step, loc);
            }

            template <typename NodePtrA, typename NodePtrB>
            void BTreeSwapTopology(const std::string &obj, NodePtrA a, NodePtrB b, SourceLoc loc, bool step = false)
            {
                if (!active_ || !a || !b)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"a\":" << BTreeNodeRef(static_cast<const void *>(a))
                     << ",\"b\":" << BTreeNodeRef(static_cast<const void *>(b)) << "}";
                WriteEvent("btree", "swap_topology", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeSetNote(const std::string &obj, NodePtr node, const std::string &note, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << BTreeNodeRef(static_cast<const void *>(node))
                     << ",\"note\":\"" << Escape(note) << "\"}";
                WriteEvent("btree", "set_note", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeSetColor(const std::string &obj, NodePtr node, const std::string &color, SourceLoc loc, bool step = true)
            {
                if (!active_ || !node)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << BTreeNodeRef(static_cast<const void *>(node))
                     << ",\"color\":\"" << Escape(color) << "\"}";
                WriteEvent("btree", "set_color", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeMark(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"ids\":[" << BTreeNodeRef(static_cast<const void *>(node)) << "]}";
                WriteEvent("btree", "mark", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeUnmark(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"ids\":[" << BTreeNodeRef(static_cast<const void *>(node)) << "]}";
                WriteEvent("btree", "unmark", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void BTreeSync(const std::string &obj, NodePtr root, SourceLoc loc, bool step = true)
            {
                if (!active_)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"root\":" << BTreeNodeRef(static_cast<const void *>(root)) << ",\"nodes\":[";
                bool first = true;
                auto emit_node = [&](auto &&self, auto *node) -> void
                {
                    if (!node)
                        return;
                    if (!first)
                        args << ",";
                    first = false;
                    args << "{\"id\":" << BTreeNodeRef(static_cast<const void *>(node))
                         << ",\"value\":" << JsonValue(node->value())
                         << ",\"left\":" << BTreeNodeRef(static_cast<const void *>(node->left()))
                         << ",\"right\":" << BTreeNodeRef(static_cast<const void *>(node->right()))
                         << "}";
                    self(self, node->left());
                    self(self, node->right());
                };
                emit_node(emit_node, root);
                args << "]}";
                WriteEvent("btree", "sync", obj, args.str(), step, loc);
            }

            template <typename NodePtr, typename V>
            void TreeNewNode(const std::string &obj, NodePtr node, const V &value, SourceLoc loc, bool step = true)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":\"" << Escape(GetOrCreateTreeNodeId(static_cast<const void *>(node))) << "\""
                     << ",\"value\":" << JsonValue(value)
                     << ",\"color\":\"black\"}";
                WriteEvent("tree", "new_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeSetRoot(const std::string &obj, NodePtr node, SourceLoc loc, bool step = true)
            {
                if (!active_)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("tree", "set_root", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeAddRoot(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("tree", "add_root", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeRemoveRoot(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("tree", "remove_root", obj, args.str(), step, loc);
            }

            template <typename ParentPtr, typename ChildPtr>
            void TreeAddChild(const std::string &obj, ParentPtr parent, ChildPtr child, SourceLoc loc, bool step = true)
            {
                if (!active_ || !parent)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"parent\":" << TreeNodeRef(static_cast<const void *>(parent))
                     << ",\"child\":" << TreeNodeRef(static_cast<const void *>(child)) << "}";
                WriteEvent("tree", "add_child", obj, args.str(), step, loc);
            }

            template <typename ParentPtr, typename ChildPtr>
            void TreeSetChild(const std::string &obj, ParentPtr parent, int pos, ChildPtr child, SourceLoc loc, bool step = true)
            {
                if (!active_ || !parent || pos < 0)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"parent\":" << TreeNodeRef(static_cast<const void *>(parent))
                     << ",\"pos\":" << pos
                     << ",\"child\":" << TreeNodeRef(static_cast<const void *>(child)) << "}";
                WriteEvent("tree", "set_child", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeRemoveNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = true)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("tree", "remove_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeDestroyNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("tree", "destroy_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeSetNote(const std::string &obj, NodePtr node, const std::string &note, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node))
                     << ",\"note\":\"" << Escape(note) << "\"}";
                WriteEvent("tree", "set_note", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeSetColor(const std::string &obj, NodePtr node, const std::string &color, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node))
                     << ",\"color\":\"" << Escape(color) << "\"}";
                WriteEvent("tree", "set_color", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeMark(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"ids\":[" << TreeNodeRef(static_cast<const void *>(node)) << "]}";
                WriteEvent("tree", "mark", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeUnmark(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"ids\":[" << TreeNodeRef(static_cast<const void *>(node)) << "]}";
                WriteEvent("tree", "unmark", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeHideNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("tree", "hide_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void TreeShowNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << TreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("tree", "show_node", obj, args.str(), step, loc);
            }

            void Message(const std::string &text, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"text\":\"" << Escape(text) << "\"}";
                WriteEvent("meta", "msg", "_", args.str(), step, loc);
            }

            void StateInit(const std::string &obj, const std::string &kind, int rows, int cols, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"kind\":\"" << Escape(kind) << "\",\"rows\":" << rows << ",\"cols\":" << cols << "}";
                WriteEvent("state", "init", obj, args.str(), step, loc);
            }

            void StateInitLabeled(
                const std::string &obj,
                const std::string &kind,
                int rows,
                int cols,
                const std::string &title,
                int row_base,
                int col_base,
                bool show_row_labels,
                bool show_col_labels,
                const std::string &axis,
                SourceLoc loc,
                bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"kind\":\"" << Escape(kind) << "\",\"rows\":" << rows << ",\"cols\":" << cols
                     << ",\"title\":\"" << Escape(title) << "\""
                     << ",\"rowBase\":" << row_base
                     << ",\"colBase\":" << col_base
                     << ",\"showRowLabels\":" << (show_row_labels ? "true" : "false")
                     << ",\"showColLabels\":" << (show_col_labels ? "true" : "false")
                     << ",\"axis\":\"" << Escape(axis) << "\"}";
                WriteEvent("state", "init", obj, args.str(), step, loc);
            }

            template <typename T>
            void StateSet(const std::string &obj, int row, int col, const T &value, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"row\":" << row << ",\"col\":" << col << ",\"value\":" << JsonValue(value) << "}";
                WriteEvent("state", "set", obj, args.str(), step, loc);
            }

            void StateFocus(const std::string &obj, int row, int col, const std::string &role, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"row\":" << row << ",\"col\":" << col << ",\"role\":\"" << Escape(role) << "\"}";
                WriteEvent("state", "focus", obj, args.str(), step, loc);
            }

            void StateClearFocus(const std::string &obj, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                WriteEvent("state", "clear_focus", obj, "{}", step, loc);
            }

            template <typename T>
            void StateSeqPush(const std::string &obj, const T &value, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"value\":" << JsonValue(value) << "}";
                WriteEvent("state", "seq_push", obj, args.str(), step, loc);
            }

            void StateSeqPop(const std::string &obj, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                WriteEvent("state", "seq_pop", obj, "{}", step, loc);
            }

            void StateSeqClear(const std::string &obj, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                WriteEvent("state", "seq_clear", obj, "{}", step, loc);
            }

            template <typename T>
            void StateHistoryAppend(const std::string &obj, int row, int col, const T &value, const std::string &note, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"row\":" << row
                     << ",\"col\":" << col
                     << ",\"value\":" << JsonValue(value)
                     << ",\"note\":\"" << Escape(note) << "\"}";
                WriteEvent("state", "history_append", obj, args.str(), step, loc);
            }

            void StringInit(const std::string &obj, const std::string &text, const std::string &pattern, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"text\":\"" << Escape(text) << "\",\"pattern\":\"" << Escape(pattern) << "\"}";
                WriteEvent("string", "init", obj, args.str(), step, loc);
            }

            void StringInitPattern(const std::string &obj, const std::string &pattern, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"mode\":\"self\",\"pattern\":\"" << Escape(pattern) << "\"}";
                WriteEvent("string", "init", obj, args.str(), step, loc);
            }

            void StringAlign(const std::string &obj, int shift, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"shift\":" << shift << "}";
                WriteEvent("string", "align", obj, args.str(), step, loc);
            }

            void StringCompare(const std::string &obj, int text_index, int pattern_index, bool match, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"i\":" << text_index << ",\"j\":" << pattern_index << ",\"match\":" << (match ? "true" : "false") << "}";
                WriteEvent("string", "compare", obj, args.str(), step, loc);
            }

            void StringFallback(const std::string &obj, int from_j, int to_j, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"from\":" << from_j << ",\"to\":" << to_j << "}";
                WriteEvent("string", "fallback", obj, args.str(), step, loc);
            }

            void StringAccept(const std::string &obj, int start, int length, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                std::ostringstream args;
                args << "{\"start\":" << start << ",\"length\":" << length << "}";
                WriteEvent("string", "accept", obj, args.str(), step, loc);
            }

            void StringClear(const std::string &obj, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                WriteEvent("string", "clear", obj, "{}", step, loc);
            }

            void EnsureGraphInit(const std::string &obj, bool directed, SourceLoc loc)
            {
                if (!active_)
                    return;
                const std::string scoped = ScopedObjKey(obj);
                if (initialized_graphs_.count(scoped))
                    return;
                initialized_graphs_.insert(scoped);
                std::ostringstream args;
                args << "{\"directed\":" << (directed ? "true" : "false") << "}";
                WriteEvent("graph", "init", obj, args.str(), false, loc);
            }

            template <typename NodePtr, typename V>
            void GraphNewNode(const std::string &obj, NodePtr node, const V &value, SourceLoc loc, bool step = true)
            {
                if (!active_ || !node)
                    return;
                std::ostringstream args;
                args << "{\"id\":\"" << Escape(GetOrCreateGraphNodeId(static_cast<const void *>(node))) << "\""
                     << ",\"value\":" << JsonValue(value) << "}";
                WriteEvent("graph", "new_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void GraphRemoveNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = true)
            {
                if (!active_ || !node)
                    return;
                std::ostringstream args;
                args << "{\"id\":" << GraphNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("graph", "remove_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr, typename V>
            void GraphSetNodeValue(const std::string &obj, NodePtr node, const V &value, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                std::ostringstream args;
                args << "{\"id\":" << GraphNodeRef(static_cast<const void *>(node))
                     << ",\"value\":" << JsonValue(value) << "}";
                WriteEvent("graph", "set_node_value", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void GraphSetNodeColor(const std::string &obj, NodePtr node, const std::string &color, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                std::ostringstream args;
                args << "{\"id\":" << GraphNodeRef(static_cast<const void *>(node))
                     << ",\"color\":\"" << Escape(color) << "\"}";
                WriteEvent("graph", "set_node_color", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void GraphMarkNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                std::ostringstream args;
                args << "{\"id\":" << GraphNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("graph", "mark_node", obj, args.str(), step, loc);
            }

            template <typename NodePtr>
            void GraphUnmarkNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                std::ostringstream args;
                args << "{\"id\":" << GraphNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("graph", "unmark_node", obj, args.str(), step, loc);
            }

            template <typename NodePtrU, typename NodePtrV, typename W>
            void GraphNewEdge(const std::string &obj, NodePtrU from, NodePtrV to, const W &label, SourceLoc loc, bool step = true)
            {
                if (!active_ || !from || !to)
                    return;
                std::ostringstream args;
                args << "{\"from\":" << GraphNodeRef(static_cast<const void *>(from))
                     << ",\"to\":" << GraphNodeRef(static_cast<const void *>(to))
                     << ",\"label\":" << JsonValue(label) << "}";
                WriteEvent("graph", "new_edge", obj, args.str(), step, loc);
            }

            template <typename NodePtrU, typename NodePtrV>
            void GraphRemoveEdge(const std::string &obj, NodePtrU from, NodePtrV to, SourceLoc loc, bool step = true)
            {
                if (!active_ || !from || !to)
                    return;
                std::ostringstream args;
                args << "{\"from\":" << GraphNodeRef(static_cast<const void *>(from))
                     << ",\"to\":" << GraphNodeRef(static_cast<const void *>(to)) << "}";
                WriteEvent("graph", "remove_edge", obj, args.str(), step, loc);
            }

            template <typename NodePtrU, typename NodePtrV, typename W>
            void GraphSetEdgeLabel(const std::string &obj, NodePtrU from, NodePtrV to, const W &label, SourceLoc loc, bool step = false)
            {
                if (!active_ || !from || !to)
                    return;
                std::ostringstream args;
                args << "{\"from\":" << GraphNodeRef(static_cast<const void *>(from))
                     << ",\"to\":" << GraphNodeRef(static_cast<const void *>(to))
                     << ",\"label\":" << JsonValue(label) << "}";
                WriteEvent("graph", "set_edge_label", obj, args.str(), step, loc);
            }

            template <typename NodePtrU, typename NodePtrV, typename W>
            void GraphSetEdgeWeight(const std::string &obj, NodePtrU from, NodePtrV to, const W &weight, SourceLoc loc, bool step = false)
            {
                GraphSetEdgeLabel(obj, from, to, weight, loc, step);
            }

            template <typename NodePtrU, typename NodePtrV>
            void GraphSetEdgeStyle(
                const std::string &obj,
                NodePtrU from,
                NodePtrV to,
                const std::string &color,
                int width,
                const std::string &dash,
                SourceLoc loc,
                bool step = false)
            {
                if (!active_ || !from || !to)
                    return;
                std::ostringstream args;
                args << "{\"from\":" << GraphNodeRef(static_cast<const void *>(from))
                     << ",\"to\":" << GraphNodeRef(static_cast<const void *>(to))
                     << ",\"color\":" << JsonValue(color)
                     << ",\"width\":" << width
                     << ",\"dash\":" << JsonValue(dash) << "}";
                WriteEvent("graph", "set_edge_style", obj, args.str(), step, loc);
            }

            template <typename NodePtrU, typename NodePtrV>
            void GraphMarkEdge(const std::string &obj, NodePtrU from, NodePtrV to, SourceLoc loc, bool step = false)
            {
                if (!active_ || !from || !to)
                    return;
                std::ostringstream args;
                args << "{\"from\":" << GraphNodeRef(static_cast<const void *>(from))
                     << ",\"to\":" << GraphNodeRef(static_cast<const void *>(to)) << "}";
                WriteEvent("graph", "mark_edge", obj, args.str(), step, loc);
            }

            template <typename NodePtrU, typename NodePtrV>
            void GraphUnmarkEdge(const std::string &obj, NodePtrU from, NodePtrV to, SourceLoc loc, bool step = false)
            {
                if (!active_ || !from || !to)
                    return;
                std::ostringstream args;
                args << "{\"from\":" << GraphNodeRef(static_cast<const void *>(from))
                     << ",\"to\":" << GraphNodeRef(static_cast<const void *>(to)) << "}";
                WriteEvent("graph", "unmark_edge", obj, args.str(), step, loc);
            }
            void GraphForceLayout(const std::string &obj, SourceLoc loc, bool step = false)
            {
                if (!active_)
                    return;
                WriteEvent("graph", "force_layout", obj, "{}", step, loc);
            }

        private:
            Logger() = default;

            std::string &GetOrCreateBTreeNodeId(const void *node)
            {
                auto it = btree_node_ids_.find(node);
                if (it != btree_node_ids_.end())
                    return it->second;
                std::ostringstream oss;
                oss << "n" << (++btree_node_seq_);
                auto inserted = btree_node_ids_.emplace(node, oss.str());
                return inserted.first->second;
            }

            std::string BTreeNodeRef(const void *node)
            {
                if (!node)
                    return "null";
                return std::string("\"") + Escape(GetOrCreateBTreeNodeId(node)) + "\"";
            }

            std::string &GetOrCreateTreeNodeId(const void *node)
            {
                auto it = tree_node_ids_.find(node);
                if (it != tree_node_ids_.end())
                    return it->second;
                std::ostringstream oss;
                oss << "t" << (++tree_node_seq_);
                auto inserted = tree_node_ids_.emplace(node, oss.str());
                return inserted.first->second;
            }

            std::string TreeNodeRef(const void *node)
            {
                if (!node)
                    return "null";
                return std::string("\"") + Escape(GetOrCreateTreeNodeId(node)) + "\"";
            }

            std::string &GetOrCreateGraphNodeId(const void *node)
            {
                auto it = graph_node_ids_.find(node);
                if (it != graph_node_ids_.end())
                    return it->second;
                std::ostringstream oss;
                oss << "g" << (++graph_node_seq_);
                auto inserted = graph_node_ids_.emplace(node, oss.str());
                return inserted.first->second;
            }

            std::string GraphNodeRef(const void *node)
            {
                if (!node)
                    return "null";
                return std::string("\"") + Escape(GetOrCreateGraphNodeId(node)) + "\"";
            }
            std::string ScopedObjKey(const std::string &obj) const
            {
                return (current_ctx_.empty() ? std::string("default") : current_ctx_) + "::" + obj;
            }

            static std::string Escape(const std::string &s)
            {
                std::string out;
                out.reserve(s.size() + 8);
                for (char c : s)
                {
                    switch (c)
                    {
                    case '\\':
                        out += "\\\\";
                        break;
                    case '"':
                        out += "\\\"";
                        break;
                    case '\n':
                        out += "\\n";
                        break;
                    case '\r':
                        out += "\\r";
                        break;
                    case '\t':
                        out += "\\t";
                        break;
                    default:
                        out += c;
                        break;
                    }
                }
                return out;
            }

            template <typename T>
            static std::string JsonValue(const T &value)
            {
                if constexpr (std::is_same_v<T, std::string>)
                {
                    return "\"" + Escape(value) + "\"";
                }
                else if constexpr (std::is_same_v<T, const char *> || std::is_same_v<T, char *>)
                {
                    return "\"" + Escape(value ? std::string(value) : std::string()) + "\"";
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    return value ? "true" : "false";
                }
                else if constexpr (std::is_arithmetic_v<T>)
                {
                    std::ostringstream oss;
                    oss << value;
                    return oss.str();
                }
                else
                {
                    std::ostringstream oss;
                    oss << value;
                    return "\"" + Escape(oss.str()) + "\"";
                }
            }

            template <typename RandIt>
            static std::string JsonArray(RandIt first, RandIt last)
            {
                std::ostringstream oss;
                oss << "[";
                bool first_item = true;
                for (auto it = first; it != last; ++it)
                {
                    if (!first_item)
                        oss << ",";
                    first_item = false;
                    oss << JsonValue(*it);
                }
                oss << "]";
                return oss.str();
            }

            void WriteEvent(
                const std::string &ds,
                const std::string &op,
                const std::string &obj,
                const std::string &args,
                bool step,
                SourceLoc loc)
            {
                if (!out_.is_open() || pause_depth_ > 0)
                    return;
                out_ << "{\"seq\":" << (++seq_)
                     << ",\"ctx\":\"" << Escape(current_ctx_.empty() ? std::string("default") : current_ctx_) << "\""
                     << ",\"ds\":\"" << Escape(ds)
                     << "\",\"op\":\"" << Escape(op)
                     << "\",\"obj\":\"" << Escape(obj)
                     << "\",\"args\":" << args
                     << ",\"step\":" << (step ? "true" : "false")
                     << ",\"loc\":{\"file\":\"" << Escape(loc.file ? loc.file : "")
                     << "\",\"line\":" << loc.line
                     << ",\"func\":\"" << Escape(loc.func ? loc.func : "")
                     << "\"}}\n";
            }

            bool active_ = false;
            std::ofstream out_;
            std::size_t seq_ = 0;
            int pause_depth_ = 0;
            std::string current_ctx_ = "default";
            std::string primary_name_ = "A";
            std::string buffer_name_ = "B";
            std::unordered_set<std::string> initialized_arrays_;
            std::unordered_set<std::string> initialized_btrees_;
            std::unordered_set<std::string> initialized_trees_;
            std::unordered_set<std::string> initialized_graphs_;
            std::unordered_map<const void *, std::string> btree_node_ids_;
            std::unordered_map<const void *, std::string> tree_node_ids_;
            std::unordered_map<const void *, std::string> graph_node_ids_;
            std::size_t btree_node_seq_ = 0;
            std::size_t tree_node_seq_ = 0;
            std::size_t graph_node_seq_ = 0;
        public:
            void SetContext(const std::string &ctx_name)
            {
                if (!active_)
                    return;
                current_ctx_ = ctx_name.empty() ? std::string("default") : ctx_name;
            }
            template <typename NodePtr>
            void BTreeDestroyNode(const std::string &obj, NodePtr node, SourceLoc loc, bool step = false)
            {
                if (!active_ || !node)
                    return;
                EnsureBTreeInit(obj, loc);
                std::ostringstream args;
                args << "{\"id\":" << BTreeNodeRef(static_cast<const void *>(node)) << "}";
                WriteEvent("btree", "destroy_node", obj, args.str(), step, loc);
            }
        };
    }
}

#if defined(DSA_VIS_ENABLE)
#define DSA_VIS_ONLY(stmt) \
    do                     \
    {                      \
        stmt;              \
    } while (false)
#define DSA_VIS_DECL(stmt) stmt
#define DSA_VIS_BEGIN(path) ::DSA::Vis::Logger::Global().Begin(path)
#define DSA_VIS_END() ::DSA::Vis::Logger::Global().End()
#define DSA_VIS_SET_ARRAY_OBJECTS(primary_name, buffer_name) ::DSA::Vis::Logger::Global().SetArrayNames((primary_name), (buffer_name))
#define DSA_VIS_CTX(ctx_name) ::DSA::Vis::Logger::Global().SetContext((ctx_name))
#define DSA_VIS_PAUSE() ::DSA::Vis::Logger::Global().Pause()
#define DSA_VIS_RESUME() ::DSA::Vis::Logger::Global().Resume()
#define DSA_VIS_LOC ::DSA::Vis::SourceLoc{__FILE__, __LINE__, __func__}
#define DSA_VIS_NODE(id) reinterpret_cast<const void *>(static_cast<std::uintptr_t>(id))
#define DSA_VIS_ARR_INIT(obj, first, last) ::DSA::Vis::Logger::Global().EnsureArrayInit((obj), (first), (last), DSA_VIS_LOC)
#define DSA_VIS_ARR_FOCUS(obj, l, r, step) ::DSA::Vis::Logger::Global().Focus((obj), (l), (r), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_SWAP(obj, i, j, step) ::DSA::Vis::Logger::Global().Swap((obj), (i), (j), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_SET(obj, i, v, step) ::DSA::Vis::Logger::Global().Set((obj), (i), (v), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_SYNC(obj, first, last, step) ::DSA::Vis::Logger::Global().SyncArray((obj), (first), (last), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_REBUILD(obj, first, last, step) ::DSA::Vis::Logger::Global().ArrayRebuild((obj), (first), (last), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_MARK(obj, i, step) ::DSA::Vis::Logger::Global().Mark((obj), (i), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_UNMARK(obj, i, step) ::DSA::Vis::Logger::Global().Unmark((obj), (i), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_INIT(obj) ::DSA::Vis::Logger::Global().EnsureBTreeInit((obj), DSA_VIS_LOC)
#define DSA_VIS_BT_NEW_NODE(obj, node, value, step) ::DSA::Vis::Logger::Global().BTreeNewNode((obj), (node), (value), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_SET_ROOT(obj, node, step) ::DSA::Vis::Logger::Global().BTreeSetRoot((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_LINK(obj, parent, link_right, child, step) ::DSA::Vis::Logger::Global().BTreeLink((obj), (parent), (link_right), (child), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_REMOVE_NODE(obj, node, step) ::DSA::Vis::Logger::Global().BTreeRemoveNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_DESTROY_NODE(obj, node, step) ::DSA::Vis::Logger::Global().BTreeDestroyNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_ROTATE_LEFT(obj, pivot, step) ::DSA::Vis::Logger::Global().BTreeRotateLeft((obj), (pivot), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_ROTATE_RIGHT(obj, pivot, step) ::DSA::Vis::Logger::Global().BTreeRotateRight((obj), (pivot), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_SWAP_TOPOLOGY(obj, a, b, step) ::DSA::Vis::Logger::Global().BTreeSwapTopology((obj), (a), (b), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_SET_NOTE(obj, node, note, step) ::DSA::Vis::Logger::Global().BTreeSetNote((obj), (node), (note), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_SET_COLOR(obj, node, color, step) ::DSA::Vis::Logger::Global().BTreeSetColor((obj), (node), (color), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_MARK(obj, node, step) ::DSA::Vis::Logger::Global().BTreeMark((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_UNMARK(obj, node, step) ::DSA::Vis::Logger::Global().BTreeUnmark((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_SYNC(obj, root, step) ::DSA::Vis::Logger::Global().BTreeSync((obj), (root), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_INIT(obj) ::DSA::Vis::Logger::Global().EnsureTreeInit((obj), DSA_VIS_LOC)
#define DSA_VIS_TREE_NEW_NODE(obj, node, value, step) ::DSA::Vis::Logger::Global().TreeNewNode((obj), (node), (value), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_SET_ROOT(obj, node, step) ::DSA::Vis::Logger::Global().TreeSetRoot((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_ADD_ROOT(obj, node, step) ::DSA::Vis::Logger::Global().TreeAddRoot((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_REMOVE_ROOT(obj, node, step) ::DSA::Vis::Logger::Global().TreeRemoveRoot((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_ADD_CHILD(obj, parent, child, step) ::DSA::Vis::Logger::Global().TreeAddChild((obj), (parent), (child), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_SET_CHILD(obj, parent, pos, child, step) ::DSA::Vis::Logger::Global().TreeSetChild((obj), (parent), (pos), (child), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_REMOVE_NODE(obj, node, step) ::DSA::Vis::Logger::Global().TreeRemoveNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_DESTROY_NODE(obj, node, step) ::DSA::Vis::Logger::Global().TreeDestroyNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_SET_NOTE(obj, node, note, step) ::DSA::Vis::Logger::Global().TreeSetNote((obj), (node), (note), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_SET_COLOR(obj, node, color, step) ::DSA::Vis::Logger::Global().TreeSetColor((obj), (node), (color), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_MARK(obj, node, step) ::DSA::Vis::Logger::Global().TreeMark((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_UNMARK(obj, node, step) ::DSA::Vis::Logger::Global().TreeUnmark((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_HIDE_NODE(obj, node, step) ::DSA::Vis::Logger::Global().TreeHideNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_TREE_SHOW_NODE(obj, node, step) ::DSA::Vis::Logger::Global().TreeShowNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_MSG(text, step) ::DSA::Vis::Logger::Global().Message((text), DSA_VIS_LOC, (step))
#define DSA_VIS_STEP(text) DSA_VIS_MSG((text), true)
#define DSA_VIS_NOTE(text) DSA_VIS_MSG((text), false)
#define DSA_VIS_STATE_INIT(obj, kind, rows, cols, step) ::DSA::Vis::Logger::Global().StateInit((obj), (kind), (rows), (cols), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_INIT_MATRIX(obj, rows, cols, step) DSA_VIS_STATE_INIT((obj), "matrix", (rows), (cols), (step))
#define DSA_VIS_STATE_INIT_TABLE(obj, rows, cols, step) DSA_VIS_STATE_INIT((obj), "table", (rows), (cols), (step))
#define DSA_VIS_STATE_INIT_VECTOR(obj, cols, step) DSA_VIS_STATE_INIT((obj), "vector", 1, (cols), (step))
#define DSA_VIS_STATE_INIT_LABELED(obj, kind, rows, cols, title, row_base, col_base, show_row_labels, show_col_labels, axis, step) ::DSA::Vis::Logger::Global().StateInitLabeled((obj), (kind), (rows), (cols), (title), (row_base), (col_base), (show_row_labels), (show_col_labels), (axis), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_INIT_VECTOR_LABELED(obj, cols, title, col_base, axis, step) DSA_VIS_STATE_INIT_LABELED((obj), "vector", 1, (cols), (title), 0, (col_base), false, true, (axis), (step))
#define DSA_VIS_STATE_INIT_MATRIX_LABELED(obj, rows, cols, title, row_base, col_base, axis, step) DSA_VIS_STATE_INIT_LABELED((obj), "matrix", (rows), (cols), (title), (row_base), (col_base), true, true, (axis), (step))
#define DSA_VIS_STATE_SET(obj, row, col, value, step) ::DSA::Vis::Logger::Global().StateSet((obj), (row), (col), (value), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_FOCUS(obj, row, col, role, step) ::DSA::Vis::Logger::Global().StateFocus((obj), (row), (col), (role), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_CLEAR_FOCUS(obj, step) ::DSA::Vis::Logger::Global().StateClearFocus((obj), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_HISTORY(obj, row, col, value, note, step) ::DSA::Vis::Logger::Global().StateHistoryAppend((obj), (row), (col), (value), (note), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_SEQ_PUSH(obj, value, step) ::DSA::Vis::Logger::Global().StateSeqPush((obj), (value), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_SEQ_POP(obj, step) ::DSA::Vis::Logger::Global().StateSeqPop((obj), DSA_VIS_LOC, (step))
#define DSA_VIS_STATE_SEQ_CLEAR(obj, step) ::DSA::Vis::Logger::Global().StateSeqClear((obj), DSA_VIS_LOC, (step))
#define DSA_VIS_STR_INIT(obj, text, pattern, step) ::DSA::Vis::Logger::Global().StringInit((obj), (text), (pattern), DSA_VIS_LOC, (step))
#define DSA_VIS_STR_INIT_PATTERN(obj, pattern, step) ::DSA::Vis::Logger::Global().StringInitPattern((obj), (pattern), DSA_VIS_LOC, (step))
#define DSA_VIS_STR_ALIGN(obj, shift, step) ::DSA::Vis::Logger::Global().StringAlign((obj), (shift), DSA_VIS_LOC, (step))
#define DSA_VIS_STR_COMPARE(obj, text_index, pattern_index, match, step) ::DSA::Vis::Logger::Global().StringCompare((obj), (text_index), (pattern_index), (match), DSA_VIS_LOC, (step))
#define DSA_VIS_STR_FALLBACK(obj, from_j, to_j, step) ::DSA::Vis::Logger::Global().StringFallback((obj), (from_j), (to_j), DSA_VIS_LOC, (step))
#define DSA_VIS_STR_ACCEPT(obj, start, length, step) ::DSA::Vis::Logger::Global().StringAccept((obj), (start), (length), DSA_VIS_LOC, (step))
#define DSA_VIS_STR_CLEAR(obj, step) ::DSA::Vis::Logger::Global().StringClear((obj), DSA_VIS_LOC, (step))
#define DSA_VIS_G_INIT(obj, directed) ::DSA::Vis::Logger::Global().EnsureGraphInit((obj), (directed), DSA_VIS_LOC)
#define DSA_VIS_G_NEW_NODE(obj, node, value, step) ::DSA::Vis::Logger::Global().GraphNewNode((obj), (node), (value), DSA_VIS_LOC, (step))
#define DSA_VIS_G_REMOVE_NODE(obj, node, step) ::DSA::Vis::Logger::Global().GraphRemoveNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_G_SET_NODE_VALUE(obj, node, value, step) ::DSA::Vis::Logger::Global().GraphSetNodeValue((obj), (node), (value), DSA_VIS_LOC, (step))
#define DSA_VIS_G_SET_NODE_COLOR(obj, node, color, step) ::DSA::Vis::Logger::Global().GraphSetNodeColor((obj), (node), (color), DSA_VIS_LOC, (step))
#define DSA_VIS_G_MARK_NODE(obj, node, step) ::DSA::Vis::Logger::Global().GraphMarkNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_G_UNMARK_NODE(obj, node, step) ::DSA::Vis::Logger::Global().GraphUnmarkNode((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_G_NEW_EDGE(obj, from, to, label, step) ::DSA::Vis::Logger::Global().GraphNewEdge((obj), (from), (to), (label), DSA_VIS_LOC, (step))
#define DSA_VIS_G_REMOVE_EDGE(obj, from, to, step) ::DSA::Vis::Logger::Global().GraphRemoveEdge((obj), (from), (to), DSA_VIS_LOC, (step))
#define DSA_VIS_G_SET_EDGE_LABEL(obj, from, to, label, step) ::DSA::Vis::Logger::Global().GraphSetEdgeLabel((obj), (from), (to), (label), DSA_VIS_LOC, (step))
#define DSA_VIS_G_SET_EDGE_WEIGHT(obj, from, to, weight, step) ::DSA::Vis::Logger::Global().GraphSetEdgeWeight((obj), (from), (to), (weight), DSA_VIS_LOC, (step))
#define DSA_VIS_G_SET_EDGE_STYLE(obj, from, to, color, width, dash, step) ::DSA::Vis::Logger::Global().GraphSetEdgeStyle((obj), (from), (to), (color), (width), (dash), DSA_VIS_LOC, (step))
#define DSA_VIS_G_MARK_EDGE(obj, from, to, step) ::DSA::Vis::Logger::Global().GraphMarkEdge((obj), (from), (to), DSA_VIS_LOC, (step))
#define DSA_VIS_G_UNMARK_EDGE(obj, from, to, step) ::DSA::Vis::Logger::Global().GraphUnmarkEdge((obj), (from), (to), DSA_VIS_LOC, (step))
#define DSA_VIS_G_LAYOUT(obj, step) ::DSA::Vis::Logger::Global().GraphForceLayout((obj), DSA_VIS_LOC, (step))
#else
#define DSA_VIS_ONLY(stmt) \
    do                     \
    {                      \
    } while (false)
#define DSA_VIS_DECL(stmt)
#define DSA_VIS_BEGIN(path) ((void)0)
#define DSA_VIS_END() ((void)0)
#define DSA_VIS_SET_ARRAY_OBJECTS(primary_name, buffer_name) ((void)0)
#define DSA_VIS_CTX(ctx_name) ((void)0)
#define DSA_VIS_PAUSE() ((void)0)
#define DSA_VIS_RESUME() ((void)0)
#define DSA_VIS_LOC ((void)0)
#define DSA_VIS_NODE(id) ((void*)0)
#define DSA_VIS_ARR_INIT(obj, first, last) ((void)0)
#define DSA_VIS_ARR_FOCUS(obj, l, r, step) ((void)0)
#define DSA_VIS_ARR_SWAP(obj, i, j, step) ((void)0)
#define DSA_VIS_ARR_SET(obj, i, v, step) ((void)0)
#define DSA_VIS_ARR_SYNC(obj, first, last, step) ((void)0)
#define DSA_VIS_ARR_REBUILD(obj, first, last, step) ((void)0)
#define DSA_VIS_ARR_MARK(obj, i, step) ((void)0)
#define DSA_VIS_ARR_UNMARK(obj, i, step) ((void)0)
#define DSA_VIS_BT_INIT(obj) ((void)0)
#define DSA_VIS_BT_NEW_NODE(obj, node, value, step) ((void)0)
#define DSA_VIS_BT_SET_ROOT(obj, node, step) ((void)0)
#define DSA_VIS_BT_LINK(obj, parent, link_right, child, step) ((void)0)
#define DSA_VIS_BT_REMOVE_NODE(obj, node, step) ((void)0)
#define DSA_VIS_BT_DESTROY_NODE(obj, node, step) ((void)0)
#define DSA_VIS_BT_ROTATE_LEFT(obj, pivot, step) ((void)0)
#define DSA_VIS_BT_ROTATE_RIGHT(obj, pivot, step) ((void)0)
#define DSA_VIS_BT_SWAP_TOPOLOGY(obj, a, b, step) ((void)0)
#define DSA_VIS_BT_SET_NOTE(obj, node, note, step) ((void)0)
#define DSA_VIS_BT_SET_COLOR(obj, node, color, step) ((void)0)
#define DSA_VIS_BT_MARK(obj, node, step) ((void)0)
#define DSA_VIS_BT_UNMARK(obj, node, step) ((void)0)
#define DSA_VIS_BT_SYNC(obj, root, step) ((void)0)
#define DSA_VIS_TREE_INIT(obj) ((void)0)
#define DSA_VIS_TREE_NEW_NODE(obj, node, value, step) ((void)0)
#define DSA_VIS_TREE_SET_ROOT(obj, node, step) ((void)0)
#define DSA_VIS_TREE_ADD_ROOT(obj, node, step) ((void)0)
#define DSA_VIS_TREE_REMOVE_ROOT(obj, node, step) ((void)0)
#define DSA_VIS_TREE_ADD_CHILD(obj, parent, child, step) ((void)0)
#define DSA_VIS_TREE_SET_CHILD(obj, parent, pos, child, step) ((void)0)
#define DSA_VIS_TREE_REMOVE_NODE(obj, node, step) ((void)0)
#define DSA_VIS_TREE_DESTROY_NODE(obj, node, step) ((void)0)
#define DSA_VIS_TREE_SET_NOTE(obj, node, note, step) ((void)0)
#define DSA_VIS_TREE_SET_COLOR(obj, node, color, step) ((void)0)
#define DSA_VIS_TREE_MARK(obj, node, step) ((void)0)
#define DSA_VIS_TREE_UNMARK(obj, node, step) ((void)0)
#define DSA_VIS_TREE_HIDE_NODE(obj, node, step) ((void)0)
#define DSA_VIS_TREE_SHOW_NODE(obj, node, step) ((void)0)
#define DSA_VIS_MSG(text, step) ((void)0)
#define DSA_VIS_STEP(text) ((void)0)
#define DSA_VIS_NOTE(text) ((void)0)
#define DSA_VIS_STATE_INIT(obj, kind, rows, cols, step) ((void)0)
#define DSA_VIS_STATE_INIT_MATRIX(obj, rows, cols, step) ((void)0)
#define DSA_VIS_STATE_INIT_TABLE(obj, rows, cols, step) ((void)0)
#define DSA_VIS_STATE_INIT_VECTOR(obj, cols, step) ((void)0)
#define DSA_VIS_STATE_INIT_LABELED(obj, kind, rows, cols, title, row_base, col_base, show_row_labels, show_col_labels, axis, step) ((void)0)
#define DSA_VIS_STATE_INIT_VECTOR_LABELED(obj, cols, title, col_base, axis, step) ((void)0)
#define DSA_VIS_STATE_INIT_MATRIX_LABELED(obj, rows, cols, title, row_base, col_base, axis, step) ((void)0)
#define DSA_VIS_STATE_SET(obj, row, col, value, step) ((void)0)
#define DSA_VIS_STATE_FOCUS(obj, row, col, role, step) ((void)0)
#define DSA_VIS_STATE_CLEAR_FOCUS(obj, step) ((void)0)
#define DSA_VIS_STATE_HISTORY(obj, row, col, value, note, step) ((void)0)
#define DSA_VIS_STATE_SEQ_PUSH(obj, value, step) ((void)0)
#define DSA_VIS_STATE_SEQ_POP(obj, step) ((void)0)
#define DSA_VIS_STATE_SEQ_CLEAR(obj, step) ((void)0)
#define DSA_VIS_STR_INIT(obj, text, pattern, step) ((void)0)
#define DSA_VIS_STR_INIT_PATTERN(obj, pattern, step) ((void)0)
#define DSA_VIS_STR_ALIGN(obj, shift, step) ((void)0)
#define DSA_VIS_STR_COMPARE(obj, text_index, pattern_index, match, step) ((void)0)
#define DSA_VIS_STR_FALLBACK(obj, from_j, to_j, step) ((void)0)
#define DSA_VIS_STR_ACCEPT(obj, start, length, step) ((void)0)
#define DSA_VIS_STR_CLEAR(obj, step) ((void)0)
#define DSA_VIS_G_INIT(obj, directed) ((void)0)
#define DSA_VIS_G_NEW_NODE(obj, node, value, step) ((void)0)
#define DSA_VIS_G_REMOVE_NODE(obj, node, step) ((void)0)
#define DSA_VIS_G_SET_NODE_VALUE(obj, node, value, step) ((void)0)
#define DSA_VIS_G_SET_NODE_COLOR(obj, node, color, step) ((void)0)
#define DSA_VIS_G_MARK_NODE(obj, node, step) ((void)0)
#define DSA_VIS_G_UNMARK_NODE(obj, node, step) ((void)0)
#define DSA_VIS_G_NEW_EDGE(obj, from, to, label, step) ((void)0)
#define DSA_VIS_G_REMOVE_EDGE(obj, from, to, step) ((void)0)
#define DSA_VIS_G_SET_EDGE_LABEL(obj, from, to, label, step) ((void)0)
#define DSA_VIS_G_SET_EDGE_WEIGHT(obj, from, to, weight, step) ((void)0)
#define DSA_VIS_G_SET_EDGE_STYLE(obj, from, to, color, width, dash, step) ((void)0)
#define DSA_VIS_G_MARK_EDGE(obj, from, to, step) ((void)0)
#define DSA_VIS_G_UNMARK_EDGE(obj, from, to, step) ((void)0)
#define DSA_VIS_G_LAYOUT(obj, step) ((void)0)
#endif
