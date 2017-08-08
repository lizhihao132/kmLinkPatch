#ifndef TEST_STRING
#define TEST_STRING
#include <string>
using std::string;
#include "kmp.h"
#include "encoding.h"
#include "math.h"
#include  "pthread.h"

//字符串长度
template<typename CHAR_TYPE>
size_t tstrLen(const CHAR_TYPE *str);

//从字符串src的len位置开始追加子串 [0,src+len-1].
//调用者需要管理 src 内存空间
template<typename CHAR_TYPE>
void selfAppend(CHAR_TYPE *src,size_t len);

//打印数组
template<typename E>
void printfArray(E *array, size_t len);

/************************************************************************/
/* 
两个字符串是否呈前缀关系。
0 ： 不存在
1 ： 第一个是第二个的前缀
2 ： 第二个是第一个的前缀
若两个字符串一样，则认为前一个是后一个的前缀
*/
/************************************************************************/
template<typename CHAR_TYPE>
char prefixRelation(const CHAR_TYPE* firstStr,const CHAR_TYPE* secondStr);

//大写转小写
void upToLowCase(char *buf,size_t bufLen);

//根据字节流的前两个字节猜测字节流的编码
char simpleGetTextEncoding(const char* buf,size_t bufLen,size_t *realNumLen);

//字符串相等
template<typename CHAR_TYPE>
bool strEqual(const CHAR_TYPE *l,const CHAR_TYPE *r);

//一个宽字节转为 16 进制字符串
template<typename WIDE_BYTE_TYPE>
std::string wideByteToHexString(WIDE_BYTE_TYPE multyByte);

template<typename WIDE_BYTE_TYPE>
std::string wideByteToHexStringEx(WIDE_BYTE_TYPE multyByte);




//字符串长度
template<typename CHAR_TYPE>
size_t tstrLen(const CHAR_TYPE *str)
{
	if(NULL == str)
		return 0;
	size_t len = 0;
	while (*str)
	{
		++ str;
		++ len;
	}
	return len;
}


//从字符串src的len位置开始追加子串 [0,src+len-1].
//调用者需要管理 src 内存空间
template<typename CHAR_TYPE>
void selfAppend(CHAR_TYPE *src,size_t len)
{
	memcpy_s(src+len,sizeof(CHAR_TYPE)*len,src,sizeof(CHAR_TYPE)*len);
	src[len << 1] = 0;
}

//打印数组
template<typename E>
void printfArray(E *array, size_t len)
{
	for (size_t i = 0;i <= len; ++ i)
	{
		std::cout << array[i] << " ";
	}
	std::cout<< std::endl;
}

/************************************************************************/
/* 
两个字符串是否呈前缀关系。
0 ： 不存在
1 ： 第一个是第二个的前缀
2 ： 第二个是第一个的前缀
若两个字符串一样，则认为前一个是后一个的前缀
*/
/************************************************************************/
template<typename CHAR_TYPE>
char prefixRelation(const CHAR_TYPE* firstStr,const CHAR_TYPE* secondStr)
{
	if(NULL == firstStr || NULL == secondStr)
		return 0;
	for(;;++ firstStr,++secondStr)
	{
		if(0 == *firstStr)	//firstStr 结束了表示前一个是后一个的前缀
			return 1;
		if(0 == *secondStr)	//prefix 还没结束但 secondStr 结束了，表示后一个是前一个的前缀
			return 2;

		if(*firstStr != *secondStr)
			break;
	}
	return 0;
}



template<typename CHAR_TYPE>
bool strEqual(const CHAR_TYPE *l,const CHAR_TYPE *r)
{
	if(NULL == l && NULL == r)	return false;
	for (;0 != *l && 0 != *r && *l == * r;++ l,++ r)
	{

	}
	return *l == *r && 0 == *l;
}





//一个宽字节转为 16 进制字符串
template<typename WIDE_BYTE_TYPE>
std::string wideByteToHexString(WIDE_BYTE_TYPE multyByte)
{
	static std::string hexChars[16] = {"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};
	size_t byteValue = (size_t)multyByte;
	std::string s("");
	for (;;)
	{
		s = hexChars[byteValue % 16] + s;
		byteValue >>= 4;
		if(0 == byteValue)
			break;
	}

	//前面补零
	int width = (sizeof(WIDE_BYTE_TYPE)) * 2;
	int c0 = int(width - s.length());
	for(int i = 0;i < c0;++ i)
		s = "0" + s;
	std::string ret("");
	//每 2 个16进制位之间补一个空格
	ret += s.substr(0,2);
	for(size_t i = 2;i < s.length();i += 2)
	{
		ret += " ";
		ret += s.substr(i,2);
	}
	return ret;
}


template<typename WIDE_BYTE_TYPE>
std::string wideByteToHexStringEx(WIDE_BYTE_TYPE multyByte)
{
	std::stringstream ioss; //定义字符串流
	std::string s_temp; //存放转化后字符
	ioss << std::hex << multyByte; //以十六制形式输出
	ioss >> s_temp; 
	int width = (sizeof(WIDE_BYTE_TYPE)) * 2;
	std::string s(width - s_temp.size(), '0'); //补0
	s += s_temp; //合并

	//每 2 个16进制位之间补一个空格
	std::string ret("");
	ret += s.substr(0,2);
	for(size_t i = 2;i < s.length();i += 2)
	{
		ret += s.substr(i,2);
		ret += " ";
	}
	return ret;
}

#endif