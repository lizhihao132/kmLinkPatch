#ifndef TEST_FILE
#define TEST_FILE
#include <string>
using std::string;
using std::wstring;
#include <iostream>
#include <fstream>
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::fstream;
using std::ios_base;
using std::ios;
#include <windows.h>
#include <stdlib.h>
#include <vector>
using std::vector;

#define NEED_DELETE 

//将路径中的反斜杠替换为斜杠. \\ -> /
string formatPath(const string& path);

//将路径中的斜杠替换为反斜杠. / --> \\ 
string formatInWindowsPath(const string& path);

//复制文件
int copyFile(const string& source,const string& target);

//递归创建文件夹
void createDirRecursion(const string& dir);

//是否是文件
bool isFile(const string& path);

//是否是目录
bool isDirectory(const string& path);

//由相对路径获得绝对路径. 适应 windows
bool fullPath(const string &path,string &full);

//由相对路径获得绝对路径. 与系统无关
bool fullPath_alt(const string &thePath,string &full);

//获得上一层目录
bool getParentDir(const string& path,string &dir);


void intsToFile(int *ints,size_t len,const string &fileName);
void wcharsToFile(wchar_t *chars,size_t len,const string &fileName);

/* 
获得文本字节流的编码
0 : 不能得知
1 : Unicode little endian
2 : Unicode big endian
3 : UTF-8
*/
char getFileTextEncoding(const char* fileText,size_t bufLen,size_t *realNumLen);

//将 Unicode 内容写进文件, 使用 UTF8 编码
void writeFileUtf8(ostream *file,wchar_t *theChs,size_t len);

//路径 path 最后一个斜杠或反斜杠的位置
template<typename CHAR_TYPE>
int lastSlashPos(const CHAR_TYPE* path);

//获得当前执行程序的目录. 最后带了一个余杠. 返回的字符串需要回收
const char* NEED_DELETE getCurrentModulePathWithSlash();

//文件是否存在. 0:不存在, 1:存在
int fileExists(const char* filename);

//读取文件多字节流
const char* NEED_DELETE readFileBytes(const char*fileName,size_t &length);

void listFiles(const string &dir, vector<string>& filePaths );

#endif