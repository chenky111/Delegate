#pragma once
#include "BaseDelegate.h"

/*
* Ĭ�ϲ����ĺ���������
* ����Ҫ�Ѵ���Ĳ�����Ϊ�� Excute ʱ���룬�������ڴ���ʱ���Ͱ󶨲���
* �����ֵ���󶨣��µĺ�������ô����Ӱ��֮ǰ�Ѿ��󶨵�ָ�룬��Ϊ������ָ�룬���������߻��������ü�����������þ�ʹ����ָ��
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
	//template<typename... VarTypes>
	//static TDelegate CreateBase(FuncType func, VarTypes&&... Vars)
	//{
	//	TDelegate result;
	//	result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(func, std::forward<VarTypes>(Vars)...);
	//	return result;
	//}

	//2�ַ���Ч����һ��
#if 0
	static TDelegate CreateBase(FuncType InFunc, ParamTypes... Vars)
	{
		TDelegate result;
		//std::cout << "FuncType : " << typeid(FuncType).name() << std::endl;
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc, Vars...);
		return result;
	}
#else
	static TDelegate CreateBase(typename TDefineType<InRetValType(*)(ParamTypes...)>::Type InFunc, ParamTypes... Vars)
	{
		TDelegate result;
		//std::cout << "FuncType : " << typeid(typename TDefineType<InRetValType(*)(ParamTypes...)>::Type).name() << std::endl;
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc, Vars...);
		//result.ins = std::make_unique<TStaticDelegateInstance<FuncType>>(InFunc, Vars...);
		return result;
	}
#endif

	template<typename... Args>
	static TDelegate CreateStatic(typename TDefineType<InRetValType(*)(ParamTypes...)>::Type InFunc, Args... args)
	{
		TDelegate result;
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType, Args...>>(InFunc, args...);
		return result;
	}

	template <typename UserClass>
	static TDelegate CreateMemberFunc(UserClass* UserObject, typename TMemberFuncPtr<false, UserClass, FuncType>::Type InFunc, ParamTypes... Vars)
	{
		TDelegate result;
		//std::cout << "FuncType False : " << typeid(typename TMemberFuncPtr<false, UserClass, FuncType>::Type).name() << std::endl;
		result.ins = std::make_shared<TMemberFuncDelegateInstance<false, UserClass, FuncType>>(UserObject, InFunc, Vars...);
		//result.ins = std::make_unique<TMemberFuncDelegateInstance<false, UserClass, FuncType>>(UserObject, InFunc, Vars...);
		return result;
	}

	template <typename UserClass>
	static TDelegate CreateMemberFunc(UserClass* UserObject, typename TMemberFuncPtr<true, UserClass, FuncType>::Type InFunc, ParamTypes... Vars)
	{
		TDelegate result;
		//std::cout << "FuncType True : " << typeid(typename TMemberFuncPtr<true, UserClass, FuncType>::Type).name() << std::endl;
		result.ins = std::make_shared<TMemberFuncDelegateInstance<true, UserClass, FuncType>>(UserObject, InFunc, Vars...);
		//result.ins = std::make_unique<TMemberFuncDelegateInstance<true, UserClass, FuncType>>(UserObject, InFunc, Vars...);
		return result;
	}

	template<typename TFunc, typename... Args>
	static void CreateAndCallLambda_Test(TFunc&& InFunc, Args... args)
	{
		auto f = TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>(std::forward<TFunc>(InFunc), args...);
		f.Execute();
	}

	template<typename TFunc, typename... Args>
	static auto CreateLambda_Test(TFunc&& InFunc, Args... args)
		-> decltype(TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>(std::forward<TFunc>(InFunc), args...))
	{
		return TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>(std::forward<TFunc>(InFunc), args...);
	}

	template<typename TFunc, typename... Args>
	static TDelegate CreateLambda(TFunc&& InFunc, ParamTypes... Vars)
	{
		TDelegate result;
		//std::cout << "FuncType : " << typeid(TFunc).name() << std::endl;
		//std::cout << "FuncType Decay : " << typeid(typename std::decay<TFunc>::type).name() << std::endl;
		result.ins = std::make_shared<TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>>(std::forward<TFunc>(InFunc), Vars...);
		//result.ins = std::make_unique<TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>>(std::forward<TFunc>(InFunc), Vars...);
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
		//�̳�ģ���࣬��Ҫʹ�� this ָ����ʹ�ø���ĳ�Ա����
		return this->ins->Execute(args...);
	}

protected:
	//std::shared_ptr<TBaseDelegateInstance<FuncType>> ins;
	//std::unique_ptr<TBaseDelegateInstance<FuncType>> ins;
};

#define FUNC_DECLAER_DELEGATE( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

FUNC_DECLAER_DELEGATE(FSimpleDelegate, void);

