#include<iostream>
#include"本地语言管理器.h"


using namespace Localization;
using namespace std;


int main()
{
    Manager localization("/project/c++20/sourcefile/languages.xml","xml");
    string color=localization.GetString(STRING_COLOR);
    cout<<"EN_US Localized string:"<<color<<endl;

    localization.SetLanguage(Languages::EN_GB);
    color=localization.GetString(STRING_COLOR);
    cout<<"EN_GB Localized string:"<<color<<endl;
    localization.SetLanguage(Languages::CN_ML);
    color=localization.GetString(1);//测试不存在的字符串
    cout<<"CN_ML Localized string:"<<color<<endl;
    return 0;
}