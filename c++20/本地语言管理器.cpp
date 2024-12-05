#include "本地语言管理器.h"

using namespace idc;
using namespace rapidxml;
namespace Localization
{
    
    Manager::Manager(const char*filename,const char * format)
    {
        //这两行感觉纯粹是为了增强可读性
        //static const uint32_t INDEX_EN_US=static_cast<uint32_t>(Languages::EN_US);
        //static const uint32_t INDEX_EN_GB=static_cast<uint32_t>(Languages::EN_GB);

        // m_stringPacks[INDEX_EN_US][STRING_COLOR]= "COLOR";
        // m_stringPacks[INDEX_EN_GB][STRING_COLOR]= "COLOUR";//英美的color不一样
        SetLanguage(Languages::EN_US);                     //默认设置为美式英语
        

        //打开文件
        cifile ifile;

        ifile.open(filename);
        string line;                                                        //0,color,colour,颜色
        ccmdstr cmdstr;                                                     //1,flavor,flavour,风味       
        int32_t INDEXWORD;
        char word_arr[3][100];
        if(strcmp(format,"csv")==0)
        {
            while(ifile.readline(line))
            {
                cmdstr.splittocmd(line,",");
                memset(word_arr, 0, sizeof(word_arr));
                cmdstr.getvalue(0,INDEXWORD);
                cmdstr.getvalue(1,word_arr[0],30);          //这里填30就好了,虽然上面开的是100,他会自己补/0
                cmdstr.getvalue(2,word_arr[1],30);
                cmdstr.getvalue(3,word_arr[2],30);
                for(int ii=0;ii<static_cast<uint32_t>(Languages::Number);ii++)
                {
                    //string temp=(word_arr[ii]);
                    m_stringPacks[static_cast<uint32_t>(ii)][static_cast<uint32_t>(INDEXWORD)]= word_arr[ii];
                }
            }
        }
        else if(strcmp(format,"xml")==0)
        {
            int  onread=0;         // 每次打算从文件中读取的字节数。 
            char buffer[1000];   // 存放读取数据的buffer，buffer的大小可参考硬盘一次读取数据量（4K为宜）。
            int  totalbytes=0;    // 从文件中已读取的字节总数。

            int totalsize=filesize(filename);
            string allbuffer;

            while (true)
            {
                memset(buffer,0,sizeof(buffer));

                // 计算本次应该读取的字节数，如果剩余的数据超过1000字节，就读1000字节。
                if (totalsize-totalbytes>1000) onread=1000;
                else onread=totalsize-totalbytes;

                // 从文件中读取数据。
                ifile.read(buffer,onread);   
                allbuffer+=buffer;      //加入总的缓冲区

                // 计算文件已读取的字节总数，如果文件已读完，跳出循环。
                totalbytes=totalbytes+onread;

                if (totalbytes==totalsize) break;
            }
            // cout<<allbuffer;
            char* buffer_=new char[allbuffer.length()+1];
            strcpy(buffer_,allbuffer.c_str());
            //xml文档对象,内部有一个解析方法,parse<0>的0表示解析器上的标志,此时就可以访问buffer内部的节点
            xml_document<>document; 
            //这几行用来初始化xml数据结构,parse只能传入char*,不能是const char*,可能是内部要修改            
            document.parse<0>(buffer_);  
            //获取节点内容,xml_node就是基础节点,用first_node就可以找到对应的节点
            //这里都是递归查进去的,当前节点作为下一节点的查询接口       
            xml_node<>* workbook = document.first_node("Workbook");
            if(workbook !=nullptr) //如果存在这个workbook就继续找
            {
                xml_node<>* worksheet = workbook->first_node("Worksheet");
                if(worksheet !=nullptr)
                {
                    xml_node<>* table=worksheet->first_node("Table");
                    if(table !=nullptr)
                    {
                        xml_node<>* row=table->first_node("Row");
                        while(row !=nullptr)//循环直到把Row找完,这个循环遍历完所有的种类(行)
                        {
                            uint32_t stringID=UINT32_MAX;
                            xml_node<>* cell=row->first_node("Cell");//row内部是cell的小单元
                            if(cell !=nullptr)//第一层的cell里面的data是id
                            {
                                xml_node<>*data=cell->first_node("Data");
                                if(data!=nullptr)
                                {
                                //data才是最终的最小的数据单元,此时value就可以拿到最终的值
                                stringID=static_cast<uint32_t>(atoi(data->value()));
                                }   
                            }
                            if(stringID !=UINT32_MAX)//说明id被成功赋值了
                            {
                                uint32_t languageIndex=0;
                                cell=cell->next_sibling("Cell");//cell去拿下一行的cell,指针移动
                                while(cell!=nullptr)
                                {
                                    xml_node<>*data=cell->first_node("Data");
                                    if(data!=nullptr)//后面的data都是那一种单词,这个循环遍历单个单词(遍历列)
                                    {
                                        //这个字段是自增,表示遍历多国,对每一个的stringid(color)赋值
                                        m_stringPacks[languageIndex++][stringID]=data->value();
                                    }
                    
                                    cell=cell->next_sibling("Cell");
                                }
                            }
                            row=row->next_sibling("Row");//row指针移动到下一行,去遍历flavor去了
                        }
                    }
                }
            }

        }
    }
    
    void Manager::SetLanguage(Languages language)        //使用什么语言就设置哪一页
    {
        //这里索引必须转化成数值,stl规定的,class的enum不允许类型转换
        m_CurrentStringPack =&(m_stringPacks[static_cast<int>(language)]);
    }
    std::string Manager::GetString(StringID stringID)const//拿到字符串
    {
        if(m_CurrentStringPack->count(stringID)!=0)//查找是否在哈希表里面
        return (*m_CurrentStringPack)[stringID];    //记得解引用在调用[]
        return std::string("!!!")+std::to_string(stringID)+std::string("!!!");//没找到返回!
    }
}
