#include<chrono>
#include<iostream>
#include<cinttypes>
using namespace std;
using chrono::high_resolution_clock;
void RunTest(uint32_t times)//测试高精度定时器
{
    //这个命名空间里面有一个这个类
    auto start = chrono::high_resolution_clock::now();//这里是个静态成员函数
    for (uint32_t ii = 0; ii < times; ii++)
    {
        uint64_t a = 1;
        uint64_t b = 2;
        uint64_t c = 3;
        uint64_t d = 4;
        uint64_t e = 5;
        uint64_t f = 6;
        uint64_t g = 7;
        uint64_t h = 8;
        uint64_t i = 9;
        uint64_t j = 10;
        uint64_t k = 11;
        uint64_t l = 12;
        uint64_t m = 13;
        uint64_t n = 14;
        uint64_t o = 15;
        uint64_t p = 16;
        uint64_t q = 17;
    }
    auto end = chrono::high_resolution_clock::now();//使用里面的duration_cast模板,可以转换成对应需要的精度
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();//此处是微秒
    std::cout << duration << std::endl;
}
int main()
{
    RunTest(10000);
    RunTest(10000000);
    RunTest(1000000000);

    return 0;
}
