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
		//ĿǰBUG״����
		//������ʹ�� After �� Before 2��������������������
		//1���޷�ʹ���������͵Ĳ���
		//2���޷�����ʱ���Ͱ󶨲�������Ϊ Excute ֱ��ʹ�� Params ������������������Բ���
		//����취��
		//1����װ tuple �࣬�Ѳ����浽�����������ٷ�װ After �� Before 2������
		//2����������޷������������ͣ��ͷ���ʹ�����ã���Ϊʹ�� ָ�� ���� ����ָ��
		//Ҳ���Դ���һ��ȫ�ֵ� Make ����������������ľ�̬��Ա����������һ�� tuple �� Ins ʵ����Ҳ�����ʹ����������
		//���еİ취��
		//���캯����Ϊ tuple ���ͣ�Create ��ʱ������ת��һ�£��洢һ����ʱ�� tuple ��Ȼ����ʱ std::move ����ʱ�� tuple �ƶ���ȥ
		//Excute ���Լ���ʹ�� Params �Ĳ��������ͣ�Ȼ�����ʱ����ִ�� Excute ����Ҫ�ٷ�װһ��ģ��ӿڣ�ͨ����������һ�������� �� tuple �⿪��Ĳ���������������� Params �Ĳ�����ͬ��Ȼ���ٵ��� Excute
		//Excute ��Ϊ���麯����Ȼ��ʹ��ģ������������ⲿ������Ȼ����÷�װ�õ� After �� Before 2��������Ȼ��ʹ�� Call ����Ϊ�麯��������������ʹ�õ��ú�����ģ������б�����
		//��Ҫ�� Args �Ĳ��������ݵ��� Params�������� Args �Ĳ���
		//������ʲô���ͣ�ģ���ʲô���ͣ�������������� int ���ͣ���������ľ��� int ���͵� tuple�����������õĺ��������õ��޸ľ���Ч��
		//����ʹ�� explicit��ȷ���ͺ�������Ҫһ�£������� int תΪ int&

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
	
	//err���������ֿܷ�
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