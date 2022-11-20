
// This module solves the Bridge Crossing problem.

#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <process.h>

#define	SEMAPHORE_MAX_LIMIT		2
#define	THREAD_MAX_COUNT		10
#define	SEMAPHORE_NAME			_T("Semaphore")

HANDLE g_hSemaphore;
HANDLE g_hThread[THREAD_MAX_COUNT];

volatile long g_iCurrCount = 0;

unsigned int WINAPI
LeftThreadFunction(
	PVOID pvParam
)
{
	DWORD dwRetVal;

	if (SEMAPHORE_MAX_LIMIT <= g_iCurrCount)
	{
		_tprintf(_T("(L) Person %d  waiting.\n"), GetCurrentThreadId());
	}

	dwRetVal = WaitForSingleObject(g_hSemaphore, INFINITE);

	if (WAIT_OBJECT_0 == dwRetVal)
	{
		_tprintf(_T("(L) Person %d  Crossing. "), GetCurrentThreadId());
		InterlockedExchangeAdd(&g_iCurrCount, 1);
		_tprintf(_T("(%d more persons can cross now.)\n"), SEMAPHORE_MAX_LIMIT - g_iCurrCount);
		Sleep(500);
		_tprintf(_T("(L) Person %d  Crossed. "), GetCurrentThreadId());
		InterlockedExchangeAdd(&g_iCurrCount, -1);
		_tprintf(_T("(%d more persons can cross now.)\n"), SEMAPHORE_MAX_LIMIT - g_iCurrCount);
		ReleaseSemaphore(g_hSemaphore, 1, NULL);
	}

	return 0;
}


unsigned int WINAPI
RightThreadFunction(
	PVOID pvParam
)
{
	DWORD dwRetVal;

	if (SEMAPHORE_MAX_LIMIT <= g_iCurrCount)
	{
		_tprintf(_T("(R) Person %d  waiting.\n"), GetCurrentThreadId());
	}

	dwRetVal = WaitForSingleObject(g_hSemaphore, INFINITE);

	if (WAIT_OBJECT_0 == dwRetVal)
	{
		_tprintf(_T("(R) Person %d  crossing. "), GetCurrentThreadId());
		InterlockedExchangeAdd(&g_iCurrCount, 1);
		_tprintf(_T("(%d more persons can cross now.)\n"), SEMAPHORE_MAX_LIMIT - g_iCurrCount);
		Sleep(1000);
		_tprintf(_T("(R) Person %d  crossed. "), GetCurrentThreadId());
		InterlockedExchangeAdd(&g_iCurrCount, -1);
		_tprintf(_T("(%d more persons can cross now.)\n"), SEMAPHORE_MAX_LIMIT - g_iCurrCount);
		ReleaseSemaphore(g_hSemaphore, 1, NULL);
	}

	return 0;
}


void main()
{
	int iLoop;
	DWORD dwRetVal;

	g_hSemaphore = CreateSemaphore(NULL, SEMAPHORE_MAX_LIMIT, SEMAPHORE_MAX_LIMIT, SEMAPHORE_NAME);
	if (NULL == g_hSemaphore)
	{
		_tprintf(_T("CreateSemaphore() for left failed\n"));
	}

	for (iLoop = 0; THREAD_MAX_COUNT > iLoop; iLoop++)
	{
		if (0 == (rand() % 2))
		{
			g_hThread[iLoop] = (HANDLE)_beginthreadex(
				NULL,
				0,
				LeftThreadFunction,
				(PVOID)&iLoop,
				0,
				NULL
			);
		}
		else
		{
			g_hThread[iLoop] = (HANDLE)_beginthreadex(
				NULL,
				0,
				RightThreadFunction,
				(PVOID)&iLoop,
				0,
				NULL
			);
		}

		if (NULL == g_hThread[iLoop])
		{
			_tprintf(_T("Thread creation failed \n"));
			iLoop--;

			while (0 <= iLoop)
			{
				CloseHandle(g_hThread[iLoop--]);
			}

			CloseHandle(g_hSemaphore);

			exit(1);
		}

		Sleep(500);
	}

	dwRetVal = WaitForMultipleObjects(
		THREAD_MAX_COUNT,
		g_hThread,
		TRUE,
		INFINITE
	);
	if (WAIT_FAILED == dwRetVal)
	{
		_tprintf(_T("Wait Failed \n Terminating Abnormaly.\n"));
	}
	else
	{
		_tprintf(_T("End Of Execution\n"));
	}

	for (iLoop = 0; THREAD_MAX_COUNT > iLoop; iLoop++)
	{
		CloseHandle(g_hThread[iLoop]);
	}

	getchar();

	CloseHandle(g_hSemaphore);

	return;
}



