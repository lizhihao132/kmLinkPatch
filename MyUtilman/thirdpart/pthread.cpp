#include "pthread.h"
#include "string.h"

#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")
#include <process.h>
#include <tlhelp32.h>
#include <tchar.h>

ProcessInfo::ProcessInfo(DWORD pid, DWORD moduleId, DWORD parentPId, TCHAR* path)
:_pid(pid),_parentPId(parentPId),_moduleId(moduleId)
{
	size_t pathLen = tstrLen(path);
	memcpy_s(_path,MAX_PATH*sizeof(TCHAR),path,pathLen*sizeof(TCHAR));
	_path[pathLen] = 0;

}

//获得当前系统内存占用
SIZE_T getCurrentMemoryUse(SIZE_T *peakMemory)
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	if(NULL != peakMemory)
		*peakMemory = pmc.PeakWorkingSetSize;
	return pmc.WorkingSetSize;
}


#pragma data_seg("Shared") 
int volatile g_lAppInstance = 0; 
#pragma data_seg() 
#pragma comment(linker,"/section:Shared,RWS")
//尝试第一次运行程序. 若之前没有运行则运行且返回 0. 否则不运行,返回1
int tryFirstRunModule()
{
	if (0 == g_lAppInstance)
	{
		g_lAppInstance = 1;
		return 0; //以前没有运行过
	}
	else 
	{
		return 1;
	}
}

//根据进程 id kill 进程. 返回 TRUE 表示 kill 成功否则失败
BOOL KillProcess(DWORD dwPid, DWORD waitKillMs)  
{  
	HANDLE hPrc;  

	if( 0 == dwPid) return FALSE;  

	hPrc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPid);  // Opens handle to the process.  

	if( !TerminateProcess(hPrc,0) ) // Terminates a process.  
	{  
		CloseHandle( hPrc );  
		return FALSE;  
	}  
	else  
		WaitForSingleObject(hPrc, waitKillMs); // At most ,waite 2000  millisecond.  

	CloseHandle(hPrc);  
	return TRUE;  
}  

//提升进程仅限, 返回 TRUE 成功否则失败
BOOL EnableDebugPrevilige(BOOL   fEnable) 
{ 
	BOOL   fOk   =   FALSE;         //   Assume   function   fails 
	HANDLE   hToken;
	if   (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,   
		&hToken))
	{
		TOKEN_PRIVILEGES   tp; 
		tp.PrivilegeCount   =   1; 
		LookupPrivilegeValue(NULL,   SE_DEBUG_NAME,   &tp.Privileges[0].Luid); 
		tp.Privileges[0].Attributes   =   fEnable   ?   SE_PRIVILEGE_ENABLED   :   0; 
		AdjustTokenPrivileges(hToken,   FALSE,   &tp,   sizeof(tp),   NULL,   NULL); 
		fOk   =   (GetLastError()   ==   ERROR_SUCCESS); 
		CloseHandle(hToken); 
	} 
	return(fOk);
}

/************************************************************************/
/*  
根据进程名 kill 进程.
返回 2 表示没找到进程
返回 1 表示关闭进程失败
返回 0 表示关闭成功
/************************************************************************/
int KillProcessByName(const TCHAR *lpszProcessName) 
{  
	EnableDebugPrevilige(TRUE);
	unsigned int   pid = -1;  
	int    retval = 2;  

	if (lpszProcessName == NULL)  
		return -1;

	DWORD dwRet = 0;  
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS,0 );  
	PROCESSENTRY32 processInfo;  
	processInfo.dwSize = sizeof( PROCESSENTRY32 );  
	int flag = Process32First( hSnapshot, &processInfo );  

	// Find the process with name as same as lpszProcessName  
	while (flag != 0)  
	{  
		if (strEqual(processInfo.szExeFile, lpszProcessName)) {  
			retval = 1;
			// Terminate the process.  
			pid = processInfo.th32ProcessID;
			
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);  

			if (TerminateProcess(hProcess, 0) != TRUE) { // Failed to terminate it.  
				break;  
			}
			else
			{
				retval = 0;	//关闭成功
			}
		}  

		flag = Process32Next(hSnapshot, &processInfo);   
	} // while (flag != 0)  

	CloseHandle(hSnapshot);  
	EnableDebugPrevilige(FALSE);

	return retval;  
}


bool getProcessInfoByName(const TCHAR *lpszProcessName, set<DWORD>& pids )
{
	EnableDebugPrevilige(TRUE);
	unsigned int   pid = -1;  
	int    retval = 2;  

	if (lpszProcessName == NULL)  
		return false;

	DWORD dwRet = 0;  
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS,0 );  
	PROCESSENTRY32 processInfo;  
	processInfo.dwSize = sizeof( PROCESSENTRY32 );  
	int flag = Process32First( hSnapshot, &processInfo );  

	// Find the process with name as same as lpszProcessName  
	while (flag != 0)  
	{  
		if (strEqual(processInfo.szExeFile, lpszProcessName) ) {  
			pids.insert(processInfo.th32ProcessID);
		}  

		flag = Process32Next(hSnapshot, &processInfo);   
	} // while (flag != 0)  

	CloseHandle(hSnapshot);  
	EnableDebugPrevilige(FALSE);

	return 0 != pids.size();  
}


bool getProcessInfoByNameEx(const TCHAR *lpszProcessName, set<ProcessInfo>& infos)
{
	EnableDebugPrevilige(TRUE);
	unsigned int   pid = -1;  
	int    retval = 2;  

	if (lpszProcessName == NULL)
		return false;

	DWORD dwRet = 0;  
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS,0 );  
	PROCESSENTRY32 processInfo;  
	processInfo.dwSize = sizeof( PROCESSENTRY32 );  
	int flag = Process32First( hSnapshot, &processInfo );  
	HMODULE hMods[1024];
	DWORD cbNeeded;
	HANDLE hProcess;
	TCHAR szModName[MAX_PATH];
	// Find the process with name as same as lpszProcessName  
	while (flag != 0)  
	{  
		if (strEqual(processInfo.szExeFile, lpszProcessName) ) 
		{
			hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, processInfo.th32ProcessID);
			
			if(NULL == hProcess)
				continue;
			
			if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
			{
				for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
				{
					if(GetModuleFileNameEx(hProcess,hMods[i], szModName,MAX_PATH))
					{
						infos.insert(ProcessInfo(processInfo.th32ProcessID,
							processInfo.th32ModuleID, processInfo.th32ParentProcessID,szModName));
					}
				}
			}
		}  

		flag = Process32Next(hSnapshot, &processInfo);   
	} // while (flag != 0)  

	CloseHandle(hSnapshot);  
	EnableDebugPrevilige(FALSE);

	return 0 != infos.size();  
}

//根据名字查找进程
BOOL findProcess(const TCHAR *lpszProcessName) 
{  
	EnableDebugPrevilige(TRUE);
	unsigned int   pid = -1;  
	int    retval = 2;  

	if (lpszProcessName == NULL)  
		return -1;

	DWORD dwRet = 0;  
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS,0 );  
	PROCESSENTRY32 processInfo;  
	processInfo.dwSize = sizeof( PROCESSENTRY32 );  
	int flag = Process32First( hSnapshot, &processInfo );  

	// Find the process with name as same as lpszProcessName  
	while (flag != 0)  
	{  
		if (strEqual(processInfo.szExeFile, lpszProcessName) ) {  
			return TRUE;
		}  

		flag = Process32Next(hSnapshot, &processInfo);   
	} // while (flag != 0)  

	CloseHandle(hSnapshot);  
	EnableDebugPrevilige(FALSE);

	return FALSE;  
}

/************************************************************************/
/* 
	暂时还有问题
*/
/************************************************************************/
bool startProcessSilent(const TCHAR *exeFile, const TCHAR *cmdLine, const TCHAR *workDirectory)
{
	TCHAR cmd[MAX_PATH] = {0};
	size_t cmdLen = NULL == cmdLine ? 0 : tstrLen(cmdLine)*sizeof(TCHAR);
	if(0 != cmdLen)
	{
		memcpy_s(cmd,MAX_PATH*sizeof(TCHAR),cmdLine,cmdLen);
		cmd[cmdLen] = 0;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );

	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;

	TCHAR *desktopName = _T("kmAnotherDesktop");
	//HDESK hdesk = ::OpenDesktop(desktopName,0,false,GENERIC_ALL);
	HDESK  hdesk = ::CreateDesktopW(desktopName,NULL,NULL,0,GENERIC_ALL,NULL);
	if(NULL != hdesk)
	{
		int errorCode = GetLastError();
		si.lpDesktop = desktopName;
	}

	//	si.lpDesktop = L"new desktop";

	if(CreateProcess(exeFile, cmdLen? cmd : NULL , NULL, NULL, FALSE, 0, NULL, workDirectory, &si, &pi))

	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return true;
	}
	else
	{
		return false;
	}
}

bool startProcess(const TCHAR *exeFile, const TCHAR *cmdLine, const TCHAR *workDirectory)
{
	TCHAR cmd[MAX_PATH] = {0};
	size_t cmdLen = NULL == cmdLine ? 0 : tstrLen(cmdLine)*sizeof(TCHAR);
	if(0 != cmdLen)
	{
		memcpy_s(cmd,MAX_PATH*sizeof(TCHAR),cmdLine,cmdLen);
		cmd[cmdLen] = 0;
	}
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );

	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;

	//	si.lpDesktop = L"new desktop";

	if(CreateProcess(exeFile, cmdLen? cmd : NULL , NULL, NULL, FALSE, 0, NULL, workDirectory, &si, &pi))

	{
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return true;
	}
	else
	{
		return false;
	}
}