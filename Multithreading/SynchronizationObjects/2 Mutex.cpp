
// https://learn.microsoft.com/en-us/windows/win32/sync/mutex-objects
// https://learn.microsoft.com/en-us/windows/win32/sync/using-mutex-objects

//Recursive Mutex:
// A thread can acquire ownership of a mutex object that it already owns (recursive ownership), 
// but a recursively acquired mutex object is not set to the Signaled state until the thread releases 
// its ownership completely. Such a thread must explicitly release the mutex as many times as it acquired 
// ownership before another thread can acquire the mutex.

// Below code is example of cursive mutex usage. Single thread acquires the same mutex twice recursively.
// There will be no deadlock.
/* 
void Fun1() {
	mtx.lock();
	Fun2();
	mtx.unlock();
}
void Fun2() {
	mtx.lock();
	//some code
	mtx.unlock();
}
*/


// This program creates two synchronized threads using mutex which print odd and even numbers respectively.
#include <windows.h>
#include <stdio.h>
#include <process.h>

HANDLE hMutex;
int g_iCount = 0;
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

		dwRetVal = WaitForSingleObject(hMutex, INFINITE);
		if (WAIT_OBJECT_0 != dwRetVal)
		{
			printf("Wait for hMutex in Odd Thread failed.\n");
			return 1;
		}

		if (1 == (g_iCount % 2))
		{
			printf("Thread ID: %d, Odd No: %d\n", GetCurrentThreadId(), g_iCount++);
		}

		ReleaseMutex(hMutex);
	}

	return 0;
}

unsigned int WINAPI ThreadEven(	PVOID pvParam)
{
	DWORD dwRetVal = 0;

	while (MAX_COUNT > g_iCount)
	{
		dwRetVal = WaitForSingleObject(hMutex, INFINITE);
		if (WAIT_OBJECT_0 != dwRetVal)
		{
			printf("\n Wait for hMutex in Even Thread failed.");
			return 1;
		}

		if (0 == (g_iCount % 2))
		{
			printf("Thread ID: %d, Even No: %d\n", GetCurrentThreadId(), g_iCount++);
		}

		ReleaseMutex(hMutex);
	}

	return 0;
}


int main()
{
	int iLoop;
	DWORD dwRetVal;
	HANDLE hThreads[2];

	dwRetVal = 0;

	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (NULL == hMutex)
	{
		printf("CreateMutex() failed.\n");
		return 0;
	}

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadEven, NULL, 0, NULL);
	if (NULL == hThreads[0])
	{
		printf("ThreadEven can't be created.\n");
		CloseHandle(hMutex);
		return 0;
	}

	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadOdd, NULL, 0, NULL);
	if (NULL == hThreads[1])
	{
		g_bTerminate = TRUE;
		printf("\n ThreadOdd can't be created.\n");
		dwRetVal = WaitForSingleObject(hThreads[0], INFINITE);
		if (WAIT_OBJECT_0 != dwRetVal)
		{
			printf("Wait for ThreadEven failed.\n");
		}
		CloseHandle(hThreads[0]);
		CloseHandle(hMutex);

		return 0;
	}

	//ReleaseMutex(hMutex);	//	It's needed if main thread gains initial ownership, i.e the 2nd parameter of CreateMutex() is TRUE.

	dwRetVal = WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	if (WAIT_FAILED == dwRetVal)
	{
		printf("Wait Failed. Terminating abnormaly.\n");
	}
	else
	{
		printf("End Of Execution.\n");
	}

	for (iLoop = 0; iLoop < 2; iLoop++)
	{
		CloseHandle(hThreads[iLoop]);
	}

	CloseHandle(hMutex);
	getchar();

	return 0;
}
