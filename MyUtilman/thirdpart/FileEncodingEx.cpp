#include <memory>
#include "FileEncoding.h"
#include "./thirdpart/uchardet/uchardet.h"
#include <windows.h>
#include <string>
using std::string;

FileEncoding::FileEncoding(void)
{
	memset(charSet,0,sizeof(char) * MAX_CHARSET_NAME);
}

FileEncoding::~FileEncoding(void)
{
}


const char * FileEncoding:: getCharSet(const char *buf,size_t bufLen)
{
	const char *retCharSet = NULL;
	uchardet_t ud = uchardet_new();
	if(0 == uchardet_handle_data(ud,buf,bufLen))
	{
		uchardet_data_end(ud);
		retCharSet = uchardet_get_charset(ud);
		memcpy_s(charSet,16,retCharSet,sizeof(char) * 16);
	}
	uchardet_delete(ud);
	return charSet;
}

int FileEncoding:: getCharSetPageCode(const char *buf,size_t bufLen)
{
	const char *charSet = getCharSet(buf,bufLen);
	printf("...[%s]...",charSet);
	if(strEqual(charSet,"UTF-8"))
	{
		return 65001;
	}	
	if(strEqual(charSet,"UTF-16"))
	{
		return 1200;
	}
	else if (strEqual(charSet,"Big5"))
	{
		return 950;
	}
	else if(strEqual(charSet,"GBK"))
	{
		return 936;
	}
	else if(strEqual(charSet,"EUC-JP"))
	{
		return 932;
	}
	else if(strEqual(charSet,"EUC-KR"))
	{
		return 949;
	}
	else if(strEqual(charSet,"gb18030"))
	{
		return 54936;
	}
	else if(strEqual(charSet,"windows-1252"))
	{
		return 1252;
	}
	else if(strEqual(charSet,"Shift_JIS"))
	{
		return 932;
	}
	return 0;//ansi CP_ACP
}



//多字节转为宽字节. char -> Unicode
wchar_t* multCharSetToWideWithGuessCodePage(const char * buf,size_t bufLen,size_t *realNumLen)
{

	setlocale(LC_ALL,"chs");

	FileEncoding fed;	//CP_UTF8
	size_t testLen = bufLen < 4096 ? bufLen : 4096;
	int CP_PAGE = fed.getCharSetPageCode(buf,testLen);
	//TODO 这里还有 BUG，有时第一次调用 MultiByteToWideChar 返回 0 字节
	//int theUTF8 = CP_UTF8;
	//多字节字符串转化为宽字符串，第一个参数表示源字符串的代码页
	size_t nMBLen = MultiByteToWideChar(CP_PAGE,0,buf,bufLen,NULL,NULL);
	if(0 == nMBLen)
	{
		int ne = GetLastError();
		printf("MultiByteToWideChar failed. source file page code is : %d\r\n",CP_PAGE);
	}
	wchar_t* szWcsBuffer = (wchar_t*)malloc(sizeof(wchar_t) * (nMBLen + 1));

	//	ZeroMemory(szWcsBuffer, sizeof(wchar_t)*(1+nMBLen));
	memset(szWcsBuffer,0,sizeof(wchar_t)*(nMBLen+1));
	MultiByteToWideChar(CP_PAGE,0,buf,bufLen, szWcsBuffer, nMBLen);
	*realNumLen = nMBLen;
	return szWcsBuffer;
}