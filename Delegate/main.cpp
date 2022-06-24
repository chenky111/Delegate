#include "Delegate.h"
#include <functional>
#include <chrono>
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

void CT2(const TestClass tc)
{
	int a = tc._a + 1;
}

#define TestCall
#if 1
int main()
{
	TestClass t;
	TestClass t2;

	auto nowTime = std::chrono::system_clock::now();
	//auto D1 = FSimpleDelegate::CreateBase(Func);
	//D1.Excute();
	//D1 = FSimpleDelegate::CreateMemberFunc(&t, &TestClass::F1);
	//D1.Excute();
	//D1 = FSimpleDelegate::CreateMemberFunc(&t, &TestClass::F2);
	//D1.Excute();

	auto D1 = TDelegate<void(const TestClass)>::CreateStatic(CT2, t2);
	D1.ExcuteP();
	auto endTime = std::chrono::system_clock::now();
	auto duration = endTime - nowTime;
	DEBUG_LOG(duration.count());

	nowTime = std::chrono::system_clock::now();
	//std::function<void()> func = std::bind(Func);
	//func();
	//func = std::bind(&TestClass::F1, &t);
	//func();
	//func = std::bind(&TestClass::F2, &t);
	//func();
	endTime = std::chrono::system_clock::now();
	duration = endTime - nowTime;
	DEBUG_LOG(duration.count());

	//int _a = 10;
	//char _b = 'p';
	//int& a = _a;
	//char& b = _b;
	//auto f2 = std::bind(Func4, _a, _b);
	//f2();
	//DEBUG_LOG(_a, _b);

	system("pause");
	return 0;
}
#endif