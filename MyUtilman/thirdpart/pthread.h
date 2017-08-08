#ifndef TEST_PROCESS_THREAD
#define TEST_PROCESS_THREAD
#include <windows.h>
#include <set>
using std::set;

class ProcessInfo 
{
public:
	ProcessInfo(DWORD pid, DWORD moduleId, DWORD parentPId, TCHAR* path);
	DWORD _pid;
	DWORD _moduleId;
	DWORD _parentPId;
	TCHAR _path [MAX_PATH];

	bool operator < (const ProcessInfo &pi) const
	{
		return this->_pid < pi._pid;
	}
};

//获得当前系统内存占用
SIZE_T getCurrentMemoryUse(SIZE_T *peakMemory);

//尝试第一次运行程序. 若之前没有运行则运行且返回 0. 否则不运行,返回1
int tryFirstRunModule();

//根据进程 id kill 进程. 返回 TRUE 表示 kill 成功否则失败
BOOL KillProcess(DWORD dwPid, DWORD waitKillMs)  ;

//提升进程仅限, 返回 TRUE 成功否则失败
BOOL EnableDebugPrevilige(BOOL   fEnable) ;


/************************************************************************/
/*  
根据进程名 kill 进程.
返回 2 表示没找到进程
返回 1 表示关闭进程失败
返回 0 表示关闭成功
/************************************************************************/
int KillProcessByName(const TCHAR *lpszProcessName) ;

//根据名字查找进程
BOOL findProcess(const TCHAR *lpszProcessName) ;

//根据进程名获得进程 id
bool getProcessInfoByName(const TCHAR *lpszProcessName, set<DWORD>& infos );

bool getProcessInfoByNameEx(const TCHAR *lpszProcessName, set<ProcessInfo>& infos);

bool startProcess(const TCHAR *exeFile, const TCHAR *cmdLine, const TCHAR *workDirectory);

bool startProcessSilent(const TCHAR *exeFile, const TCHAR *cmdLine, const TCHAR *workDirectory);

#endif