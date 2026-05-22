#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>
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
                current_ctx_ = "default";
                initialized_arrays_.clear();
                initialized_btrees_.clear();
                btree_node_ids_.clear();
                btree_node_seq_ = 0;
            }

            void End()
            {
                if (out_.is_open())
                    out_.close();
                active_ = false;
                seq_ = 0;
                initialized_arrays_.clear();
                initialized_btrees_.clear();
                btree_node_ids_.clear();
                btree_node_seq_ = 0;
            }

            bool IsActive() const noexcept { return active_; }

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
                if (!out_.is_open())
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
            std::string current_ctx_ = "default";
            std::string primary_name_ = "A";
            std::string buffer_name_ = "B";
            std::unordered_set<std::string> initialized_arrays_;
            std::unordered_set<std::string> initialized_btrees_;
            std::unordered_map<const void *, std::string> btree_node_ids_;
            std::size_t btree_node_seq_ = 0;
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
#define DSA_VIS_BEGIN(path) ::DSA::Vis::Logger::Global().Begin(path)
#define DSA_VIS_END() ::DSA::Vis::Logger::Global().End()
#define DSA_VIS_SET_ARRAY_OBJECTS(primary_name, buffer_name) ::DSA::Vis::Logger::Global().SetArrayNames((primary_name), (buffer_name))
#define DSA_VIS_CTX(ctx_name) ::DSA::Vis::Logger::Global().SetContext((ctx_name))
#define DSA_VIS_LOC ::DSA::Vis::SourceLoc{__FILE__, __LINE__, __func__}
#define DSA_VIS_ARR_INIT(obj, first, last) ::DSA::Vis::Logger::Global().EnsureArrayInit((obj), (first), (last), DSA_VIS_LOC)
#define DSA_VIS_ARR_FOCUS(obj, l, r, step) ::DSA::Vis::Logger::Global().Focus((obj), (l), (r), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_SWAP(obj, i, j, step) ::DSA::Vis::Logger::Global().Swap((obj), (i), (j), DSA_VIS_LOC, (step))
#define DSA_VIS_ARR_SET(obj, i, v, step) ::DSA::Vis::Logger::Global().Set((obj), (i), (v), DSA_VIS_LOC, (step))
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
#define DSA_VIS_BT_SET_NOTE(obj, node, note, step) ::DSA::Vis::Logger::Global().BTreeSetNote((obj), (node), (note), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_SET_COLOR(obj, node, color, step) ::DSA::Vis::Logger::Global().BTreeSetColor((obj), (node), (color), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_MARK(obj, node, step) ::DSA::Vis::Logger::Global().BTreeMark((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_UNMARK(obj, node, step) ::DSA::Vis::Logger::Global().BTreeUnmark((obj), (node), DSA_VIS_LOC, (step))
#define DSA_VIS_BT_SYNC(obj, root, step) ::DSA::Vis::Logger::Global().BTreeSync((obj), (root), DSA_VIS_LOC, (step))
#else
#define DSA_VIS_BEGIN(path) ((void)0)
#define DSA_VIS_END() ((void)0)
#define DSA_VIS_SET_ARRAY_OBJECTS(primary_name, buffer_name) ((void)0)
#define DSA_VIS_CTX(ctx_name) ((void)0)
#define DSA_VIS_LOC ((void)0)
#define DSA_VIS_ARR_INIT(obj, first, last) ((void)0)
#define DSA_VIS_ARR_FOCUS(obj, l, r, step) ((void)0)
#define DSA_VIS_ARR_SWAP(obj, i, j, step) ((void)0)
#define DSA_VIS_ARR_SET(obj, i, v, step) ((void)0)
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
#define DSA_VIS_BT_SET_NOTE(obj, node, note, step) ((void)0)
#define DSA_VIS_BT_SET_COLOR(obj, node, color, step) ((void)0)
#define DSA_VIS_BT_MARK(obj, node, step) ((void)0)
#define DSA_VIS_BT_UNMARK(obj, node, step) ((void)0)
#define DSA_VIS_BT_SYNC(obj, root, step) ((void)0)
#endif
