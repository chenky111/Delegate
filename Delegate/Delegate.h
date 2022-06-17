#pragma once
#include "BaseDelegate.h"

/*
* Ĭ�ϲ����ĺ���������
* ����Ҫ�Ѵ���Ĳ�����Ϊ�� Excute ʱ���룬�������ڴ���ʱ���Ͱ󶨲���
* �����ֵ���󶨣��µĺ�������ô����Ӱ��֮ǰ�Ѿ��󶨵�ָ�룬��Ϊ������ָ�룬���������߻��������ü�����������þ�ʹ����ָ��
*/
template<typename FuncType>
class TDelegate;

template<typename InRetValType, typename... ParamTypes>
class TDelegate<InRetValType(ParamTypes...)> : public TBaseDelegate<InRetValType(ParamTypes...)>
{
public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using Super = TBaseDelegate<FuncType>;

public:
	using Super::Super;

public:
	//template<typename... VarTypes>
	//static TDelegate CreateBase(FuncType func, VarTypes&&... Vars)
	//{
	//	TDelegate result;
	//	result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(func, std::forward<VarTypes>(Vars)...);
	//	return result;
	//}

	//2�ַ���Ч����һ��
#if 0
	template<typename... Args>
	static TDelegate CreateBase(FuncType InFunc, Args&&... args)
	{
		TNewDelegate result;
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}
#else
	template<typename... Args>
	static TDelegate CreateBase(typename TDefineType<InRetValType(*)(ParamTypes...)>::Type InFunc, Args&&... args)
	{
		return TDelegate();
		//TDelegate result;
		//result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc);
		//result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		//return result;
	}
#endif

	template<typename... Args>
	static TDelegate CreateStatic(typename TDefineType<InRetValType(*)(ParamTypes...)>::Type InFunc, Args&&... args)
	{
		TDelegate result;
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template <typename UserClass, typename... Args>
	static TDelegate CreateMemberFunc(UserClass* UserObject, typename TMemberFuncPtr<false, UserClass, FuncType>::Type InFunc, Args&&... args)
	{
		TDelegate result;
		result.ins = std::make_shared<TMemberFuncDelegateInstance<false, UserClass, FuncType>>(UserObject, InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template <typename UserClass, typename... Args>
	static TDelegate CreateMemberFunc(UserClass* UserObject, typename TMemberFuncPtr<true, UserClass, FuncType>::Type InFunc, Args&&... args)
	{
		TDelegate result;
		result.ins = std::make_shared<TMemberFuncDelegateInstance<true, UserClass, FuncType>>(UserObject, InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template<typename TFunc, typename... Args>
	static void CreateAndCallLambda_Test(TFunc&& InFunc, Args&&... args)
	{
		auto f = TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>(std::forward<TFunc>(InFunc));
		f->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		f.Execute();
	}

	template<typename TFunc, typename... Args>
	static auto CreateLambda_Test(TFunc&& InFunc, Args&&... args)
		-> decltype(TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>(std::forward<TFunc>(InFunc)))
	{
		auto f = TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>(std::forward<TFunc>(InFunc));
		f->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return f;
		//return TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>(std::forward<TFunc>(InFunc), args...);
	}

	template<typename TFunc, typename... Args>
	static TDelegate CreateLambda(TFunc&& InFunc, Args&&... args)
	{
		TDelegate result;
		result.ins = std::make_shared<TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>>(std::forward<TFunc>(InFunc));
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

public:
	//���ò���, ����������Ҫʹ�� <> ָ������
	template<typename... Args>
	void setParamters(const Args&... args)
	{
		this->ins->setParamters<Args...>(args...);
	}

	InRetValType ExcuteIfSave()
	{
		if (this->ins.use_count() > 0 && this->ins->IsVaild())
			return this->ins->Execute();

		return InRetValType();
	}

	//�����޷�ת��Ϊ�������͵�ģ��
	template<typename... Args>
	InRetValType Excute(Args&&... args)
	{
		return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::make_tuple(std::forward<Args>(args)...)));
	}

	//���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	InRetValType ExcuteEx(const Args&... args)
	{
		return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::tuple<Args...>(args...)));
	}

	//�����ú���ִ��
	template<typename... Args>
	InRetValType ExcuteAfter(Args&&... args)
	{
		return this->ins->Execute(std::tuple_cat(std::make_tuple(std::forward<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>()));
	}

	//�����ú���ִ��, ���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	InRetValType ExcuteAfterEx(const Args&... args)
	{
		return this->ins->Execute((std::tuple_cat(std::tuple<Args...>(args...), this->ins->BackCastParamters<sizeof...(Args)>())));
	}
};

#define FUNC_DECLAER_DELEGATE( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

FUNC_DECLAER_DELEGATE(FSimpleDelegate, void);

