#pragma once
#include "BaseDelegate.h"

/*
* ����ò���ȫ���������ͣ�Ч�ʲ����ԭ�� std::bind ��ܶ�
* �ŵ�: ��ǰ�󶨵Ĳ�����������ʱ�滻����������ǰ�����
* ע�⣺ʹ������ʱ����Ҫȷ�����õĶ����Ƿ���Ч
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

	InRetValType ExcuteIfSave()
	{
		if (this->ins.use_count() > 0 && this->ins->IsVaild())
			return this->ins->Execute();

		return InRetValType();
	}

	constexpr bool IsSave()
	{
		return this->ins.use_count() > 0 && this->ins->IsVaild();
	}

	//ִ�к���, ע��:�����޷�ת������������
	template<typename... Args>
	InRetValType Excute(Args&&... args)
	{
		try
		{
			if(IsSave())
				return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::make_tuple(std::forward<Args>(args)...)));
			else
				ERROR_LOG("Execute is not save");
		}
		catch (const std::exception& e)
		{
			ERROR_LOG("Execute Error:", e.what());
		}
		
		return InRetValType();
	}

	//���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	InRetValType ExcuteEx(Args... args)
	{
		try
		{
			if(IsSave())
				return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::tuple<Args...>(static_cast<Args>(args)...)));
			else
				ERROR_LOG("Execute is not save");
		}
		catch (const std::exception& e)
		{
			ERROR_LOG("Execute Error:", e.what());
		}

		return InRetValType();
	}

	//�����ú���ִ��
	template<typename... Args>
	InRetValType ExcuteAfter(Args&&... args)
	{
		try
		{
			if(IsSave())
				return this->ins->Execute(std::tuple_cat(std::make_tuple(std::forward<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>()));
			else
				ERROR_LOG("Execute is not save");
		}
		catch (const std::exception& e)
		{
			ERROR_LOG("Execute Error:", e.what());
		}

		return InRetValType();
	}

	//�����ú���ִ��, ���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	InRetValType ExcuteAfterEx(Args... args)
	{
		try
		{
			if (IsSave())
				return this->ins->Execute((std::tuple_cat(std::tuple<Args...>(static_cast<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>())));
			else
				ERROR_LOG("Execute is not save");
		}
		catch (const std::exception& e)
		{
			ERROR_LOG("Execute Error:", e.what());
		}

		return InRetValType();
	}
};

#define FUNC_DECLAER_DELEGATE( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

FUNC_DECLAER_DELEGATE(FSimpleDelegate, void);

