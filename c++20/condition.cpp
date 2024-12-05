#include<cstdlib>
#include<iostream>
#include<stack>
#include<thread>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<chrono>
#include<cassert>
#include<cstdio>
#include<functional>
using namespace std;

class Obj
{
private:
    static const int MAX_OBJ_SIZE =4;//static const 是一个例外,可以直接类内初始化
    using Pool=vector<Obj>;          //内部的对象池,static保证独立于对象存在,只属于类的池
    static Pool pool;                //对象池,在new的时候被装入
    static stack<long> free_stack;    //存放空位置的下标,在delete里面被装入
    long value;                       //没啥用,就是一个值而已
    static mutex m_mutex;            //创建互斥锁,声明为全局
public:
    Obj()=default;
    Obj(long value_):value(value_){};
    void *operator new(size_t size)
    {
        void* ptr=nullptr;
        if(pool.capacity()<MAX_OBJ_SIZE)//先扩容到最大
        {//reserve不保证一定是这么大,可能会更大,所以capacity第二次if可能就大于了
            pool.reserve(MAX_OBJ_SIZE);//这应该不会默认初始化,应该只是毛坯房
            //ps:resize是会装修的,就不是毛坯房了,size()也会是resize的值
        }
        //分配的size合理,且池里没满(size是实际东西,不是毛坯房)
        //m_mutex.lock();////////////////////////////////////////////////////////////////
        lock_guard<mutex> lock(m_mutex);
        if(size==sizeof(Obj)&&pool.size()<pool.capacity())
        {
            long index=0xFFFFFFFF;
            if(free_stack.empty()==false)      //表示有可用的对象
            {
                index=free_stack.top();
                free_stack.pop();
            }
            if(index==0xFFFFFFFF)              //表示没有空闲的
            {
                pool.push_back({});            //直接这样表示默认初始化,不然会解析成函数指针
                index=pool.size()-1;            //此时表示下标,在pool里的下标
            }
            ptr=pool.data()+index;            //data用来拿到容器的起始地址,string有单独的c_str()
            
        }                                     //且指针类型是容器内容的类型,不是类型,就像c_str()是char*
        else        //不合理或者池里满了
        {
            ptr=malloc(size);//直接分配过去
            //cout<<pool.size()<<endl;
        }
        m_mutex.unlock();////////////////////////////////////////////////////////////////
        return ptr;
    }
    void operator delete(void *ptr)
    {//intptr_t其实就是long,下面这一行是delete的位置和起始位置差值,然后除以Obj大小就是数量了
        const intptr_t index=(static_cast<Obj*>(ptr)-pool.data())/
                             (static_cast<intptr_t>(sizeof(Obj)));
        //m_mutex.lock();  //////////////////////////////////////////////////////
        lock_guard<mutex> lock(m_mutex);
        if(index>=0&&index<pool.size())
        {
            //cout<<"成功入栈\n";
            free_stack.push(index);//把空位置下标注入
        }
        else//不在池子里面
        {
            free(ptr);
        }
        //m_mutex.unlock();///////////////////////////////////////////////////////
    }
    long getvalue()
    {
        return value;
    }

};
Obj::Pool Obj::pool{};//这边一次命名空间是拿到类型,然后再次命名空间拿到static成员
stack<long> Obj::free_stack{};
mutex Obj::m_mutex{};

//增加线程,内容和main函数差不多

using producerQueue = vector<int>;

void ThreadTask(reference_wrapper<condition_variable>conditionRef,
                reference_wrapper<mutex>qmutexRef,
                reference_wrapper<producerQueue> producerRef,
                reference_wrapper<bool>dieRef)//这里全是函数参数
{//这里才是函数体,用reference这样打包一下更安全和易用,用get()可以从打包拿到参数
    producerQueue& pqueue=producerRef.get();    //记得使用引用接收,不然使用的就是副本了
    bool &die=dieRef.get(); 
    condition_variable& condition=conditionRef.get();
    mutex& qmutex=qmutexRef.get();
    while(!die||pqueue.size())
    {
        unique_lock<mutex> lock(qmutex);//在这里被加锁,然后在底下wait解锁进入睡眠
        function<bool()> predicate=[&pqueue]()
                                    {return !pqueue.empty();};
        condition.wait(lock,predicate);     //要同时被唤醒+满足谓词为true才能出来
                                            //而且一次只能出来一个,唤醒后会立刻申请加锁
        //所以流程就是:一起醒来,轮流加锁,看一看满足谓词了吗,不满足就解锁继续睡,满足就带锁出去
        //而且只要被唤醒了,就一定可以出去看一次满足谓词了吗,即使要等很久(排队看)
        int temp=pqueue.back();
        pqueue.pop_back();
        cout<<"消费了一份:"<<temp<<endl;
        for(int ii=0;ii<temp;ii++)
        {
            //开始处理内部的东西
            int i=ii;
        }
    }

}
int main()
{   //main函数用于当作生产者
    condition_variable condition;
    mutex qmutex;
    producerQueue pqueue;
    bool die=false;         //底下这个ref用于把参数传递给那个reference模板
    thread t1(ThreadTask,ref(condition),ref(qmutex),ref(pqueue),ref(die));
    thread t2(ThreadTask,ref(condition),ref(qmutex),ref(pqueue),ref(die));
    qmutex.lock();
    pqueue.push_back(10000);
    pqueue.push_back(10000);//生产两条数据
    qmutex.unlock();

    condition.notify_all();     //唤醒全部阻塞在condition.wait();
    this_thread::sleep_for(10ms);
    while(!qmutex.try_lock())
    {
        cout<<"还在处理...\n";
        this_thread::sleep_for(100ms);
    }
    pqueue.push_back(10000);
    
    die=true;
    condition.notify_one();
    this_thread::sleep_for(1000ms);
    pqueue.push_back(20000);
    condition.notify_one();
    this_thread::sleep_for(1000ms);
    qmutex.unlock();
    die=true;
    cout<<"================================"<<endl;//这个程序有点bug,不能正确结束
    t1.join();
    t2.join();

}