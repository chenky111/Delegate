#pragma once
#include "ColorDebug.h"
#include <tuple>
#include <iostream>

using BaseFuncType = void(*)(void);

//自定义的类型（用于把分散的模板参数，合并为一个类型）
template<typename T>
struct TDefineType
{
	typedef T Type;
};

//成员函数指针模板类型
//前置声明（第一个模板参数，用来判断是否为常函数）
template<bool bConst, typename T, typename F>
class TMemberFuncPtr;

template<typename UserClass, typename RetValue, typename... Args>
class TMemberFuncPtr<false, UserClass, RetValue(Args...)> //模板特例化
{
public:
	using Type = RetValue(UserClass::*)(Args...);
	using ReturnType = RetValue;
};

template<typename UserClass, typename RetValue, typename... Args>
class TMemberFuncPtr<true, UserClass, RetValue(Args...)> //模板特例化
{
public:
	using Type = RetValue(UserClass::*)(Args...) const;
	using ReturnType = RetValue;
};


template<typename FuncType, typename... Args>
class FT;

template<typename Ret, typename... Paras, typename... Args>
class FT<Ret(Paras...), Args...>
{
	//Ret: 函数的返回值类型
	//Paras: 函数的参数包类型
	//Args: 调用函数的参数包类型
};


template<typename... Types>
class TTuple
{
public:
	//TTuple() = default;
	//TTuple(TTuple&& Other) = default;
	//TTuple(const TTuple& Other) = default;
	//TTuple& operator=(TTuple&& Other) = default;
	//TTuple& operator=(const TTuple& Other) = default;

	explicit TTuple(Types... Params)
	: TupleIns(std::forward_as_tuple(Params...))
	{

	}

	template<typename FuncType, typename... ArgTypes>
	decltype(auto) ApplyAfter(FuncType&& Func, ArgTypes&&... Args) const
	{
		//Invoke(std::forward<FuncType>(Func), TupleSequence{}, false, std::forward<ArgTypes>(Args)...);
		auto t = std::forward_as_tuple(std::forward<ArgTypes>(Args)...);
		constexpr size_t TotalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<decltype(TupleIns)>;
		//std::cout << TotalSize << std::endl;
		static_assert(TotalSize >= 0, "Params Count Is Error");
		using TupleSequence = std::make_index_sequence<TotalSize>;
		auto CallTuple = std::tuple_cat(TupleIns, t);
		return Call(Func, CallTuple, TupleSequence{});
	}

	template<typename FuncType, typename... ArgTypes>
	decltype(auto) ApplyBefor(FuncType&& Func, ArgTypes&&... Args) const
	{
		//Invoke(std::forward<FuncType>(Func), TupleSequence{}, false, std::forward<ArgTypes>(Args)...);
		auto t = std::forward_as_tuple(std::forward<ArgTypes>(Args)...);
		constexpr size_t TotalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<decltype(TupleIns)>;
		//std::cout << TotalSize << std::endl;
		static_assert(TotalSize >= 0, "Params Count Is Error");
		using TupleSequence = std::make_index_sequence<TotalSize>;
		auto CallTuple = std::tuple_cat(t, TupleIns);
		return Call(Func, CallTuple, TupleSequence{});
	}


private:
// 	template<typename FuncType, size_t... Index, bool Nothing, typename... ArgTypes>
// 	auto Invoke(FuncType&& Func, std::index_sequence<Index...>, Nothing Flag, ArgTypes&&... Args)
// 		-> decltype(std::forward<FuncType>(Func)(std::get<Index>(TupleIns)..., std::forward<ArgTypes>(Args)...))
// 	{
// 		return std::forward<FuncType>(Func)(std::get<Index>(TupleIns)..., std::forward<ArgTypes>(Args)...);
// 	}

	//使用 decltype(auto) 不能接受 void 返回值
	template<typename FuncType, typename InTuple, size_t... Index>
	auto Call(FuncType&& Func, InTuple&& t, std::index_sequence<Index...>) const
		-> decltype(std::forward<FuncType>(Func)(std::get<Index>(t)...))
	{
		return std::forward<FuncType>(Func)(std::get<Index>(t)...);
	}

	std::tuple<Types...> TupleIns;
};

template<typename FuncType, typename InTuple, size_t... Index>
auto Invoke(FuncType&& func, InTuple&& t, std::index_sequence<Index...>)
	-> decltype(std::forward<FuncType>(func)(std::get<Index>(t)...)) //使用 decltype(auto) 不能接受 void 返回值
{
	return std::forward<FuncType>(func)(std::get<Index>(t)...);
}

template<typename FuncType, typename TupleType, typename... ParamTypes>
decltype(auto) InvokeAfter(FuncType&& func, TupleType&& tupleIns, ParamTypes&&... params)
{
	//auto t = std::forward_as_tuple(std::forward<ParamTypes>(params)...); //err
	auto t = std::make_tuple(std::forward<ParamTypes>(params)...);
	constexpr size_t TotalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<std::decay_t<TupleType>>;
	static_assert(TotalSize >= 0, "params count is error");
	using TupleSequence = std::make_index_sequence<TotalSize>;
	auto CallTuple = std::tuple_cat(tupleIns, t);
	WARNING_LOG(typeid(decltype(CallTuple)).name());
	WARNING_LOG(typeid(decltype(tupleIns)).name());
	WARNING_LOG(typeid(decltype(t)).name());
	return Invoke(std::forward<FuncType>(func), CallTuple, TupleSequence{});
}

