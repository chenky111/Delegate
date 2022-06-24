#pragma once
#include "Common.h"
#include <memory>
#include "DelegateInstance.h"

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
		CopyForm(other);
	}

	TBaseDelegate(TBaseDelegate&& other) noexcept
	{
		MoveForm(std::move(other));
	}

	TBaseDelegate& operator=(const TBaseDelegate& other)
	{
		CopyForm(other);
		return *this;
	}

	TBaseDelegate& operator=(TBaseDelegate&& other) noexcept
	{
		MoveForm(std::move(other));
		return *this;
	}

public:
	virtual void CopyForm(const TBaseDelegate& other)
	{
		this->ins = other.ins;
	}

	virtual void MoveForm(TBaseDelegate&& other)
	{
		this->ins.swap(other.ins);
		other.ins = nullptr;
	}

	void Count()
	{
		DEBUG_LOG("Count = ", ins.use_count());
	}

protected:
	std::shared_ptr<TBaseDelegateInstance<FuncType>> ins;
};
