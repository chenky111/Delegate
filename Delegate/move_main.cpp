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

#if 1
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

	}


	{
		auto lm = [](int a) { DEBUG_LOG(a); };
		using F = void(int);
		auto l = TDelegate<F>::CreateLambda(lm, 90);
		auto l2 = std::move(l);
		l2.setParamters(88);
		//l.Excute();
		l2.Excute();
	}


	system("pause");
	return 0;
}
#endif
