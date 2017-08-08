#include <stdio.h>
#include "./utils.h"

#ifdef _WIN64
#pragma comment(lib,"./thirdpart/uchardet/uCharDetDll_64.lib")
#else
#pragma comment(lib,"./thirdpart/uchardet/uCharDetDll_32.lib")
#endif

void main()
{
	if(tryFirstRunModule())
	{
		printf("the process has been runned. will exit!");
		system("PAUSE");
		return;
	}
	printf("%s\r\n",strEqual("abcd","abcd")?"yes":"no");
	int array[] = {46,47,48,49};
	intsToFile(array,sizeof(array)/sizeof(int),"ints.txt");
	const static size_t l=23,n=37;
	wchar_t chs[l*n+1] = {0};
	randomRepeatStrLN(chs,l,n);
	printf("random str len: %d\r\n", tstrLen(chs));
	LOG_INFO("edgarlli test");
	system("PAUSE");
}