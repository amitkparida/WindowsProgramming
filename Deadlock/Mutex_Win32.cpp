
// Deadlock:
// If Thread1 and Thread2 acquire locks in different order, they will wait for each other forever resulting in deadlock situation.
// Fix: Thread1 and Thread2 should acquire locks in same order. Deadlock will not happen.

#include <windows.h>
#include <stdio.h>
#include <process.h>

HANDLE hMutex1;
HANDLE hMutex2;

unsigned int WINAPI Thread1(PVOID pvParam)
{
	WaitForSingleObject(hMutex1, INFINITE);
	Sleep(1000);
	WaitForSingleObject(hMutex2, INFINITE);

	printf("Critical Section of Thread1.\n");

	ReleaseMutex(hMutex2);
	ReleaseMutex(hMutex1);

	return 0;
}


unsigned int WINAPI Thread2(PVOID pvParam)
{
	WaitForSingleObject(hMutex2, INFINITE);
	Sleep(1000);
	WaitForSingleObject(hMutex1, INFINITE);

	printf("Critical Section of Thread2.\n");

	ReleaseMutex(hMutex1);
	ReleaseMutex(hMutex2);

	return 0;
}


int main()
{
	HANDLE hThreads[2]{};

	hMutex1 = CreateMutex(NULL, FALSE, NULL);
	if (NULL == hMutex1)
	{
		printf("CreateMutex(1) failed.\n");
		return 0;
	}

	hMutex2 = CreateMutex(NULL, FALSE, NULL);
	if (NULL == hMutex2)
	{
		printf("CreateMutex(2) failed.\n");
		CloseHandle(hMutex1);
		return 0;
	}

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, Thread1, NULL, 0, NULL);
	if (NULL == hThreads[0])
	{
		printf("Thread1 can't be created.\n");
		CloseHandle(hMutex1);
		CloseHandle(hMutex2);
		return 0;
	}

	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, Thread2, NULL, 0, NULL);
	if (NULL == hThreads[1])
	{
		printf("Thread2 can't be created.\n");
		CloseHandle(hThreads[0]);
		CloseHandle(hMutex1);
		CloseHandle(hMutex2);

		return 0;
	}

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);
	CloseHandle(hMutex1);
	CloseHandle(hMutex2);

	system("PAUSE");

	return 0;
}
