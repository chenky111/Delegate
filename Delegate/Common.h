#pragma once
#include "ColorDebug.h"
#include "TupleUtil.h"
#include <tuple>
#include <iostream>

using BaseFuncType = void(*)(void);

//自定义的类型（用于把分散的模板参数，合并为一个类型）
template<typename T>
struct TDefineType
{
	typedef T Type;
};

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



