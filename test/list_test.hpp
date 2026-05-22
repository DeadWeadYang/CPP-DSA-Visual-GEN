#pragma once
#include "../collections/list_forwardlist.hpp"
namespace DSA
{
    namespace Collections
    {
        namespace ListOrForwardlist
        {
            struct DemoList
            {
                struct operation
                {
                    int opt, key, value;
                    friend std::ostream &operator<<(std::ostream &os, const operation &op)
                    {
                        switch (op.opt)
                        {
                        case 0:
                        {
                            os << "(insert:" << "(" << (op.key < 0 ? "front" : "back") << "," << op.value << "))";
                            break;
                        }
                        case 1:
                        {
                            os << "(pop:" << (op.key < 0 ? "front" : "back") << ")";
                            break;
                        }
                        case 2:
                        {
                            os << "(find and insert:" << op.key << "," << op.value << ")";
                            break;
                        }
                        default:
                        {
                            if (op.opt == 3)
                                os << "(sort and output)";
                            else
                                os << "(output)";
                            break;
                        }
                        }
                        return os;
                    }
                };
                std::vector<operation> ops;
                template <typename T>
                static void Print(const std::vector<T> &v, std::ostream &os)
                {
                    os << "[ ";
                    for (auto i : v)
                        os << i << ", ";
                    os << "]";
                }
                void ListDemo()
                {
                    List<int> ls;
                    if (!requires {
                            ls.front();
                            ls.back();
                            ls.empty();
                            ls.size();
                            ls.clear();
                            ls.push_back(0);
                            ls.push_front(0);
                            ls.pop_back();
                            ls.pop_front();
                            ls.insert(ls.end(), 0);
                            ls.erase(ls.begin());
                            ls.resize(0);
                            ls.swap(ls);
                            ls.merge(ls);
                            ls.sort();
                        })
                    {
                        throw std::runtime_error("BST-List test fail, api not fully implemented");
                    }
                    std::list<int> st;
                    int cnt = 0;
                    for (auto oo : ops)
                    {
                        ++cnt;
                        switch (oo.opt)
                        {
                        case 0:
                        {
                            bool flag = oo.key < 0;
                            int c, d;
                            std::string dir = flag ? "front" : "back";
                            if (flag)
                            {
                                st.push_front(oo.value);
                                ls.push_front(oo.value);
                                c = st.front();
                                d = ls.front();
                            }
                            else
                            {
                                st.push_back(oo.value);
                                ls.push_back(oo.value);
                                c = st.back();
                                d = ls.back();
                            }
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "List test fail on the " << cnt << " operation :\n";
                                ss << "pushing " << dir << " ; expected " << c << " at " << dir << ", but got " << d;
                                ss << "\ntotal operations:";
                                auto tmp = ops;
                                tmp.resize(cnt);
                                Print(tmp, ss);
                                throw std::runtime_error(ss.str());
                            }
                            break;
                        }
                        case 1:
                        {
                            bool flag = oo.key < 0;
                            int c = 0, d = 0;
                            std::string dir = flag ? "front" : "back";
                            if (flag)
                            {
                                if (!st.empty())
                                    c = st.front(),st.pop_front();
                                if (!ls.empty())
                                    d = ls.front(),ls.pop_front() ;
                            }
                            else
                            {
                                if (!st.empty())
                                    c = st.back(),st.pop_back() ;
                                if (!ls.empty())
                                    d = ls.back(),ls.pop_back();
                            }
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "List test fail on the " << cnt << " operation :\n";
                                ss << "popping " << dir << " ; expected " << c << " at " << dir << ", but got " << d;
                                ss << "\ntotal operations:";
                                auto tmp = ops;
                                tmp.resize(cnt);
                                Print(tmp, ss);
                                throw std::runtime_error(ss.str());
                            }
                            break;
                        }
                        case 2:
                        {
                            int c = *st.insert(std::find(st.begin(), st.end(), oo.key), oo.value);
                            int d = *ls.insert(std::find(ls.begin(), ls.end(), oo.key), oo.value);
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "List test fail on the " << cnt << " operation :\n";
                                ss << "find " << oo.key << " and insert " << oo.value << " ; expected " << c << " inserted, " << "but got " << d << " inserted";
                                ss << "\ntotal operations:";
                                auto tmp = ops;
                                tmp.resize(cnt);
                                Print(tmp, ss);
                                throw std::runtime_error(ss.str());
                            }
                            break;
                        }
                        default:
                        {
                            std::vector<int> c;
                            std::vector<int> d;
                            if (oo.opt == 3)
                            {
                                st.sort();
                                ls.sort();
                            }
                            for (auto it : st)
                            {
                                c.push_back(it);
                            }
                            for (auto it : ls)
                            {
                                d.push_back(it);
                            }
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "List test fail on the " << cnt << " operation :\n";
                                ss << (oo.opt == 3 ? "sorting and " : "") << "outputing ; expected ";
                                Print(c, ss);
                                ss << "\nbut got ";
                                Print(d, ss);
                                ss << "\ntotal operations:";
                                auto tmp = ops;
                                tmp.resize(cnt);
                                Print(tmp, ss);
                                throw std::runtime_error(ss.str());
                            }
                            break;
                        }
                        }
                        if (st.size() != ls.size())
                        {
                            std::ostringstream ss;
                            ss << "List test fail on the " << cnt << " operation :\n";
                            ss << "size ; expected " << st.size() << ", but got " << ls.size();
                            ss << "\ntotal operations:";
                            auto tmp = ops;
                            tmp.resize(cnt);
                            Print(tmp, ss);
                            throw std::runtime_error(ss.str());
                        }
                    }
                    return;
                }
                static std::vector<operation> RandomGen(int n, int w, unsigned int seed = 0)
                {
                    std::mt19937 rng{seed};
                    std::vector<operation> res(n);
                    auto odist = std::uniform_int_distribution<int>(0, 9);
                    auto vdist = std::uniform_int_distribution<int>(-w, w);
                    for (int i = 0; i < n; i++)
                    {
                        int tmp = odist(rng);
                        res[i].opt = (tmp < 4 ? 0 : (tmp < 6 ? 1 : (tmp < 8 ? 2 : (tmp < 9 ? 3 : 4))));
                        if (res[i].opt < 3)
                        {
                            res[i].key = vdist(rng);
                            res[i].value = vdist(rng);
                        }
                    }
                    return res;
                }
                static void Demo(const std::vector<operation> &ops)
                {
                    auto instance = DemoList();
                    instance.ops = ops;
                    instance.ListDemo();
                }
                static void TestCases()
                {

                    int case_index = 0;
                    try
                    {

                        // ++case_index;
                        // Demo(RandomGen(5, 2));
                        // ++case_index;
                        // Demo(RandomGen(10, 3));
                        ++case_index;
                        Demo(RandomGen(25, 4));
                        ++case_index;
                        Demo(RandomGen(50, 5));
                        ++case_index;
                        Demo(RandomGen(100, 10));
                        ++case_index;
                        Demo(RandomGen(200, 20));
                        ++case_index;
                        Demo(RandomGen(500, 40));
                        ++case_index;
                        Demo(RandomGen(2000, 100));
                        ++case_index;
                        Demo(RandomGen(5000, 200));
                        ++case_index;
                        Demo(RandomGen(5000, 300));
                        ++case_index;
                        Demo(RandomGen(5000, 500));

                        std::cout
                            << "List test passed" << std::endl;
                    }
                    catch (const std::exception &ex)
                    {
                        std::cerr << "List test case " << case_index << " fail\n"
                                  << ex.what() << std::endl;
                    }
                }
            };
        }
    }
}