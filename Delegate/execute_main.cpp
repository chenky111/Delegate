#include <iostream>
#include <stdlib.h>
#include <memory>
#include "Delegate.h"
#include "newDelegate.h"

using namespace std;

void F0()
{
	DEBUG_LOG("is ok");
}

void F1(int a, char b, int& c, char* d)
{
	DEBUG_LOG(a, b, c, *d);
	c = 90;
	*d = 'k';
}

void F2(int a, char* b)
{
	DEBUG_LOG(a, b);
}

int F3(int a, int& b)
{
	b = 99;
	return a + b;
}

int F4(int a, int& b)
{
	b = 500;
	return a + b;
}

class Ca
{
public:
	void f1() { DEBUG_LOG("f1 call"); }
	void f2(int a) { DEBUG_LOG("f2 cal: ", a); };
	void f3(int&& a) { DEBUG_LOG("f3 cal: ", a); };
	void f4(int& a, int&& b) { DEBUG_LOG("f4 cal: ", a, b); };
	void f5(int a, int& b, int&& c) { DEBUG_LOG("f5 cal: ", a, b, c); };
	int f6(int a, double& b, char c, const std::string& str) const { DEBUG_LOG("f6 cal: ", a, b, c, str); return a + 300; }
	static int f7(int a, const char& b) { DEBUG_LOG("f7 cal", a, b); return a % 20; }
};

int main()
{
#if 0
	int a = 10;
	int& b = a;
	using FType = decltype(F3);
	TNewDelegate<FType> D1 = TNewDelegate<FType>::CreateStatic(F3, a);
	auto D2 = D1;
	D1.setParamters<int, int&>(a, b);
	int c = D1.Excute();
	DEBUG_LOG(a, b, c);

	a = 20;
	D1 = TNewDelegate<FType>::CreateStatic(F4);
	c = D1.ExcuteEx<int, int&>(a, b);
	DEBUG_LOG(a, b, c);

	WARNING_LOG("*********");
	D1 = TNewDelegate<FType>::CreateStatic(F3);
	D1.setParamters<int&>(b);
	DEBUG_LOG(D1.ExcuteAfter(a));
	DEBUG_LOG(D1.ExcuteAfterEx<int>(a));

	WARNING_LOG("*********");
	char p = 's';
	using FType2 = decltype(F1);
	TNewDelegate<FType2> D3 = TNewDelegate<FType2>::CreateStatic(F1);
	D3.setParamters<int&, char*>(b, &p);
	D3.ExcuteAfter(a, p);
	ERROR_LOG(b, p);

#elseif 0
	int a = 10;
	int& b = a;
	using FType = decltype(F3);
	TDelegate<FType> D1 = TDelegate<FType>::CreateStatic(F3, a);
	auto D2 = D1;
	D1.setParamters<int, int&>(a, b);
	int c = D1.Excute();
	DEBUG_LOG(a, b, c);

	a = 20;
	D1 = TDelegate<FType>::CreateStatic(F4);
	c = D1.ExcuteEx<int, int&>(a, b);
	DEBUG_LOG(a, b, c);
	
	WARNING_LOG("*********");
	D1 = TDelegate<FType>::CreateStatic(F3);
	D1.setParamters<int&>(b);
	DEBUG_LOG(D1.ExcuteAfter(a));
	DEBUG_LOG(D1.ExcuteAfterEx<int>(a));
	
	WARNING_LOG("*********");
	char p = 's';
	using FType2 = decltype(F1);
	TDelegate<FType2> D3 = TDelegate<FType2>::CreateStatic(F1);
	D3.setParamters<int&, char*>(b, &p);
	D3.ExcuteAfter(a, p);
	ERROR_LOG(b, p);

#elseif 0
	Ca a;
	int ia = 30;
	int& ib = ia;
	double ic = 20.f;

	using FType1 = void();
	auto D1 = TDelegate<FType1>::CreateMemberFunc(&a, &Ca::f1);
	D1.Excute();

	using FType2 = void(int);
	auto D2 = TDelegate<FType2>::CreateMemberFunc(&a, &Ca::f2);
	D2.Excute(20);

	using FType3 = void(int&&);
	auto D3 = TDelegate<FType3>::CreateMemberFunc(&a, &Ca::f3);
	D3.ExcuteEx<int&&>(20);

	using FType4 = void(int&, int&&);
	auto D4 = TDelegate<FType4>::CreateMemberFunc(&a, &Ca::f4);
	D4.setParamters<int&>(ia);
	D4.ExcuteEx<int&&>(20);

	using FType5 = void(int, int&, int&&);
	auto D5 = TDelegate<FType5>::CreateMemberFunc(&a, &Ca::f5, 90);
	D5.ExcuteEx<int&, int&&>(ia, 200);

	using FType6 = int(int, double&, char, const std::string&);
	auto D6 = TDelegate<FType6>::CreateMemberFunc(&a, &Ca::f6);
	D6.setParamters<int, double&>(ia, ic);
	WARNING_LOG(D6.ExcuteEx<char, std::string>('a', "str"));

	using FType7 = int(int, const char&);
	auto D7 = TDelegate<FType7>::CreateStatic(&Ca::f7, ia);
	WARNING_LOG(D7.ExcuteEx<const char&>('s'));

#else
	auto lam1 = []() { DEBUG_LOG("lam"); };
	using FType1 = void(void);
	auto D1 = TDelegate<FType1>::CreateLambda(lam1);
	D1.Excute();

	char b = 'c';
	auto lam2 = [](int a, char& b) { DEBUG_LOG("lam2", a, b); b = 'g'; };
	using FType2 = void(int, char&);
	auto D2 = TDelegate<FType2>::CreateLambda(lam2, 20);
	D2.ExcuteEx<char&>(b);
	DEBUG_LOG(b);

	double f = 58.f;
	auto lam3 = [&f](int a) { DEBUG_LOG("lam3", a); f = 98.f; };
	using FType3 = void(int);
	auto D3 = TDelegate<FType3>::CreateLambda(lam3, 20);
	D3.Excute();
	DEBUG_LOG(f);

	int ib = 50;
	using FType4 = double(int, const int);
	auto D4 = TDelegate<FType4>::CreateLambda([&f](int a, const int b) {
		f = 202.22f;
		return f;
	}, 20);
	DEBUG_LOG(D4.ExcuteEx<const int>(ib));
	//D4.setParamters<const int>(99);
	//DEBUG_LOG(D4.ExcuteAfter(88));

	using FType5 = void(const int, const char&, const int&);
	auto D5 = TDelegate<FType5>::CreateLambda([](const int a, const char& b, const int& c) { DEBUG_LOG("lam s"); });
	D5.setParamters<const int, const char&>(20, 's');
	D5.ExcuteEx<const int&>(90);

#endif



	system("pause");
	return 0;
}

