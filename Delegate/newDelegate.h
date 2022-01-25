#pragma once
#include "Common.h"
#include <memory>

template<typename FuncType>
class TNewBaseDelegateInstance;

template<typename InRetValType, typename... ParamTypes>
class TNewBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TNewBaseDelegate;

public:
	TNewBaseDelegateInstance() = default;
	virtual ~TNewBaseDelegateInstance() = default;

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
class TNewStaticDelegateInstance;

template<typename InRetValType, typename... ParamTypes, typename... VarTypes>
class TNewStaticDelegateInstance<InRetValType(ParamTypes...), VarTypes...> : public TNewBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TNewBaseDelegate;

public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using FuncTypePtr = InRetValType(*)(ParamTypes...);
	using TupleType = std::tuple<VarTypes...>;
	using TupleSequence = std::index_sequence_for<ParamTypes...>;

public:
	//防止隐式转换，需要函数和参数类型一致
	explicit TNewStaticDelegateInstance(FuncTypePtr InFunc, VarTypes... Vars)
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


///=====================================================================================

template<typename FuncType>
class TNewBaseDelegate;

template<typename InRetValType, typename... ParamTypes>
class TNewBaseDelegate<InRetValType(ParamTypes...)>
{
public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);

	TNewBaseDelegate() = default;
	virtual ~TNewBaseDelegate() = default;

	TNewBaseDelegate(const TNewBaseDelegate& other)
	{
		std::cout << "const TNewBaseDelegate&  ";
		CopyForm(other);
	}

	TNewBaseDelegate(TNewBaseDelegate&& other) noexcept
	{
		std::cout << "TNewBaseDelegate&&  ";
		MoveForm(std::move(other));
	}

	TNewBaseDelegate& operator=(const TNewBaseDelegate& other)
	{
		std::cout << "operator= const TNewBaseDelegate& other  ";
		CopyForm(other);
		return *this;
	}

	TNewBaseDelegate& operator=(TNewBaseDelegate&& other) noexcept
	{
		std::cout << "operator= TNewBaseDelegate&&  ";
		MoveForm(std::move(other));
		return *this;
	}

public:
	virtual void CopyForm(const TNewBaseDelegate& other)
	{
		std::cout << "Is Copy" << std::endl;
		this->ins = other.ins;
		//*this->ins = *other.ins;
		//*this = other;
	}

	virtual void MoveForm(TNewBaseDelegate&& other)
	{
		std::cout << "Is Move" << std::endl;
		this->ins.swap(other.ins);
		other.ins = nullptr;
	}

	void Count()
	{
		std::cout << "Count = " << ins.use_count() << std::endl;
	}

protected:
	std::shared_ptr<TNewBaseDelegateInstance<FuncType>> ins;
};

///==============================================================================================

/*
* 默认参数的函数不能用
* 后续要把传入的参数改为在 Excute 时传入，而不是在创建时，就绑定参数
* 如果赋值（绑定）新的函数，那么不会影响之前已经绑定的指针，因为是智能指针，其他保留者还存在引用计数，所以最好就使用弱指针
*/
template<typename FuncType>
class TNewDelegate;

template<typename InRetValType, typename... ParamTypes>
class TNewDelegate<InRetValType(ParamTypes...)> : public TNewBaseDelegate<InRetValType(ParamTypes...)>
{
public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using Super = TNewBaseDelegate<FuncType>;

public:
	using Super::Super;

public:
	template<typename... Args>
	static TNewDelegate CreateStatic(typename TDefineType<InRetValType(*)(ParamTypes...)>::Type InFunc, Args... args)
	{
		TNewDelegate result;
		result.ins = std::make_shared<TNewStaticDelegateInstance<FuncType, Args...>>(InFunc, args...);
		return result;
	}

public:
	InRetValType ExcuteIfSave()
	{
		if (this->ins.use_count() > 0 && this->ins->IsVaild())
			//if(this->ins.get() && this->ins->IsVaild())
			return this->ins->Execute();

		return InRetValType();
	}

	template<typename... Args>
	InRetValType Excute(Args... args)
	{
		//继承模板类，需要使用 this 指针来使用父类的成员变量
		return this->ins->Execute(args...);
	}
};