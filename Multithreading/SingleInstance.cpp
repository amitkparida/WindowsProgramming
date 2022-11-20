
// 	This module creates an application such that one instance of it must be running at a given time.

#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <process.h>

HANDLE g_hMutex;

int main()
{
	DWORD dwErrorCode;

	g_hMutex = CreateMutex(NULL, FALSE, _T("MyMutex"));
	dwErrorCode = GetLastError();
	if (NULL == g_hMutex)
	{
		_tprintf(_T("CreateMutex() failed\n"));
		getchar();
		return 1;
	}

	if (ERROR_ALREADY_EXISTS == dwErrorCode)
	{
		return 1;
	}

	getchar();

	CloseHandle(g_hMutex);
	return 0;
}
