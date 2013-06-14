#ifndef __OOG_UTIL_DEBUGTYPE_H__
#define __OOG_UTIL_DEBUGTYPE_H__
#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <cxxabi.h>

#define DEBUGVAL(x) do { std::cout << #x << " => " << x << std::endl; } while(0)
#define DEBUGVAL_BOOL(x) do { std::cout << #x << " => " << std::boolalpha << x << std::endl; } while(0)
#define DEBUG_TYPE2(x) do { typedef void(*T)x; debug_type(T(), #x); } while(0)
#define DEBUG_TYPE(x) do { typedef void(*T)x; debug_type2::print<T>(#x); } while(0)

struct debug_type
{
    template<typename U>
    debug_type(void(*)(U), const std::string& p_str)
    {
        std::string str(p_str.begin() + 1, p_str.end() - 1);
        std::cout << str << " => ";
        std::string name;
        int status;
        name = abi::__cxa_demangle(typeid(U).name(), 0, 0, &status);
        std::cout << name << std::endl;
    }
};

struct debug_type2
{
    template<typename U>
    static void print(const std::string& p_str)
    {
        std::string str(p_str.begin() + 1, p_str.end() - 1);
        std::cout << str << " => ";
        char* name;
        int status;
        name = abi::__cxa_demangle(typeid(U).name(), 0, 0, &status);
        std::string stripName2 = name;
        free(name);
        std::string stripName(stripName2.begin() + 9, stripName2.end() - 1);
        std::cout << stripName << std::endl;
    }
};
#endif
