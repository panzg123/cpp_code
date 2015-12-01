#pragma  once
class NonCopyable
{
	NonCopyable(const NonCopyable&) = delete; //����
	NonCopyable& operator = (const NonCopyable&) = delete; //����
	NonCopyable() = default;
	~NonCopyable() = default;
};