#include <iostream>
#include <stdlib.h>
#include <memory>
#include "Delegate.h"
using namespace std;

void Func()
{
	cout << "Is Func Call" << endl;
}

void Func2(int a, char b)
{
	cout << "Is Func2 Call " << a << " , " << b << endl;
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
	void F1() { cout << " TestClass F1" << endl; };
	void F2() const { cout << " TestClass F1 const" << endl; }
	int F3(int a) { return a - 5; }
	int F4(int a) const { return a - 5; }
};

#if 0
int main()
{
	cout << "========= F1 ==========" << endl;
	using F1Type = void(void);
	TDelegate<F1Type> C1 = TDelegate<F1Type>::CreateBase(Func);
	C1.ExcuteIfSave();

	cout << endl << "========= F2 ==========" << endl;
	using F2Type = void(int, char);
	TDelegate<F2Type> C2 = TDelegate<F2Type>::CreateBase(Func2, 10, 'c');
	C2.ExcuteIfSave();

	cout << endl << "========= F3 ==========" << endl;
	using F3Type = int(int, int);
	TDelegate<F3Type> C3 = TDelegate<F3Type>::CreateBase(Func3, 10, 20);
	cout << C3.ExcuteIfSave() << endl;

	cout << endl << "========= F4 ==========" << endl;
	int f4_a = 0;
	char f4_b = 'p';
	cout << "F4 Befor: " << f4_a << " , " << f4_b << endl;
	using F4Type = int(int&, char&);
	TDelegate<F4Type> C4 = TDelegate<F4Type>::CreateBase(Func4, f4_a, f4_b);
	cout << C4.ExcuteIfSave() << endl;
	cout << "F4 After: " << f4_a << " , " << f4_b << endl;

	cout << endl << "========= F5 ==========" << endl;
	cout << "F5 Befor: " << f4_a << " , " << f4_b << endl;
	using F5Type = int(int*, char*);
	TDelegate<F5Type> C5 = TDelegate<F5Type>::CreateBase(Func5, &f4_a, &f4_b);
	cout << C5.ExcuteIfSave() << endl;
	cout << "F5 After: " << f4_a << " , " << f4_b << endl;

	cout << endl << "========= F6 ==========" << endl;
	int f6_a = 5;
	int&& f6_b = 45;
	int f6_c = 80;
	using F6Type = decltype(Func6);
	cout << typeid(F6Type).name() << endl;
	TDelegate<F6Type> C6 = TDelegate<F6Type>::CreateBase(Func6, f6_a, f6_b, f6_c);
	cout << C6.ExcuteIfSave() << endl;


	//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	cout << endl;
	TestClass obj;

	cout << endl << "========= FM1 ==========" << endl;
	using FM1Type = void(void);
	TDelegate<FM1Type> CM1 = TDelegate<FM1Type>::CreateMemberFunc(&obj, &TestClass::F1);
	CM1.Excute();

	cout << endl << "========= FM2 ==========" << endl;
	using FM2Type = void(void);
	TDelegate<FM2Type> CM2 = TDelegate<FM2Type>::CreateMemberFunc(&obj, &TestClass::F2);
	CM2.Excute();

	cout << endl << "========= FM3 ==========" << endl;
	using FM3Type = int(int);
	TDelegate<FM3Type> CM3 = TDelegate<FM3Type>::CreateMemberFunc(&obj, &TestClass::F3, 90);
	cout << CM3.Excute() << endl;

	cout << endl << "========= FM4 ==========" << endl;
	using FM4Type = int(int);
	TDelegate<FM4Type> CM4 = TDelegate<FM4Type>::CreateMemberFunc(&obj, &TestClass::F4, 90);
	cout << CM4.ExcuteIfSave() << endl;

	//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

	cout << endl << "========= L1 ==========" << endl;
	using L1Type = void(void);
	TDelegate<L1Type> L1 = TDelegate<L1Type>::CreateLambda([]() {
		cout << "L1 Is Call" << endl;
	});
	L1.Excute();

	cout << endl << "========= L2 ==========" << endl;
	using L2Type = void(int, char);
	TDelegate<L2Type> L2 = TDelegate<L2Type>::CreateLambda([](int a, char b) {
		cout << "L2 Is Call" << endl;
		cout << a + b << endl;
	}, 10, 'y');
	L2.Excute();

	cout << endl << "========= L3 ==========" << endl;
	using L3Type = int(int, char);
	TDelegate<L3Type> L3 = TDelegate<L3Type>::CreateLambda([](int a, char b) {
		cout << "L3 Is Call" << endl;
		return a + b;
	}, 10, 'y');
	cout << L3.Excute() << endl;

	cout << endl << "========= L4 ==========" << endl;
	int la = 10;
	char lb = 'y';
	using L4Type = int(int&, char&);
	TDelegate<L4Type> L4 = TDelegate<L4Type>::CreateLambda([](int& a, char& b) {
		cout << "L4 Is Call" << endl;
		a = 30;
		b = 'c';
		return a + b;
	}, la, lb);
	cout << L4.Excute() << endl;
	cout << la << " , " << lb << endl;

	cout << endl << "========= L5 ==========" << endl;
	using L5Type = int(int&, int, char&);
	TDelegate<L5Type> L5 = TDelegate<L5Type>::CreateLambda([](int& a, int t, char& b) {
		cout << "L5 Is Call" << endl;
		a = 80;
		b = 'd';
		return a + t + b;
	}, la, 20, lb);
	cout << L5.Excute() << endl;
	cout << la << " , " << lb << endl;

	cout << endl << "========= L6 ==========" << endl;
	using L6Type = int(void);
	TDelegate<L6Type> L6 = TDelegate<L6Type>::CreateLambda([&]() {
		cout << "L6 Is Call" << endl;
		return la + lb;
	});
	cout << L6.Excute() << endl;

	cout << endl << "========= L7 ==========" << endl;
	int lc = 90;
	using L7Type = int(int&);
	TDelegate<L7Type> L7 = TDelegate<L7Type>::CreateLambda([&](int& c) {
		cout << "L7 Is Call" << endl;
		c = 40;
		return la + lb + c;
	}, lc);
	cout << L7.ExcuteIfSave() << endl;
	cout << lc << endl;

	system("pause");
	return 0;
}
#endif