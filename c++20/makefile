



all:firstcpp test_language Vehicle template Singleton smartptr memory_leak high_timer test_memory memory_pool thread obj_pool obj_pool2 obj_pool3 condition promise mess_queue


firstcpp:firstcpp.cpp
	g++ -g -std=c++20 -o  firstcpp firstcpp.cpp 
	cp -f firstcpp ./bin/.
test_language:test_language.cpp 本地语言管理器.h 本地语言管理器.cpp /project/rapidxml/rapidxml-1.13/rapidxml.hpp
	g++ -g -std=c++20 -o test_language test_language.cpp 本地语言管理器.cpp -I/project/public /project/public/_public.cpp -I/project/rapidxml/rapidxml-1.13
本地语言管理器:本地语言管理器.cpp
	g++ -g -std=c++20 -o 本地语言管理器 本地语言管理器.cpp -I/project/public /project/public/_public.cpp
	cp -f 本地语言管理器 ./bin/.
Vehicle:Vehicle.cpp
	g++ -g -std=c++20 -o Vehicle Vehicle.cpp 
	cp -f Vehicle ./bin/.
template:template.cpp
	g++ -g -std=c++20 -o template template.cpp 
	cp -f Singleton ./bin/.
Singleton:Singleton.cpp
	g++ -g -std=c++20 -o Singleton Singleton.cpp 
	cp -f Singleton ./bin/.
smartptr:smartptr.cpp
	g++ -g -std=c++20 -o smartptr smartptr.cpp 
	cp -f smartptr ./bin/.
memory_leak:memory_leak.cpp
	g++ -g -std=c++20 -o memory_leak memory_leak.cpp 
	cp -f memory_leak ./bin/.
high_timer:high_timer.cpp
	g++ -g -std=c++20 -o high_timer high_timer.cpp 
	cp -f high_timer ./bin/.
test_memory:test_memory.cpp
	g++ -g -std=c++20 -o test_memory test_memory.cpp 
	cp -f test_memory ./bin/.
memory_pool:memory_pool.cpp
	g++ -g -std=c++20 -o memory_pool memory_pool.cpp 
	cp -f memory_pool ./bin/.
thread:thread.cpp
	g++ -g -std=c++20 -o thread thread.cpp 
	cp -f thread ./bin/.
obj_pool:obj_pool.cpp
	g++ -g -std=c++20 -o obj_pool obj_pool.cpp -pthread
	cp -f obj_pool ./bin/.
obj_pool2:obj_pool2.cpp
	g++ -g -std=c++20 -o obj_pool2 obj_pool2.cpp -pthread
	cp -f obj_pool2 ./bin/.
obj_pool3:obj_pool3.cpp
	g++ -g -std=c++20 -o obj_pool3 obj_pool3.cpp -pthread
	cp -f obj_pool3 ./bin/.
condition:condition.cpp
	g++ -g -std=c++20 -o condition condition.cpp -pthread
	cp -f condition ./bin/.
promise:promise.cpp
	g++ -g -std=c++20 -o promise promise.cpp -pthread
	cp -f promise ./bin/.
mess_queue:mess_queue.cpp
	g++ -g -std=c++20 -o mess_queue mess_queue.cpp -pthread
	cp -f mess_queue ./bin/.
lua_test:lua_test.cpp 
	g++ -g -std=c++20 -o lua_test lua_test.cpp -I/lua/sol2/include/sol -I/lua/sol2/include /lua/sol2/include/sol/sol.hpp
	cp -f lua_test ./bin/.
clean:
	rm -f firstcpp