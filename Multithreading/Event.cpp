
// https://learn.microsoft.com/en-us/windows/win32/sync/event-objects
// https://learn.microsoft.com/en-us/windows/win32/sync/using-event-objects

// Manual Reset Event: 
// 1. It schedules all the threads waiting on an event ONCE the event gets signaled.
// 2. When the state of a Manual reset event object is signaled, it remains signaled until it is explicitly 
//    reset to nonsignaled by the ResetEvent function. Any number of waiting threads, or threads that subsequently 
//    begin wait operations for the specified event object, can be released while the object's state is signaled.

// Auto Reset Event: 
// 1. It schedules only one of the threads(we don't know which one) waiting on event once the event object gets signaled.
// 2. When the state of an Auto reset event object is signaled, it remains signaled until a single waiting thread is released; 
//    the system then automatically resets the state to nonsignaled.If no threads are waiting, the event object's state remains signaled.


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
			// As EventEven is a Manual Reset Event, call ResetEvent to make it nonsignaled, so that no other thread's wait function (on EventEven) does not return.
			// For Auto Reset Events, calling ResetEvent is not required.
			ResetEvent(g_hEventEven);//EventEven is nonsignaled. Now ThreadOdd will wait until ThreadEven calls SetEvent(g_hEventEven).

			if (1 == (g_iCount % 2))
			{
				printf("ThreadID:%d , Odd No : %d\n", GetCurrentThreadId(), g_iCount++);
			}

			SetEvent(g_hEventOdd);//EventOdd is signaled. Now ThreadEven (who was waiting on EventOdd) will be scheduled.
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
	DWORD dwRetVal = 0;

	while (MAX_COUNT > g_iCount)
	{
		dwRetVal = WaitForSingleObject(g_hEventOdd, INFINITE);
		if (WAIT_OBJECT_0 == dwRetVal)
		{
			// As EventOdd is a Manual Reset Event, call ResetEvent to make it nonsignaled, so that no other thread's wait function (on EventOdd) does not return.
			// For Auto Reset Events, calling ResetEvent is not required.
			ResetEvent(g_hEventOdd); //EventOdd is nonsignaled. Now ThreadEven will wait until ThreadOdd calls SetEvent(g_hEventOdd).

			if (0 == (g_iCount % 2))
			{
				printf("ThreadID:%d , Even No: %d\n", GetCurrentThreadId(), g_iCount++);
			}

			SetEvent(g_hEventEven); //EventEven is signaled. Now ThreadOdd (who was waiting on EventEven) will be scheduled.
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
		printf("Odd Event Creation Failed.\n");
		CloseHandle(g_hEventOdd);
		return 1;
	}


	//Note that EvenEvent's initial state is set as signaled (3rd parameter is TRUE)
	g_hEventEven = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (NULL == g_hEventEven)
	{
		printf("Even Event Creation Failed\n");
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



