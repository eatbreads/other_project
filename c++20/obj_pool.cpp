#include<cstdlib>
#include<iostream>
#include<stack>
#include<thread>
#include<vector>
using namespace std;

class Obj
{
private:
    static const int MAX_OBJ_SIZE =4;//static const 是一个例外,可以直接类内初始化
    using Pool=vector<Obj>;          //内部的对象池,static保证独立于对象存在,只属于类的池
    static Pool pool;                //对象池,在new的时候被装入
    static stack<long> free_stack;    //存放空位置的下标,在delete里面被装入
    long value;                       //没啥用,就是一个值而已
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
        return ptr;
    }
    void operator delete(void *ptr)
    {//intptr_t其实就是long,下面这一行是delete的位置和起始位置差值,然后除以Obj大小就是数量了
        const intptr_t index=(static_cast<Obj*>(ptr)-pool.data())/
                             (static_cast<intptr_t>(sizeof(Obj)));
        if(index>=0&&index<pool.size())
        {
            //cout<<"成功入栈\n";
            free_stack.push(index);//把空位置下标注入
        }
        else//不在池子里面
        {
            free(ptr);
        }
    }

};
Obj::Pool Obj::pool{};//这边一次命名空间是拿到类型,然后再次命名空间拿到static成员
stack<long> Obj::free_stack{};

//增加线程,内容和main函数差不多
void thread_func()
{
    Obj *m5=new Obj(5);
    cout<<"m5的地址:"<<m5<<endl;
    Obj *m6=new Obj(6);
    cout<<"m6的地址:"<<m6<<endl;
    Obj *m7=new Obj(7);
    cout<<"m7的地址:"<<m7<<endl;
    delete m5;;
    m5=new Obj(8);
    cout<<"新的m5的地址:"<<m5<<endl;
    delete m5;
    delete m6;
    delete m7;

}
int main()                           //这里是正常使用这个类,没有使用多线程
{
    
    cout<<hex<<showbase;            //这样可以使得用16进制输出
    thread myThread(thread_func);       //加一个线程有一点问题
    thread myThread2(thread_func);      //加两个线程cout标准输出都会粘连在一起
    Obj *m1=new Obj(1);
    cout<<"m1的地址:"<<m1<<endl;
    Obj *m2=new Obj(2);
    cout<<"m2的地址:"<<m2<<endl;
    Obj *m3=new Obj(3);
    cout<<"m3的地址:"<<m3<<endl;
    delete m1;
    delete m2;
    delete m3;
    m1=new Obj(4);
    cout<<"新的m1的地址:"<<m1<<endl;
    delete m1;
    // delete m2;
    // delete m3;
    myThread.join();
    myThread2.join();
    return 0;
}

