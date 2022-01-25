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
	TBaseDelegateInstance() = default;
	virtual ~TBaseDelegateInstance() = default;

public:
	virtual InRetValType Execute(ParamTypes... args)
	{
		return InRetValType();
	}

	virtual bool IsVaild()
	{
		return true;
	}
};

//因为 std::get 返回的类型是左值，转换不了右值，所以不支持右值引用类型，支持万能引用
template<typename FuncType, typename... VarTypes>
class TStaticDelegateInstance;

template<typename InRetValType, typename... ParamTypes, typename... VarTypes>
class TStaticDelegateInstance<InRetValType(ParamTypes...), VarTypes...> : public TBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TBaseDelegate;

public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using FuncTypePtr = InRetValType(*)(ParamTypes...);
	using TupleType = std::tuple<VarTypes...>;
	using TupleSequence = std::index_sequence_for<ParamTypes...>;

public:
	//防止隐式转换，需要函数和参数类型一致
	explicit TStaticDelegateInstance(FuncTypePtr InFunc, VarTypes... Vars)
		: Functor(InFunc)
		, paramters(std::make_tuple(Vars...))		//函数使用引用参数会报错，因为内部调用 std::decay (朽化)，移除了引用类型
	{

	}

public:
	bool IsVaild() override final
	{
		//静态函数一直都为 true
		return true;
	}

	InRetValType Execute(ParamTypes... args) override final
	{
		//std::cout << "Is Base" << std::endl;
		//return _CallFunc(TupleSequence{});
		return InvokeAfter(*Functor, paramters, args...);
		//return InRetValType();
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(std::index_sequence<Index...>) const
	{
		//std::cout << "Test Type: ";
		//std::initializer_list<char> { (std::cout << typeid(std::get<Index>(paramters)).name() << " , ", 0)... };
		//std::cout << std::endl;

		return (*Functor)(std::get<Index>(paramters)...);
	}

	FuncTypePtr Functor;
	TupleType paramters;
};

template<bool bConst, typename UserClass, typename FuncType>
class TMemberFuncDelegateInstance;

template<bool bConst, typename UserClass, typename InRetValType, typename... ParamTypes>
class TMemberFuncDelegateInstance<bConst, UserClass, InRetValType(ParamTypes...)> : public TBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TBaseDelegate;

public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using FuncTypePtr = typename TMemberFuncPtr<bConst, UserClass, FuncType>::Type;
	using TupleType = std::tuple<ParamTypes...>;
	using TupleSequence = std::index_sequence_for<ParamTypes...>;

	//防止隐式转换，需要函数和参数类型一致
	explicit TMemberFuncDelegateInstance(UserClass* InUserObject, FuncTypePtr InFunc, ParamTypes... Params)
		: Functor(InFunc)
		, UserObject(InUserObject)
		//, paramters(std::make_tuple(Params...))		//函数使用引用参数会报错，因为内部调用 std::decay (朽化)，移除了引用类型
		, paramters(std::forward_as_tuple(Params...))	//函数使用引用参数不报错
	{
		//assert(InUserObject != nullptr && InFunc != nullptr, "InUserObject or InFunc is Null");
	}

public:
	bool IsVaild() override final
	{
		//无法确保指针是否有效，只能由使用者来判断，最好使用基于智能指针类型的对象
		return true;
	}

	InRetValType Execute() override final
	{
		WARNING_LOG("Is MemberFunc");
		if (Functor == nullptr)
		{
			std::cout << "TMemberFuncDelegateInstance Execute Is Error" << std::endl;
			return InRetValType();
		}

		return _CallFunc(TupleSequence{});
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(std::index_sequence<Index...>) const
	{
		//std::cout << "Test Type: ";
		//std::initializer_list<char> { (std::cout << typeid(std::get<Index>(paramters)).name() << " , ", 0)... };
		//std::cout << std::endl;

		return (UserObject->*Functor)(std::get<Index>(paramters)...);
	}

	UserClass* UserObject;
	FuncTypePtr Functor;
	TupleType paramters;
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
		, paramters(std::forward_as_tuple(Params...))
	{
	}

	explicit TLambdaDelegateInstance(FunctorType&& InFunctor, ParamTypes... Params)
		: Functor(InFunctor)
		, paramters(std::forward_as_tuple(Params...))
	{
	}

	bool IsVaild() override final
	{
		//Lambda 总是安全的可执行对象
		return true;
	}

	InRetValType Execute() override final
	{
		WARNING_LOG("Is Lambda Call");
		return _CallFunc(TupleSequence{});
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(std::index_sequence<Index...>)
	{
		return Functor(std::get<Index>(paramters)...);
	}

	//使用 mutable 主要是为了 Lambda 可以被绑定和执行
	//不想把 Functor 的方法直接给委托子对象
	//这样在常函数的情况下保持 const 的传递性，因为绑定不影响复制的可替换性
	mutable typename std::remove_const_t<FunctorType> Functor;
	TupleType paramters;
};
