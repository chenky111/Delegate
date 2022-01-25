#pragma once
#include "Common.h"
#include <memory>
#include <tuple>
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
		std::cout << "const TBaseDelegate&  ";
		CopyForm(other);
	}

	TBaseDelegate(TBaseDelegate&& other) noexcept
	{
		std::cout << "TBaseDelegate&&  ";
		MoveForm(std::move(other));
	}

	TBaseDelegate& operator=(const TBaseDelegate& other)
	{
		std::cout << "operator= const TBaseDelegate& other  ";
		CopyForm(other);
		return *this;
	}

	TBaseDelegate& operator=(TBaseDelegate&& other) noexcept
	{
		std::cout << "operator= TBaseDelegate&&  ";
		MoveForm(std::move(other));
		return *this;
	}

public:
	virtual void CopyForm(const TBaseDelegate& other)
	{
		std::cout << "Is Copy" << std::endl;
		this->ins = other.ins;
		//*this->ins = *other.ins;
		//*this = other;
	}

	virtual void MoveForm(TBaseDelegate&& other)
	{
		std::cout << "Is Move" << std::endl;
		this->ins.swap(other.ins);
		other.ins = nullptr;
	}

	void Count()
	{
		std::cout << "Count = " << ins.use_count() << std::endl;
	}

protected:
	std::shared_ptr<TBaseDelegateInstance<FuncType>> ins;
};
