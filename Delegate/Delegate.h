#pragma once
#include "BaseDelegate.h"

/*
* 如果用参数全是引用类型, 效率不会和原生 std::bind 差很多, 调用时多1次拷贝
* 优点: 提前绑定的参数, 可以随时替换掉, 还可以前后调用
* 注意: 使用引用时, 需要确保引用的对象是否有效, 还是建议使用智能指针
* 注意: 函数参数不能使用右值引用类型
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
	template<typename... Args>
	static TDelegate CreateBase(FuncType InFunc, Args&&... args)
	{
		TDelegate result;
		result.setType(EDelegateType::Static);
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template<typename... Args>
	static TDelegate CreateStatic(typename TDefineType<InRetValType(*)(ParamTypes...)>::type InFunc, Args&&... args)
	{
		TDelegate result;
		result.setType(EDelegateType::Static);
		result.ins = std::make_shared<TStaticDelegateInstance<FuncType>>(InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template <typename UserClass, typename... Args>
	static TDelegate CreateMemberFunc(UserClass* UserObject, typename TMemberFuncPtr<false, UserClass, FuncType>::Type InFunc, Args&&... args)
	{
		TDelegate result;
		result.setType(EDelegateType::MemberFunc);
		result.ins = std::make_shared<TMemberFuncDelegateInstance<false, UserClass, FuncType>>(UserObject, InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template <typename UserClass, typename... Args>
	static TDelegate CreateMemberFunc(UserClass* UserObject, typename TMemberFuncPtr<true, UserClass, FuncType>::Type InFunc, Args&&... args)
	{
		TDelegate result;
		result.setType(EDelegateType::MemberFunc);
		result.ins = std::make_shared<TMemberFuncDelegateInstance<true, UserClass, FuncType>>(UserObject, InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template<typename TFunc, typename... Args>
	static TDelegate CreateLambda(TFunc&& InFunc, Args&&... args)
	{
		TDelegate result;
		result.setType(EDelegateType::Lambda);
		result.ins = std::make_shared<TLambdaDelegateInstance<FuncType, typename std::decay<TFunc>::type>>(std::forward<TFunc>(InFunc));
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template <typename UserClass, typename... Args>
	static TDelegate CreateSharePtr(const std::shared_ptr<UserClass>& UserPtr, typename TMemberFuncPtr<false, UserClass, FuncType>::Type InFunc, Args&&... args)
	{
		TDelegate result;
		result.setType(EDelegateType::SharePtr);
		result.ins = std::make_shared<TSharePtrDelegateInstance<false, UserClass, FuncType>>(UserPtr, InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

	template <typename UserClass, typename... Args>
	static TDelegate CreateSharePtr(const std::shared_ptr<UserClass>& UserPtr, typename TMemberFuncPtr<true, UserClass, FuncType>::Type InFunc, Args&&... args)
	{
		TDelegate result;
		result.setType(EDelegateType::SharePtr);
		result.ins = std::make_shared<TSharePtrDelegateInstance<true, UserClass, FuncType>>(UserPtr, InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

public:
	//设置参数, 会覆盖原有的参数或者绑定的参数
	//引用类型需要使用 <> 指定类型, 不支持右值引用类型
	template<typename... Args>
	void setParamters(Args... args)
	{
		//移除 const
		//例如使用 void(const int, cosnt int&) -> void(int, const int&) 时
		//这样就能够匹配 any_cast，且不影响参数传递使用
		
		this->ins->setParamters<std::remove_cv_t<Args>...>(args...);
	}

	constexpr bool isSave()
	{
		return this->ins.use_count() > 0 && this->ins->isVaild();
	}

	template<typename... Args>
	constexpr InRetValType saveExecute(Args&&... args)
	{
		if (isSave())
			Execute(std::forward<Args>(args)...);

		return InRetValType();
	}

	//执行函数
	template<typename... Args>
	constexpr InRetValType operator()(Args&&... args)
	{
		return Execute(std::forward<Args>(args)...);
	}

	//执行函数
	template<typename... Args>
	constexpr InRetValType Execute(Args&&... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _Execute(TupleSequence{}, this->ins->castParamters<sizeof...(Args)>(), std::forward<Args>(args)...);
	}

	//参数置前再执行
	template<typename... Args>
	constexpr InRetValType ExecuteAfter(Args&&... args)
	{
		constexpr size_t index = sizeof...(ParamTypes) - sizeof...(Args);
		using TupleSequence = std::make_index_sequence<index>;
		return _ExecuteBack(TupleSequence{}, this->ins->backCastParamters<sizeof...(Args)>(), std::forward<Args>(args)...);
	}

protected:
	template<size_t... Index, typename... TupleArgs, typename... Args>
	constexpr InRetValType _Execute(std::index_sequence<Index...>, const std::tuple<TupleArgs...>& t, Args&&... args)
	{
		return this->ins->execute(remove_cvr_t<TupleArgs>(std::get<Index>(t))..., std::forward<Args>(args)...);
	}

	template<size_t... Index, typename... TupleArgs, typename... Args>
	constexpr InRetValType _ExecuteBack(std::index_sequence<Index...>, const std::tuple<TupleArgs...>& t, Args&&... args)
	{
		return this->ins->execute(std::forward<Args>(args)..., remove_cvr_t<TupleArgs>(std::get<Index>(t))...);
	}
};

#define FUNC_DECLAER_DELEGATE( DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

FUNC_DECLAER_DELEGATE(FSimpleDelegate, void);

