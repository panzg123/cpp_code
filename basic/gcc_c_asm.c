/*
 ============================================================================
 Name        : gcc_c_asm.c
 Author      : panzg
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

//整数相加的内嵌汇编
void test_case_add()
{
	int foo = 10, bar = 15;
	__asm__ __volatile__("addl  %%ebx,%%eax"
			:"=a"(foo)
			:"a"(foo), "b"(bar)
	);
	printf("foo+bar=%d\n", foo);
}

//字符串拷贝的内嵌汇编，多行汇编需要加上"\n\t"
static inline char * m_strcpy(char * dest,const char *src)
{
         int d0, d1, d2;
         __asm__ __volatile__(  "1:\t lodsb\n\t"              //1：只是一个跳转标志
                       "stosb \n\t"
                       "testb %%al,%%al\n\t"        //判断字符串是否复制结束
                       "jne 1b"                   //如果字符串未结束，跳转到1：处
                     : "=&S" (d0), "=&D" (d1), "=&a" (d2)
                     : "0" (src),"1" (dest)
                     : "memory");
         return dest;
}
//字符串拷貝的測試
void test_case_strcpy()
{
	char* str1="helloworld";
	char str2[11];
	m_strcpy(str2,str1);
	printf("%s\n",str2);
}
//printf內嵌汇编实现
void m_print()
{
	char* str="helloworld!\n";
	asm(" movl $13,%%edx \n\t"
			"movl $0,%%ecx \n\t"
			"movl $0,%%ebx \n\t"
			"movl $4,%%eax \n\t"
			::"r"(str):"edx","ecx","ebx");
}
//测试不成功
void test_case_printf()
{
	m_print();
}



int main(void)
{
	test_case_printf();
	return EXIT_SUCCESS;
}
