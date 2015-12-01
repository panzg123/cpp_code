/*
author:panzg
time:2015-10-22
description:ʵ����dll�İ����࣬����dll�����ĵ��ã����붼���Խ��ϴ���C++11�飬�Լ�����һ��������
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
	T��ģ�����
	�磺 auto fMax = GetFunction<int(int,int)>("Max")
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
	ͨ��result_of�������������ֵ��ͳһ������
	ͨ���ɱ���������β�ͳһ������
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
