
// https://learn.microsoft.com/en-us/windows/win32/sync/semaphore-objects
// https://learn.microsoft.com/en-us/windows/win32/sync/using-semaphore-objects


// 1. Semaphore kernel objects are used for resource counting.
// 2. They contain a usage count, as all kernel objects do, but they also contain two additional signed 32 - bit values: 
//    i. Maximum resource count (Maximum number of resources that the semaphore can control)
//    ii. Current resource count (number of these resources that are currently available)
// 
// 3. If the current resource count is 0, the semaphore is nonsignaled.[Threads waiting to acquire the semaphore will be blocked]
// 4. If the current resource count is greater than 0, the semaphore is signaled.[Threads waiting to acquire the semaphore will be readied/scheduled]
// 5. The system never allows the current resource count to be negative. 
// 6. The current resource count can never be greater than the maximum resource count.


// This program creates two synchronized threads using Semaphore which print odd and even numbers respectively.
#include <windows.h>
#include <stdio.h>
#include <process.h>

HANDLE hSemaphore;
int g_iCount = 1;
BOOLEAN g_bTerminate = FALSE;
#define MAX_COUNT	20

unsigned int WINAPI ThreadOdd(PVOID pvParam)
{
	DWORD dwRetVal = 0;

	while (MAX_COUNT > g_iCount)
	{
		if (TRUE == g_bTerminate)
		{
			return 1;
		}

		dwRetVal = WaitForSingleObject(hSemaphore, INFINITE);
		if (WAIT_OBJECT_0 != dwRetVal)
		{
			printf("Wait for hSemaphore in Odd Thread failed.\n");
			return 1;
		}

		if (1 == (g_iCount % 2))
		{
			printf("Thread ID: %d, Odd No: %d\n", GetCurrentThreadId(), g_iCount++);
		}

		ReleaseSemaphore(hSemaphore, 1, NULL);
	}

	return 0;
}

unsigned int WINAPI ThreadEven(PVOID pvParam)
{
	DWORD dwRetVal = 0;

	while (MAX_COUNT > g_iCount)
	{
		dwRetVal = WaitForSingleObject(hSemaphore, INFINITE);
		if (WAIT_OBJECT_0 != dwRetVal)
		{
			printf("Wait for hSemaphore in Even Thread failed.\n");

			return 1;
		}

		if (0 == (g_iCount % 2))
		{
			printf("Thread ID: %d, Even No: %d\n", GetCurrentThreadId(), g_iCount++);
		}

		ReleaseSemaphore(hSemaphore, 1, NULL);
	}

	return 0;
}

int main()
{
	int iLoop;
	DWORD dwRetVal = 0;
	HANDLE hThreads[2] = {};

	hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	if (NULL == hSemaphore)
	{
		printf("CreateSemaphore() failed.\n");
	}

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadOdd, NULL, 0, NULL);
	if (NULL == hThreads[0])
	{
		printf("Thread1 can't be created.\n");
		CloseHandle(hSemaphore);
		return 0;
	}

	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadEven,NULL, 0, NULL);
	if (NULL == hThreads[1])
	{
		g_bTerminate = TRUE;
		printf("\n Thread2 can't be created.\n");
		dwRetVal = WaitForSingleObject(hThreads[0], INFINITE);
		if (WAIT_OBJECT_0 != dwRetVal)
		{
			printf("Wait for Odd Thread failed.\n");
		}
		CloseHandle(hThreads[0]);
		CloseHandle(hSemaphore);
		return 0;
	}

	dwRetVal = WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	if (WAIT_FAILED == dwRetVal)
	{
		printf(" Wait Failed. Terminating abnormaly\n");
	}
	else
	{
		printf("End Of Execution\n");
	}

	for (iLoop = 0; iLoop < 2; iLoop++)
	{
		CloseHandle(hThreads[iLoop]);
	}

	CloseHandle(hSemaphore);
	getchar();
	return 0;
}


