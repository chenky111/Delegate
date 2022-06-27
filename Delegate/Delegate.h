#pragma once
#include "BaseDelegate.h"

/*
* ����ò���ȫ����������, Ч�ʲ����ԭ�� std::bind ��ܶ�, ����ʱ��1�ο���
* �ŵ�: ��ǰ�󶨵Ĳ���, ������ʱ�滻��, ������ǰ�����
* ע��: ʹ������ʱ, ��Ҫȷ�����õĶ����Ƿ���Ч, ���ǽ���ʹ������ָ��
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
	//2�ַ���Ч����һ��
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
	//���ò���, ����������Ҫʹ�� <> ָ������
	template<typename... Args>
	void setParamters(const Args&... args)
	{
		//�Ƴ� const
		//����ʹ�� void(const int, cosnt int&) -> void(int, const int&) ʱ
		//�������ܹ�ƥ�� any_cast���Ҳ�Ӱ���������ʹ��
		
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
	//ִ�к���, ע��:�����޷�ת������������
	template<typename... Args>
	constexpr InRetValType Excute(Args&&... args)
	{
		if (IsSave())
			return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::make_tuple(std::forward<Args>(args)...)));

		return InRetValType();
	}

	//���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	constexpr InRetValType ExcuteEx(Args... args)
	{
		if(IsSave())
			return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::tuple<Args...>(static_cast<Args>(args)...)));

		return InRetValType();
	}

	//�����ú���ִ��
	template<typename... Args>
	constexpr InRetValType ExcuteAfter(Args&&... args)
	{
		if (IsSave())
			return this->ins->Execute(std::tuple_cat(std::make_tuple(std::forward<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>()));

		return InRetValType();
	}

	//�����ú���ִ��, ���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	constexpr InRetValType ExcuteAfterEx(Args... args)
	{
		if (IsSave())
			return this->ins->Execute((std::tuple_cat(std::tuple<Args...>(static_cast<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>())));

		return InRetValType();
	}
#else
	//ִ�к���, ע��:�����޷�ת������������
	template<typename... Args>
	constexpr InRetValType Excute(Args&&... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _Excute(TupleSequence{}, this->ins->CastParamters<sizeof...(Args)>(), std::forward<Args>(args)...);
	}

	//���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	constexpr InRetValType ExcuteEx(Args... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _ExcuteEx(TupleSequence{}, this->ins->CastParamters<sizeof...(Args)>(), static_cast<Args>(args)...);
	}

	//�����ú���ִ��
	template<typename... Args>
	constexpr InRetValType ExcuteAfter(Args&&... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _ExcuteBack(TupleSequence{}, this->ins->CastParamters<sizeof...(Args)>(), std::forward<Args>(args)...);
	}

	//�����ú���ִ��, ���Խ�����������, ��Ҫʹ�� <> ָ������
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

