#pragma once
#include "Common.h"
#include <memory>

template<typename FuncType, typename... VarTypes>
class TNewBaseDelegateInstance;

template<typename InRetValType, typename... ParamTypes, typename... VarTypes>
class TNewBaseDelegateInstance<InRetValType(ParamTypes...), VarTypes...>
{
	template<typename T>
	friend class TNewBaseDelegate;

public:
	using FuncType = InRetValType(ParamTypes...);
	using TupleType = TTuple<VarTypes...>;
	using ExcuteTuple = typename TMakeTupleOffsetUtil<FuncType, ParamTypes...>::type;

public:
	TNewBaseDelegateInstance(VarTypes... Vars)
		: paramters(std::make_tuple(Vars...))
	{

	}

	virtual ~TNewBaseDelegateInstance() = default;

public:
	virtual InRetValType Execute(ExcuteTuple&& argsTuple)
	{
		return InRetValType();
	}

	virtual bool IsVaild()
	{
		return true;
	}

protected:
	TupleType paramters;
};

//��Ϊ std::get ���ص���������ֵ��ת��������ֵ�����Բ�֧����ֵ�������ͣ�֧����������
template<typename FuncType, typename... VarTypes>
class TNewStaticDelegateInstance;

template<typename InRetValType, typename... ParamTypes, typename... VarTypes>
class TNewStaticDelegateInstance<InRetValType(ParamTypes...), VarTypes...> : public TNewBaseDelegateInstance<InRetValType(ParamTypes...), VarTypes...>
{
	template<typename T>
	friend class TNewBaseDelegate;

public:
	using Super = TNewBaseDelegateInstance<InRetValType(ParamTypes...), VarTypes...>;
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using FuncTypePtr = InRetValType(*)(ParamTypes...);
	using TupleType = std::tuple<VarTypes...>;
	//using TupleType = typename Super::TupleType;
	using TupleSequence = std::index_sequence_for<ParamTypes...>;
	using ExcuteTuple = typename Super::ExcuteTuple;

public:
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	explicit TNewStaticDelegateInstance(FuncTypePtr InFunc, VarTypes... Vars)
		: Super(Vars...)
		, Functor(InFunc)
		//, paramters(std::make_tuple(Vars...))		//����ʹ�����ò����ᱨ����Ϊ�ڲ����� std::decay (�໯)���Ƴ�����������
	{
	}

public:
	bool IsVaild() override final
	{
		//��̬����һֱ��Ϊ true
		return true;
	}

	InRetValType Execute(ExcuteTuple&& argsTuple) override final
	{
		//std::cout << "Is Base" << std::endl;
		//return _CallFunc(TupleSequence{});
		//return InvokeAfter(*Functor, paramters, args...);
		//return this->paramters.ApplyAfter(*Functor, std::forward<ExcuteTuple>(argsTuple));
		
		//constexpr size_t totalSize = std::tuple_size_v<decltype(this->paramters.get())> + std::tuple_size_v<decltype(argsTuple)>;
		//static_assert(totalSize >= 0, "Params Count Is Error");
		//using TupleSequence = std::make_index_sequence<totalSize>;
		//auto CallTuple = std::tuple_cat(this->paramters.get(), argsTuple);

		//std::tuple<> a;
		//using at = std::decay_t<ExcuteTuple>;

		if constexpr (std::is_same_v<std::decay_t<ExcuteTuple>, std::tuple<>>)
		{
			auto size = 0;
		}
		else
		{
			auto size = std::tuple_size_v<decltype(ExcuteTuple)>;
		}

		//return InRetValType();
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(std::index_sequence<Index...>) const
	{
		//std::cout << "Test Type: ";
		//std::initializer_list<char> { (std::cout << typeid(std::get<Index>(paramters)).name() << " , ", 0)... };
		//std::cout << std::endl;

		//return (*Functor)(std::get<Index>(this->paramters)...);
		return InRetValType();
	}

	FuncTypePtr Functor;
	//TupleType paramters;
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
		WARNING_LOG("const TNewBaseDelegate&  ");
		CopyForm(other);
	}

	TNewBaseDelegate(TNewBaseDelegate&& other) noexcept
	{
		WARNING_LOG("TNewBaseDelegate&&  ");
		MoveForm(std::move(other));
	}

	TNewBaseDelegate& operator=(const TNewBaseDelegate& other)
	{
		WARNING_LOG("operator= const TNewBaseDelegate& other  ");
		CopyForm(other);
		return *this;
	}

	TNewBaseDelegate& operator=(TNewBaseDelegate&& other) noexcept
	{
		WARNING_LOG("operator= TNewBaseDelegate&&  ");
		MoveForm(std::move(other));
		return *this;
	}

public:
	virtual void CopyForm(const TNewBaseDelegate& other)
	{
		WARNING_LOG("Is Copy");
		this->ins = other.ins;
		//*this->ins = *other.ins;
		//*this = other;
	}

	virtual void MoveForm(TNewBaseDelegate&& other)
	{
		WARNING_LOG("Is Move");
		this->ins.swap(other.ins);
		other.ins = nullptr;
	}

	void Count()
	{
		WARNING_LOG("Count = " << ins.use_count());
	}

protected:
	std::shared_ptr<TNewBaseDelegateInstance<FuncType>> ins;
};

///==============================================================================================

/*
* Ĭ�ϲ����ĺ���������
* ����Ҫ�Ѵ���Ĳ�����Ϊ�� Excute ʱ���룬�������ڴ���ʱ���Ͱ󶨲���
* �����ֵ���󶨣��µĺ�������ô����Ӱ��֮ǰ�Ѿ��󶨵�ָ�룬��Ϊ������ָ�룬���������߻��������ü�����������þ�ʹ����ָ��
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
	InRetValType Excute(Args&&... args)
	{
		//return this->ins->Execute(std::make_tuple<Args>(args)...);
		return this->ins->Execute(std::forward_as_tuple(std::forward<Args>(args)...));
	}
};