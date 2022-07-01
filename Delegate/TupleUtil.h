#pragma once
#include <tuple>
#include <iostream>

//default_type 始终为 true
template<typename T>
struct default_type : std::true_type {
	using type = T;
};


/*
* 偏移元组参数类型, 偏移 index 之后的 tuple 类型
* @index: 必须为 [0 ~ N-1]
*/
template<int index, typename... Args>
struct TTupleOffset;

template<int index, typename FirstType, typename... Args>
struct TTupleOffset<index, FirstType, Args...> : std::true_type
{
	using type = typename TTupleOffset<index - 1, Args...>::type;
};

template<typename T, typename... Args>
struct TTupleOffset<0, T, Args...> : std::true_type
{
	using type = std::tuple<T, Args...>;
};

/*
* 创建偏移元组参数类型,
* 需要 C++17 的短路编译,否则会编译不过,失败默认返回 std::tuple<>
* @Index: 移除参数的索引位置, 从0开始
* @Params: 参数
*/
template<size_t Index, typename... Params>
using TMakeTupleOffset = typename std::disjunction<
	std::conditional_t<(sizeof...(Params) >= 1 && sizeof...(Params) == Index), default_type<std::tuple<>>, std::false_type>,
	std::conditional_t<(sizeof...(Params) >= 1 && sizeof...(Params) > Index), TTupleOffset<Index, Params...>, std::false_type>,
	default_type<std::tuple<>>
>::type;

//去除 const，不包括 const T&
template<size_t Index, typename... Params>
using TMakeTupleOffset_NoConst = TMakeTupleOffset<Index, std::remove_cv_t<Params>...>;

/*
* 使用 函数类型 来创建偏移元组参数类型
* 用法: 
	using FF1 = void(int, char, bool);

	using FT1 = TMakeTupleOffsetUtil<FF1>;
	using FT2 = TMakeTupleOffsetUtil<FF1, int>;
	using FT3 = TMakeTupleOffsetUtil<FF1, int, char>;
	using FT4 = TMakeTupleOffsetUtil<FF1, int, char, bool>;
	using FT5 = TMakeTupleOffsetUtil<void()>;
	std::cout << typeid(FT1::type).name() << std::endl;
	std::cout << typeid(FT2::type).name() << std::endl;
	std::cout << typeid(FT3::type).name() << std::endl;
	std::cout << typeid(FT4::type).name() << std::endl;
	std::cout << typeid(FT5::type).name() << std::endl;
*/
template<typename FuncType, typename... Args>
struct TMakeTupleOffsetUtil;

template<typename T, typename... Params, typename... Args>
struct TMakeTupleOffsetUtil<T(Params...), Args...>
{
	static constexpr size_t paramsNum = sizeof...(Params);
	static constexpr size_t argsNum = sizeof...(Args);
	//static constexpr size_t index = argsNum >= paramsNum ? argsNum : paramsNum - argsNum - 1;

	struct checkArgs { static_assert(argsNum <= paramsNum, "args is too many!!!"); } _check;

	using type = TMakeTupleOffset<argsNum, Params...>;
};

/*=====================================================================================================*/

template<size_t N, typename TupleType, typename T, typename... Args>
struct TTupleForwardOffset;

template<size_t N, typename... Params, typename T, typename... Args>
struct TTupleForwardOffset<N, std::tuple<Params...>, T, Args...> : std::true_type
{
	using type = typename TTupleForwardOffset<N - 1, std::tuple<Params..., T>, Args...>::type;
};

template<typename... Params, typename T, typename... Args>
struct TTupleForwardOffset<0, std::tuple<Params...>, T, Args...> : std::true_type
{
	using type = std::tuple<Params...>;
};


/*
* 偏移参数后的 tuple 类型(前面的参数)
* @index: 必须为 [0 ~ N-1]
*/
template<size_t N, typename T = void, typename... Args>
struct TTupleForwardOffsetUtil : TTupleForwardOffset<N, std::tuple<>, T, Args...> {};

/*
* 创建偏移后，前面的参数类型,
* 需要 C++17 的短路编译,否则会编译不过,失败默认返回 std::tuple<>
* @Index: 参数的偏移位置, 从0开始
* @Params: 参数
*/
template<size_t Index, typename... Params>
using TMakeTupleForwardOffset = typename std::disjunction<
	std::conditional_t<(sizeof...(Params) >= 1 && sizeof...(Params) == Index), default_type<std::tuple<Params...>>, std::false_type>,
	std::conditional_t<(sizeof...(Params) >= 1 && sizeof...(Params) > Index), TTupleForwardOffsetUtil<Index, Params...>, std::false_type>,
	default_type<std::tuple<>>
>::type;

//去除 const，不包括 const T&
template<size_t Index, typename... Params>
using TMakeTupleForwardOffset_NoConst = TMakeTupleForwardOffset<Index, std::remove_cv_t<Params>...>;

/*=====================================================================================================*/

//TTuple 没怎么测，可能有各种问题
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
		: _tupleIns(std::forward_as_tuple(Params...))
	{

	}

	explicit TTuple(const std::tuple<Types...>& InTuple)
		: _tupleIns(InTuple)
	{
	}

	template<typename FuncType, typename InTuple>
	decltype(auto) ApplyAfter(FuncType&& Func, std::tuple<InTuple>&& t) const
	{
		constexpr size_t totalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<decltype(_tupleIns)>;
		static_assert(totalSize >= 0, "Params Count Is Error");
		using TupleSequence = std::make_index_sequence<totalSize>;
		auto CallTuple = std::tuple_cat(_tupleIns, t);
		return Call(Func, CallTuple, TupleSequence{});
	}

	template<typename FuncType, typename InTuple>
	decltype(auto) ApplyBefor(FuncType&& Func, std::tuple<InTuple>&& t) const
	{
		constexpr size_t totalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<decltype(_tupleIns)>;
		static_assert(totalSize >= 0, "Params Count Is Error");
		using TupleSequence = std::make_index_sequence<totalSize>;
		auto CallTuple = std::tuple_cat(t, _tupleIns);
		return Call(Func, CallTuple, TupleSequence{});
	}

	template<typename FuncType, typename... ArgTypes>
	decltype(auto) ApplyAfter(FuncType&& Func, ArgTypes&&... Args) const
	{
		auto t = std::forward_as_tuple(std::forward<ArgTypes>(Args)...);
		constexpr size_t totalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<decltype(_tupleIns)>;
		static_assert(totalSize >= 0, "Params Count Is Error");
		using TupleSequence = std::make_index_sequence<totalSize>;
		auto CallTuple = std::tuple_cat(_tupleIns, t);
		return Call(Func, CallTuple, TupleSequence{});
	}

	template<typename FuncType, typename... ArgTypes>
	decltype(auto) ApplyBefor(FuncType&& Func, ArgTypes&&... Args) const
	{
		auto t = std::forward_as_tuple(std::forward<ArgTypes>(Args)...);
		constexpr size_t totalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<decltype(_tupleIns)>;
		static_assert(totalSize >= 0, "Params Count Is Error");
		using TupleSequence = std::make_index_sequence<totalSize>;
		auto CallTuple = std::tuple_cat(t, _tupleIns);
		return Call(Func, CallTuple, TupleSequence{});
	}


public:
	std::tuple<Types...> get()
	{
		return _tupleIns;
	}

private:
	// 	template<typename FuncType, size_t... Index, bool Nothing, typename... ArgTypes>
	// 	auto Invoke(FuncType&& Func, std::index_sequence<Index...>, Nothing Flag, ArgTypes&&... Args)
	// 		-> decltype(std::forward<FuncType>(Func)(std::get<Index>(_tupleIns)..., std::forward<ArgTypes>(Args)...))
	// 	{
	// 		return std::forward<FuncType>(Func)(std::get<Index>(_tupleIns)..., std::forward<ArgTypes>(Args)...);
	// 	}

	//使用 decltype(auto) 不能接受 void 返回值
	template<typename FuncType, typename InTuple, size_t... Index>
	auto Call(FuncType&& Func, InTuple&& t, std::index_sequence<Index...>) const
		-> decltype(std::forward<FuncType>(Func)(std::get<Index>(t)...))
	{
		return std::forward<FuncType>(Func)(std::get<Index>(t)...);
	}

	std::tuple<Types...> _tupleIns;
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
	auto t = std::make_tuple(std::forward<ParamTypes>(params)...);
	constexpr size_t TotalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<std::decay_t<TupleType>>;
	static_assert(TotalSize >= 0, "params count is error");
	using TupleSequence = std::make_index_sequence<TotalSize>;
	auto CallTuple = std::tuple_cat(tupleIns, std::move(t));
	return Invoke(std::forward<FuncType>(func), CallTuple, TupleSequence{});
}

template<typename FuncType, typename TupleType, typename... ParamTypes>
decltype(auto) InvokeBefor(FuncType&& func, TupleType&& tupleIns, ParamTypes&&... params)
{
	auto t = std::make_tuple(std::forward<ParamTypes>(params)...);
	constexpr size_t TotalSize = std::tuple_size_v<decltype(t)> + std::tuple_size_v<std::decay_t<TupleType>>;
	static_assert(TotalSize >= 0, "params count is error");
	using TupleSequence = std::make_index_sequence<TotalSize>;
	auto CallTuple = std::tuple_cat(std::move(t), tupleIns);
	return Invoke(std::forward<FuncType>(func), CallTuple, TupleSequence{});
}