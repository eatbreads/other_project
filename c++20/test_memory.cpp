#include<chrono>
#include<iostream>
using namespace std;

const int ROWS=10000;
const int COLS=1000;
int element[ROWS][COLS];
int *element_ptr[ROWS][COLS];
int main()
{
    for(int i=0; i<ROWS; i++)//创建一个这么大的数组,并初始化
    {
        for(int j=0; j<COLS; j++)
        {
            element[i][j]=i+j;
            element_ptr[i][j]=new int(element[i][j]);//ptr是new出来的,不是指向element的指针
        }
    }
    auto allstart=chrono::steady_clock::now();
    auto start=chrono::steady_clock::now();//测试按顺序访问element数组花费的时间
    for(int i=0; i<ROWS; i++)
    {
        for(int j=0; j<COLS; j++)
        {   //按顺序访问,element的内存布局其实是线性的
            int temp=element[i][j];
        }
    }
    auto end=chrono::steady_clock::now();
    auto duration=chrono::duration_cast<chrono::microseconds>(end-start).count();    
    cout<<"按顺序访问element数组花费的时间:"<<duration<<endl;
    
    start=chrono::steady_clock::now();//测试按照列来访问数组花费的时间
    for(int i=0; i<ROWS; i++)
    {
        for(int j=0; j<COLS; j++)
        {   //按列来访问,element的内存布局其实是线性的,所以这里会跳跃访问,可能缓存不命中
            int temp=element[j][i];
        }
    }
    end=chrono::steady_clock::now();
    duration=chrono::duration_cast<chrono::microseconds>(end-start).count();
    cout<<"条约按列访问element数组花费的时间:"<<duration<<endl;

    //测试通过指针来间接访问二维数组
    start=chrono::steady_clock::now();
    for(int i=0; i<ROWS; i++)
    {
        for(int j=0; j<COLS; j++)
        {
            int temp=*(element_ptr[i][j]);
        }
    }
    end=chrono::steady_clock::now();
    duration=chrono::duration_cast<chrono::microseconds>(end-start).count();
    cout<<"通过指针间接访问二维数组花费的时间:"<<duration<<endl;
    
    auto allend=chrono::steady_clock::now();
    auto allduration=chrono::duration_cast<chrono::microseconds>(allend-allstart).count();
    cout<<"程序总花费的时间:"<<allduration<<endl;
    return 0;
}