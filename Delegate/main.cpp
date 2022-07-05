#include "Delegate.h"

using namespace std;

class BT
{
public:
	int Call(int a, char b, const int c, int& d, const int& e)
	{
		DEBUG_LOG(a, b, c, d, e);
		d = a + e;
		return c + d;
	}

	int Call2(int a, char b, const int c, int& d, const int& e)
	{
		return 10;
	}
};

int main()
{
	int rI = 6;
	auto tPtr = std::make_shared<BT>();
	using FT1 = int(int, char, const int, int&, const int&);
	auto D1 = TDelegate<FT1>::CreateSharePtr(tPtr, &BT::Call, 30, 'd');
	WARNING_LOG(D1.Execute(90, rI, 70));
	DEBUG_LOG(rI);

	D1.setParamters<int&, const int&>(rI, 40);
	WARNING_LOG(D1.ExecuteAfter(10, 'g', 88));
	DEBUG_LOG(rI);

	auto D2 = D1;
	D2.Count();

	D2 = TDelegate<FT1>::CreateSharePtr(tPtr, &BT::Call2);
	DEBUG_LOG(D2.Execute(0, '1', 0, rI, 0));
	D1.Count();

	system("pause");
	return 0;
}