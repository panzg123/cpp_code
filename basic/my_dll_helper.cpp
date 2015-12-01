/*
author:panzg
time:2015-10-22
description:实现了dll的帮助类，简化了dll函数的调用，代码都来自江南大大的C++11书，自己敲了一遍便于理解
*/

#include <Windows.h>
#include <string>
#include <map>
#include <functional>
using namespace std;

class Dll_Helper
{
public:
	Dll_Helper() :m_hMod(nullptr)
	{

	}
	~Dll_Helper()
	{
		UnLoad();
	}
	bool Load(const string& dllPath)
	{
		m_hMod = LoadLibraryA(dllPath.data());
		if (m_hMod == nullptr)
		{
			printf("LoadLibrary failed!\n");
			return false;
		}
		return true;
	}

	bool UnLoad()
	{
		if (m_hMod == nullptr)
			return true;
		auto b = FreeLibrary(m_hMod);
		if (!b)
			return false;
		m_hMod = nullptr;
		return true;
	}
	/*
	T是模板参数
	如： auto fMax = GetFunction<int(int,int)>("Max")
	auto fGet = GetFunction<int(int)>("Get")
	*/
	template<typename T>
	std::function<T> GetFunction(const string& funcName)
	{
		auto it = m_map.find(funcName);
		if (it ==m_map.end())
		{
			auto addr = GetProcAddress(m_hMod, funcName.c_str());
			if (!addr)
				return nullptr;
			m_map.insert(std::make_pair(funcName, addr);
			it = m_map.find(funcName);
		}
		return std::function<T>((T*)(it->second));
	}

	/*
	通过result_of，解决函数返回值不统一的问题
	通过可变参数解决入参不统一的问题
	*/
	template<typename T,typename... Args>
	typename std::result_of<std::function<T>(Args...)>::type ExecuteFunc(const string& funcName, Args&&... args)
	{
		auto f = GetFunction<T>(funcName);
		if (f == nullptr)
		{
			string s = "can not find this function " + funcName;
			throw std::exception(s.c_str());
		}
		return f(std::forward<Args>(args)...);
	}

private:
	HMODULE m_hMod;
	std::map<string, FARPROC> m_map;
};
