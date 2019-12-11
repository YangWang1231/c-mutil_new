#define BOOST_THREAD_VERSION 5
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include<array>
#include<random>
#include<iostream>
#include <boost/thread.hpp>
#include<boost/thread/future.hpp>
#include <thread>

using namespace std;

const int big_number = 200000000;
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

	int compute_syn()
	{//const sleep time 2s.
		//std::this_thread::sleep_for(2s);
		
		//auto start = std::chrono::high_resolution_clock::now();

		//int result = traverse_async(boost::ref(new_t.get_root())).get();
		for (int i = 0; i < big_number; ++i)
			;

		/*auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "compute_syn cost time " << elapsed.count() << " ms\n";*/

		return m_const_time;
	}

	boost::future<int> compute_result()
	{
		boost::future<int> f = boost::async(&TreeNode::compute_syn, this);
		return f;
	}

	int compute_result_syn()
	{
		compute_syn();
		return m_const_time;
	}

private:
	shared_ptr<TreeNode> p_left;
	shared_ptr<TreeNode> p_right;

	int m_const_time; //tree node has only one data member, indicate how long will cost when process the node.
	static int total_const_time;
};

int TreeNode::total_const_time = 0;

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

	TreeNode::total_const_time += m_const_time;
}

TreeNode::~TreeNode()
{
//	cout << "tree node destruct was called." << endl;
}

class Tree
{
public:
	Tree();
	~Tree();
	void createTree(int level);
	void printTree();
	TreeNode& get_root() { return *p_root; }
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
	//printf("total cost time is %d\n", TreeNode::total_const_time);
}

Tree::Tree()
{

}

Tree::~Tree()
{

}

int some_costtime_cal(int n)
{
	for (int i = 0; i < big_number; ++i)
		;
	return n;
}

#include <tuple>
boost::future<int> combine_results_async(boost::future<std::tuple<boost::future<boost::future<int>>, boost::future<boost::future<int>>, boost::future<int>>>  a)
{
	auto tuple_future = a.get();
	auto v1 = boost::move(std::get<0>(tuple_future));
	auto v2 = boost::move(std::get<1>(tuple_future));
	auto v3 = boost::move(std::get<2>(tuple_future));
	int value1 = v1.get().get();
	int value2 = v2.get().get();
	int value3 = v3.get();
	return boost::async(some_costtime_cal,value1 + value2 + value3);
}

boost::future<int> traverse_async(TreeNode& t)
{
	if (t.has_child()) {
		std::array<boost::future<boost::future<int>>, 2> results;
		results[0] = boost::async(traverse_async, boost::ref(t.get_l_child()) );
		results[1] = boost::async(traverse_async, boost::ref(t.get_r_child()));
		//boost::future<std::tuple<boost::future<boost::future<int>>, boost::future<boost::future<int>>, boost::future<int>>> f = boost::when_all(boost::move(results[0]), boost::move(results[1]), boost::move(t.compute_result()));
		auto f = boost::when_all(boost::move(results[0]), boost::move(results[1]), boost::move(t.compute_result()));
		return combine_results_async(boost::move(f));
	}
	return t.compute_result();
}

int combine_results_async_simple(boost::future<std::tuple<boost::future<int>, boost::future<int>>>  a, int currentval)
{
	auto tuple_future = a.get();
	auto v1 = boost::move(std::get<0>(tuple_future));
	auto v2 = boost::move(std::get<1>(tuple_future));
	int value1 = v1.get();
	int value2 = v2.get();
	int value3 = currentval;
	return some_costtime_cal(value1 + value2 + value3);
}

int traverse_async_simple(TreeNode& t)
{
	if (t.has_child()) {
		std::array<boost::future<int>, 2> results;
		results[0] = boost::async(traverse_async_simple,boost::ref(t.get_l_child()));
		results[1] = boost::async(traverse_async_simple,boost::ref(t.get_r_child()));

		//boost::future<std::tuple<boost::future<boost::future<int>>, boost::future<boost::future<int>>, boost::future<int>>> f = boost::when_all(boost::move(results[0]), boost::move(results[1]), boost::move(t.compute_result()));
		auto f = boost::when_all(boost::move(results[0]), boost::move(results[1]));
		int currentval = t.compute_result_syn();
		return combine_results_async_simple(boost::move(f), currentval);
	}
	return t.compute_result_syn();
}


int combine_results_sync(std::array<int, 3> a)
{
	auto v1 = a[0];
	auto v2 = a[1];
	auto v3 = a[2];
	return some_costtime_cal(v1 + v2 + v3);
}

int traverse_sync(TreeNode& t)
{
	if (t.has_child()) {
		std::array<int, 3> results;
		results[0] = traverse_sync(boost::ref(t.get_l_child()));
		results[1] = traverse_sync(boost::ref(t.get_r_child()));
		results[2] = t.compute_syn();
		return combine_results_sync(boost::move(results));
	}
	return t.compute_syn();
}

#include <chrono>

void calc_syn(int n)
{
	Tree new_t;
	new_t.createTree(n);
	//new_t.printTree();

	auto start = std::chrono::high_resolution_clock::now();

	//int result = traverse_async_simple(boost::ref(new_t.get_root()));
	//int result = traverse_async(boost::ref(new_t.get_root())).get();
	int result = traverse_sync(boost::ref(new_t.get_root()));

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "Waited " << elapsed.count() << " ms\n";

	printf("the result of cal tree is %d\n", result);
}

void calc_asyn(int n)
{
	Tree new_t;
	new_t.createTree(n);
	//new_t.printTree();

	auto start = std::chrono::high_resolution_clock::now();

	//int result = traverse_async_simple(boost::ref(new_t.get_root()));
	int result = traverse_async(boost::ref(new_t.get_root())).get();

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "Waited " << elapsed.count() << " ms\n";

	printf("the result of cal tree is %d\n", result);
}

#include<iostream>
void experiment(int n)
{
	std::cout << "tree level: " << n << std::endl;
	std::cout << std::endl;


	std::cout << "ayn cal: " << std::endl;
	calc_asyn(n);

	std::cout << "syn cal: " << std::endl;
	calc_syn(n);
	std::cout << std::endl;
}

void main()
{
	experiment(9);
	//for(auto i = 4; i < 20; ++i)
	//	experiment(i);
}