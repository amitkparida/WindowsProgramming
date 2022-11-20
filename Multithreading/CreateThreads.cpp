#include <windows.h>
#include <stdio.h>
#include<TCHAR.h>
#include<process.h>

#define MAX_COUNT 20
int g_iCount = 1;

DWORD WINAPI ThreadFunc1(PVOID pvParam)
{
	while (g_iCount < MAX_COUNT)
	{
		Sleep(100);
		printf("\n Thread1(Id %d): %d", GetCurrentThreadId(), g_iCount);
		g_iCount += 1;
	}

	return 0;
}

DWORD WINAPI ThreadFunc2(PVOID pvParam)
{
	while (g_iCount < MAX_COUNT)
	{
		Sleep(100);
		printf("\n Thread2 (Id %d): %d", GetCurrentThreadId(), g_iCount);
		g_iCount += 1;
	}

	return 0;
}

int main()
{
	DWORD dwThread1ID;
	DWORD dwThread2ID;
	HANDLE arrhHandles[2]{};

	arrhHandles[0] = CreateThread(NULL, 0, ThreadFunc1, NULL, 0, &dwThread1ID);

	Sleep(100);

	arrhHandles[1] = CreateThread(NULL, 0, ThreadFunc2, NULL, 0, &dwThread2ID);

	WaitForMultipleObjects(2, arrhHandles, TRUE, INFINITE);

	CloseHandle(arrhHandles[0]);
	CloseHandle(arrhHandles[1]);

	return 0;
}





/*

// Using _beginthreadex()
// _beginthreadex() is C runtime library call that call CreateThread() behind the scenes. 
// Once CreateThread() has returned, _beginthreadex() takes care of additional bookkeeping 
// to make the C runtime library usable & consistent in the new thread. 
// (the CRT state will be properly initialized and destroyed).

#include <windows.h>
#include <stdio.h>
#include<process.h>

#define MAX_COUNT 20
int g_iCount = 1;

unsigned int WINAPI ThreadFunc1(PVOID pvParam)
{
	while (g_iCount < MAX_COUNT)
	{
		Sleep(100);
		printf("\n THREAD1(%d)  %d", GetCurrentThreadId(), g_iCount);
		g_iCount += 1;
	}

	return 0;
}

unsigned int WINAPI ThreadFunc2(PVOID pvParam)
{
	while (g_iCount < MAX_COUNT)
	{
		Sleep(100);
		printf("\n THREAD2(%d)  %d", GetCurrentThreadId(), g_iCount);
		g_iCount += 1;
	}

	return 0;
}


int main()
{
	unsigned int uiThread1ID;
	unsigned int uiThread2ID;
	HANDLE arrhHandles[2];

	//You need to typecast the return value of _beginthreadex() to HANDLE type, in order to be able to call WaitForMultipleObjects. 
	arrhHandles[0] = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc1, NULL, 0, &uiThread1ID);
	Sleep(100);
	arrhHandles[1] = (HANDLE)_beginthreadex(NULL, 0, &ThreadFunc2, NULL, 0, &uiThread2ID);

	WaitForMultipleObjects(2, arrhHandles, TRUE, INFINITE);

	CloseHandle(arrhHandles[0]);
	CloseHandle(arrhHandles[1]);

	return 0;
}
*/





