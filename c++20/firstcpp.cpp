#include<iostream>
#include<string>

int main(void)
{
    using namespace std::string_literals;
    auto output="Hello world!"s;
    std::cout<< output<<std::endl;
    return 0;
}