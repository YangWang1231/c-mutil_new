#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <functional>
#include <iostream>
#include<string>
using std::string;
using boost::future;
using boost::async;
//using boost::then;

void accumulate(boost::promise<int>& p)
{
    int sum = 0;
    for (int i = 0; i < 5; ++i)
        sum += i;
    p.set_value(sum);
}


int calculate_the_answer_to_life_the_universe_and_everything()
{
    std::cout << "calc function was called." << std::endl;
    return 42;
}

void invoke_lazy_task(boost::packaged_task<int>& task)
{
    try
    {
        task();
    }
    catch (boost::task_already_started&)
    {
    }
}

void test_set_wait_callback()
{
    boost::packaged_task<int> task(calculate_the_answer_to_life_the_universe_and_everything);
    task.set_wait_callback(invoke_lazy_task);
    boost::future<int> f(task.get_future());

    assert(f.get() == 42);

}

//int main()
//{
//    test_set_wait_callback();
//
//    boost::future<int> f1 = async([]() { return 123; });
//
//    boost::future<int> f2 = f1.then([](future<int> f) { return f.get() + 10; }); // here .get() won't block });
//    std::cout << "result is " << f2.get() << std::endl;
//
//    boost::promise<int> p;
//    boost::future<int> f = p.get_future();
//    boost::thread t{ accumulate, std::ref(p) };
//    std::cout << f.get() << '\n';
//}