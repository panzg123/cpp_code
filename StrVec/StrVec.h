//c++ primer,5th,StrVec
#ifndef STRVEC_H
#define STRVEC_H

#include <iostream>
#include <memory>
#include <utility>
#include <initializer_list>

// simplified implementation of the memory allocation strategy for a vector-like class
class StrVec {
public:
    // copy control members
    StrVec():
            elements(nullptr), first_free(nullptr), cap(nullptr) { }

    StrVec(const StrVec&);            // copy constructor
    StrVec &operator=(const StrVec&); // copy assignment

    StrVec(StrVec&&) noexcept;            // move constructor
    StrVec &operator=(StrVec&&) noexcept; // move assignment

    ~StrVec() noexcept;                   // destructor

    // additional constructor
    StrVec(std::initializer_list<std::string>);

    void push_back(const std::string&);  // copy the element
    void push_back(std::string&&);       // move the element

    // add elements
    size_t size() const { return first_free - elements; }
    size_t capacity() const { return cap - elements; }

    // iterator interface
    std::string *begin() const { return elements; }
    std::string *end() const { return first_free; }

    // operator functions covered in chapter 14
    StrVec &operator=(std::initializer_list<std::string>);

    std::string& operator[](std::size_t n)
    { return elements[n]; }

    const std::string& operator[](std::size_t n) const
    { return elements[n]; }

    // emplace member covered in chapter 16
    template <class... Args> void emplace_back(Args&&...);
private:
    static std::allocator<std::string> alloc; // 内存分配器

    // utility functions:
    //  used by members that add elements to the StrVec
    void chk_n_alloc()
    { if (size() == capacity()) reallocate(); }
    // used by the copy constructor, assignment operator, and destructor
    std::pair<std::string*, std::string*> alloc_n_copy
            (const std::string*, const std::string*);
    void free();             // 销毁元素并释放内存
    void reallocate();       // 分配更多内存并拷贝元素
    std::string *elements;   // 指向数组的首元素的指针
    std::string *first_free; // 指向数组的第一个空闲元素的指针
    std::string *cap;        // 指向数组尾后位置的指针
};

#include <algorithm>

inline
StrVec::~StrVec() noexcept { free(); }

inline
std::pair<std::string*, std::string*>
StrVec::alloc_n_copy(const std::string *b, const std::string *e)
{
    // 分配空间保存给定范围的元素
    auto data = alloc.allocate(e - b);
    
    // 通过uninitialized_copy来拷贝数据
    // 返回新分配空间的开始位置和拷贝的尾后位置
    return {data, uninitialized_copy(b, e, data)};
}

inline
StrVec::StrVec(StrVec &&s) noexcept  // 移动构造函数不应该抛出异常
// 成员初始化接管s中的资源
        : elements(s.elements), first_free(s.first_free), cap(s.cap)
{
    // 置空s中的数据成员
    s.elements = s.first_free = s.cap = nullptr;
}

// 拷贝构造函数
inline
StrVec::StrVec(const StrVec &s)
{
    // 调用alloc_n_copy分配与s一样大的空间
    auto newdata = alloc_n_copy(s.begin(), s.end());
    elements = newdata.first;
    first_free = cap = newdata.second;
}

//释放空间
inline
void StrVec::free()
{
    // 判断是否为空指针
    if (elements) {
        // 逆序销毁所有旧元素，需要先destory，然后deallocate
        for (auto p = first_free; p != elements; /* empty */)
            alloc.destroy(--p);
        // 释放空间
        alloc.deallocate(elements, cap - elements);
    }
}

inline
StrVec &StrVec::operator=(std::initializer_list<std::string> il)
{
    // alloc_n_copy allocates space and copies elements from the given range
    auto data = alloc_n_copy(il.begin(), il.end());
    free();   // destroy the elements in this object and free the space
    elements = data.first; // update data members to point to the new space
    first_free = cap = data.second;
    return *this;
}

inline
StrVec &StrVec::operator=(StrVec &&rhs) noexcept
{
    // 检测自赋值
    if (this != &rhs) {
        free();                   // 释放已存在的元素
        elements = rhs.elements;  // 接管rhs中的资源
        first_free = rhs.first_free;
        cap = rhs.cap;
        // 置空rhs中的资源
        rhs.elements = rhs.first_free = rhs.cap = nullptr;
    }
    return *this;
}

inline
StrVec &StrVec::operator=(const StrVec &rhs)
{
    // 分配rhs大小的空间，并拷贝
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    free();
    elements = data.first;
    first_free = cap = data.second;
    return *this;
}

inline
void StrVec::reallocate()
{
    // 计算空间大小，目前2倍的空间
    auto newcapacity = size() ? 2 * size() : 1;

    // 分配空间
    auto newdata = alloc.allocate(newcapacity);

    // 拷贝数据
    auto dest = newdata;  // points to the next free position in the new array
    auto elem = elements; // points to the next element in the old array
    for (size_t i = 0; i != size(); ++i)
        alloc.construct(dest++, std::move(*elem++));//使用move语义，避免string的拷贝

    free();  // 释放旧的空间

    // 更新数据结构
    elements = newdata;
    first_free = dest;
    cap = elements + newcapacity;
}

inline
StrVec::StrVec(std::initializer_list<std::string> il)
{
    // call alloc_n_copy to allocate exactly as many elements as in il
    auto newdata = alloc_n_copy(il.begin(), il.end());
    elements = newdata.first;
    first_free = cap = newdata.second;
}

// push_back版本1，提供const的左值引用，拷贝
inline
void StrVec::push_back(const std::string& s)
{
    chk_n_alloc(); // 确保有空间容纳新元素
    // 在first_free指向的元素中构造s的副本
    alloc.construct(first_free++, s);
}

// push_back版本2，提供非const的右值引用，移动
inline
void StrVec::push_back(std::string &&s)
{
    chk_n_alloc(); // 确保空间可容纳
    alloc.construct(first_free++, std::move(s));
}


// emplace member covered in chapter 16
template <class... Args>
inline
void StrVec::emplace_back(Args&&... args)
{
    chk_n_alloc(); // reallocates the StrVec if necessary
    alloc.construct(first_free++, std::forward<Args>(args)...);
}

#endif