#include<iostream>
#include"sol.hpp"

using namespace std;

int main()
{
    sol::state state;
    state.open_libraries(sol::lib::base,sol::lib::package);
    state.do_string("print(\'Hello World!\')");
                    state.script("print('printing from a script now!')");
                    state.~state();
                    return 0;
}