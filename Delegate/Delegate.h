#pragma once
#include "BaseDelegate.h"

/*
* 如果用参数全是引用类型, 效率不会和原生 std::bind 差很多, 调用时多1次拷贝
* 优点: 提前绑定的参数, 可以随时替换掉, 还可以前后调用
* 注意: 使用引用时, 需要确保引用的对象是否有效, 还是建议使用智能指针
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
		TDelegate result;
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
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
		//移除 const
		//例如使用 void(const int, cosnt int&) -> void(int, const int&) 时
		//这样就能够匹配 any_cast，且不影响参数传递使用
		
		//this->ins->setParamters<Args...>(args...);
		this->ins->setParamters<std::remove_cv_t<Args>...>(args...);
	}

	constexpr bool IsSave()
	{
		return this->ins.use_count() > 0 && this->ins->IsVaild();
	}

	InRetValType ExcuteIfSave()
	{
		if (IsSave())
			return this->ins->Execute();

		return InRetValType();
	}

#ifndef ExecutTest
	//执行函数, 注意:参数无法转换成引用类型
	template<typename... Args>
	constexpr InRetValType Excute(Args&&... args)
	{
		if (IsSave())
			return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::make_tuple(std::forward<Args>(args)...)));

		return InRetValType();
	}

	//可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	constexpr InRetValType ExcuteEx(Args... args)
	{
		if(IsSave())
			return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::tuple<Args...>(static_cast<Args>(args)...)));

		return InRetValType();
	}

	//参数置后再执行
	template<typename... Args>
	constexpr InRetValType ExcuteAfter(Args&&... args)
	{
		if (IsSave())
			return this->ins->Execute(std::tuple_cat(std::make_tuple(std::forward<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>()));

		return InRetValType();
	}

	//参数置后再执行, 可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	constexpr InRetValType ExcuteAfterEx(Args... args)
	{
		if (IsSave())
			return this->ins->Execute((std::tuple_cat(std::tuple<Args...>(static_cast<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>())));

		return InRetValType();
	}
#else
	//执行函数, 注意:参数无法转换成引用类型
	template<typename... Args>
	constexpr InRetValType Excute(Args&&... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _Excute(TupleSequence{}, this->ins->CastParamters<sizeof...(Args)>(), std::forward<Args>(args)...);
	}

	//可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	constexpr InRetValType ExcuteEx(Args... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _ExcuteEx(TupleSequence{}, this->ins->CastParamters<sizeof...(Args)>(), static_cast<Args>(args)...);
	}

	//参数置后再执行
	template<typename... Args>
	constexpr InRetValType ExcuteAfter(Args&&... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _ExcuteBack(TupleSequence{}, this->ins->CastParamters<sizeof...(Args)>(), std::forward<Args>(args)...);
	}

	//参数置后再执行, 可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	constexpr InRetValType ExcuteAfterEx(Args... args)
	{
		return ExcuteAfter(static_cast<Args>(args)...);
	}

	template<size_t... Index, typename... TupleArgs, typename... Args>
	constexpr InRetValType _Excute(std::index_sequence<Index...>, const std::tuple<TupleArgs...>& t, Args&&... args)
	{
		if (IsSave())
			return this->ins->Execute(std::get<Index>(t)..., std::forward<Args>(args)...);

		return InRetValType();
	}

	template<size_t... Index, typename... TupleArgs, typename... Args>
	constexpr InRetValType _ExcuteEx(std::index_sequence<Index...>, const std::tuple<TupleArgs...>& t, Args... args)
	{
		PrintType<void(Args...)>();
		if (IsSave())
			return this->ins->ExecuteEx(std::get<Index>(t)..., args...);

		return InRetValType();
	}

	template<size_t... Index, typename... TupleArgs, typename... Args>
	constexpr InRetValType _ExcuteBack(std::index_sequence<Index...>, const std::tuple<TupleArgs...>& t, Args&&... args)
	{
		if (IsSave())
			return this->ins->Execute(std::forward<Args>(args)..., std::get<Index>(t)...);

		return InRetValType();
	}
#endif
};

#define FUNC_DECLAER_DELEGATE( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

FUNC_DECLAER_DELEGATE(FSimpleDelegate, void);

