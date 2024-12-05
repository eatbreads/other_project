#include<iostream>
#include<string.h>
#include<cstdio>
#include<cstring>
#include<string>
using namespace std;

template<class T>
class smartptr
{
private:
    T *ptr=nullptr;
    int * reference=nullptr;
public:
    smartptr()=default;
    smartptr(T *p)
    {
        ptr=p;
        reference=new int;
        *reference=1;
        //cout<<*reference<<endl;
    }
    smartptr(const smartptr<T>& other)
    {
        ptr=other.ptr;
        reference=other.reference;
        (*reference)++;
        //cout<<*reference<<endl;
    }
    smartptr(smartptr<T>&& other)
    {
        ptr=other.ptr;
        reference=other.reference;
        other.reference=nullptr;
        other.ptr=nullptr;
    }
    ~smartptr()
    {
        if(ptr!=nullptr)
        {
            (*reference)--;
            //cout<<*reference<<endl;
            if(*reference==0)
            {
                //cout<<"start delete "<<endl;
                delete ptr;
                delete reference;
            }
            ptr=nullptr;
            reference=nullptr;
            
        }
    }
    smartptr<T>& operator=(const smartptr<T> &other)
    {   
        ptr=other.ptr;
        reference=other.reference;
        *(reference)++;
        //cout<<*reference<<endl;
        return *this;
    }
    smartptr<T>& operator=(smartptr<T> && other)
    {
        if(this!=&other)
        {
        ptr=other.ptr;
        reference=other.reference;
        other.reference=nullptr;
        other.ptr=nullptr;
        }
        return *this;
    }

    T& operator*()
    {
        return *ptr;
    }
    

};

class MyClass 
{
    char *name;
    public:
    MyClass( const char *n)
    {
        name=new char[strlen(n)+1];
        strcpy(name,n);
    }
    ~MyClass()
    {
        delete[] name;
        cout<<"destroy MyClass"<<endl;
    }
    void print()
    {
        cout<<name<<endl;
    }
};
int main()
{
    smartptr<MyClass> p1(new MyClass("xiaomi"));
    smartptr<MyClass> p2(p1);
    smartptr<MyClass> p3=p2;
    smartptr<MyClass> p4=std::move(p3);
    (*p4).print();
    //(*p3).print();
    return 0;
}