/*
author:panzg
time:2015-10-22
description:ʵ���˻�����ioc���������붼���Խ��ϴ���C++11�飬�Լ�����һ��������
*/
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
#include "Any.hpp"   /*Any���ʵ��*/
#include "NonCopyable.hpp" /*���ɸ�����ӿ�*/

using namespace std;

/*����iocʵ�֣��ܴ������е��޲�����������*/
template<class T>
class IocContainer
{
public:
	IocContainer(){}
	~IocContainer(){}

	//ע����Ҫ���������캯������Ҫ����һ��Ψһ�ı�־���Ա��ں��洴������ʱ�������
	template<class Drived>
	void RegisterType(string strKey)
	{
		std::function<T*()> function = [] {return new Drived(); };
		RegisterType(strKey, function);
	}

	//����Ψһ�ı�־ȥ���Ҷ�Ӧ�Ĺ�������������ָ�����
	T* Resolve(string strKey)
	{
		if (m_creatorMap.find(strKey) == m_creatorMap.end())
			return nullptr;
		std::function<T*() > function = m_creatorMap[strKey];
		return function();
	}

	//��������ָ�����
	std::shared_ptr<T> ResolveShared(string strKey)
	{
		T* ptr = Resolve(strKey);
		return std::shared_ptr<T>(ptr);
	}
private:
	map<string, std::function<T*()>> m_creatorMap;
	void RegisterType(string strKey, std::function<T* ()> creator)
	{
		if (m_creatorMap.find(strKey) != m_creatorMap.end())
			throw std::invalid_argument("this key has already exist!");

		m_creatorMap.emplace(strKey, creator);
	}

};
/*����Any�������Ͳ��������Դ������е��޲����ӿ����͵Ķ���*/
class IocContainer_Any
{
public:
	IocContainer_Any(){}
	~IocContainer_Any(){}

	template<class T,typename Depend>
	void RegisterType(const string &strKey)
	{
		//ͨ���հ������˲�������
		std::function<T*()> function = []{return new T(new Depend()); };
		RegisterType(strKey, function);
	}
	template<class T>
	T* Resolve(const string &strKey)
	{
		if (m_creatorMap.find(strKey) == m_creatorMap.end())
		{
			return nullptr;
		}
		Any  resolver = m_creatorMap[strKey];
		//���鵽��anyתΪfunction
		std::function<T*()> function = resolver.AnyCast<std::function<T*()>>();
		return function();
	}
	template<class T>
	std::shared_ptr<T> ResolveShared(const string& strKey)
	{
		T* t = Resolve<T>(strKey);
		return std::shared_ptr<T>(t);
	}
private:
	void RegisterType(const string& strKey, Any constructor)
	{
		if (m_creatorMap.find(strKey)!=m_creatorMap.end())
		{
			throw std::invalid_argument("this key has already exist!");
		}
		//����any�����˲�ͬ���͵Ĺ�����
		m_creatorMap.emplace(strKey, constructor);
	}

	unordered_map<string, Any> m_creatorMap;
	
};

/*������IocContainer_Any�Ļ����ϣ�ͨ���ɱ����������˲��ܴ����������Ķ��������*/
class IocContainer_Final : NonCopyable
{
public:
	IocContainer_Final(void){}
	~IocContainer_Final(void){}

	template<class T, typename Depend, typename... Args>
	void RegisterType(const string& strKey)
	{
		std::function<T* (Args...)> function = [](Args... args){ return new T(new Depend(args...)); };//ͨ���հ������˲�������
		RegisterType(strKey, function);
	}

	template<class T, typename... Args>
	T* Resolve(const string& strKey, Args... args)
	{
		if (m_creatorMap.find(strKey) == m_creatorMap.end())
			returnnullptr;

		Any resolver = m_creatorMap[strKey];
		std::function<T* (Args...)> function = resolver.AnyCast<std::function<T* (Args...)>>();

		return function(args...);
	}

	template<class T, typename... Args>
	std::shared_ptr<T> ResolveShared(const string& strKey, Args... args)
	{
		T* t = Resolve<T>(strKey, args...);

		return std::shared_ptr<T>(t);
	}

private:
	void RegisterType(const string& strKey, Any constructor)
	{
		if (m_creatorMap.find(strKey) != m_creatorMap.end())
			throw std::invalid_argument("this key has already exist!");

		//ͨ��Any�����˲�ͬ���͵Ĺ�����
		m_creatorMap.emplace(strKey, constructor);
	}

private:
	unordered_map<string, Any> m_creatorMap;
};
/*���Ժ���*/
void IocContainer_test()
{
	struct ICar
	{
		virtual ~ICar(){}
		virtual void test() const = 0;
	};
	struct Bus :ICar
	{
		Bus(){}
		void test()const
		{
			std::cout << "Bus::test()";
		}
	};
	struct Car :ICar
	{
		Car(){}
		void test() const
		{
			std::cout << "Car::test()";
		}
	};

	IocContainer<ICar> carioc;
	carioc.RegisterType<Bus>("bus");
	carioc.RegisterType<Car>("car");

	std::shared_ptr<ICar> bus = carioc.ResolveShared("bus");
	bus->test();
	std::shared_ptr<ICar> car = carioc.ResolveShared("car");
	car->test();
}
/*IocContainer_Any ��Ĳ��Ժ���*/
void IocContainer_Any_Test()
{
	struct Bus
	{
		void Test() const{ std::cout << "Bus::Test()";}
	};
	struct Car
	{
		void Test() const{ std::cout << "Car::Test()"; }
	};
	struct Base
	{
		virtual void Func(){}
		virtual ~Base(){}
	};
	struct DerivedB :Base
	{
		void Func() override
		{
			cout << "call func in DerivedB" << endl;
		}
	};
	struct DerivedC :Base
	{
		void Func() override
		{
			cout << "call func in DerivedC" << endl;
		}
	};
	struct DerivedD :Base
	{
		void Func() override
		{
			cout << "call func in DerivedD" << endl;
		}
	};
	struct A
	{
		A(Base *ptr) :m_ptr(ptr)
		{
		}
		void Func()
		{
			m_ptr->Func();
		}
		~A()
		{
			if (m_ptr!=nullptr)
			{
				delete m_ptr;
				m_ptr = nullptr;
			}
		}
	private:
		Base * m_ptr;
	};

	IocContainer_Any ioc;
	ioc.RegisterType<A, DerivedB>("B");
	ioc.RegisterType<A, DerivedC>("C");
	ioc.RegisterType<A, DerivedD>("D");

	auto pa = ioc.ResolveShared<A>("B");
	pa->Func();
	auto pa1 = ioc.ResolveShared<A>("C");
	pa1->Func();

	/* ���������⣬ģ�岻���Ƶ�Depend
	ioc.RegisterType<Bus>("bus");
	ioc.RegisterType<Car>("car");*/
	auto bus = ioc.ResolveShared<Bus>("bus");
	bus->Test();
	auto car = ioc.ResolveShared<Car>("car");
	car->Test();

}
/*IocContainer_Final��Ĳ��Ժ���*/
void IocContainer_Final_Test()
{
	struct Base
	{
		virtual void Func(){}
		virtual ~Base(){}
	};

	struct DerivedB : public Base
	{
		DerivedB(int a, double b) :m_a(a), m_b(b)
		{
		}
		void Func()override
		{
			cout << m_a + m_b << endl;
		}
	private:
		int m_a;
		double m_b;
	};

	struct DerivedC : public Base
	{
	};

	struct A
	{
		A(Base * ptr) :m_ptr(ptr)
		{
		}

		~A()
		{
			if (m_ptr != nullptr)
			{
				delete m_ptr;
				m_ptr = nullptr;
			}
		}

	private:
		Base * m_ptr;
	};

	IocContainer_Final ioc;
	ioc.RegisterType<A, DerivedC>("C");      //����������ϵ
	auto c = ioc.ResolveShared<A>("C");

	ioc.RegisterType<A, DerivedB, int, double>("C");   //ע��ʱҪע��DerivedB�Ĳ���int��double
	auto b = ioc.ResolveShared<A>("C", 1, 2.0); //��Ҫ�������
	//��һ�д���������
	/* b->Func();*/
}

#if 1
int main()
{
	IocContainer_Final_Test();
	system("pause");
	return 0;
}
#endif