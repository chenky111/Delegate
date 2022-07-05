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

	//Ĭ��, ���ܽ����������ͣ�ʹ�� std::decay_t ���Ի´��
	template<typename... Args>
	void setParamtersDefault(Args&&... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		_paramters = std::make_tuple(std::forward<Args>(args)...);
	}

	//���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	void setParamters(Args... args)
	{
		static_assert(sizeof...(ParamTypes) >= sizeof...(args), "is to many args");
		_paramters = std::tuple<Args...>(args...);
	}

	//��ͷ��ʼƫ�Ʋ���, any_cast ����ɶ����һ�ο���
	template<size_t ArgsSize>
	decltype(auto) castParamters_Any()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//�õ��Ѿ�����˶��ٸ�����
		return std::any_cast<TMakeTupleForwardOffset<index, ParamTypes...>>(_paramters);
	}

	//��ͷ��ʼƫ�Ʋ���, �����ж��⿽��
	template<size_t ArgsSize, size_t index = sizeof...(ParamTypes) - ArgsSize>
	constexpr const TMakeTupleForwardOffset<index, ParamTypes...>& castParamters()
	{
		return reinterpret_cast<const TMakeTupleForwardOffset<index, ParamTypes...>&>(_paramters);
	}

	//��β��ʼƫ�Ʋ���, any_cast ����ɶ����һ�ο���
	template<size_t ArgsSize>
	decltype(auto) backCastParamters_Any()
	{
		return std::any_cast<TMakeTupleOffset<ArgsSize, ParamTypes...>>(_paramters);
	}

	//��β��ʼƫ�Ʋ���, �����ж��⿽��
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
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	constexpr explicit TStaticDelegateInstance(FuncTypePtr inFunc)
		: Super()
		, _functor(inFunc)
	{
	}

public:
	bool isVaild() override final
	{
		//��̬����һֱ��Ϊ true
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
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	constexpr explicit TMemberFuncDelegateInstance(UserClass* inUserObject, FuncTypePtr inFunc)
		: _functor(inFunc)
		, _userObject(inUserObject)
	{
	}

public:
	bool isVaild() override final
	{
		//�޷�ȷ��ָ���Ƿ���Ч��ֻ����ʹ�������жϣ����ʹ�û�������ָ�����͵Ķ���
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
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
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
		//Lambda ���ǰ�ȫ�Ŀ�ִ�ж���
		return true;
	}

	InRetValType execute(const ParamTypes&... args) override final
	{
		return _functor(args...);
	}

private:

	//ʹ�� mutable ��Ҫ��Ϊ�� Lambda ���Ա���(�޸�)��ִ��
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
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
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
