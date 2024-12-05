#include<iostream>


using namespace std;

//选出较大的那个值,使用模板
template<typename T,typename U>
T Max(const T& x,const U& y)
{
    return x>y?x:y;
}

class A
{
    public:
    int x;
};
//想不添加operator的情况下依旧使用Max就可以使用偏特化
//当参数是A这个类的时候就会匹配进来
template<>
A Max(const A& a,const A& b)
{
    return a.x>b.x?a:b;
}

//创建一个可以存放任何类型的数组类
template<typename T>
class m_Array
{
    private:
    T* m_data;

    public:
    m_Array(int size)
    {
        m_data=new T[size];
    }
    T& operator[](int index)
    {
        return m_data[index];
    }
    ~m_Array()
    {
        delete[] m_data;
        m_data=NULL;
    }
};
int main()
{
    m_Array<int> arr(10);
    for(int i=0; i<10; i++)
    {
        arr[i]=i;
    }
    for(int i=0; i<10; i++)
    {
        cout<<arr[i]<<" ";
    }
    cout<<endl;

    // A a1,a2;
    // a1.x = 10;
    // a2.x = 20;
    // int a = 10;
    // int b = 20;
    // cout<<Max(a,b)<<endl;

    // double c = 10.0;
    // double d = 20.0;
    // cout<<Max(c,d)<<endl;

    // cout<<Max(a,d)<<endl;

    // A a1,a2;
    // a1.x = 10;
    // a2.x = 20;
    // A a3=Max(a1,a2);
    // cout<<a3.x<<endl;
    return 0;
}
