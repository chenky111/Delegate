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


void R1(int& a)
{
	DEBUG_LOG("int&", a);
}

void R1(const int& a)
{
	DEBUG_LOG("const int&", a);
}

void R1(int&& a)
{
	DEBUG_LOG("int&&", a);
}


template<typename T>
void ward(T&& a)
{
	R1(std::forward<T>(a));
}

#if 0
int main()
{
	TestClass t;
	TestClass t2;

	{
		RunTimeTest();
		//std::function<void()> func = std::bind(Func);
		//func();
		//func = std::bind(&TestClass::F1, &t);
		//func();
		//func = std::bind(&TestClass::F2, &t);
		//func();

		auto func = std::bind(CT1, std::placeholders::_1, t2);
		func(t);
	}

	ERROR_LOG("========================");
	{
		RunTimeTest();
		//auto D1 = FSimpleDelegate::CreateBase(Func);
		//D1.Excute();
		//D1 = FSimpleDelegate::CreateMemberFunc(&t, &TestClass::F1);
		//D1.Excute();
		//D1 = FSimpleDelegate::CreateMemberFunc(&t, &TestClass::F2);
		//D1.Excute();

		auto D1 = TDelegate<decltype(CT11)>::CreateStatic(CT11);
		D1.setParamters<TestClass&>(t2);
		D1.Excute(t);
		DEBUG_LOG(t._a, t2._a);
	}

	{
		int a = 10;
		int& b = a;
		int&& c = 20;
		const int d = 30;
		const int& e = 40;
		ward(a);
		ward(b);
		ward(c);
		ward(d);
		ward(e);
		ward(30);
	}

	system("pause");
	return 0;
}
#endif