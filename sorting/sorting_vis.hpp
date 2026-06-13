#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include "../vis_trace.hpp"

#ifdef DSA_VIS_ENABLE
#define DSA_SORT_VIS(stmt) DSA_VIS_ONLY(stmt)
#define DSA_SORT_VIS_DECL(stmt) DSA_VIS_DECL(stmt)
#define DSA_SORT_STEP(...) DSA_VIS_STEP(__VA_ARGS__)
#define DSA_SORT_NOTE(...) DSA_VIS_NOTE(__VA_ARGS__)
#define DSA_SORT_FOCUS_RANGE(...) DSA_SORT_VIS(::DSA::Sorting::Vis::FocusRangeAt(DSA_VIS_LOC, __VA_ARGS__))
#define DSA_SORT_MARK(...) DSA_SORT_VIS(::DSA::Sorting::Vis::Mark(__VA_ARGS__))
#define DSA_SORT_UNMARK(...) DSA_SORT_VIS(::DSA::Sorting::Vis::Unmark(__VA_ARGS__))
#define DSA_SORT_SWAP_EVENT(...) DSA_SORT_VIS(::DSA::Sorting::Vis::SwapEventAt(DSA_VIS_LOC, __VA_ARGS__))
#define DSA_SORT_SET_AT(...) DSA_SORT_VIS(::DSA::Sorting::Vis::SetAtLoc(DSA_VIS_LOC, __VA_ARGS__))
#define DSA_SORT_SET_BUFFER_AT(...) DSA_SORT_VIS(::DSA::Sorting::Vis::SetBufferAtLoc(DSA_VIS_LOC, __VA_ARGS__))
#define DSA_SORT_MOVE_SET_AT(...) DSA_SORT_VIS(::DSA::Sorting::Vis::MoveSetAtLoc(DSA_VIS_LOC, __VA_ARGS__))
#define DSA_SORT_MOVE_TO_BUFFER_AT(...) DSA_SORT_VIS(::DSA::Sorting::Vis::MoveToBufferAtLoc(DSA_VIS_LOC, __VA_ARGS__))
#define DSA_SORT_MOVE_FROM_BUFFER_AT(...) DSA_SORT_VIS(::DSA::Sorting::Vis::MoveFromBufferAtLoc(DSA_VIS_LOC, __VA_ARGS__))
#else
#define DSA_SORT_VIS(stmt) DSA_VIS_ONLY(stmt)
#define DSA_SORT_VIS_DECL(stmt) DSA_VIS_DECL(stmt)
#define DSA_SORT_STEP(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_NOTE(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_FOCUS_RANGE(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_MARK(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_UNMARK(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_SWAP_EVENT(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_SET_AT(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_SET_BUFFER_AT(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_MOVE_SET_AT(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_MOVE_TO_BUFFER_AT(...) DSA_SORT_VIS((void)0)
#define DSA_SORT_MOVE_FROM_BUFFER_AT(...) DSA_SORT_VIS((void)0)
#endif

namespace DSA
{
    namespace Sorting
    {
        namespace Vis
        {
            template <typename RandIt>
            struct ArrayContext
            {
                bool active = false;
                int depth = 0;
                RandIt first{};
                RandIt last{};
                std::string name = "A";
            };

            template <typename RandIt>
            ArrayContext<RandIt> &PrimaryContext()
            {
                static thread_local ArrayContext<RandIt> ctx;
                return ctx;
            }

            template <typename RandIt>
            ArrayContext<RandIt> &BufferContext()
            {
                static thread_local ArrayContext<RandIt> ctx;
                return ctx;
            }

            template <typename RandIt, ArrayContext<RandIt> &(*Context)()>
            class ArrayScope
            {
            public:
                ArrayScope(RandIt first, RandIt last, const std::string &name)
                {
                    auto &ctx = Context();
                    owner_ = !ctx.active;
                    if (owner_)
                    {
                        ctx.active = true;
                        ctx.first = first;
                        ctx.last = last;
                        ctx.name = name;
                        DSA_VIS_ARR_INIT(name, first, last);
                    }
                    ++ctx.depth;
                }

                ~ArrayScope()
                {
                    auto &ctx = Context();
                    if (ctx.depth > 0)
                        --ctx.depth;
                    if (owner_ && ctx.depth == 0)
                        ctx.active = false;
                }

                bool Owner() const { return owner_; }

            private:
                bool owner_ = false;
            };

            template <typename RandIt>
            using PrimaryScope = ArrayScope<RandIt, PrimaryContext<RandIt>>;

            template <typename RandIt>
            using BufferScope = ArrayScope<RandIt, BufferContext<RandIt>>;

            template <typename RandIt>
            void InitArray(RandIt first, RandIt last, const std::string &name)
            {
                DSA_VIS_ARR_INIT(name, first, last);
            }

            template <typename RandIt>
            int Index(RandIt it)
            {
                auto &ctx = PrimaryContext<RandIt>();
                return ctx.active ? static_cast<int>(it - ctx.first) : 0;
            }

            template <typename RandIt>
            bool HasPrimary(RandIt)
            {
                return PrimaryContext<RandIt>().active;
            }

            template <typename RandIt>
            int Index(RandIt base, int offset)
            {
                return Index(base) + offset;
            }

            template <typename RandIt>
            int BufferIndex(RandIt it)
            {
                auto &ctx = BufferContext<RandIt>();
                return ctx.active ? static_cast<int>(it - ctx.first) : 0;
            }

            template <typename RandIt>
            bool HasBuffer(RandIt)
            {
                return BufferContext<RandIt>().active;
            }

            inline void Focus(const std::string &name, int l, int r, bool step = false)
            {
                DSA_VIS_ARR_FOCUS(name, l, r, step);
            }

            template <typename RandIt>
            void FocusRangeAt(::DSA::Vis::SourceLoc loc, const std::string &name, RandIt first, RandIt last, bool step = false)
            {
                if (!HasPrimary(first))
                    return;
                ::DSA::Vis::Logger::Global().Focus(name, Index(first), Index(last), loc, step);
            }

            template <typename RandIt>
            void FocusRange(const std::string &name, RandIt first, RandIt last, bool step = false)
            {
                FocusRangeAt(DSA_VIS_LOC, name, first, last, step);
            }

            inline void Step(const std::string &text)
            {
                DSA_VIS_STEP(text);
            }

            inline void Step(const char *text)
            {
                DSA_VIS_STEP(text);
            }

            inline void Note(const std::string &text)
            {
                DSA_VIS_NOTE(text);
            }

            inline void Note(const char *text)
            {
                DSA_VIS_NOTE(text);
            }

            inline void StepAt(::DSA::Vis::SourceLoc loc, const std::string &text)
            {
                ::DSA::Vis::Logger::Global().Message(text, loc, true);
            }

            inline void StepAt(::DSA::Vis::SourceLoc loc, const char *text)
            {
                ::DSA::Vis::Logger::Global().Message(text, loc, true);
            }

            inline void MarkPair(const std::string &name, int i, int j)
            {
                DSA_VIS_ARR_MARK(name, i, false);
                DSA_VIS_ARR_MARK(name, j, false);
            }

            template <typename RandIt>
            void Mark(const std::string &name, RandIt it)
            {
                if (HasPrimary(it))
                    DSA_VIS_ARR_MARK(name, Index(it), false);
            }

            inline void UnmarkPair(const std::string &name, int i, int j)
            {
                DSA_VIS_ARR_UNMARK(name, i, false);
                DSA_VIS_ARR_UNMARK(name, j, false);
            }

            template <typename RandIt>
            void Unmark(const std::string &name, RandIt it)
            {
                if (HasPrimary(it))
                    DSA_VIS_ARR_UNMARK(name, Index(it), false);
            }

            template <typename RandIt>
            void SwapEventAt(::DSA::Vis::SourceLoc loc, RandIt arr, int i, int j, const std::string &message, const std::string &name = "A")
            {
                int visual_i = Index(arr, i);
                int visual_j = Index(arr, j);
                bool visible = HasPrimary(arr);
                if (visible)
                    MarkPair(name, visual_i, visual_j);
                if (visible)
                    DSA_VIS_ARR_SWAP(name, visual_i, visual_j, false);
                StepAt(loc, message);
                if (visible)
                    UnmarkPair(name, visual_i, visual_j);
            }

            template <typename RandIt>
            void SwapEvent(RandIt arr, int i, int j, const std::string &message, const std::string &name = "A")
            {
                SwapEventAt(DSA_VIS_LOC, arr, i, j, message, name);
            }

            template <typename RandIt>
            void SwapWithVisualIndex(RandIt arr, int local_i, int local_j, int visual_i, int visual_j, const std::string &message, const std::string &name = "A")
            {
                MarkPair(name, visual_i, visual_j);
                std::swap(arr[local_i], arr[local_j]);
                DSA_VIS_ARR_SWAP(name, visual_i, visual_j, false);
                Step(message);
                UnmarkPair(name, visual_i, visual_j);
            }

            template <typename T>
            void Set(const std::string &name, int i, const T &value, const std::string &message, bool make_step = true)
            {
                DSA_VIS_ARR_SET(name, i, value, false);
                if (make_step)
                    Step(message);
            }

            template <typename RandIt, typename T>
            void SetAtLoc(::DSA::Vis::SourceLoc loc, const std::string &name, RandIt it, const T &value, const std::string &message, bool make_step = true)
            {
                if (HasPrimary(it))
                    DSA_VIS_ARR_SET(name, Index(it), value, false);
                if (make_step)
                    StepAt(loc, message);
            }

            template <typename RandIt, typename T>
            void SetAt(const std::string &name, RandIt it, const T &value, const std::string &message, bool make_step = true)
            {
                SetAtLoc(DSA_VIS_LOC, name, it, value, message, make_step);
            }

            template <typename RandIt, typename T>
            void MoveSetAtLoc(::DSA::Vis::SourceLoc loc, const std::string &name, RandIt src, RandIt dst, const T &value, const std::string &message, bool make_step = true)
            {
                if (HasPrimary(src) && HasPrimary(dst))
                {
                    int from = Index(src);
                    int to = Index(dst);
                    DSA_VIS_ARR_MARK(name, from, false);
                    DSA_VIS_ARR_MARK(name, to, false);
                    DSA_VIS_ARR_SET(name, to, value, false);
                    if (make_step)
                        StepAt(loc, message);
                    DSA_VIS_ARR_UNMARK(name, from, false);
                    DSA_VIS_ARR_UNMARK(name, to, false);
                }
                else if (make_step)
                {
                    StepAt(loc, message);
                }
            }

            template <typename RandIt, typename T>
            void SetBufferAtLoc(::DSA::Vis::SourceLoc loc, const std::string &name, RandIt it, const T &value, const std::string &message, bool make_step = true)
            {
                if (HasBuffer(it))
                    DSA_VIS_ARR_SET(name, BufferIndex(it), value, false);
                if (make_step)
                    StepAt(loc, message);
            }

            template <typename RandIt, typename T>
            void SetBufferAt(const std::string &name, RandIt it, const T &value, const std::string &message, bool make_step = true)
            {
                SetBufferAtLoc(DSA_VIS_LOC, name, it, value, message, make_step);
            }

            template <typename PrimaryIt, typename BufferIt, typename T>
            void MoveToBufferAtLoc(::DSA::Vis::SourceLoc loc, const std::string &primary_name, const std::string &buffer_name, PrimaryIt src, BufferIt dst, const T &value, const std::string &message, bool make_step = true)
            {
                bool has_src = HasPrimary(src);
                bool has_dst = HasBuffer(dst);
                int from = has_src ? Index(src) : 0;
                int to = has_dst ? BufferIndex(dst) : 0;
                if (has_src)
                    DSA_VIS_ARR_MARK(primary_name, from, false);
                if (has_dst)
                {
                    DSA_VIS_ARR_MARK(buffer_name, to, false);
                    DSA_VIS_ARR_SET(buffer_name, to, value, false);
                }
                if (make_step)
                    StepAt(loc, message);
                if (has_src)
                    DSA_VIS_ARR_UNMARK(primary_name, from, false);
                if (has_dst)
                    DSA_VIS_ARR_UNMARK(buffer_name, to, false);
            }

            template <typename BufferIt, typename PrimaryIt, typename T>
            void MoveFromBufferAtLoc(::DSA::Vis::SourceLoc loc, const std::string &buffer_name, const std::string &primary_name, BufferIt src, PrimaryIt dst, const T &value, const std::string &message, bool make_step = true)
            {
                bool has_src = HasBuffer(src);
                bool has_dst = HasPrimary(dst);
                int from = has_src ? BufferIndex(src) : 0;
                int to = has_dst ? Index(dst) : 0;
                if (has_src)
                    DSA_VIS_ARR_MARK(buffer_name, from, false);
                if (has_dst)
                {
                    DSA_VIS_ARR_MARK(primary_name, to, false);
                    DSA_VIS_ARR_SET(primary_name, to, value, false);
                }
                if (make_step)
                    StepAt(loc, message);
                if (has_src)
                    DSA_VIS_ARR_UNMARK(buffer_name, from, false);
                if (has_dst)
                    DSA_VIS_ARR_UNMARK(primary_name, to, false);
            }
        }
    }
}
