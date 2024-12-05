#include<iostream>
#include<cassert>
#include<future>
#include <functional>
#include<time.h>
#include <unistd.h>
using namespace std;

template<typename T>//使用模板,适合各种类型队列
class MessageQueue
{
private:
    //噢噢设计原理是外部源源不断的请求进来后,一直加锁解锁效率太差,就使用一个缓冲区(队列),一批一批的执行
    //这样加锁解锁只发生在缓冲区交换,消费缓冲区空了就把(接收)生产缓冲区换上来,自己变成生产缓冲区,以此往复
    using Queue= vector<T>;
    using Queueptr= typename Queue::iterator;//这里要加typename才能拿到内部using类型
    Queue m_A;
    Queue m_B;          //生产者和消费者的具体载体,但是使用指针操作(应该也可以new出来),获取元素是使用对应迭代器

    Queue* m_producer =&m_A;//这里的初始化顺序感觉可能有点疑惑,可能还是new出来好一点
    Queue* m_consumer =&m_B;//估计这样不写在构造函数里面的话就是按照声明顺序来,应该也没问题
    Queueptr m_consumerptr=m_B.end();//一开始消费缓冲区没东西,所以放在一个不存在的尾部,但是在swap之后他实际会指向begin
    condition_variable& m_MessageCondition;     //这个是引用,从外部传进来的,和外部配合的
    condition_variable m_ConsumtionFinished;    //通知别人我这边消费完了

    mutex m_producermutex;
    mutex m_consumermutex;

    int swapCount=0;    //交换次数
public:
    MessageQueue(condition_variable& MessageCondition)
    :m_MessageCondition(MessageCondition){}
    int getswapCount()
    {return swapCount;}

    void add(T && value)    //这里是右值模板,不是万能引用,不存在类型推断
    {
        unique_lock<mutex> lock(m_producermutex);
        m_producer->insert(m_producer->end(), std::move(value));//这里应该等价于push_back?
    }

    void BeginConsumer()
    {
        m_consumermutex.lock();
    }
    T Consumer()//这边采用了非阻塞的手法,没用条件变量来阻塞等待size>0这个事件成真
    {
        T value;
        if(m_consumer->size()>0)
        {
            value = *m_consumerptr; //这个ptr在swap后会在消费队头的
            m_consumerptr=m_consumer->erase(m_consumerptr);
            //erase操作返回下一个的迭代器(取代了的新的元素),因为迭代器会失效,需要重新定位
            assert(m_consumerptr==m_consumer->begin());//如果满足这个条件就继续执行,不满足就报错跳出
            //即等价于,我们希望每次获得和删除的都是第一个元素,防止没发生swap就开始消费,就会消费到end
        }
        return value;   //返回的是副本,能不能进行优化?
    }
    void EndConsumer()
    {
        assert(m_consumer->size() == 0); //就是结束的时候消费队列应该没东西了,即从队头一直把全部都吃完
        m_consumermutex.unlock();
        m_ConsumtionFinished.notify_all();//唤醒全部的等待swap的线程,就可以更换缓冲区了
    }
    int swap()//这个是阻塞的访问的,带锁进去,发现消费者里面还有东西,就让锁睡眠
    {   
        unique_lock<mutex> lockB(m_consumermutex);//当消费者没东西了(空了),才开始执行
        //理论上这个谓词应该肯定是true(不true在上面就会assert报错了),但是第一次进来可能是空的,或者被手动调用?
        m_ConsumtionFinished.wait(lockB, [this]{return m_consumer->size()==0;});    //噢噢如果一开始他就会直接检查谓词
        cout<<"swap 被唤醒"<<endl;                      //即第一次调用swap肯定成功
        unique_lock<mutex> lockA(m_producermutex);   //注意看这里有两份锁,swap的时候要同时对俩都上锁
        ::swap(m_producer, m_consumer);        //这里编译没过,swap要用全局的
        m_consumerptr=m_consumer->begin();   //指向头部
        m_MessageCondition.notify_all();    //这是和外部传进来的接口,告诉外部我已经发生交换了,等价于唤醒task
        swapCount++;
        return swapCount;
    }

};

class RunningTotal  //运行时的总和
{
private:
    int m_Value=0;            //根据业务设计逻辑值,可能是不断累加的状态或者啥
    bool m_Finished=false;        //结束标志
public:
    //一些操作符重载
    RunningTotal& operator+=(int value)
    {
        m_Value+=value;
        return *this;
    }
    RunningTotal& operator-=(int value)
    {
        m_Value-=value;
        return *this;
    }
    RunningTotal& Finish()  //只有执行了这个操作,才能合法的操作getValue
    {
        m_Finished=true;
        return *this;
    }
    int getValue()const //throw(int)  //这里表示throw的类型只能是int,好像c++17不给用动态异常
    {
        if(!m_Finished)//大部分都没结束,所以都会抛出异常
        {
            throw m_Value;  //不是合法的值,还没加上,就会在get的时候抛出异常int
        }
        return m_Value;
    }
};
using RunningOpeation=function<RunningTotal&()>;    //使用using快速创建函数类,这个函数返回的是引用
using Runningqueue=MessageQueue<RunningOpeation>;  //使用using+模板快速创建一个类,这个队列里面是函数

int Task(reference_wrapper<mutex> messMutex,    //外部锁
         reference_wrapper<condition_variable> messCondition,//外部条件变量,就是上文那个
         reference_wrapper<Runningqueue> messQueue)     //对应的双缓冲,快速创建的类
{
    int ret=0;
    Runningqueue &queue =messQueue.get();   //先拿到队列实例
    int currentSwap=0 ; //当前交换次数

    bool finished=false;
    while(!finished)        //就是说当他读到带finish的就会结束,但是没有读到,也不会有新增的
    {                       
        unique_lock<mutex>lock(messMutex.get());
        //用外部条件变量进入睡眠,然后持有外部锁看一下是否没有被处理,处理了currentSwap就会和缓冲区交换次数一样
        messCondition.get().wait(lock, [& queue,&currentSwap]()
                                        {return currentSwap!=queue.getswapCount();});//一开始是等于的所以会睡眠
        cout<<"task 被唤醒\n";
        queue.BeginConsumer();//要先进行这一步才能执行下一步,要先上消费者锁
        currentSwap=queue.getswapCount();//赋值,相当与处理了
        while(RunningOpeation func=queue.Consumer())    
        {   //好像这个while在执行的时候swap是可以操作的,while会突然感觉Consumer内置指针变了
            //RunningTotal& obj=queue.Consumer();     
            //RunningOpeation func=queue.Consumer();//消费一次,队列就出队一个元素,队列里面是函数对象
            RunningTotal& obj=func();   //这用引用接收,因为这个func函数对象也是引用
            try
            {
                ret = obj.getValue();   //拿到内部值,大部分会抛出异常
                finished = true;
                cout<<"finished"<<endl;
                break;          //如果totol还没有在外部执行Finish(),就不会执行这两行
            }
            catch(int param)
            {
                std::cout << "total 还没结束"<< '\n';
            }   
        }
        //sleep(5);    //表示处理数据至少要五秒,处理完就告诉swap可以换一批进来了,换进来了之后这里又会被唤醒
        //不抛出异常程序就结束了,然后这段代码逻辑是要程序员自己保证退出while的时候队列被处理完了
        queue.EndConsumer();//否则这里会唤醒swap,然后assert断言不通过
    }
    return ret;
}

int main()
{
    RunningTotal obj;
    mutex messmutex;
    condition_variable messCondition;
    Runningqueue messQueue(messCondition);
    //创建线程,设置为优先级较高,并且返回期值
    auto myfuture = async(launch::async,Task,ref(messmutex),ref(messCondition),ref(messQueue));
    //然后main函数充当生产者,不断地往里面塞东西,线程内部执行消费,线程一进去就会睡眠
    //这里add是要传入T对象,我们using已经提前设置好了<>,但是这里bind不是返回函数对象吗,他会自己调用来拿到返回值吗
    // messQueue.add(bind(&RunningTotal::operator+=,&obj,3));  //这里有一个成员函数指针和this指针的绑定
    // //messQueue.add(bind(&RunningTotal::Finish,&obj));
    // messQueue.swap();       //swap之后唤醒task
    // cout<<"swap:1"<<endl;            //所以说理论上只能绑定一次这个task只能处理一次
    messQueue.add(bind(&RunningTotal::operator+=,&obj,100));//队列里面存放的是函数
    //messQueue.add(bind(&RunningTotal::Finish,&obj));        //如果finish之后还插入了东西,就会导致线程退出的时侯消费者还有东西
    messQueue.add(bind(&RunningTotal::operator+=,&obj,1999));//导致断言不通过
    messQueue.add(bind(&RunningTotal::Finish,&obj));    
    messQueue.swap();               //task处理完之后又会唤醒这个swap,swap又会唤醒task,但是理论上这个swap执行不到,因为
    //cout<<"swap:2"<<endl;           //上面没有finish结尾就会导致Task在内层while出不来
    // messQueue.add(bind(&RunningTotal::operator+=,&obj,1999));
    // messQueue.add(bind(&RunningTotal::Finish,&obj));
    // cout<<"等待swap3...\n";
    // messQueue.swap();
    sleep(1);
    cout<<"最后拿到的值为"<<myfuture.get()<<endl;
    return 0;
}