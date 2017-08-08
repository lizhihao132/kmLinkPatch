#include "math.h"

//2^x <= num，求 x 的最大值
int radix(int num)
{
	if(num < 0)
	{
		num = -num;
	}
	int retDix = 1;
	long long ret = 1;//这里必须要定义多于 32 位的整数，如对于 INT_MAX 来说，ret 不断左移直到小于0，再到小于0.
	while (ret <= num)
	{
		ret <<= 1;
		++ retDix;
	}
	return --retDix;//ret >> 1;
}