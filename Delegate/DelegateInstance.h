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

	virtual InRetValType ExecuteP(ParamTypes&&... args)
	{
		return InRetValType();
	}

	//Ĭ��, ���ܽ����������ͣ��ڲ�Ϊ std::decay_t ���Ի´��
	template<typename... Args>
	void setParamtersDefault(Args&&... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		paramters = std::make_tuple(std::forward<Args>(args)...);
	}

	//���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	void setParamters(Args... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		paramters = std::tuple<Args...>(args...);
	}

	//��ͷ��ʼƫ�Ʋ���, any_cast ����ɶ����һ�ο���
	template<size_t ArgsSize>
	decltype(auto) CastParamters_Any()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//�õ��Ѿ�����˶��ٸ�����
		return std::any_cast<TMakeTupleForwardOffset<index, ParamTypes...>>(paramters);
	}

	//��ͷ��ʼƫ�Ʋ���, �����ж��⿽��
	template<size_t ArgsSize, size_t index = sizeof...(ParamTypes) - ArgsSize>
	constexpr const TMakeTupleForwardOffset<index, ParamTypes...>& CastParamters()
	{
		return reinterpret_cast<const TMakeTupleForwardOffset<index, ParamTypes...>&>(paramters);
	}

	//��β��ʼƫ�Ʋ���, any_cast ����ɶ����һ�ο���
	template<size_t ArgsSize>
	decltype(auto) BackCastParamters_Any()
	{
		return std::any_cast<TMakeTupleOffset<ArgsSize, ParamTypes...>>(paramters);
	}

	//��β��ʼƫ�Ʋ���, �����ж��⿽��
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
	using TupleSequence = std::index_sequence_for<ParamTypes...>;
	using TupleType = std::tuple<ParamTypes...>;

public:
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	constexpr explicit TStaticDelegateInstance(FuncTypePtr InFunc)
		: Super()
		, Functor(InFunc)
	{
	}

public:
	bool IsVaild() override final
	{
		//��̬����һֱ��Ϊ true
		return true;
	}

	InRetValType Execute(TupleType&& argsTuple) override final
	{
		return std::apply(Functor, std::move(argsTuple));
		//return _CallFunc(TupleSequence{});
	}

	InRetValType ExecuteP(ParamTypes&&... args) override final
	{
		return (*Functor)(std::forward<ParamTypes>(args)...);
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(TupleType&& tupIns, std::index_sequence<Index...>) const
	{
		return (*Functor)(std::forward<ParamTypes>(std::get<Index>(tupIns))...);
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
	using TupleType = std::tuple<ParamTypes...>;

public:
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	constexpr explicit TMemberFuncDelegateInstance(UserClass* InUserObject, FuncTypePtr InFunc)
		: Functor(InFunc)
		, UserObject(InUserObject)
	{
		//assert(InUserObject != nullptr && InFunc != nullptr, "InUserObject or InFunc is Null");
	}

public:
	bool IsVaild() override final
	{
		//�޷�ȷ��ָ���Ƿ���Ч��ֻ����ʹ�������жϣ����ʹ�û�������ָ�����͵Ķ���
		return true;
	}

	InRetValType Execute(TupleType&& argsTuple) override final
	{
		if (Functor == nullptr)
		{
			ERROR_LOG("TMemberFuncDelegateInstance Execute Is Error");
			return InRetValType();
		}

		return _CallFunc(std::forward<TupleType>(argsTuple), TupleSequence{});
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(TupleType&& tupIns, std::index_sequence<Index...>) const
	{
		return (UserObject->*Functor)(std::forward<ParamTypes>(std::get<Index>(tupIns))...);
		//return std::invoke(Functor, UserObject, std::forward<ParamTypes>(std::get<Index>(tupIns))...);
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
	using TupleType = std::tuple<ParamTypes...>;

public:
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
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
		//Lambda ���ǰ�ȫ�Ŀ�ִ�ж���
		return true;
	}

	InRetValType Execute(TupleType&& argsTuple) override final
	{
		return _CallFunc(std::forward<TupleType>(argsTuple), TupleSequence{});
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(TupleType&& tupIns, std::index_sequence<Index...>) const
	{
		return Functor(std::forward<ParamTypes>(std::get<Index>(tupIns))...);
	}

	//ʹ�� mutable ��Ҫ��Ϊ�� Lambda ���Ա��󶨺�ִ��
	//����� Functor �ķ���ֱ�Ӹ�ί���Ӷ���
	//�����ڳ�����������±��� const �Ĵ����ԣ���Ϊ�󶨲�Ӱ�츴�ƵĿ��滻��
	mutable typename std::remove_const_t<FunctorType> Functor;
};
