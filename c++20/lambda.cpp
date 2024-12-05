#include<algorithm>
#include<array>
#include<cstdint>
#include<functional>
#include<iostream>
#include<typeinfo>
#include<vector>
using namespace std;

using Myarray=std::array<uint64_t,5>;
using Myvector=std::vector<Myarray::value_type>;


void PrintArray(const function<void (Myarray::value_type)>& myfunction)
{
    Myarray myarray{1,2,3,4,5};
    for_each(myarray.begin(),myarray.end(),myfunction);
}

int main()
{
    Myvector mycopy;
    auto myclosure=[&mycopy](auto&& value){mycopy.push_back(value);};
    PrintArray(myclosure);

    
    return 0;
}