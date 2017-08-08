#pragma once

class FileEncoding
{
private:
	inline bool strEqual(const char *l,const char *r)
	{
		if(NULL == l && NULL == r)	return false;
		for (;0 != *l && 0 != *r && *l == * r;++ l,++ r)
		{

		}
		return *l == *r && 0 == *l;
	}
	static const int MAX_CHARSET_NAME = 16;
	char charSet[MAX_CHARSET_NAME];
public:
	FileEncoding(void);
	~FileEncoding(void);
	const char * getCharSet(const char *buf,size_t bufLen);

	int getCharSetPageCode(const char *buf,size_t bufLen);
};

//多字节转为宽字节，尝试根据字节流猜测 code page. char -> Unicode
wchar_t* multCharSetToWideWithGuessCodePage(const char * buf,size_t bufLen,size_t *realNumLen);

//utf 多字节转为宽字节. char -> Unicode
wchar_t* utf8MultCharSetToWide(const char * buf,size_t bufLen,size_t *realNumLen);

//default(与地域相关，在中国，就是 gbk) 编码的多字节转为宽字节. char -> Unicode
wchar_t* defaultMultCharSetToWide(const char * buf,size_t bufLen,size_t *realNumLen);

//宽字节转为 UTF8 多字节. Unicode -> char
char* wideCharSetToUtf8Mult(const wchar_t * buf,size_t bufLen,size_t *realNumLen);

//宽字节转为 系统默认的(在中国，就是 gbk) 多字节. Unicode -> char
char* wideCharSetToDefaultMult(const wchar_t * buf,size_t bufLen,size_t *realNumLen);

//多字节转为宽字节. char -> Unicode
wchar_t* multCharSetToWide(const char * buf,size_t bufLen,DWORD codeOfBuf,size_t *realNumLen);

//宽字节转为多字节. Unicode -> char
char* wideCharSetToMult(const wchar_t * buf,size_t bufLen, DWORD codeOfTarget,size_t *realNumLen);

//获得系统当前 codePage
DWORD getSystemCodePage();