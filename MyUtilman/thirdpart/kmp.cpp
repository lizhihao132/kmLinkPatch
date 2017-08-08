#pragma warning (disable:4267)
#include "kmp.h"
#include "random.h"
#include <memory.h>
#include "string.h"

KMP::KMP(const char *pStr)
	:paternStr(pStr)
	{
		paternLen = tstrLen(pStr);
		t1Length = new int[paternLen + 1];
		t1LastLength = new int[paternLen + 1];

		memset(t1Length,0,paternLen);
		memset(t1LastLength,0,paternLen);

		hasInitT1Length = false;
		hasInitReverseT1Length = false;
	}

KMP::~KMP()
{
	delete[] t1Length;
	delete[] t1LastLength;
}


int KMP:: indexOf(const char *sourceStr,int beginPos)
{
	int firstIndex = -1;
	initT1Length();
	int sLength = strlen(sourceStr);
	for (int i = beginPos,j=0;i < sLength;)
	{
		if(*(sourceStr + i) != *(paternStr + j))
		{
			if(0 == j)
			{
				++ i;
				j = 0;//可省略
			}
			else if(1 == j)
			{
				j = 0;
			}
			else 
			{

				//i 不变，j 向前移动
				j = t1Length[j];
			}


		}
		else
		{
			++ j;
			++ i;
			if(paternLen == j)
			{
				firstIndex = i - paternLen;
				break;
			}
		}
	}
	return firstIndex;
}


int KMP:: lastIndexOf(const char *sourceStr,int beginPos)
{
	int firstIndex = -1;
	initReverseT1Length();
	int sLength = strlen(sourceStr);
	int pLength = paternLen;
	for (int i = sLength - 1 -  beginPos,j = pLength - 1;i != -1;)
	{
		if(*(sourceStr + i) != *(paternStr + j))
		{
			if(pLength - 1 == j)
			{
				-- i;
			}
			else if(pLength - 2 == j)
			{
				j = pLength - 1;
			}
			else 
			{

				//i 不变，j 向前移动
				j = pLength - 1 - t1Length[j];
			}


		}
		else
		{
			-- j;
			-- i;
			if(-1 == j)
			{
				firstIndex = i+ 1;
				break;
			}
		}
	}
	return firstIndex;
}


void KMP:: reverseStr(char *str)
{
	char exCh;
	int strLen = strlen(str);
	int maxExPos = strLen / 2;
	for (int i = 0;i < maxExPos;++ i)
	{
		exCh = *(str + i);
		*(str + i) = *(str + strLen - 1 - i);
		*(str + strLen - 1 - i) = exCh;
	}
}


void KMP:: initT1Length()
{
	if(hasInitT1Length)
		return;
	hasInitT1Length = true;
	const char *pStr = paternStr;
	const char firstChar = *pStr;
	char lastChar;	//最后一个字符
	char beforeLastChar;	//倒数第二个字符
	int lastT1Length;	//上一次求的 t1 的长度
	t1Length[0] = t1Length[1] = 0;
	for (int i = 2;i <= paternLen;++ i)
	{
		lastT1Length = t1Length[i-1];	//前一个，所以是 i - 1
		if(0 == lastT1Length)	//前一个字母没有对应
		{
			t1Length[i] = (firstChar == *(pStr + i - 1));
		}
		else
		{
			lastChar = *(pStr + i - 1);
			beforeLastChar = *(pStr + i - 2);
			while(0 != lastT1Length && *(pStr + lastT1Length) != lastChar)	//对应字符的后面一个字符如果与最后一个字符相等则退出
			{
				lastT1Length = t1Length[lastT1Length];
			}
			if(0 != lastT1Length)	//因为 *(pStr + lastT1Length) == lastChar 跳出的循环
			{
				t1Length[i] = lastT1Length + 1;
			}
			else
			{
				t1Length[i] = (firstChar == *(pStr + i - 1));
			}
		}
	}

}


void KMP::initReverseT1Length()
{
	if(hasInitReverseT1Length)
		return;
	hasInitReverseT1Length = true;
	const char *pStr = paternStr;
	int totalLength = strlen(pStr);
	const char firstChar = *(pStr + totalLength - 1);
	char lastChar;	//最后一个字符
	char beforeLastChar;	//倒数第二个字符
	int lastT1Length;	//上一次求的 t1 的长度
	t1LastLength[0] = t1LastLength[1] = 0;
	for (int i = 2;i <= totalLength;++ i)
	{
		lastT1Length = t1LastLength[i-1];	//前一个，所以是 i - 1
		if(0 == lastT1Length)	//前一个字母没有对应
		{
			t1LastLength[i] = (firstChar == *(pStr + totalLength - i));
		}
		else
		{
			lastChar = *(pStr + totalLength - i);
			beforeLastChar = *(pStr + totalLength - i + 1);
			while(0 != lastT1Length && *(pStr + totalLength - 1 - lastT1Length) != lastChar)	//对应字符的后面一个字符如果与最后一个字符相等则退出
			{
				lastT1Length = t1LastLength[lastT1Length];
			}
			if(0 != lastT1Length)	//因为 *(pStr + lastT1Length) == lastChar 跳出的循环
			{
				t1LastLength[i] = lastT1Length + 1;
			}
			else
			{
				t1LastLength[i] = (firstChar == lastChar);
			}
		}
	}
}


bool KMP:: isT1LengthArrayEqual(char *pStr,int aLength)
{

	KMP z(pStr);
	z.initT1Length();

	reverseStr(pStr);

	KMP f(pStr);
	f.initReverseT1Length();
	reverseStr(pStr);
	for (int i = 0;i < aLength;++ i)
	{
		if(z.t1Length[i] != f.t1LastLength[i])
		{
			return false;
		}
	}

	return true;
}

void KMP:: testT1LengthRigth()
{
	char pStr[1024];
	while(true)
	{
		randomStr(pStr,12);
		*(pStr + 12) = 0;
		printf("%s : ",pStr);
		bool r = isT1LengthArrayEqual(pStr,13);
		printf(r ? "yes\r\n" : "no\r\n");
	}
}