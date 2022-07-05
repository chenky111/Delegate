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

	virtual InRetValType execute(const ParamTypes&... args)
	{
		return InRetValType();
	}

	//默认, 不能接受引用类型，使用 std::decay_t 所以会拷贝
	template<typename... Args>
	void setParamtersDefault(Args&&... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		_paramters = std::make_tuple(std::forward<Args>(args)...);
	}

	//可以接受引用类型, 需要使用 <> 指定类型
	template<typename... Args>
	void setParamters(Args... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		_paramters = std::tuple<Args...>(args...);
	}

	//从头开始偏移参数, any_cast 会造成额外的一次拷贝
	template<size_t ArgsSize>
	decltype(auto) castParamters_Any()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//得到已经填充了多少个参数
		return std::any_cast<TMakeTupleForwardOffset<index, ParamTypes...>>(_paramters);
	}

	//从头开始偏移参数, 不会有额外拷贝
	template<size_t ArgsSize, size_t index = sizeof...(ParamTypes) - ArgsSize>
	constexpr const TMakeTupleForwardOffset<index, ParamTypes...>& castParamters()
	{
		return reinterpret_cast<const TMakeTupleForwardOffset<index, ParamTypes...>&>(_paramters);
	}

	//从尾开始偏移参数, any_cast 会造成额外的一次拷贝
	template<size_t ArgsSize>
	decltype(auto) backCastParamters_Any()
	{
		return std::any_cast<TMakeTupleOffset<ArgsSize, ParamTypes...>>(_paramters);
	}

	//从尾开始偏移参数, 不会有额外拷贝
	template<size_t ArgsSize>
	constexpr const TMakeTupleOffset<ArgsSize, ParamTypes...>& backCastParamters()
	{
		return reinterpret_cast<const TMakeTupleOffset<ArgsSize, ParamTypes...>&>(_paramters);
	}

	virtual bool isVaild()
	{
		return true;
	}

	constexpr const std::any& getParamters() { return _paramters; }

protected:
	std::any _paramters;
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
	constexpr explicit TStaticDelegateInstance(FuncTypePtr inFunc)
		: Super()
		, _functor(inFunc)
	{
	}

public:
	bool isVaild() override final
	{
		//静态函数一直都为 true
		return true;
	}

	InRetValType execute(const ParamTypes&... args) override final
	{
		return (*_functor)(args...);
	}

private:

	FuncTypePtr _functor;
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
	constexpr explicit TMemberFuncDelegateInstance(UserClass* inUserObject, FuncTypePtr inFunc)
		: _functor(inFunc)
		, _userObject(inUserObject)
	{
	}

public:
	bool isVaild() override final
	{
		//无法确保指针是否有效，只能由使用者来判断，最好使用基于智能指针类型的对象
		return true;
	}

	InRetValType execute(const ParamTypes&... args) override final
	{
		if (_functor == nullptr)
		{
			ERROR_LOG("TMemberFuncDelegateInstance execute Is Error");
			return InRetValType();
		}

		return (_userObject->*_functor)(args...);
	}

private:

	UserClass* _userObject;
	FuncTypePtr _functor;
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
	constexpr explicit TLambdaDelegateInstance(const FunctorType& inFunctor)
		: _functor(inFunctor)
	{
	}

	constexpr explicit TLambdaDelegateInstance(FunctorType&& inFunctor)
		: _functor(std::move(inFunctor))
	{
	}

	bool isVaild() override final
	{
		//Lambda 总是安全的可执行对象
		return true;
	}

	InRetValType execute(const ParamTypes&... args) override final
	{
		return _functor(args...);
	}

private:

	//使用 mutable 主要是为了 Lambda 可以被绑定(修改)和执行
	mutable typename std::remove_const_t<FunctorType> _functor;
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
	constexpr explicit TSharePtrDelegateInstance(const SharePtrType& inUserPtr, FuncTypePtr inFunc)
		: _userPtr(inUserPtr)
		, _functor(inFunc)
	{
	}

	bool isVaild() override final
	{
		return _userPtr.lock() != nullptr;
	}

	InRetValType execute(const ParamTypes&... args) override final
	{
		if (!isVaild())
		{
			ERROR_LOG("TSharePtrDelegateInstance execute Is Error");
			return InRetValType();
		}

		return (_userPtr.lock().get()->*_functor)(args...);
	}

private:

	WeakPtrType _userPtr;
	FuncTypePtr _functor;
};
