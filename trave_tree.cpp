#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include<array>
#include<random>
#include<iostream>
using namespace std;

class Tree; //forward declare

class TreeNode
{
	friend class Tree;
public:
	TreeNode();
	~TreeNode();

	bool create_node(int level);
	void print_node(int level);
	bool has_child() { return p_left && p_right; }

	TreeNode& get_l_child() { return *p_left; }
	TreeNode& get_r_child() { return *p_right; }
private:
	shared_ptr<TreeNode> p_left;
	shared_ptr<TreeNode> p_right;

	int m_const_time; //tree node has only one data member, indicate how long will cost when process the node.
};

void TreeNode::print_node(int level)
{
	for (int i = 0; i < level; ++i) {
		printf("---");
	}

	printf("%d\n", m_const_time);
	printf("|");
	if(p_left)
		p_left->print_node(level + 1);
	if(p_right)
		p_right->print_node(level + 1);
}

bool TreeNode::create_node(int level)
{//create current node, if level != 0, create children
	if (level > 0)
	{
		p_left = make_shared<TreeNode>();
		p_right = make_shared<TreeNode>();
		
		if (p_left->create_node(level - 1) &&
			p_right->create_node(level - 1)) {
			return true;
		}
		return false;
	}
	else
		return true;
}

TreeNode::TreeNode()
{
	std::random_device r;

	// Choose a random mean between 300 and 6000
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(300, 6000);

	m_const_time = uniform_dist(e1);
}

TreeNode::~TreeNode()
{
	cout << "tree node destruct was called." << endl;
}

class Tree
{
public:
	Tree();
	~Tree();
	void createTree(int level);
	void printTree();
private:
	shared_ptr<TreeNode> p_root;
};

void Tree::printTree()
{
	p_root->print_node(0);
}

void Tree::createTree(int nlevel)
{//create a tree has level == nlevel, and full bintree.
	p_root = make_shared<TreeNode>();
	p_root->create_node(nlevel);
}

Tree::Tree()
{

}

Tree::~Tree()
{

}


#include <boost/thread.hpp>
#include<boost/thread/future.hpp>

int twice(int m) {
	return 2 * m;
}

#include<future>
void test_stl_async(int i)
{
	std::future<int> a = std::async(twice, i);
}

int accumulate()
{
	int sum = 0;
	for (int i = 0; i < 5; ++i)
		sum += i;
	return sum;
}

int parallel_sum(int* data, int size)
{
	int sum = 0;
	if (size < 1000)
		for (int i = 0; i < size; ++i)
			sum += data[i];
	else {
		auto handle = boost::async(parallel_sum, data + size / 2, size - size / 2);
		sum += parallel_sum(data, size / 2);
		sum += handle.get();
	}
	return sum;
}

boost::future<int> traverse(TreeNode& t)
{
	boost::future<int> f = boost::async(accumulate);
	boost::future<int> f = boost::async(twice, 5);
	if (t.has_child()) {
		std::array<boost::future<int>, 2> results;
		results[0] = boost::async(traverse, boost::ref(t));
		
	}
	boost::future<int> p;
	return p;
}

void main()
{
	Tree new_t;
	new_t.createTree(4);
	new_t.printTree();
}