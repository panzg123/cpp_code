//============================================================================
// Name        : pstring_test.cpp
// Author      : panzg
// Version     :
// Copyright   : Your copyright notice
// Description : 对p_string的测试
//============================================================================

#include <iostream>
#include<vector>
#include"../src/p_string.h"
using namespace std;

void foo(p_string x)
{
}

void bar(const p_string& x)
{
}

p_string baz()
{
	p_string ret("world");
	return ret;
}
#if 0
int main()
{
	p_string s0;
	cin>>s0;
	cout<<s0<<endl;
	p_string s1("hello");
	p_string s2(s0);
	p_string s3 = s1;
	s2 = s1;

	cout<<s1<<endl;
	cout<<s3.size()<<endl;

	foo(s1);
	bar(s1);
	foo("temporary");
	bar("temporary");
	p_string s4 = baz();

	std::vector<p_string> svec;
	svec.push_back(s0);
	svec.push_back(s1);
	svec.push_back(baz());
	svec.push_back("good job");
}
#endif
