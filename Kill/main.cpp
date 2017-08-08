#include "../MyUtilman/thirdpart/utils.h"
#include <tchar.h>

void main(int argc, char ** argv)
{
	if(1 < argc && '?' == *(argv[1]) && findProcess(_T("kmLinkPatch.exe")))
	{
		printf("kmLinkPatch is running.\r\n");
		return;
	}
	int ret = KillProcessByName(_T("kmLinkPatch.exe"));
	if(0 == ret)
	{
		printf("kill kmLinkPatch success\r\n");
	}
	else if(2 == ret)
	{
		printf("no kmLinkPatch.exe running\r\n");
	}
	else
	{
		printf("kill kmLinkPatch failed. %d\r\n", GetLastError());
	}
	system("PAUSE");
}