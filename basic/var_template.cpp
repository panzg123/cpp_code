/*
author:panzg
time:2015-10-23
description:可变参数函数和可变参数模板类 demo
*/
#include <iostream>
using namespace std;
template<class... T>

/*下面是一个可变参数模板函数的定义*/
void f(T... args)
{
	cout << sizeof...(args) << endl;
}
void test_f()
{
	f(); //1个参数
	f(1); //2个参数
	f(1, 2); //3个参数
	f(1, 2.5, ""); //4个参数
}

/*递归方式展开模板函数的参数包*/
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

/*简单可变参数模板类的实现*/
template<typename... Args> struct Sum;  /*第一部分：前向声明，申明这个sum类是一个可变参数模板类*/

template<typename First, typename... Rest> /*第二部分：类的定义*/
struct Sum<First, Rest...>
{
	enum {value = Sum<First>::value + Sum<Rest...>::value};
};

template<typename Last>   /*第三部分：特化的递归终止类*/
struct Sum<Last>
{
	enum { value = sizeof(Last) };
};

void test_struct_sum()  /*测试函数，int+double+short 共 14个字节*/
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