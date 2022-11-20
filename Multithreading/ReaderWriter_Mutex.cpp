
// This module implements the multiple writers multiple readers problem.

#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <process.h>

HANDLE g_hReadMutex;
HANDLE g_hWriteMutex;

int g_iCount = 1;
BOOLEAN g_bTerminate = FALSE;


unsigned int WINAPI
ReaderThread(
	PVOID pvParam
)
{
	DWORD dwRetVal;
	DWORD dwThreadID;

	dwRetVal = 0;

	if (TRUE == g_bTerminate)
	{
		return 1;
	}

	dwRetVal = WaitForSingleObject(g_hWriteMutex, INFINITE);
	if (WAIT_FAILED == dwRetVal)
	{
		printf("\n Wait for g_hMutex failed.");

		return 1;
	}

	dwThreadID = GetCurrentThreadId();

	_tprintf(_T("Reader Thread %d\t Reading.\n"), dwThreadID);
	Sleep(100);
	_tprintf(_T("Reader Thread %d\t Completed.\n"), dwThreadID);

	ReleaseMutex(g_hWriteMutex);

	return 0;
}


unsigned int WINAPI
WriterThread(
	PVOID pvParam
)
{
	DWORD dwRetVal;
	DWORD dwRetVal1;
	DWORD dwThreadID;

	dwRetVal = 0;

	if (TRUE == g_bTerminate)
	{
		return 1;
	}

	dwRetVal = WaitForSingleObject(g_hReadMutex, INFINITE);
	if (WAIT_FAILED == dwRetVal)
	{
		printf("\n Wait for g_hReadMutex failed.");

		return 1;
	}

	dwRetVal1 = WaitForSingleObject(g_hWriteMutex, INFINITE);
	if (WAIT_FAILED == dwRetVal)
	{
		printf("\n Wait for g_hWriteMutex failed.");

		return 1;
	}

	dwThreadID = GetCurrentThreadId();

	_tprintf(_T("Writer Thread %d\t Writing.\n"), dwThreadID);
	Sleep(1000);
	_tprintf(_T("Writer Thread %d\t Completed.\n"), dwThreadID);

	ReleaseMutex(g_hReadMutex);
	ReleaseMutex(g_hWriteMutex);
	return 0;
}


void main()
{
	int iLoop;
	DWORD dwRetVal;
	HANDLE hThread[10];

	dwRetVal = 0;

	g_hReadMutex = CreateMutex(NULL, FALSE, NULL);
	if (NULL == g_hReadMutex)
	{
		_tprintf(_T("\n CreateMutex() for reading failed."));

		return;
	}

	g_hWriteMutex = CreateMutex(NULL, FALSE, NULL);
	if (NULL == g_hWriteMutex)
	{
		_tprintf(_T("\n CreateMutex() for reading failed."));

		return;
	}

	for (iLoop = 0; iLoop < 5; iLoop++)
	{
		hThread[iLoop] = (HANDLE)_beginthreadex(
			NULL,
			0,
			ReaderThread,
			NULL,
			0,
			NULL
		);
		if (NULL == hThread[iLoop])
		{
			_tprintf(_T("Thread creation Failuer \n"));
			iLoop--;
			while (iLoop != 0)
			{
				DeleteObject(hThread[iLoop--]);
			}
			CloseHandle(g_hReadMutex);
			CloseHandle(g_hWriteMutex);

			exit(1);
		}
	}

	for (iLoop = 5; iLoop < 10; iLoop++)
	{
		hThread[iLoop] = (HANDLE)_beginthreadex(
			NULL,
			0,
			WriterThread,
			NULL,
			0,
			NULL
		);
		if (NULL == hThread[iLoop])
		{
			_tprintf(_T("Thread creation failed.\n"));
			iLoop--;
			while (iLoop != 5)
			{
				DeleteObject(hThread[iLoop--]);
			}
			CloseHandle(g_hReadMutex);
			CloseHandle(g_hWriteMutex);

			exit(1);
		}
	}

	dwRetVal = WaitForMultipleObjects(
		10,
		hThread,
		TRUE,
		INFINITE
	);
	if (WAIT_FAILED == dwRetVal)
	{
		_tprintf(_T("\nWait Failed. \n Terminating abnormaly.\n"));
	}
	else
	{
		_tprintf(_T("\nEnd Of Execution.\n"));
	}

	for (iLoop = 0; iLoop < 10; iLoop++)
	{
		CloseHandle(hThread[iLoop]);
	}

	CloseHandle(g_hReadMutex);
	CloseHandle(g_hWriteMutex);

	getchar();
}


