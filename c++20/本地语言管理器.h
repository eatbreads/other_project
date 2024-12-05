#pragma once
#include<array>
#include<cinttypes>
#include<string>
#include<unordered_map>
#include"_public.h"
#include"rapidxml.hpp"

namespace Localization
{
    using StringID = int32_t;
    
    enum class Languages
    {
        EN_US,              //美式英语
        EN_GB,              //英式英语 
        CN_ML, //           //中国
        Number              //按顺序增加所以Number就会是总的语言数量
    };
    const StringID STRING_COLOR=0; //把颜色字符串的id初始化为0,且设置为const无法修改

    class Manager
    {
    private:
        //使用using这样操作,相当于创造了简易的类
        using Strings =std::unordered_map<StringID,std::string>;    //用哈希来实现数组的功能,相当于二维char数组  
        //二维char数组再套一层,类似于很多页,每一页都是一个语言,页数就是enum的Number      
        using StringPacks =std::array<Strings,static_cast<size_t>(Languages::Number)>;

        StringPacks m_stringPacks;
        Strings *m_CurrentStringPack=nullptr;       //页的索引
        uint32_t m_LanguageIndex;                   //行的索引
    public:
        Manager(const char* filename="/project/c++20/sourcefile/languages.csv",const char * format="csv");
        void SetLanguage(Languages language);        //使用什么语言就设置哪一页
        std::string GetString(StringID stringID)const;//拿到字符串

    };
}