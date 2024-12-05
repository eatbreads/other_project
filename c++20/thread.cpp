#include<iostream>
#include<thread>
#include<cinttypes>

using namespace std;

int main()
{
    const int NumOfProcessors =thread::hardware_concurrency();
    cout<<"处理器数量为: "<<NumOfProcessors<<endl;
    return 0;
}