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

//��Ϊ std::get ���ص���������ֵ��ת��������ֵ�����Բ�֧����ֵ�������ͣ�֧����������
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
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	explicit TStaticDelegateInstance(FuncTypePtr InFunc, VarTypes... Vars)
		: Functor(InFunc)
		, paramters(std::make_tuple(Vars...))		//����ʹ�����ò����ᱨ����Ϊ�ڲ����� std::decay (�໯)���Ƴ�����������
	{

	}

public:
	bool IsVaild() override final
	{
		//��̬����һֱ��Ϊ true
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

	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	explicit TMemberFuncDelegateInstance(UserClass* InUserObject, FuncTypePtr InFunc, ParamTypes... Params)
		: Functor(InFunc)
		, UserObject(InUserObject)
		//, paramters(std::make_tuple(Params...))		//����ʹ�����ò����ᱨ����Ϊ�ڲ����� std::decay (�໯)���Ƴ�����������
		, paramters(std::forward_as_tuple(Params...))	//����ʹ�����ò���������
	{
		//assert(InUserObject != nullptr && InFunc != nullptr, "InUserObject or InFunc is Null");
	}

public:
	bool IsVaild() override final
	{
		//�޷�ȷ��ָ���Ƿ���Ч��ֻ����ʹ�������жϣ����ʹ�û�������ָ�����͵Ķ���
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

	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
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
		//Lambda ���ǰ�ȫ�Ŀ�ִ�ж���
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

	//ʹ�� mutable ��Ҫ��Ϊ�� Lambda ���Ա��󶨺�ִ��
	//����� Functor �ķ���ֱ�Ӹ�ί���Ӷ���
	//�����ڳ�����������±��� const �Ĵ����ԣ���Ϊ�󶨲�Ӱ�츴�ƵĿ��滻��
	mutable typename std::remove_const_t<FunctorType> Functor;
	TupleType paramters;
};
