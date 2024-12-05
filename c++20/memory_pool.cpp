#include<iostream>
#include<string>
#include<string.h>

using namespace std;

class Page
{
private:
    char m_Memory[1024*32];//一页内存分配32kb,正好是一份高速缓存,但是其中一个小块是32字节,有1024小块
    bool m_Free[1024];     //整个页有1024个小块,ps:一个小块只会被分配一次,被独享
    Page* m_pNextPage;     //指向下一页,结构是链表
public:
    Page():m_pNextPage(nullptr)
    {
        memset(m_Free,1,1024);//表示一开始所有块都是有位置
    }
    ~Page(){
        delete m_pNextPage;
        cout<<"析构了一页\n";
        m_pNextPage=nullptr;
    }
    void *alloc()
    {
        void * ptr=nullptr;
        for(int ii=0;ii<1024;ii++)//在当前页寻找到没有分配的小块
        {
            if(m_Free[ii]==true)
            {
                m_Free[ii]=false;
                ptr=&m_Memory[ii*32];
                cout<<"分配了第"<<ii<<"个小块\n";
                break;
            }
        }
        if(ptr==nullptr)//这一页没有空位置了
        {
            if(m_pNextPage==nullptr)//如果没有下一页就分配新页
            {
                m_pNextPage=new Page();//分配一个新页
            }//如果下一页有就不进入if直接去下一页找
            ptr=m_pNextPage->alloc();//指向下一页创建的,如果下一页也满了就会到下下页
        }
        
        return ptr;
    }
    bool Free(void * ptr)
    {
        bool ret=false ;//是否释放了
        bool inPage=(ptr>=m_Memory&&ptr<=&m_Memory[1024*32-1]);//是否存在于当前page里面,通过指针的比较
        if(inPage)       //如果存在
        {   //根据指针的地址差值,可以计算出是第几个块,这里也用到指针强转int
            long index=(reinterpret_cast<long>(ptr)-reinterpret_cast<long>(m_Memory))/32;
            m_Free[index]=true;//把这个块是否未分配设置为true
            ret=true;   //我们不需要把这块内存清空,我们不保证分配出来的东西是空的,是调用者自己要清空
        }
        else if(m_pNextPage)//如果不在当前page,且还有下一个page就进这个if
        {
            ret=m_pNextPage->Free(ptr);//递归拿到后续的结果
            if(ret&&m_pNextPage->isempty())//如果确实被清空了,且下一页1024个都是空的,就回收整页
            {
                Page*oldPage=m_pNextPage;
                m_pNextPage=m_pNextPage->m_pNextPage;
                oldPage->m_pNextPage=nullptr;
                delete oldPage;
            }
        }
        return ret;
    }
    bool isempty() const
    {
        for(int ii=0;ii<1024;ii++)
        {
            if(m_Free[ii]==false)
            {
                return false;
            }
        }
        return true;
    }

};
class SmallBlockAllocator
{
private:
    Page page;
public:
    SmallBlockAllocator()=default;
    void* Alloc()
    {
        return page.alloc();
    }
    bool Free(void *ptr)
    {
        return page.Free(ptr);
    }
};

#include<cstdlib>
static SmallBlockAllocator Myallocator;
struct MemoryHeader//新增的头部,用来储存此次分配的信息
{
    const char* m_filename=nullptr;
    int m_line=0;
};
//new和delete其实都是函数签名,这里对其进行重载ps:不仅是类里面可以重载,外面也可以
//普通的new函数签名为void new(size_t size);
void* operator new(size_t size, const char* filename,int line) noexcept //承诺不抛出异常
{
    void* pMemstart=nullptr;
    if((size+sizeof(MemoryHeader))<=32)//修改这个new,当时小块的时候就是正常分配
    {
        pMemstart=Myallocator.Alloc();
    }
    else
    {
        pMemstart=malloc(size+sizeof(MemoryHeader));
    }
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
    if(Myallocator.Free(pHeader)==false)free(pHeader);//如果从分配器里回收失败就正常回收

}
void* operator new[](size_t size, const char* filename,int line) noexcept //承诺不抛出异常
{
    void* pMemstart=nullptr;
    if((size+sizeof(MemoryHeader))<=32)
    {
        pMemstart=Myallocator.Alloc();
    }
    else
    {
        pMemstart=malloc(size+sizeof(MemoryHeader));
    }
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
    if(Myallocator.Free(pHeader)==false)free(pHeader);//输出信息并且释放一开始的头部

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
        cout<<"Myclass operator new\n";//这边懒得重写了,这个类应该是比32大的,就正常分配即可
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