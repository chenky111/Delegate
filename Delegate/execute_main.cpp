#include <iostream>
#include <stdlib.h>
#include <memory>
#include "Delegate.h"
#include "Common.h"
#include "newDelegate.h"
using namespace std;

void F1(int a, char b, int& c, char* d)
{
	cout << a << " , " << b << " , " << c << " , " << d;
	cout << endl;
	c = 90;
	*d = 'k';
}

void F2(int a, char* b)
{
	cout << a <<  ", " << b << endl;
}

int F3(int a, int& b)
{
	return a + b;
}

#if 1
int main()
{
// 	int a = 10;
// 	char b = 'y';
// 	int& c = a;
// 	char* d = &b;
// 
// 	cout << d << endl;
// 
// 	{
// 		TTuple<int, char> t1(a, b);
// 		t1.ApplyAfter(F1, c, d);
// 
// 		TTuple<int&, char*> t2(c, d);
// 		t2.ApplyBefor(F1, a, b);
// 	}
// 
// 	cout << a << " , " << b << " , " << c << " , " << d;
// 	cout << endl;

	{
		//目前BUG状况：
		//如果坚决使用 After 和 Before 2个函数，就有以下问题
		//1：无法使用引用类型的参数
		//2：无法构造时，就绑定参数，因为 Excute 直接使用 Params 参数包，参数个数会对不上
		//解决办法：
		//1：封装 tuple 类，把参数存到这个类里，里面再封装 After 和 Before 2个函数
		//2：如果还是无法保存引用类型，就放弃使用引用，改为使用 指针 或者 智能指针
		//也可以创建一个全局的 Make 工厂函数，或者类的静态成员函数，构造一个 tuple 的 Ins 实例，也许可以使用引用类型
		//可行的办法：
		//构造函数改为 tuple 类型，Create 的时候，完美转发一下，存储一个临时的 tuple ，然后构造时 std::move 把临时的 tuple 移动过去
		//Excute 可以继续使用 Params 的参数包类型，然后调用时，不执行 Excute ，需要再封装一个模板接口，通过传递另外一个参数包 和 tuple 解开后的参数，结合起来，和 Params 的参数相同，然后再调用 Excute
		//Excute 改为非虚函数，然后使用模板参数，接收外部参数，然后调用封装好的 After 和 Before 2个函数，然后使用 Call 函数为虚函数，函数参数，使用调用函数的模板参数列表类型
		//需要把 Args 的参数，传递到给 Params，否则构造 Args 的参数
		//变量是什么类型，模板就什么类型，所以如果变量是 int 类型，构造出来的就是 int 类型的 tuple，调用有引用的函数，引用的修改就无效了
		//或者使用 explicit，确保和函数类型要一致，而不是 int 转为 int&

		int a = 10;
		char b = 'y';
		int& c = a;
		char* d = &b;

		//using F1Type = decltype(F1);
		//TDelegate<F1Type> C1 = TDelegate<F1Type>::CreateStatic(F1, a, b, c, d);
		//C1.Excute(10, c, 20, &c);
		//C1.Excute();
		
		//char c2 = 'p';
		//auto t = make_tuple(10, 'c');
		//InvokeAfter(F1, t, a, &c2);
		////F1(10, b, a, d);
		//cout << a << endl;
	}

	//ok
	//{
	//	int c = 'c';
	//	using F1Type = decltype(F2);
	//	TDelegate<F1Type> C1 = TDelegate<F1Type>::CreateStatic(F2);
	//	C1.Excute(10, (char*)&c);
	//}
	
	//err，参数不能分开
	//{
	//	int a = 10;
	//	using FType = decltype(F3);
	//	TDelegate<FType> C1 = TDelegate<FType>::CreateStatic(F3, 10);
	//	C1.Excute(a);
	//}

	{
		int c = 'c';
		using FType = decltype(F2);
		TNewDelegate<FType> C1 = TNewDelegate<FType>::CreateStatic(F2);
		C1.Excute(10, (char*)&c);
	}

	system("pause");
	return 0;
}
#endif