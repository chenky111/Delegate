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
	TestClass operator=(const TestClass& other) { WARNING_LOG("is copy ="); return TestClass(); }

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
		D1.ExcuteAfter();
	}

	ERROR_LOG("========================");
	{
		auto r = RunTimeTest();


		//auto func = std::bind(CT1, std::placeholders::_1, t2);
		//func(t);

		auto func = std::bind(CT2, t);
		func();
	}


	system("pause");
	return 0;
}
#endif