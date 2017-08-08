#include "./thirdpart/utils.h"
#include <tchar.h>
#include <cassert>

/************************************************************************/
/* 
	使用的 power sync link 经常出问题，副控机(非直连键盘，鼠标的机器)经常卡
	死，鼠标，键盘用不了
	
	偶然发现，只需要重启 LinkEngKM.exe 这个进程就可以了.

	实际上，power sync link 有三个进程: power sync link 是 UI 进程，MacKMLink 进程
	估计是逻辑处理进程，LinkEngKM 处理两台机器之间的数据传输.由于公司的网络策略
	变更，导致 LinkEngKm 进程挂掉.于是也就有了上面的解决思路。
	杀掉 LinkEngKM 进程后，只要机器发生输入事件，它就会被 MacKMLink 进程启动。因此
	我们只需要杀掉它就可以了。

	后来陆续发现，MacMLink 进程也可能崩溃，简单的处理是，杀掉这个进程，让UI 进程去启动
	这个进程。

	本程序的逻辑是: 
		1.设置超时时间(1分钟)，若没有输入事件则杀掉 LinkEngKM 进程(程序自我定期触发)。[已去掉此逻辑]
		2.锁屏，重启进程(程序留出的主动触发接口)。

	后面发现一个可以优化的问题: 
		在副控机上杀掉 LinkEngKM 进程后，由主控机进入到副控机时，会有一点不流畅，因为副
		机在接收到输入事件时才会去启动 LinkEngKm ，于是首次进入时就会有卡顿现象。优化的方案是，每
		次杀掉 LinkEngKM 后发送一个模拟输入，使 LinkEngKM 启动.
*/
/************************************************************************/

//没有键盘鼠标事件多长时间了
DWORD loseInputWithSecs()
{
	LASTINPUTINFO lii;
	
	lii.cbSize = sizeof(LASTINPUTINFO);
	//取得自上一次发生 input 事件时，系统已运行的时间段. lii.dwTime=GetTickCount. 单位毫秒
	GetLastInputInfo(&lii);
	DWORD lastInputTickcount = lii.dwTime;
	return (GetTickCount() - lastInputTickcount)/1000;
}

//1 当前没有锁定，且上次监测到也是没锁定
//2 当前没有锁定，且上次监测到锁定
//3 当前锁定，且上次监测到未锁定
//4 当前锁定，且上次监测到也是锁定
//以上, 若返回 2 或 3 则表示有差异
int screenLockState()
{
	static BOOL lastScreenLocked = FALSE;
	int retkd = 0;
	BOOL curLocked = hasScreenLocked();
	if(!curLocked)
	{
		if(!lastScreenLocked)
		{
			retkd = 1;
		}
		else
		{
			retkd = 2;
		}
	}
	else
	{
		if(!lastScreenLocked)
		{
			retkd = 3;
		}
		else
		{
			retkd = 4;
		}
	}
	lastScreenLocked = curLocked;
	return retkd;
}


bool killLinkEngineIfCrash()
{
	set<DWORD> pids;
	if(!getProcessInfoByName(_T("WerFault.exe"), pids))
	{
		return false;
	}

	HWND faultWnd = NULL;//GetForegroundWindow();
	faultWnd = ::FindWindow(0,_T("Link Engine"));
	DWORD wndProcessId = 0;
	GetWindowThreadProcessId(faultWnd,&wndProcessId);
	if(pids.end() != pids.find(wndProcessId))
	{
		LOG_INFO("Link Engine has crashed");
		KillProcess(wndProcessId,0);
		KillProcessByName(_T("LinkEngKM.exe"));
		return true;
	}
	return false;
}

bool killMacKMLinkIfCrash()
{
	set<DWORD> pids;
	if(!getProcessInfoByName(_T("WerFault.exe"), pids))
	{
		return false;
	}

	HWND faultWnd = NULL;//GetForegroundWindow();
	faultWnd = ::FindWindow(0,_T("Mac KM Link"));
	DWORD wndProcessId = 0;
	GetWindowThreadProcessId(faultWnd,&wndProcessId);
	if(pids.end() != pids.find(wndProcessId))
	{
		LOG_INFO("Mac KM Link has crashed");
		KillProcess(wndProcessId,0);
		KillProcessByName(_T("MacKMLink.exe"));
		return true;
	}
	return false;

}

TCHAR powerKMExePath[MAX_PATH];
TCHAR powerKMWorkDir[MAX_PATH];
bool storePoerKMpath(ProcessInfo* thisInfo=NULL)
{
	set<ProcessInfo> infos;
	ProcessInfo *info = thisInfo;
	if(NULL == info)
	{	
		if(!getProcessInfoByNameEx(_T("PowerSyncKM.exe"),infos) || infos.empty())
		{
			return false;
		}
		info = &(*(infos.begin()));
	}

	size_t len = tstrLen(info->_path);
	memcpy_s(powerKMExePath,sizeof(TCHAR)* MAX_PATH,info->_path,len*sizeof(TCHAR));
	powerKMExePath[len] = 0;

	size_t charLen = 0;
	char * chPath = wideCharSetToDefaultMult(powerKMExePath,len,&charLen);

	string workDir;
	getParentDir(chPath, workDir);

	delete[] chPath;

	size_t whLen;
	TCHAR *chWorkDir = defaultMultCharSetToWide(workDir.c_str(),workDir.length(),&whLen);
	
	memcpy_s(powerKMWorkDir,MAX_PATH*sizeof(TCHAR),chWorkDir,whLen*sizeof(TCHAR));
	powerKMWorkDir[whLen] = 0;
	delete[] chWorkDir;
	return true;
}

bool restartAll()
{
	set<ProcessInfo> infos;
	if(getProcessInfoByNameEx(_T("PowerSyncKM.exe"),infos) && !infos.empty())
	{
		if(1 < infos.size())
		{
			LOG_INFO("PowerSyncKM.exe started more than one time");
		}

		set<ProcessInfo> ::iterator it = infos.begin();
		for(;it != infos.end(); ++it)
		{
			ProcessInfo &info = *it;
			KillProcess(info._pid,0);
		}

		KillProcessByName(_T("LEWD.exe"));
		KillProcessByName(_T("LinkEngKM.exe"));
		KillProcessByName(_T("MacKMLink.exe"));

		storePoerKMpath(&(*infos.begin()));
	}
	
	if(0 != tstrLen(powerKMExePath))
		startProcess(powerKMExePath, NULL, powerKMWorkDir);
	return true;
}

static int timeoutSecs = 60;

bool killDetetor()
{
	bool timeout = loseInputWithSecs()>timeoutSecs;
	if (timeout)
	{
		LOG_INFO("timeout");
	}
	return timeout;
}

/************************************************************************/
/* 
	触发机器输入事件，UI 进程启动 Mac Link 进程(如果没启动)，Mac Link 进程
	会启动 linkeng Km 进程(如果没启动)
*/
/************************************************************************/
void startLinkEngine()
{
	BYTE vks[] = {VK_CONTROL,VK_MULTIPLY};	// ctrl + *
	sendToForegroundWnd(vks, sizeof(vks));
}

void monitorCallback(void *data)
{
	storePoerKMpath();
	if(3 == screenLockState())
	{
		LOG_INFO("screen lock just now");
		restartAll();
		return;
	}
	if(killLinkEngineIfCrash())
	{
		startLinkEngine();
		LOG_INFO("deal success!");
	}
	if(killMacKMLinkIfCrash())
	{
		startLinkEngine();
		LOG_INFO("deal success!");
	}
}

void monitor()
{
	timeRun(monitorCallback, 0, 1);
}

void testModule()
{
	set<ProcessInfo> infos;
	getProcessInfoByNameEx(_T("PowerSyncKM.exe"),infos);
	return;
}



int WINAPI WinMain(
				   HINSTANCE hInstance,       //程序当前实例的句柄，以后随时可以用GetModuleHandle(0)来获得  
				   HINSTANCE hPrevInstance,   //这个参数在Win32环境下总是0，已经废弃不用了  
				   char * lpCmdLine,          //指向以/0结尾的命令行，不包括EXE本身的文件名，可以用GetCommandLine()来获取完整的命令行  
				   int nCmdShow               //指明应该以什么方式显示主窗口  
				   )
{
	if(tryFirstRunModule())
	{
		LOG_INFO("app has been started");
		return -1;
	}
	monitor();
	return 0;
}