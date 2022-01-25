#include <iostream>
#include <stdlib.h>
#include <memory>
#include "Delegate.h"
using namespace std;

void Func1()
{
	cout << "is Func1" << endl;
}

int Func2(int a = 10)
{
	return a;
}

class TA
{
public:
	void Call()
	{
		cout << "TA Call" << endl;
	}
};

template<typename T, size_t... Index>
void PT(const T& InTuple, std::index_sequence<Index...>)
{
	cout << "++++++++++++" << endl;
	std::initializer_list<char>{
		(
			cout << std::get<Index>(InTuple) << endl, 0)...
	};
}

#if 0
int main()
{

	{
		FSimpleDelegate f1 = FSimpleDelegate::CreateBase(Func1);
		//FSimpleDelegate f2 = f1;
		f1.Excute();

		cout << "========================" << endl;
		f1 = FSimpleDelegate::CreateLambda([]() { cout << "Lambda" << endl; });
		f1.Excute();
		f1.Count();

		cout << "========================" << endl;
		f1 = FSimpleDelegate::CreateLambda([]() { cout << "Lambda2" << endl; });
		f1.Excute();
		f1.Count();

		cout << "========================" << endl;
		FSimpleDelegate f2;
		f2 = f1;
		f2.Count();
		f1.Count();

		cout << "========================" << endl;
		FSimpleDelegate f3 = FSimpleDelegate::CreateLambda([]() { cout << "Lambda3" << endl; });
		f2 = f3;
		f2.Count();
		f2.Excute();

		cout << "========" << endl;
		TA a;
		f3 = FSimpleDelegate::CreateMemberFunc(&a, &TA::Call);
		f3.Excute();
		f3.Count();
		f2.Excute();
		f2.Count();

		//测试tuple参数是否一致

	}


	// 	int a = 10;
	// 	int a1 = 20;
	// 	int a2 = 30;
	// 	int* b = &a1;
	// 	int& c = a2;
	// 	tuple<int&, int*, int> t1 = std::forward_as_tuple(c, b, a);
	// 	constexpr size_t len = std::tuple_size<decltype(t1)>::value;
	// 	using SquenceTuple = std::make_index_sequence<len>;
	// 
	// 	{
	// 		auto t2 = std::move(t1);
	// 
	// 		std::get<0>(t1) = 51;
	// 		int* p1 = (std::get<1>(t1));
	// 		*p1 = 52;
	// 		std::get<2>(t1) = 53;
	// 
	// 		PT(t1, SquenceTuple{});
	// 		PT(t2, SquenceTuple{});
	// 	}
	// 	PT(t1, SquenceTuple{});


	system("pause");
	return 0;
}
#endif
