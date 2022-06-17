#pragma once
#include "BaseDelegate.h"

/*
* 默认参数的函数不能用
* 后续要把传入的参数改为在 Excute 时传入，而不是在创建时，就绑定参数
* 如果赋值（绑定）新的函数，那么不会影响之前已经绑定的指针，因为是智能指针，其他保留者还存在引用计数，所以最好就使用弱指针
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

	//2种方法效果都一样
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
	//设置参数, 引用类型需要使用 <> 指定类型
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

	//参数无法转换为引用类型的模板
	template<typename... Args>
	InRetValType Excute(Args&&... args)
	{
		return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::make_tuple(std::forward<Args>(args)...)));
	}

	//可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	InRetValType ExcuteEx(const Args&... args)
	{
		return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::tuple<Args...>(args...)));
	}

	//参数置后再执行
	template<typename... Args>
	InRetValType ExcuteAfter(Args&&... args)
	{
		return this->ins->Execute(std::tuple_cat(std::make_tuple(std::forward<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>()));
	}

	//参数置后再执行, 可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	InRetValType ExcuteAfterEx(const Args&... args)
	{
		return this->ins->Execute((std::tuple_cat(std::tuple<Args...>(args...), this->ins->BackCastParamters<sizeof...(Args)>())));
	}
};

#define FUNC_DECLAER_DELEGATE( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

FUNC_DECLAER_DELEGATE(FSimpleDelegate, void);

