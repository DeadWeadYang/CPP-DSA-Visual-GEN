#pragma once
#include "../collections/map_multimap.hpp"

namespace DSA
{
    namespace Collections
    {
        namespace MapOrMultimap
        {
            template <typename K, typename V>
            static std::ostream &operator<<(std::ostream &os, const std::pair<K, V> &p)
            {
                return (os << '<' << p.first << ',' << p.second << '>');
            }
            struct DemoMap
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
                            os << "(insert:" << "(" << op.key << "," << op.value << "))";
                            break;
                        }
                        case 1:
                        {
                            os << "(erase:" << op.key << ")";
                            break;
                        }
                        case 2:
                        {
                            os << "(find:" << op.key << ")";
                            break;
                        }
                        default:
                        {
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
                void BSTMapDemo()
                {
                    Map<int, int, std::less<int>> bst;
                    if (!requires {
                            bst.at(0);
                            bst[0];
                            bst.empty();
                            bst.size();
                            bst.clear();
                            bst.insert({0, 0});
                            bst.erase(0);
                            bst.swap(bst);
                            bst.count(0);
                            bst.find(0);
                            bst.erase(bst.find(0));
                            bst.equal_range(0);
                            bst.lower_bound(0);
                            bst.upper_bound(0);
                        })
                    {
                        throw std::runtime_error("BST-Map test fail, api not fully implemented");
                    }
                    std::map<int, int> st;
                    int cnt = 0;
                    for (auto oo : ops)
                    {
                        ++cnt;

                        switch (oo.opt)
                        {
                        case 0:
                        {
                            int c = (st[oo.key] = oo.value);
                            int d = (bst[oo.key] = oo.value);
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-Map test fail on the " << cnt << " operation :\n";
                                ss << "inserting " << oo.value << " ; expected " << c << " node inserted, " << "but got " << d << " node inserted";
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
                            auto c = st.erase(oo.key);
                            auto d = bst.erase(oo.key);
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-Map test fail on the " << cnt << " operation :\n";
                                ss << "erasing " << oo.value << " ; expected " << c << " node erased, " << "but got " << d << " node erased";
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
                            auto c = (st[oo.key]);
                            auto d = (bst[oo.key]);
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-Map test fail on the " << cnt << " operation :\n";
                                ss << "finding " << oo.value << " ; expected " << c << " node found, " << "but got " << d << " node found";
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
                            std::vector<std::pair<int, int>> c;
                            std::vector<std::pair<int, int>> d;
                            for (auto it : st)
                            {
                                c.push_back(it);
                            }
                            for (auto it : bst)
                            {
                                d.push_back(it);
                            }
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-Map test fail on the " << cnt << " operation :\n";
                                ss << "outputing ; expected ";
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
                        if (st.size() != bst.size())
                        {
                            std::ostringstream ss;
                            ss << "BST-Map test fail on the " << cnt << " operation :\n";
                            ss << "size ; expected " << st.size() << ", but got " << bst.size();
                            ss << "\ntotal operations:";
                            auto tmp = ops;
                            tmp.resize(cnt);
                            Print(tmp, ss);
                            throw std::runtime_error(ss.str());
                        }
                        if (!bst.checker())
                        {
                            std::ostringstream ss;
                            ss << "BST-Map test fail on the " << cnt << " operation :\n";
                            ss << "checker fail;";
                            ss << "\ntotal operations:";
                            auto tmp = ops;
                            tmp.resize(cnt);
                            Print(tmp, ss);
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                void BSTMultiMapDemo()
                {
                    MultiMap<int, int, std::less<int>> bst;
                    if (!requires {
                            bst.empty();
                            bst.size();
                            bst.clear();
                            bst.insert({0, 0});
                            bst.erase(0);
                            bst.swap(bst);
                            bst.count(0);
                            bst.find(0);
                            bst.erase(bst.find(0));
                            bst.equal_range(0);
                            bst.lower_bound(0);
                            bst.upper_bound(0);
                        })
                    {
                        throw std::runtime_error("BST-MultiMap test fail, api not fully implemented");
                    }
                    std::multimap<int, int> st;
                    int cnt = 0;
                    for (auto oo : ops)
                    {
                        ++cnt;

                        switch (oo.opt)
                        {
                        case 0:
                        {
                            int c = st.insert({oo.key, oo.value}) != st.end();
                            int d = bst.insert({oo.key, oo.value}) != bst.end();
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-MultiMap test fail on the " << cnt << " operation :\n";
                                ss << "inserting " << oo.value << " ; expected " << c << " node inserted, " << "but got " << d << " node inserted";
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
                            auto c = st.erase(oo.key);
                            auto d = bst.erase(oo.key);
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-MultiMap test fail on the " << cnt << " operation :\n";
                                ss << "erasing " << oo.value << " ; expected " << c << " node erased, " << "but got " << d << " node erased";
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
                            int c = (st.count(oo.key));
                            int d = (bst.count(oo.key));
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-MultiMap test fail on the " << cnt << " operation :\n";
                                ss << "finding " << oo.value << " ; expected " << c << " node found, " << "but got " << d << " node found";
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
                            std::vector<std::pair<int, int>> c;
                            std::vector<std::pair<int, int>> d;
                            for (auto it : st)
                            {
                                c.push_back(it);
                            }
                            for (auto it : bst)
                            {
                                d.push_back(it);
                            }
                            if (c != d)
                            {
                                std::ostringstream ss;
                                ss << "BST-MultiMap test fail on the " << cnt << " operation :\n";
                                ss << "outputing ; expected ";
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
                        if (st.size() != bst.size())
                        {
                            std::ostringstream ss;
                            ss << "BST-MultiMap test fail on the " << cnt << " operation :\n";
                            ss << "size ; expected " << st.size() << ", but got " << bst.size();
                            ss << "\ntotal operations:";
                            auto tmp = ops;
                            tmp.resize(cnt);
                            Print(tmp, ss);
                            throw std::runtime_error(ss.str());
                        }
                        if (!bst.checker())
                        {
                            std::ostringstream ss;
                            ss << "BST-MultiMap test fail on the " << cnt << " operation :\n";
                            ss << "checker fail;";
                            ss << "\ntotal operations:";
                            auto tmp = ops;
                            tmp.resize(cnt);
                            Print(tmp, ss);
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                static std::vector<operation> RandomGen(int n, int w, unsigned int seed = 0)
                {
                    std::mt19937 rng{seed};
                    std::vector<operation> res(n);
                    auto odist = std::uniform_int_distribution<int>(0, 8);
                    auto vdist = std::uniform_int_distribution<int>(-w, w);
                    for (int i = 0; i < n; i++)
                    {
                        int tmp = odist(rng);
                        res[i].opt = (tmp < 4 ? 0 : (tmp < 6 ? 1 : (tmp < 8 ? 2 : 3)));
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
                    auto instance = DemoMap();
                    instance.ops = ops;
                    instance.BSTMapDemo();
                    instance.BSTMultiMapDemo();
                }
                static void TestCases()
                {

                    int case_index = 0;
                    try
                    {

                        ++case_index;
                        Demo(RandomGen(5, 2));
                        ++case_index;
                        Demo(RandomGen(10, 3));
                        ++case_index;
                        Demo(RandomGen(25, 4));
                        ++case_index;
                        Demo(RandomGen(50, 5));
                        ++case_index;
                        Demo(RandomGen(100, 10));
                        ++case_index;
                        Demo(RandomGen(200, 30));
                        ++case_index;
                        Demo(RandomGen(500, 40));
                        ++case_index;
                        Demo(RandomGen(2000, 500));
                        ++case_index;
                        Demo(RandomGen(5000, 1000));
                        ++case_index;
                        Demo(RandomGen(5000, 3000));

                        std::cout
                            << "Map/MultiMap test passed" << std::endl;
                    }
                    catch (const std::exception &ex)
                    {
                        std::cerr << "Map/MultiMap test case " << case_index << " fail\n"
                                  << ex.what() << std::endl;
                    }
                }
            };
        }
    }
}