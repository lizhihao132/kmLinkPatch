#include "string.h"
#include "FileEncoding.h"
#include <windows.h>

void upToLowCase(char *buf,size_t bufLen)
{
	char ch;
	const int diff = 'a' - 'A';
	for (size_t i = 0;i < bufLen;++ i)
	{
		ch = *(buf + i);
		if(ch >= 'A' && ch <= 'Z')
		{
			*(buf + i) = ch + diff;
		}
	}
}