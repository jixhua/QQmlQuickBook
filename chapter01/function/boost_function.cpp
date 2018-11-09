﻿#include <boost/function.hpp>
#include <iostream>
#if __has_include(<memory_resource>)
#include <memory_resource>
#else
#include <memory>
#endif
#include <boost/thread.hpp>
#include <future>

void test_boost_function(){

    boost::function<void(void)> hellowWorld([](){
        std::cout << "Hellow World!" <<std::endl;
    }, std::allocator<int>{});
    hellowWorld();

}
















