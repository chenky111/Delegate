#include "Delegate.h"
#include <functional>
using namespace std;

void Func()
{
	DEBUG_LOG("Is Func Call");
}

void Func2(int a, char b)
{
	DEBUG_LOG("Is Func2 Call", a, b);
}

int Func3(int a, int b)
{
	return a + b;
}

int Func4(int& ra, char& rb)
{
	ra = 190;
	rb = 'j';
	return ra + rb;
}

int Func44(int& ra, char& rb)
{
	ra = 190;
	rb = 'j';
	return ra + rb;
}

int Func5(int* ra, char* rb)
{
	*ra = 90;
	*rb = 'b';
	return *ra + *rb;
}

char Func6(int& ra, const int& rb, int c)
{
	ra = 10;
	c = 30;
	return ra + rb + c;
}

class TestClass
{
public:
	void F1() { DEBUG_LOG("TestClass F1"); };
	void F2() const { DEBUG_LOG("TestClass F1 const"); }
	int F3(int a) { return a - 5; }
	int F4(int a) const { return a - 5; }

	TestClass() = default;
	TestClass(const TestClass& other) { WARNING_LOG("is copy"); }
	TestClass& operator=(const TestClass& other) { WARNING_LOG("is copy ="); return *this; }

public:
	int _a = 0;
};

void CT1(const TestClass& tc, const TestClass& tc2)
{
	int a = tc._a + tc2._a;
}

void CT11(TestClass& tc, TestClass& tc2)
{
	tc._a = 90;
	tc2._a = 100;
}

void CT2(const TestClass tc)
{
	int a = tc._a + 1;
}

#if 0
int main()
{
	TestClass t;
	TestClass t2;

	{
		auto r = RunTimeTest();

		//auto D1 = TDelegate<decltype(CT11)>::CreateStatic(CT11);
		//D1.setParamters<TestClass&>(t2);
		//D1.Excute(t);
		//DEBUG_LOG(t._a, t2._a);

		auto D1 = TDelegate<decltype(CT2)>::CreateStatic(CT2, t);
		//D1.setParamters<TestClass&>(t2);
		D1.ExecuteAfter();
	}

	ERROR_LOG("========================");
	{
		auto r = RunTimeTest();


		//auto func = std::bind(CT1, std::placeholders::_1, t2);
		//func(t);

		auto func = std::bind(CT2, t);
		func();
	}

	{
		auto tPtr = std::make_shared<TestClass>();
		auto D1 = TDelegate<int(int)>::CreateSharePtr(tPtr, &TestClass::F3);
		DEBUG_LOG(D1.Execute(20));
	}



	system("pause");
	return 0;
}

#else

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

#endif