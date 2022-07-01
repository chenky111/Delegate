#pragma once
#include "ColorDebug.h"
#include "TupleUtil.h"
#include <any>
#include <tuple>
#include <iostream>
#include <stdexcept>
#include <chrono>

#define ExecutTest 1

using BaseFuncType = void(*)(void);

//自定义的类型（用于把分散的模板参数，合并为一个类型）
template<typename T>
struct TDefineType
{
	//typedef T Type;
	using type = T;
};

template<typename T>
struct remove_r_value
{
	using type = T;
};

template<typename T>
struct remove_r_value<T&>
{
	using type = T&;
};

template<typename T>
struct remove_r_value<T&&>
{
	using type = T;
};

//移除右值引用
template<typename T>
using remove_r_value_t = typename remove_r_value<T>::type;

template<typename T>
using remove_cvr_t = remove_r_value_t<std::remove_cv_t<T>>;


//成员函数指针模板类型
//前置声明（第一个模板参数，用来判断是否为常函数）
template<bool bConst, typename T, typename F>
class TMemberFuncPtr;

template<typename UserClass, typename RetValue, typename... Args>
class TMemberFuncPtr<false, UserClass, RetValue(Args...)> //模板特例化
{
public:
	using Type = RetValue(UserClass::*)(Args...);
	using ReturnType = RetValue;
};

template<typename UserClass, typename RetValue, typename... Args>
class TMemberFuncPtr<true, UserClass, RetValue(Args...)> //模板特例化
{
public:
	using Type = RetValue(UserClass::*)(Args...) const;
	using ReturnType = RetValue;
};

template<typename... Args>
void PrintType(Args&&... args)
{
	int i = 0;
	(ColorDebug(enmCFC_Blue, enmCBC_Black, __GET_FILE_LINE(), 
		(++i, std::string("Type -> ") + std::string(typeid(std::forward<Args>(args)).name()))), ...);
}

template<typename T>
void PrintType()
{
	ColorDebug(enmCFC_Blue, enmCBC_Black, __GET_FILE_LINE(), typeid(T).name());
}

//代码运行时间测试，建议使用 {} 测试
class RunTimeTest {
public:
	RunTimeTest() : start(clock()) {}
	~RunTimeTest() { auto t = clock() - start; WARNING_LOG("runTime:", t); }

private:
	clock_t start;
};


