
// https://learn.microsoft.com/en-us/windows/win32/sync/event-objects
// https://learn.microsoft.com/en-us/windows/win32/sync/using-event-objects

// Manual Reset Event: It schedules all the threads waiting on an event ONCE the event gets signaled.
// Auto Reset Event: It schedules only one of the threads(we don't know which one) waiting on event once the event object gets signaled.

// This program creates two synchronized threads using events which print odd and even numbers respectively.
#include <windows.h>
#include <stdio.h>
#include <process.h>

HANDLE g_hEventOdd;
HANDLE g_hEventEven;

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

		dwRetVal = WaitForSingleObject(g_hEventEven, INFINITE);

		if (TRUE == g_bTerminate)
		{
			return 1;
		}

		if (WAIT_OBJECT_0 == dwRetVal)
		{
			if (1 == (g_iCount % 2))
			{
				printf("ThreadID:%d , Odd No : %d\n", GetCurrentThreadId(), g_iCount++);
			}

			ResetEvent(g_hEventEven);//Even Thread can begin only.
			SetEvent(g_hEventOdd);//Odd Thread is signaled, i.e finished.
		}
		else
		{
			printf("\n Wait for even event failed\n");
			return 1;
		}
	}

	return 0;
}


unsigned int WINAPI ThreadEven(PVOID pvParam)
{
	DWORD dwRetVal;

	dwRetVal = 0;

	while (MAX_COUNT > g_iCount)
	{
		dwRetVal = WaitForSingleObject(g_hEventOdd, INFINITE);

		if (WAIT_OBJECT_0 == dwRetVal)
		{
			if (0 == (g_iCount % 2))
			{
				printf("ThreadID:%d , Even No: %d\n", GetCurrentThreadId(), g_iCount++);
			}

			ResetEvent(g_hEventOdd);
			SetEvent(g_hEventEven);
		}
		else
		{
			printf("Wait for odd event failed\n");

			return 1;
		}
	}

	return 0;
}

int main()
{
	int iLoop;
	DWORD dwRetVal = 0;
	HANDLE hThreads[2]{};
	unsigned int dwThreadOddID;
	unsigned int dwThreadEvenID;

	g_hEventOdd = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == g_hEventOdd)
	{
		printf("Event Creation Failed.\n");
		CloseHandle(g_hEventOdd);
		return 1;
	}

	g_hEventEven = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (NULL == g_hEventEven)
	{
		printf("Event Creation Failed\n");
		CloseHandle(g_hEventEven);
		return 1;
	}

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadOdd, NULL, 0, &dwThreadOddID);
	if (NULL == hThreads[0])
	{
		printf("Thread1 can't be created.\n");
		CloseHandle(g_hEventOdd);
		CloseHandle(g_hEventEven);

		return 0;
	}

	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadEven, NULL, 0, &dwThreadEvenID	);
	if (NULL == hThreads[1])
	{
		g_bTerminate = TRUE;
		SetEvent(g_hEventEven);
		printf("Thread2 can't be created.\n");
		dwRetVal = WaitForSingleObject(hThreads[0], INFINITE);
		if (WAIT_OBJECT_0 != dwRetVal)
		{
			printf("Wait for Odd Thread failed.\n");
		}
		CloseHandle(hThreads[0]);
		CloseHandle(g_hEventOdd);
		CloseHandle(g_hEventEven);

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

	for (iLoop = 0; iLoop < 2; iLoop++)
	{
		CloseHandle(hThreads[iLoop]);
	}

	CloseHandle(g_hEventOdd);
	CloseHandle(g_hEventEven);

	getchar();
	return 0;
}



