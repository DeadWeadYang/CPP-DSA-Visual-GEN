#include <iostream>
#include "test_all.hpp"

int main()
{
    using namespace DSA;
    
	Graph::SpanningTree::MinimumSpanningTree::DemoMST::TestCases();
	Graph::ShortestPath::DemoShortestPath::TestCases();
	Graph::TopologicalSorting::DemoTopo::TestCases();
	Graph::TopologicalSorting::DemoAOE::TestCases();
	Tree::Heap::DemoPriorityQueue::TestCases();
	Tree::BinaryTree::HuffmanTree::DemoHuffmanTree::TestCases();
	Sorting::DemoSorting::TestCases();
	Collections::SetOrMultiset::DemoSet::TestCases();
	Collections::MapOrMultimap::DemoMap::TestCases();
	Collections::ListOrForwardlist::DemoList::TestCases();
    return 0;
}