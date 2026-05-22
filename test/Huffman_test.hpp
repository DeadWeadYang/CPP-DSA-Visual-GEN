#pragma once
#include "../tree/binary_tree/Huffman_tree.hpp"

namespace DSA{

    namespace Tree{
        namespace BinaryTree{
            namespace HuffmanTree{
                
                struct DemoHuffmanTree
                {
                    std::map<int, int> mp;
                    int expected_WPL;
                    void HuffmanTreeDemo()
                    {
                        HuffmanTree<int> ht;
                        ht.build(mp);
                        int wpl = ht.getWeightedPathLength();
                        if (wpl != expected_WPL)
                        {
                            std::ostringstream ss;
                            ss << "HuffmanTree test fail: output WPL: " << wpl << " ; expected WPL: " << expected_WPL;
                            throw std::runtime_error(ss.str());
                        }
                    }
                    static void Demo(std::string text, int expected_WPL)
                    {
                        std::map<int, int> mp;
                        for (auto c : text)
                            ++mp[c];
                        auto instance = DemoHuffmanTree();
                        instance.mp = mp;
                        instance.expected_WPL = expected_WPL;
                        instance.HuffmanTreeDemo();
                    }
                    static void TestCases()
                    {
                        int case_index = 0;
                        try
                        {
                            ++case_index;
                            Demo("this is an example of a huffman tree", 135);
                            std::cout << "HuffmanTree test passed" << std::endl;
                        }
                        catch (const std::exception &ex)
                        {

                            std::cerr << "HuffmanTree test case " << case_index << " fail\n"
                                      << ex.what() << std::endl;
                        }
                    }
                };
            }
        }
    }

}