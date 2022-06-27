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

//�Զ�������ͣ����ڰѷ�ɢ��ģ��������ϲ�Ϊһ�����ͣ�
template<typename T>
struct TDefineType
{
	//typedef T Type;
	using Type = T;
};

//��Ա����ָ��ģ������
//ǰ����������һ��ģ������������ж��Ƿ�Ϊ��������
template<bool bConst, typename T, typename F>
class TMemberFuncPtr;

template<typename UserClass, typename RetValue, typename... Args>
class TMemberFuncPtr<false, UserClass, RetValue(Args...)> //ģ��������
{
public:
	using Type = RetValue(UserClass::*)(Args...);
	using ReturnType = RetValue;
};

template<typename UserClass, typename RetValue, typename... Args>
class TMemberFuncPtr<true, UserClass, RetValue(Args...)> //ģ��������
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

//��������ʱ����ԣ�����ʹ�� {} ����
class RunTimeTest {
public:
	RunTimeTest() : start(clock()) {}
	~RunTimeTest() { auto t = clock() - start; WARNING_LOG("runTime:", t); }

private:
	clock_t start;
};


