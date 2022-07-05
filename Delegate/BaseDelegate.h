#pragma once
#include "Common.h"
#include <memory>
#include "DelegateInstance.h"

enum class EDelegateType
{
	None = 0,
	Static,
	MemberFunc,
	Lambda,
	SharePtr,
};

template<typename FuncType>
class TBaseDelegate;

template<typename InRetValType, typename... ParamTypes>
class TBaseDelegate<InRetValType(ParamTypes...)>
{
public:
	using RetValType = InRetValType;
	using FuncType = InRetValType(ParamTypes...);

	TBaseDelegate() = default;
	virtual ~TBaseDelegate() = default;

	TBaseDelegate(const TBaseDelegate& other)
	{
		copyForm(other);
	}

	TBaseDelegate(TBaseDelegate&& other) noexcept
	{
		moveForm(std::move(other));
	}

	TBaseDelegate& operator=(const TBaseDelegate& other)
	{
		copyForm(other);
		return *this;
	}

	TBaseDelegate& operator=(TBaseDelegate&& other) noexcept
	{
		moveForm(std::move(other));
		return *this;
	}

public:
	virtual void copyForm(const TBaseDelegate& other)
	{
		this->ins = other.ins;
	}

	virtual void moveForm(TBaseDelegate&& other)
	{
		this->ins.swap(other.ins);
		other.ins = nullptr;
	}

	void Count()
	{
		DEBUG_LOG("Count = ", ins.use_count());
	}

	constexpr void setType(const EDelegateType& type)
	{
		delegate_type = type;
	}

protected:
	std::shared_ptr<TBaseDelegateInstance<FuncType>> ins;
	EDelegateType delegate_type = EDelegateType::None;
};
