#include<future>
#include<iostream>
#include<thread>
using namespace std;

using FactoryPromise=promise<long long>;//创建承诺的工厂类,可以生产期值

long long Factorial(unsigned int value)
{       //递归计算阶乘
    cout<<"task:"<<this_thread::get_id()<<endl;
    this_thread::sleep_for(1000ms);
    return value==1?1:value*Factorial(value-1);
}

int main()
{
    using namespace chrono;
    cout<<"main:"<<this_thread::get_id()<<endl;
    //感觉async就是很像是有一个返回值(期值)的thread
    //第一次调用async就是和正常thread一样调用,但是他好像不一定保证线程怎么执行的
    //但是会保证在创建对象的时候和调用get的某个时间调用这个函数
    auto future=async(Factorial,3);
    cout<<"future:"<<future.get()<<endl;
    //后面就要加上launch的不同枚举组合
    auto future2=async(launch::async,Factorial,3);
    cout<<"future2:"<<future2.get()<<endl;
    
    auto future3=async(launch::deferred,Factorial,3);
    cout<<"future3:"<<future3.get()<<endl;
    //后面这个就是前两个的或操作
    auto future4=async(launch::async|launch::deferred,Factorial,3);
    cout<<"future4:"<<future4.get()<<endl;
    return 0;
}





// int main()
// {
//     using namespace chrono; //packaged_task这个包装任务,里面封装了一个promise
//     packaged_task<long long(unsigned int)>task(Factorial);  //和function包装器很像,模板里面是函数签名
//     auto future=task.get_future();                  //同样可以拿到future
//     thread mythread(move(task),5);          //启用线程,和原本差不多,只是传入的是包装任务
//     //这个while循环没有看懂,但好像是等待当前时间到达了future应该等待到的时间
//     while(future.wait_until(system_clock::now()+seconds(1))!=future_status::ready)
//     {
//         cout<<"still waiting for..."<<endl;
//     } 
//     cout<<"waiting end,future:"<<future.get()<<endl;
//     mythread.join();
//     return 0;

// }

// int main()
// {   //这个操作主要是为了做到"把工作线程的数据传递到启动任务的线程(此时为main函数)"
//     //就是说这也是一种线程之间传递数据的方法,在外部创建future/promise对子,promise执行函数
//     //然后把函数结果在未来注入到绑定好的future,future调用get拿到这个值
//     using namespace chrono;
//     FactoryPromise promise;//创建工厂类

//     auto future=promise.get_future();//获取期值,对promise调用get_future
//     promise.set_value(Factorial(5));    //执行阶乘函数
//     cout<<"hello"<<endl;
//     cout<<future.get()<<endl;           //对future调用get
//     cout<<"world"<<endl;

//     return 0;
// }