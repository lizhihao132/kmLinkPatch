#include "file.h"
#include "kmp.h"
#include <assert.h>
#include <direct.h>
#include <windows.h>
#include "string.h"
#include <io.h>


void intsToFile(int *ints,size_t len,const string &fileName)
{
	ofstream file(fileName.c_str());
	if(file.is_open())
	{
		for(size_t i = 0;i < len;++ i)
		{
			file << ints[i] << '\n';
		}
		file.close();
	}
}

void wcharsToFile(wchar_t *chars,size_t len,const string &fileName)
{
	ofstream file(fileName.c_str());
	if(file.is_open())
	{
		for(size_t i = 0;i < len;++ i)
		{
			file << chars[i] << '\n';
		}
		file.close();
	}
}



/************************************************************************/
/* 
通过文本流的魔数判断文本编码
0 : 不能得知
1 : Unicode little endian
2 : Unicode big endian
3 : UTF-8
ANSI：　　　　　　　　	无格式定义； 
Unicode(UCS-2) little endian： 前两个字节为FFFE； 
Unicode(UCS-2) big endian：　	前两字节为FEFF；　 
UTF-8 有 Bom：　 　　　　前两字节为EFBB；
Unicode(UCS-4) little endian： 前四个字节为FFFE0000；
Unicode(UCS-4) big endian：　	前四个字节为0000FEFF；

大端：高地址中存放低位数据；小端：高地址存放高位数据。
0x1234 
内存地址		0x40000		0x4001		
大端			0x12		0x34
小端			0x34		0x12
注意，0x1234 中的 34 是低位，12 是高位
*/
/************************************************************************/
char getFileTextEncoding(const char* fileText,size_t bufLen,size_t *realNumLen)
{
	if(bufLen < 2)
		return 0;
	unsigned char first = fileText[0],second = fileText[1];
	if(first == 0xff && second == 0xfe)
		return 1;
	else if(first == 0xfe && second == 0xff)
		return 2;
	else if(first == 0xef && second == 0xbb)
		return 3;
	else
	{
		//暂时不支持 UCS-4
	}
	return 0;
}

//将 Unicode 内容写进文件, 使用 UTF8 编码
void writeFileUtf8(ostream *file,wchar_t *theChs,size_t len)
{
	//第一个参数表示目标字符串的代码页
	//这个参数只影响输出文件的编码，如果设置为 0 则使用系统默认的代码页，即 asscii 编码。如果使用 UTF-8，则输出的文件编码为 UTF-8
	size_t nMBLen = WideCharToMultiByte(CP_UTF8, 0,theChs, len, NULL, 0, NULL, NULL);
	char* pBuffer = new char[nMBLen+1];
	pBuffer[nMBLen] = 0;
	WideCharToMultiByte(CP_UTF8, 0,theChs, len, pBuffer, nMBLen+1, NULL, NULL);
	if(NULL != file)	*file << pBuffer;
	delete[] pBuffer;
}

//路径 path 最后一个斜杠或反斜杠的位置
template<typename CHAR_TYPE>
int lastSlashPos(const CHAR_TYPE* path)
{
	int lastSlashPos = 0;
	int i = 0;
	CHAR_TYPE c = 0;
	while (0 != (c = *(path + i)))
	{
		if ('/' == c || '\\' == c)
		{
			lastSlashPos = i;
		}
		++ i;
	}
	return lastSlashPos;
}


//获得当前执行程序的目录. 最后带了一个余杠. 返回的字符串需要回收
const char* NEED_DELETE getCurrentModulePathWithSlash()
{
	char moduleFile[256]  = {0};
	GetModuleFileNameA(NULL,moduleFile,255);
	int length = lastSlashPos(moduleFile) + 1;

	char *ret = new char[length + 1];
	//memset(ret,0,(length + 1) * sizeof(char));
	ret[length] = 0;
	memcpy_s(ret,length,moduleFile,length);
	return ret;
}

//文件是否存在. 0:不存在, 1:存在
int fileExists(const char* filename)
{
	fstream file;
	file.open(filename,ios::in);
	if(!file)	//与 file.failed() 等价
	{
		return 0;
	}
	else
	{
		file.close();
		return 1;
	}
}



//由相对路径获得绝对路径. 与系统无关
bool fullPath_alt(const string &thePath,string &full)
{
	string path = formatPath(thePath);

	if(-1 != path.find_first_of(':'))	//已经是绝对路径
	{
		full = string(path);
	}

	if(0 == KMP("./").indexOf(path.c_str()))	//当前目录
	{
		path = path.substr(2,path.length() - 2);
	}

	char pBuf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH,pBuf);
	string curDir = formatPath(pBuf);
	if ('/' != curDir[curDir.length()-1])
	{
		curDir += "/";
	}
	printf("work dir : %s\r\n",curDir.c_str());
	int upLevel = 0;
	int pos = 0;
	KMP kmp("../");

	while (string::npos != (pos = kmp.indexOf(path.c_str(),pos)))
	{
		pos += 3;
		++ upLevel;
	}
	if(0 == upLevel)	//当前目录下
	{
		full = curDir + path;
		return true;
	}
	else	//上 upLevel 层目录
	{

		pos = kmp.indexOf(path.c_str());	//pos 指向 ../ 中的 /
		string coreDir;
		if(pos == path.length()-1)
			coreDir = "";
		else
			coreDir = path.substr(pos + 1,path.length() - pos + 1);
		string parentDir(curDir);
		for (int i = 0;i < upLevel;++ i)
		{
			if(!getParentDir(parentDir,parentDir))
			{
				return false;
			}
		}
		if ('/' != parentDir[parentDir.length()-1])
		{
			parentDir += "/";
		}
		full = parentDir + coreDir;
		return true;
	}
}

//由相对路径获得绝对路径. 适应 windows
bool fullPath(const string &thePath,string &full)
{
	char theDir[MAX_PATH] = {0};
	char theFull[MAX_PATH] = {0};
	char *namePos = NULL;
	memcpy_s(theDir,MAX_PATH,thePath.c_str(),thePath.length());
	if(::GetFullPathNameA(theDir,MAX_PATH,theFull,&namePos) == 0)
	{
		int errorCode = GetLastError();
		printf("getLastError : %d",errorCode);
		return false;
	}
	else
	{
		full = formatPath(theFull);
		return true;
	}
}


string formatPath(const string& path)
{
	char tmp[MAX_PATH];

	int len = path.length();

	if (0 == len) {
		return "";
	}


	memset(tmp, '\0', sizeof(tmp));
	memcpy_s(tmp,len,path.c_str(),len);

	for (int i = 0;i < len;++ i)
	{
		if(tmp[i] == '\\')
			tmp[i] = '/';
	}

	return string(tmp);
}

string formatInWindowsPath(const string& path)
{
	char tmp[MAX_PATH];

	int len = path.length();

	if (0 == len) {
		return "";
	}


	memset(tmp, '\0', sizeof(tmp));
	memcpy_s(tmp,len,path.c_str(),len);

	for (int i = 0;i < len;++ i)
	{
		if(tmp[i] == '/')
			tmp[i] = '\\';
	}

	return string(tmp);
}


bool isDirectory(const string& path)
{
	WIN32_FIND_DATAA fd;  
	bool ret = false;  
	HANDLE hFind = FindFirstFileA(path.c_str(), &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  
	{  
		ret = TRUE;  
	}  
	FindClose(hFind);  
	return ret;
}

bool isFile(const string& path)
{
	WIN32_FIND_DATAA fd;  
	bool ret = false;  
	HANDLE hFind = FindFirstFileA(path.c_str(), &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & 
		(FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)))
	{  
		ret = TRUE;  
	}  
	FindClose(hFind);  
	return ret;
}

bool DirectoryExist(const string& path)  
{  
	return isDirectory(path);
} 

void mkdirs(const char *dir)
{
	if(DirectoryExist(string(dir)))
		return;
	char tmp[1024];
	char *p;

	if (strlen(dir) == 0 || dir == NULL) {
		printf("strlen(dir) is 0 or dir is NULL.\n");
		return;
	}

	int len = strlen(dir);
	memset(tmp, '\0', sizeof(tmp));
	memcpy_s(tmp,len,dir,len);

	if (tmp[0] == '/') 
		p = strchr(tmp + 1, '/');
	else 
		p = strchr(tmp, '/');

	if (p) 
	{
		*p = '\0';
		_mkdir(tmp);
		_chdir(tmp);
	} else 
	{
		_mkdir(tmp);
		_chdir(tmp);
		return;
	}

	mkdirs(p + 1);
}

int copyFile(const string& source,const string& target)
{
	FILE *sfile = NULL;
	fopen_s(&sfile,source.c_str(),"rb");
	if(NULL == sfile)
	{
		return 1;
	}
	fseek(sfile,0,SEEK_END);
	long sfileSize = ftell(sfile);
	fseek(sfile,0,SEEK_SET);

	FILE* tfile = NULL;
	fopen_s(&tfile,target.c_str(),"w+b");
	if(NULL == tfile)
	{
		return 2;
	}
	char buff[1024];

	size_t total = 0;
	size_t rc = 0;
	while (0 != (rc = fread_s(buff,1024,1,1024,sfile)))
	{
		total += rc;
		size_t wc = fwrite(buff,1,rc,tfile);
		assert(rc == wc);
	}
	fclose(sfile);
	fclose(tfile);
	assert(total == sfileSize);
	return 0;
}

void createDirRecursion(const string& dir)
{
	mkdirs(dir.c_str());
}

bool getParentDir(const string& thePath,string &dir)
{
	string path;
	if(!fullPath(thePath,path))
	{
		return false;
	}

	int pos = path.find_last_of('/');
	if(-1 == pos)
		return false;
	else
	{
		if(pos == path.length()-1)
			pos = path.substr(0,path.length()-2).find_last_of('/');
		if(-1 == pos)
			return false;
		dir = path.substr(0,pos);	//0 ~ pos-1
		return true;
	}
}

//读取文件多字节流
const char* NEED_DELETE readFileBytes(const char*fileName,size_t &length)
{
	ifstream file(fileName);
	if(!file)
	{
		length = 0;
		return NULL;
	}
	file.seekg(0,std::ios::end);
	length = file.tellg();
	file.seekg(0,std::ios::beg);
	char *ret = new char[length];
	file.read(ret,length);
	file.close();
	return ret;
}


void listFiles(const string &dir, vector<string>& filePaths )
{
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst((dir+"\\*.*").c_str(), &findData);
	if (handle == -1)        // 检查是否成功
		return;

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			listFiles(dir+"\\"+findData.name,filePaths);
		}
		else
		{
			filePaths.push_back(dir + "\\" + findData.name);
		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // 关闭搜索句柄
}