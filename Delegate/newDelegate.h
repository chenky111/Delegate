#pragma once
#include "Common.h"
#include <memory>
#include <assert.h>

#define _TestInterface 0;

template<typename FuncType>
class TNewBaseDelegateInstance;

template<typename InRetValType, typename... ParamTypes>
class TNewBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TNewBaseDelegate;

public:
	using FuncType = InRetValType(ParamTypes...);
	//using ExcuteTuple = typename TMakeTupleOffsetUtil<FuncType, ParamTypes...>::type;

public:
	TNewBaseDelegateInstance() = default;
	virtual ~TNewBaseDelegateInstance() = default;

public:

	virtual InRetValType Execute(std::tuple<ParamTypes...>&& argsTuple)
	{
		return InRetValType();
	}

	//Ĭ��, ���ܽ�����������
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

	//��ͷ��ʼƫ�Ʋ���
	template<size_t ArgsSize>
	decltype(auto) CastParamters()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//�õ��Ѿ�����˶��ٸ�����
		return std::any_cast<TMakeTupleForwardOffset<index, ParamTypes...>>(paramters);
	}

	//��β��ʼƫ�Ʋ���
	template<size_t ArgsSize>
	decltype(auto) BackCastParamters()
	{
		constexpr size_t index = sizeof...(ParamTypes) - ArgsSize;	//�õ��Ѿ�����˶��ٸ�����
		return std::any_cast<TMakeTupleOffset<index, ParamTypes...>>(paramters);
	}

	virtual bool IsVaild()
	{
		return true;
	}

protected:
	std::any paramters;
};


template<typename FuncType>
class TNewStaticDelegateInstance;

template<typename InRetValType, typename... ParamTypes>
class TNewStaticDelegateInstance<InRetValType(ParamTypes...)> : public TNewBaseDelegateInstance<InRetValType(ParamTypes...)>
{
	template<typename T>
	friend class TNewBaseDelegate;

public:
	using Super = TNewBaseDelegateInstance<InRetValType(ParamTypes...)>;
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);
	using FuncTypePtr = InRetValType(*)(ParamTypes...);
	using TupleSequence = std::index_sequence_for<ParamTypes...>;

public:
	//��ֹ��ʽת������Ҫ�����Ͳ�������һ��
	explicit TNewStaticDelegateInstance(FuncTypePtr InFunc)
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

	InRetValType Execute(std::tuple<ParamTypes...>&& argsTuple) override final
	{
		return std::apply(Functor, std::move(argsTuple));
		//return _CallFunc(TupleSequence{});
	}

private:
	template<size_t... Index>
	InRetValType _CallFunc(std::index_sequence<Index...>) const
	{
		return (*Functor)(std::get<Index>(this->paramters)...);
	}

	FuncTypePtr Functor;
};


//=====================================================================================


template<typename FuncType>
class TNewBaseDelegate;

template<typename InRetValType, typename... ParamTypes>
class TNewBaseDelegate<InRetValType(ParamTypes...)>
{
public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);

	virtual ~TNewBaseDelegate() = default;
	TNewBaseDelegate() = default;

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
	static TNewDelegate CreateStatic(typename TDefineType<InRetValType(*)(ParamTypes...)>::Type InFunc, Args&&... args)
	{
		TNewDelegate result;
		result.ins = std::make_shared<TNewStaticDelegateInstance<FuncType>>(InFunc);
		result.ins->setParamtersDefault<Args...>(std::forward<Args>(args)...);
		return result;
	}

public:
	//���ò���, ����������Ҫʹ�� <> ָ������
	template<typename... Args>
	void setParamters(const Args&... args)
	{
		this->ins->setParamters<Args...>(args...);
	}

	InRetValType ExcuteIfSave()
	{
		if (this->ins.use_count() > 0 && this->ins->IsVaild())
			return this->ins->Execute();

		return InRetValType();
	}

	//�����޷�ת��Ϊ�������͵�ģ��
	template<typename... Args>
	InRetValType Excute(Args&&... args)
	{
		return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::make_tuple(std::forward<Args>(args)...)));
	}

	//���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	InRetValType ExcuteEx(const Args&... args)
	{
		return this->ins->Execute(std::tuple_cat(this->ins->CastParamters<sizeof...(Args)>(), std::tuple<Args...>(args...)));
	}

	//�����ú���ִ��
	template<typename... Args>
	InRetValType ExcuteAfter(Args&&... args)
	{
		return this->ins->Execute(std::tuple_cat(std::make_tuple(std::forward<Args>(args)...), this->ins->BackCastParamters<sizeof...(Args)>()));
	}

	//�����ú���ִ��, ���Խ�����������, ��Ҫʹ�� <> ָ������
	template<typename... Args>
	InRetValType ExcuteAfterEx(const Args&... args)
	{
		return this->ins->Execute((std::tuple_cat(std::tuple<Args...>(args...), this->ins->BackCastParamters<sizeof...(Args)>())));
	}
};