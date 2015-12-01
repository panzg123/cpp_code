#pragma  once
class NonCopyable
{
	NonCopyable(const NonCopyable&) = delete; //½ûÓÃ
	NonCopyable& operator = (const NonCopyable&) = delete; //½ûÓÃ
	NonCopyable() = default;
	~NonCopyable() = default;
};