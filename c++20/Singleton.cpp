#include<cassert>
#include<iostream>

using namespace std;

//创建单例类
template<typename T>
class Singleton     //这个类是作为基类被继承的,派生类作为单例
{                   //但因为基类是单例,所以派生类只能被构筑一次
private:
    static T* m_Instance;       //默认初始化为nullptr
public:
    Singleton()
    {
        //assert用于调试,表示一定要满足括号内的条件,不然编译报错
        assert(m_Instance==nullptr);
        //只有没有创建过实例的类能走到下面这一步
        m_Instance=static_cast<T *>(this);//这个T会被识别为派生类的类型
    }
    virtual ~Singleton()
    {
        m_Instance=nullptr;//重置这个类的可创建性
    }
    static T* GetInstanceptr()
    {
        return m_Instance;
    }
    static T& GetInstance() //提供两个接口给外部直接拿到单例
    {
        return *m_Instance;
    }
};
template<typename T>
T* Singleton<T>::m_Instance=nullptr;

//这里没有用到自己声明的模板T,所以不用加template那些
class Manger:public Singleton<Manger>
{   
public:
    Manger()
    {
        cout<<"Manger()"<<endl;
    }
};

int main()
{
    {
    Manger manger;
    }
    Manger manger1;

    return 0;
}