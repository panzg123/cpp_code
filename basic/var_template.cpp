/*
author:panzg
time:2015-10-23
description:�ɱ���������Ϳɱ����ģ���� demo
*/
#include <iostream>
using namespace std;
template<class... T>

/*������һ���ɱ����ģ�庯���Ķ���*/
void f(T... args)
{
	cout << sizeof...(args) << endl;
}
void test_f()
{
	f(); //1������
	f(1); //2������
	f(1, 2); //3������
	f(1, 2.5, ""); //4������
}

/*�ݹ鷽ʽչ��ģ�庯���Ĳ�����*/
void func_print()
{
	cout << "empty" << endl;
}
template<class T,class... Args>
void func_print(T head, Args... rest)
{
	cout << "parameter:" << head << endl;
	func_print(rest...);
}
void test_func_print()
{
	func_print(1, 2, 3, 4);
}

/*�򵥿ɱ����ģ�����ʵ��*/
template<typename... Args> struct Sum;  /*��һ���֣�ǰ���������������sum����һ���ɱ����ģ����*/

template<typename First, typename... Rest> /*�ڶ����֣���Ķ���*/
struct Sum<First, Rest...>
{
	enum {value = Sum<First>::value + Sum<Rest...>::value};
};

template<typename Last>   /*�������֣��ػ��ĵݹ���ֹ��*/
struct Sum<Last>
{
	enum { value = sizeof(Last) };
};

void test_struct_sum()  /*���Ժ�����int+double+short �� 14���ֽ�*/
{
	cout <<Sum<int, double, short>::value << endl;
}

#if 1
int main()
{
	test_f();
	test_func_print();
	test_struct_sum();
	system("pause");
	return 0;
}
#endif