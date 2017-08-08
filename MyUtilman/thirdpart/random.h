#ifndef TEST_RANDOM
#define TEST_RANDOM

#include <ctime>
#include <stdlib.h>

//获得一个长度为 strLen 的随机小写字母字符串
template<typename CHAR_TYPE>
void randomStr(CHAR_TYPE *str,size_t strLen);

//获得一个长度最多为 maxLen 的随机小写字母重复的字符串
//返回: 1.字符串长度 2.n:重复次数 3.l:循环单元长度
template<typename CHAR_TYPE>
size_t randomRepeatStr(CHAR_TYPE *str,size_t maxLen,size_t& l,size_t & n);

//获得一个长度为 l 的串重复 n 次的随机小写字母字符串
//返回: 字符串长度
template<typename CHAR_TYPE>
size_t randomRepeatStrLN(CHAR_TYPE *str, size_t l, size_t n);


//获得一个长度为 strLen 的随机小写字母字符串. str 应该申请 strLen+1 个 CHAR_TYPE 空间
template<typename CHAR_TYPE>
void randomStr(CHAR_TYPE *str,size_t strLen)
{
	srand((unsigned)time(NULL));  /*随机种子*/
	//97 ~ 122
	for (size_t i = 0;i < strLen;++ i)
	{
		*(str + i) = rand() % 26 + 97;
	}
	*(str + strLen) = char(0);
}


//获得一个长度最多为 maxLen 的随机小写字母重复的字符串
// str 应该申请 strLen+1 个 CHAR_TYPE 空间
//返回: 1.字符串长度 2.n:重复次数 3.l:循环单元长度.
template<typename CHAR_TYPE>
size_t randomRepeatStr(CHAR_TYPE *str,size_t maxLen,size_t& l,size_t & n)
{
	srand((unsigned)time(NULL));  /*随机种子*/
	n = rand() % (maxLen/RAND_MAX) + 1;		//1 ~ maxLen/RAND_MAX
	l = rand() % RAND_MAX + 1;				//1 ~ RAND_MAX
	//	printf("len: %d, count: %d\r\n", l, n);
	assert(l * n <= maxLen);
	//97 ~ 122
	for (size_t i = 0;i < l;++ i)
	{
		*(str + i) = rand() % 26 + 97;
	}

	for (size_t i = 0;i < n;++ i)
	{
		selfAppend(str + l*i,l);
	}
	*(str + l*n) = char(0);
	return l*n;
}

//获得一个长度为 l 的串重复 n 次的随机小写字母字符串
//返回: 字符串长度
template<typename CHAR_TYPE>
size_t randomRepeatStrLN(CHAR_TYPE *str, size_t l, size_t n)
{
	srand((unsigned)time(NULL));  /*随机种子*/

	//97 ~ 122
	for (size_t i = 0;i < l;++ i)
	{
		*(str + i) = rand() % 26 + 97;
	}

	for (size_t i = 0;i < n-1;++ i)
	{
		selfAppend(str + l*i,l);
	}
	*(str + l*n) = CHAR_TYPE(0);
	return l*n;
}

#endif