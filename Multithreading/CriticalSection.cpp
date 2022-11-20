
// https://learn.microsoft.com/en-us/windows/win32/sync/critical-section-objects
// https://learn.microsoft.com/en-us/windows/win32/sync/using-critical-section-objects

// Video: https://www.youtube.com/watch?v=_Cz1-kiBljA&list=PLhnN2F9NiVmAmMdwXEkpmSpBIogC8MqT6&index=6

// CRITICAL SECTION is a user mode synchronization object 
// i.e., this synchronization object can only synchronize the threads running under the same process.
//
// Recursion Count : How many times a thread has acquired a critical section objectand to the same number of times you need to release it i.e.you need to just : Leave the critical section those many times.
//
// Spin Count : The number of times the API TryEnterCriticalSection will poll the critical section object to find if it is available.


// This program creates two synchronized threads using CriticalSection which print odd and even numbers respectively.
#include <stdio.h>
#include <windows.h>
#include <process.h>

int g_iCount = 1;
#define	MAX_COUNT	20
CRITICAL_SECTION g_cs;

unsigned int WINAPI ThreadOdd(PVOID pvParam)
{
	while (MAX_COUNT > g_iCount)
	{
		EnterCriticalSection(&g_cs);

		if (0 != (g_iCount % 2))
		{
			printf("Thread ID : %d, Odd No : %d\n", GetCurrentThreadId(), g_iCount++);
		}

		LeaveCriticalSection(&g_cs);
	}

	return 0;
}


unsigned int WINAPI ThreadEven(PVOID pvParam)
{
	while (MAX_COUNT > g_iCount)
	{
		EnterCriticalSection(&g_cs);

		if (0 == (g_iCount % 2))
		{
			printf("Thread ID : %d, Even No : %d\n", GetCurrentThreadId(), g_iCount++);
		}

		LeaveCriticalSection(&g_cs);
	}

	return 0;
}


int main()
{
	DWORD dwRetVal = 0;
	HANDLE hThreads[2];

	InitializeCriticalSection(&g_cs);

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadOdd, NULL, 0, NULL);
	if (NULL == hThreads[0])
	{
		printf("Thread1 can't be created.\n");
		DeleteCriticalSection(&g_cs);

		return 0;
	}

	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadEven, NULL, 0, NULL);
	if (NULL == hThreads[1])
	{
		printf("Thread2 can't be created.\n");
		WaitForSingleObject(hThreads[0], INFINITE);
		CloseHandle(hThreads[0]);
		DeleteCriticalSection(&g_cs);

		return 0;
	}

	dwRetVal = WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	if (WAIT_FAILED == dwRetVal || WAIT_ABANDONED == dwRetVal || WAIT_TIMEOUT == dwRetVal)
	{
		printf("Wait Failed. Terminating abnormaly\n");
	}
	else
	{
		printf("End Of Execution\n");
	}

	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);

	DeleteCriticalSection(&g_cs);
	getchar();

	return 0;
}