#include <cstdint>
#include <iostream>
using namespace std;

//拼接两个unsigned char 为一个 unsigned int16
uint16_t makeword16(unsigned char low,unsigned char high)
{
	uint16_t data = high;
	data <<= 8;
	data |= low;
	return data;
}
//拼接4个unsigned char 为一个unsigned int
uint32_t makeword32(unsigned char low1, unsigned char low2, unsigned char high1, unsigned char high2)
{
	uint16_t low = makeword16(low1, low2);
	uint16_t high = makeword16(high1, high2);
	uint32_t ret = high;
	ret <<= 16;
	ret |= low;
	return ret;
}


void makeword_test()
{
	unsigned char a = 0x34;
	unsigned char b = 0x33;
	unsigned char c = 0x97;
	unsigned char d = 0x41;

	uint16_t ret = makeword16(a, b);
	uint16_t ret2 = makeword16(c, d);
	uint32_t ret3 = makeword32(a, b, c, d);
	std::cout << ret << " " << ret2 << " " << ret3 << std::endl;
}
int main()
{
	makeword_test();
	system("pause");
	return 0;
}