#include<cstdlib>
#include<iostream>
using namespace std;

struct MemoryHeader//新增的头部,用来储存此次分配的信息
{
    const char* m_filename=nullptr;
    int m_line=0;
};
//new和delete其实都是函数签名,这里对其进行重载ps:不仅是类里面可以重载,外面也可以
//普通的new函数签名为void new(size_t size);
void* operator new(size_t size, const char* filename,int line) noexcept //承诺不抛出异常
{
    void* pMemstart=malloc(size+sizeof(MemoryHeader));
    //reinterpret的作用就是把完全不同的类型进行转换,int->指针这种
    MemoryHeader* pHeader=reinterpret_cast<MemoryHeader*>(pMemstart);
    pHeader->m_filename=filename;   //这里是底层const,可以直接指过去,而且不用深拷贝,反正外面是代码段的__FILE__
    pHeader->m_line=line;
    cout<<"正在构造由:"<<pHeader->m_filename<<":"<<pHeader->m_line<<"分配的内存"<<endl;
    //通过指针加法进行右移,跳过这段内存头分配给用户
    return reinterpret_cast<void*>(pMemstart)+sizeof(MemoryHeader);
}

void operator delete(void* pMemory) noexcept
{
    //用char*来暂时接收指针左移的实际内存块
    char* temp = reinterpret_cast<char*>(pMemory)-sizeof(MemoryHeader);
    //然后二次转换成一开始的头部
    MemoryHeader* pHeader = reinterpret_cast<MemoryHeader*>(temp);
    cout<<"正在析构由:"<<pHeader->m_filename<<":"<<pHeader->m_line<<"分配的内存"<<endl;
    free(pHeader);//输出信息并且释放一开始的头部

}
void* operator new[](size_t size, const char* filename,int line) noexcept //承诺不抛出异常
{
    void* pMemstart=malloc(size+sizeof(MemoryHeader));
    //reinterpret的作用就是把完全不同的类型进行转换,int->指针这种
    MemoryHeader* pHeader=reinterpret_cast<MemoryHeader*>(pMemstart);
    pHeader->m_filename=filename;   //这里是底层const,可以直接指过去,而且不用深拷贝,反正外面是代码段的__FILE__
    pHeader->m_line=line;
    cout<<"正在构造由:"<<pHeader->m_filename<<":"<<pHeader->m_line<<"分配的内存"<<endl;
    //通过指针加法进行右移,跳过这段内存头分配给用户
    return reinterpret_cast<void*>(pMemstart)+sizeof(MemoryHeader);
}
void operator delete[](void* pMemory) noexcept
{
    //用char*来暂时接收指针左移的实际内存块
    char* temp = reinterpret_cast<char*>(pMemory)-sizeof(MemoryHeader);
    //然后二次转换成一开始的头部
    MemoryHeader* pHeader = reinterpret_cast<MemoryHeader*>(temp);
    cout<<"正在析构由:"<<pHeader->m_filename<<":"<<pHeader->m_line<<"分配的内存"<<endl;
    free(pHeader);//输出信息并且释放一开始的头部

}
class Myclass
{
    int m_data;
    public:
    Myclass(){}
    Myclass(int i)
    {
        m_data=i;
        cout<<"Myclass construct\n";
    }
    void* operator new(size_t size, const char* filename,int line)
    {
        cout<<"Myclass operator new\n";
        return malloc(size);
    }
    void operator delete(void* ptr)
    {
        cout<<"Myclass operator delete\n";
        free(ptr);
    }
    ~Myclass(){}
};
#define new new(__FILE__, __LINE__)//这里宏定义使得后文的new都换成上面版本的new
//即new int 等价于调用operator new(4,__FILE__, __LINE__) 
//delete不用重新define,他会自己匹配到新的,define只是为了免去写__FILE__这些的
int main()
{
    int *intptr=new int;
    Myclass *myclass=new Myclass(3);
    Myclass *myclass2=new Myclass[3];
    delete intptr;
    delete myclass;
    delete[] myclass2;
}