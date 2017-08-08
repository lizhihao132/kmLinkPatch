#ifndef KLOG_H
#define KLOG_H

#define LOG_INFO(str) klog.info(str,__FILE__,__LINE__)
#define LOG_ERROR(str, errorCode) klog.error(str,errorCode,__FILE__,__LINE__)

#include <fstream>
#include <time.h>
using namespace std;

class KLog
{
public :
	KLog(const char* filePath);

	void error(const char* str, int errorCode,const char* srcFile, int srcLine);

	void info(const char* str, const char* srcFile, int srcLine);

	~KLog();
private:
	void formatCurrentTime(char *curTimeStr);

	const char* formatFileName(const char* fileName);

	void getCurDate(char *curDate);

	void dayCutIfNeed();

	ofstream ofile;
	char curDate[32];
	const char *filePath;
};

extern KLog klog;

#endif