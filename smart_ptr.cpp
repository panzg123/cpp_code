#include <iostream>
using namespace std;


template <typename T>
class smart_ptr 
{
public:
	explicit smart_ptr():p(nullptr),count(nullptr){}//�����ָ��
	explicit smart_ptr(T*);            //����ָͨ���ʼ������ָ��
	smart_ptr(smart_ptr&);            //��������
	smart_ptr& operator=(smart_ptr&); //��ֵ
	T* operator->();                    //ָ�������
	T& operator*();                     //�����������
	~smart_ptr();                       //�Զ�����������
	void reset(T* ptr);                 //reset pointer
	int use_count();                    //�������ü���
private:
	int *count;                         //���ü���
	T *p;                               //����ָ��ײ㱣�ܵ�ָ��
};

template <typename T>
smart_ptr<T>::smart_ptr(T *p) : count(new int(1)), p(p) 
{
}

template <typename T>
smart_ptr<T>::smart_ptr(smart_ptr &sp) : count(&(++*sp.count)), p(sp.p)  
{
}

template <typename T>
T* smart_ptr<T>::operator->() 
{
	return p;
}

template <typename T>
T& smart_ptr<T>::operator*() 
{
	return *p;
}

template <typename T>
smart_ptr<T>& smart_ptr<T>::operator=(smart_ptr& sp) 
{
	++*sp.count;
	if (--*count == 0) 
	{
		delete count;
		delete p;
		p = nullptr;
		count = nullptr;
	}
	this->p = sp.p;
	this->count = sp.count;
	return *this;
}

template <typename T>
smart_ptr<T>::~smart_ptr() 
{
	if (--*count == 0) 
	{
		delete count;
		delete p;
		p = nullptr;
		count = nullptr;
	}
}


template <typename T>
int smart_ptr<T>::use_count()
{
	return *count;
}

template <typename T>
void smart_ptr<T>::reset(T* ptr)
{
	//TODO:����д�Ƿ���ȷ
	if (p && *count <= 1)
	{
		delete p;
		delete count;
	}
	else
		--*count;
	p = ptr;
	count = new int(1);
}

void test_case3()
{

	smart_ptr<string> pstr(new string("hello"));
	cout << pstr.use_count()<< endl;
	smart_ptr<string> pstr2(pstr);
	cout << pstr.use_count() << " " << pstr2.use_count() << endl;
	pstr2.reset(new string("world"));
	cout << pstr.use_count() << " "<< pstr2.use_count() << endl;
}



int main()
{
	test_case3();
	getchar();
	return 0;
}