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
		DEBUG_LOG("const TBaseDelegate&  ");
		CopyForm(other);
	}

	TBaseDelegate(TBaseDelegate&& other) noexcept
	{
		DEBUG_LOG("TBaseDelegate&&  ");
		MoveForm(std::move(other));
	}

	TBaseDelegate& operator=(const TBaseDelegate& other)
	{
		DEBUG_LOG("operator= const TBaseDelegate& other  ");
		CopyForm(other);
		return *this;
	}

	TBaseDelegate& operator=(TBaseDelegate&& other) noexcept
	{
		DEBUG_LOG("operator= TBaseDelegate&&  ");
		MoveForm(std::move(other));
		return *this;
	}

public:
	virtual void CopyForm(const TBaseDelegate& other)
	{
		DEBUG_LOG("Is Copy");
		this->ins = other.ins;
		//*this->ins = *other.ins;
		//*this = other;
	}

	virtual void MoveForm(TBaseDelegate&& other)
	{
		DEBUG_LOG("Is Move");
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
