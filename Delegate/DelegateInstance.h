#pragma once
#include "Common.h"

template<typename FuncType>
class TBaseDelegateInstance;

template<typename InRetValType, typename... ParamTypes>
class TBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TBaseDelegate;

public:
	using FuncType = InRetValType(ParamTypes...);
	//using ExcuteTuple = typename TMakeTupleOffsetUtil<FuncType, ParamTypes...>::type;

public:
	TBaseDelegateInstance() = default;
	virtual ~TBaseDelegateInstance() = default;

public:

	virtual InRetValType Execute(std::tuple<ParamTypes...>&& argsTuple)
	{
		return InRetValType();
	}

	//默认, 不能接受引用类型
	template<typename... Args>
	void setParamtersDefault(Args&&... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		paramters = std::make_tuple(std::forward<Args>(args)...);
	}

	//可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	void setParamters(Args... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		paramters = std::tuple<Args...>(args...);
	}

	//从头开始偏移参数
	template<size_t ArgsSize>
	decltype(auto) CastParamters()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//得到已经填充了多少个参数
		return std::any_cast<TMakeTupleForwardOffset<index, ParamTypes...>>(paramters);
	}

	//从尾开始偏移参数
	template<size_t ArgsSize>
	decltype(auto) BackCastParamters()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//得到已经填充了多少个参数
		return std::any_cast<TMakeTupleOffset<index, ParamTypes...>>(paramters);
	}

	virtual bool IsVaild()
	{
		return true;
	}

protected:
	std::any paramters;
};


template<typename FuncType>
class TStaticDelegateInstance;

template<typename InRetValType, typename... ParamTypes>
class TStaticDelegateInstance<InRetValType(ParamTypes...)> : public TBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TBaseDelegate;

public:
	using Super = TBaseDelegateInstance<InRetValType(ParamTypes...)>;
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using FuncTypePtr = InRetValType(*)(ParamTypes...);
	using TupleSequence = std::index_sequence_for<ParamTypes...>;

public:
	//防止隐式转换，需要函数和参数类型一致
	explicit TStaticDelegateInstance(FuncTypePtr InFunc)
		: Super()
		, Functor(InFunc)
	{
	}

public:
	bool IsVaild() override final
	{
		//静态函数一直都为 true
		return true;
	}

	InRetValType Execute(std::tuple<ParamTypes...>&& argsTuple) override final
	{
		return std::apply(Functor, std::move(argsTuple));
		//return _CallFunc(TupleSequence{});
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(std::index_sequence<Index...>) const
	{
		return (*Functor)(std::get<Index>(this->paramters)...);
	}

	FuncTypePtr Functor;
};


template<bool bConst, typename UserClass, typename FuncType>
class TMemberFuncDelegateInstance;

template<bool bConst, typename UserClass, typename InRetValType, typename... ParamTypes>
class TMemberFuncDelegateInstance<bConst, UserClass, InRetValType(ParamTypes...)> : public TBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TBaseDelegate;

public:
	using Super = TBaseDelegateInstance<InRetValType(ParamTypes...)>;
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using FuncTypePtr = typename TMemberFuncPtr<bConst, UserClass, FuncType>::Type;
	using TupleSequence = std::index_sequence_for<ParamTypes...>;

	//防止隐式转换，需要函数和参数类型一致
	explicit TMemberFuncDelegateInstance(UserClass* InUserObject, FuncTypePtr InFunc)
		: Functor(InFunc)
		, UserObject(InUserObject)
	{
		//assert(InUserObject != nullptr && InFunc != nullptr, "InUserObject or InFunc is Null");
	}

public:
	bool IsVaild() override final
	{
		//无法确保指针是否有效，只能由使用者来判断，最好使用基于智能指针类型的对象
		return true;
	}

	InRetValType Execute(std::tuple<ParamTypes...>&& argsTuple) override final
	{
		if (Functor == nullptr)
		{
			ERROR_LOG("TMemberFuncDelegateInstance Execute Is Error");
			return InRetValType();
		}

		return _CallFunc(argsTuple, TupleSequence{});
	}

private:
	template<typename TupIns, size_t... Index>
	InRetValType _CallFunc(TupIns&& tupIns, std::index_sequence<Index...>) const
	{
		//return (UserObject->*Functor)(std::get<Index>(tupIns)...);
		return std::invoke(Functor, UserObject, std::get<Index>(tupIns)...);
	}

	UserClass* UserObject;
	FuncTypePtr Functor;
};


template<typename FType, typename FunctorType>
class TLambdaDelegateInstance;

template<typename InRetValType, typename... ParamTypes, typename FunctorType>
class TLambdaDelegateInstance<InRetValType(ParamTypes...), FunctorType> : public TBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TBaseDelegate;

public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using TupleType = std::tuple<ParamTypes...>;
	using TupleSequence = std::index_sequence_for<ParamTypes...>;

	//防止隐式转换，需要函数和参数类型一致
	explicit TLambdaDelegateInstance(const FunctorType& InFunctor, ParamTypes... Params)
		: Functor(InFunctor)
	{
	}

	explicit TLambdaDelegateInstance(FunctorType&& InFunctor, ParamTypes... Params)
		: Functor(InFunctor)
	{
	}

	bool IsVaild() override final
	{
		//Lambda 总是安全的可执行对象
		return true;
	}

	InRetValType Execute(std::tuple<ParamTypes...>&& argsTuple) override final
	{
		return _CallFunc(argsTuple, TupleSequence{});
	}

private:
	template<typename TupIns, size_t... Index>
	InRetValType _CallFunc(TupIns&& tupIns, std::index_sequence<Index...>) const
	{
		return Functor(std::get<Index>(tupIns)...);
	}

	//使用 mutable 主要是为了 Lambda 可以被绑定和执行
	//不想把 Functor 的方法直接给委托子对象
	//这样在常函数的情况下保持 const 的传递性，因为绑定不影响复制的可替换性
	mutable typename std::remove_const_t<FunctorType> Functor;
};
