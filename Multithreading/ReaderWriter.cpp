// 	This module implements the multiple writers multiple readers problem.

#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <process.h>

HANDLE g_hWriteEvent;
HANDLE g_hReadSemaphore;

int g_iResourseCount;

//BOOLEAN bTerminate = FALSE;

unsigned int WINAPI
ReaderThread(
	PVOID pvParam
)
{
	DWORD dwRetVal;
	DWORD dwThreadID;

	dwRetVal = 0;

	//if (TRUE == bTerminate)
	//{
	//	return 1;
	//}

	if (0 < g_iResourseCount && WAIT_OBJECT_0 == WaitForSingleObject(g_hReadSemaphore, INFINITE))
	{
		dwThreadID = GetCurrentThreadId();
		_tprintf(_T("Reader Thread %d\t Reading, "), dwThreadID);
		printf("%d readers available\n", 4 - g_iResourseCount);
		ResetEvent(g_hWriteEvent);
		g_iResourseCount++;
		Sleep(1000);
		_tprintf(_T("Reader Thread %d\t Completed\n"), dwThreadID);
		g_iResourseCount--;

		if (0 == g_iResourseCount)
		{
			SetEvent(g_hWriteEvent);
		}
	}
	else if (WAIT_OBJECT_0 == WaitForSingleObject(g_hReadSemaphore, INFINITE))
	{
		dwRetVal = WaitForSingleObject(g_hWriteEvent, INFINITE);
		if (WAIT_FAILED == dwRetVal)
		{
			printf("\n Wait for g_hWriteEvent failed.");

			return 1;
		}
		dwThreadID = GetCurrentThreadId();
		_tprintf(_T("Reader Thread %d\t Reading, "), dwThreadID);
		printf("%d readers available\n", 4 - g_iResourseCount);
		ResetEvent(g_hWriteEvent);
		g_iResourseCount++;
		Sleep(1000);
		_tprintf(_T("Reader Thread %d\t Completed\n"), dwThreadID);
		g_iResourseCount--;

		if (0 == g_iResourseCount)
		{
			SetEvent(g_hWriteEvent);
		}
	}

	ReleaseSemaphore(g_hReadSemaphore, 5, NULL);

	return 0;
}


unsigned int WINAPI
WriterThread(
	PVOID pvParam
)
{
	DWORD dwRetVal;
	DWORD dwThreadID;

	dwRetVal = 0;

	//if (TRUE == bTerminate)
	//{
	//	return 1;
	//}

	//ResetEvent(g_hWriteEvent);
	dwRetVal = WaitForSingleObject(g_hWriteEvent, INFINITE);
	if (WAIT_FAILED == dwRetVal)
	{
		printf("\n Wait for g_hWriteEvent failed.");

		return 1;
	}

	ResetEvent(g_hWriteEvent);
	dwThreadID = GetCurrentThreadId();

	_tprintf(_T("Writer Thread %d\t Writing.\n"), dwThreadID);

	Sleep(500);
	_tprintf(_T("Writer Thread %d\t Completed.\n"), dwThreadID);
	SetEvent(g_hWriteEvent);

	return 0;
}


void
main(
)
{
	int iLoop;
	DWORD dwRetVal;
	HANDLE hReaderThread[5];
	HANDLE hWriterThread[5];

	dwRetVal = 0;

	g_hReadSemaphore = CreateSemaphore(NULL, 5, 5, NULL);
	if (g_hReadSemaphore == NULL)
	{
		printf("CreateSemaphore() failed: %d\n", GetLastError());
		return;
	}

	printf("%d readers available\n", 5 - g_iResourseCount);

	g_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (g_hWriteEvent == NULL)
	{
		printf("CreateEvent() failed: %d\n", GetLastError());
		CloseHandle(g_hReadSemaphore);

		return;
	}

	for (iLoop = 0; iLoop < 5; iLoop++)
	{
		hWriterThread[iLoop] = (HANDLE)_beginthreadex(
			NULL,
			0,
			WriterThread,
			NULL,
			0,
			NULL
		);
		if (NULL == hWriterThread[iLoop])
		{
			_tprintf(_T("Thread can't be created."));
			CloseHandle(g_hReadSemaphore);
			CloseHandle(g_hWriteEvent);

			return;
		}

		Sleep(200);

		hReaderThread[iLoop] = (HANDLE)_beginthreadex(
			NULL,
			0,
			ReaderThread,
			NULL,
			0,
			NULL
		);
		if (NULL == hReaderThread[iLoop])
		{
			_tprintf(_T("Thread can't be created."));
			CloseHandle(g_hReadSemaphore);
			CloseHandle(g_hWriteEvent);

			return;
		}

		Sleep(100);
	}

	dwRetVal = WaitForMultipleObjects(
		5,
		hReaderThread,
		TRUE,
		INFINITE
	);
	if (WAIT_FAILED == dwRetVal)
	{
		_tprintf(_T("\nWait Failed. \n Terminating abnormaly.\n"));

		CloseHandle(g_hReadSemaphore);
		CloseHandle(g_hWriteEvent);
		return;
	}

	dwRetVal = WaitForMultipleObjects(
		5,
		hWriterThread,
		TRUE,
		INFINITE
	);
	if (WAIT_FAILED == dwRetVal)
	{
		_tprintf(_T("\nWait Failed. \n Terminating abnormaly.\n"));

		CloseHandle(g_hReadSemaphore);
		CloseHandle(g_hWriteEvent);
		return;
	}

	for (iLoop = 0; iLoop < 5; iLoop++)
	{
		CloseHandle(hReaderThread[iLoop]);
		CloseHandle(hWriterThread[iLoop]);
	}

	CloseHandle(g_hReadSemaphore);
	CloseHandle(g_hWriteEvent);

	getchar();
}