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

public:
	TBaseDelegateInstance() = default;
	virtual ~TBaseDelegateInstance() = default;

public:

	virtual InRetValType Execute(const ParamTypes&... args)
	{
		return InRetValType();
	}

	//默认, 不能接受引用类型，使用 std::decay_t 所以会拷贝
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

	//从头开始偏移参数, any_cast 会造成额外的一次拷贝
	template<size_t ArgsSize>
	decltype(auto) CastParamters_Any()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//得到已经填充了多少个参数
		return std::any_cast<TMakeTupleForwardOffset<index, ParamTypes...>>(paramters);
	}

	//从头开始偏移参数, 不会有额外拷贝
	template<size_t ArgsSize, size_t index = sizeof...(ParamTypes) - ArgsSize>
	constexpr const TMakeTupleForwardOffset<index, ParamTypes...>& CastParamters()
	{
		return reinterpret_cast<const TMakeTupleForwardOffset<index, ParamTypes...>&>(paramters);
	}

	//从尾开始偏移参数, any_cast 会造成额外的一次拷贝
	template<size_t ArgsSize>
	decltype(auto) BackCastParamters_Any()
	{
		return std::any_cast<TMakeTupleOffset<ArgsSize, ParamTypes...>>(paramters);
	}

	//从尾开始偏移参数, 不会有额外拷贝
	template<size_t ArgsSize>
	constexpr const TMakeTupleOffset<ArgsSize, ParamTypes...>& BackCastParamters()
	{
		return reinterpret_cast<const TMakeTupleOffset<ArgsSize, ParamTypes...>&>(paramters);
	}

	virtual bool IsVaild()
	{
		return true;
	}

	constexpr const std::any& GetParamters() { return paramters; }

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

public:
	//防止隐式转换，需要函数和参数类型一致
	constexpr explicit TStaticDelegateInstance(FuncTypePtr InFunc)
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

	InRetValType Execute(const ParamTypes&... args) override final
	{
		return (*Functor)(args...);
	}

private:

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

public:
	//防止隐式转换，需要函数和参数类型一致
	constexpr explicit TMemberFuncDelegateInstance(UserClass* InUserObject, FuncTypePtr InFunc)
		: Functor(InFunc)
		, UserObject(InUserObject)
	{
	}

public:
	bool IsVaild() override final
	{
		//无法确保指针是否有效，只能由使用者来判断，最好使用基于智能指针类型的对象
		return true;
	}

	InRetValType Execute(const ParamTypes&... args) override final
	{
		if (Functor == nullptr)
		{
			ERROR_LOG("TMemberFuncDelegateInstance Execute Is Error");
			return InRetValType();
		}

		return (UserObject->*Functor)(args...);
	}

private:

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
	using Super = TBaseDelegateInstance<InRetValType(ParamTypes...)>;
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);

public:
	//防止隐式转换，需要函数和参数类型一致
	constexpr explicit TLambdaDelegateInstance(const FunctorType& InFunctor)
		: Functor(InFunctor)
	{
	}

	constexpr explicit TLambdaDelegateInstance(FunctorType&& InFunctor)
		: Functor(std::move(InFunctor))
	{
	}

	bool IsVaild() override final
	{
		//Lambda 总是安全的可执行对象
		return true;
	}

	InRetValType Execute(const ParamTypes&... args) override final
	{
		return Functor(args...);
	}

private:

	//使用 mutable 主要是为了 Lambda 可以被绑定和执行
	//不想把 Functor 的方法直接给委托子对象
	//这样在常函数的情况下保持 const 的传递性，因为绑定不影响复制的可替换性
	mutable typename std::remove_const_t<FunctorType> Functor;
};


template<bool bConst, typename UserClass, typename FType>
class TSharePtrDelegateInstance;

template<bool bConst, typename UserClass, typename InRetValType, typename... ParamTypes>
class TSharePtrDelegateInstance<bConst, UserClass, InRetValType(ParamTypes...)> : public TBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TBaseDelegate;

public:
	using Super = TBaseDelegateInstance<InRetValType(ParamTypes...)>;
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using SharePtrType = std::shared_ptr<UserClass>;
	using WeakPtrType = std::weak_ptr<UserClass>;
	using FuncTypePtr = typename TMemberFuncPtr<bConst, UserClass, FuncType>::Type;

public:
	//防止隐式转换，需要函数和参数类型一致
	constexpr explicit TSharePtrDelegateInstance(const SharePtrType& InUserPtr, FuncTypePtr InFunc)
		: UserPtr(InUserPtr)
		, Functor(InFunc)
	{
	}

	bool IsVaild() override final
	{
		return UserPtr.lock() != nullptr;
	}

	InRetValType Execute(const ParamTypes&... args) override final
	{
		if (!IsVaild())
		{
			ERROR_LOG("TSharePtrDelegateInstance Execute Is Error");
			return InRetValType();
		}

		return (UserPtr.lock().get()->*Functor)(args...);
	}

private:

	WeakPtrType UserPtr;
	FuncTypePtr Functor;
};
