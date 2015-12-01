/*
file: Timer.hpp
author:panzg
time:2015-10-22
description:实现了简单的计时器
*/
#pragma  once
#include <chrono>
using namespace std;
using namespace std::chrono;

class panzg_timer
{
public:
	//构造函数
	panzg_timer() :m_begin(high_resolution_clock::now()){}
	//重置计时器
	void reset(){ m_begin = high_resolution_clock::now(); }

	//默认输出毫秒
	int64_t elapsed() const
	{
		return duration_cast<chrono::milliseconds>(high_resolution_clock::now() - m_begin).count();
	}
	//秒
	int64_t elapsed_second() const
	{
		//return duration_cast<chrono::seconds>(high_resolution_clock::now() - m_begin).count();
		return duration_cast<duration<double>>(high_resolution_clock::now() - m_begin).count();
	}
	//微妙
	int64_t elapsed_micro() const
	{
		return duration_cast<chrono::microseconds>(high_resolution_clock::now() - m_begin).count();
	}
	//纳秒
	int64_t elapsed_nano() const
	{
		return duration_cast<chrono::nanoseconds>(high_resolution_clock::now() - m_begin).count();
	}
	//分钟
	int64_t elapsed_minutes() const
	{
		return duration_cast<chrono::minutes>(high_resolution_clock::now() - m_begin).count();
	}
	//小时
	int64_t elapsed_hours() const
	{
		return duration_cast<chrono::hours>(high_resolution_clock::now() - m_begin).count();
	}
private:
	time_point<high_resolution_clock> m_begin;
};