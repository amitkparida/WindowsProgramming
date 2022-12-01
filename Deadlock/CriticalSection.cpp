
// Deadlock:
// If Thread1 and Thread2 acquire locks in different order, they will wait for each other forever resulting in deadlock situation.
// Fix: Thread1 and Thread2 should acquire locks in same order. Deadlock will not happen.

#include <stdio.h>
#include <windows.h>
#include <process.h>

CRITICAL_SECTION cs1;
CRITICAL_SECTION cs2;

unsigned int WINAPI Thread1(PVOID pvParam)
{
	EnterCriticalSection(&cs1);
	Sleep(1000);
	EnterCriticalSection(&cs2);

	printf("Critical Section of Thread1.\n");

	LeaveCriticalSection(&cs2);
	LeaveCriticalSection(&cs1);

	return 0;
}


unsigned int WINAPI Thread2(PVOID pvParam)
{
	EnterCriticalSection(&cs2);
	Sleep(1000);
	EnterCriticalSection(&cs1);

	printf("Critical Section of Thread2.\n");

	LeaveCriticalSection(&cs1);
	LeaveCriticalSection(&cs2);

	return 0;
}


int main()
{
	HANDLE hThreads[2]{};

	InitializeCriticalSection(&cs1);
	InitializeCriticalSection(&cs2);

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, Thread1, NULL, 0, NULL);
	if (NULL == hThreads[0])
	{
		printf("Thread1 can't be created.\n");
		DeleteCriticalSection(&cs1);
		DeleteCriticalSection(&cs2);

		return 0;
	}

	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, Thread2, NULL, 0, NULL);
	if (NULL == hThreads[1])
	{
		printf("Thread2 can't be created.\n");
		WaitForSingleObject(hThreads[0], INFINITE);
		CloseHandle(hThreads[0]);
		DeleteCriticalSection(&cs1);
		DeleteCriticalSection(&cs2);

		return 0;
	}

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);
	DeleteCriticalSection(&cs1);
	DeleteCriticalSection(&cs2);
	system("pause");

	return 0;
}