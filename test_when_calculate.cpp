//find out when a future will process result.
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
#include<limits>


int cal_value_really()
{
	int i;
	for (i = 0; i < std::numeric_limits<int>::max(); i++)
	{
		;
	}
	
	return i / 2;
}

boost::future<int> cal_value()
{
	return boost::async(cal_value_really);
}

boost::future<boost::future<int>> cal_result()
{
	return boost::async(cal_value);
}

//int main()
//{
//	auto result_ff = cal_result();
//	auto result_f = result_ff.get();
//	int result = result_f.get();
//}