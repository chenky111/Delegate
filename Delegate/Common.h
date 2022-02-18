#pragma once
#include "ColorDebug.h"
#include "TupleUtil.h"
#include <tuple>
#include <iostream>

using BaseFuncType = void(*)(void);

//�Զ�������ͣ����ڰѷ�ɢ��ģ��������ϲ�Ϊһ�����ͣ�
template<typename T>
struct TDefineType
{
	typedef T Type;
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



