// This module implements the multiple writers multiple, readers problem using multiple instance of the same process.

#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <process.h>


HANDLE g_hReaderEvent;
HANDLE g_hWriterMutex;

#pragma data_seg("Shared")
volatile LONG g_lReaderCount = 0;
volatile LONG g_lWriterCount = 0;
volatile LONG g_iProcessCount = 0;
//BOOLEAN bIsWriting = FALSE;
//BOOLEAN bIsReading = FALSE;
#pragma data_seg()
#pragma comment(linker, "/Section:Shared,RWS")


void ReaderFunction()
{
	DWORD dwRetVal;

	dwRetVal = 0;

	//if (TRUE == bIsWriting)
	if (g_lWriterCount > 0)
	{
		_tprintf(_T("\n  Hey Reader! Please wait. "));
		printf("Someone is writing.\n");
	}

	dwRetVal = WaitForSingleObject(g_hWriterMutex, INFINITE);

	ReleaseMutex(g_hWriterMutex);

	if (WAIT_OBJECT_0 == dwRetVal)
	{
		ResetEvent(g_hReaderEvent);	//	It prevents writing while reading.
		InterlockedExchangeAdd(&g_lReaderCount, 1);
		_tprintf(_T("  Reader is reading... "));
		_tprintf(_T("(%d Readers and %d Writers available.)\n"), g_iProcessCount - g_lReaderCount, g_iProcessCount - g_lWriterCount);
		//bIsReading = TRUE;
		Sleep(20000);
		_tprintf(_T("  Reading completed.\n"));
		//bIsReading = FALSE;
		InterlockedExchangeAdd(&g_lReaderCount, -1);
		SetEvent(g_hReaderEvent);	//	It allows writing after reading.
	}
}


void WriterFunction()
{
	BOOLEAN bFlag;
	DWORD dwRetVal;
	DWORD dwRetVal1;

	bFlag = TRUE;
	dwRetVal = 0;
	dwRetVal1 = 0;

	//if (TRUE == bIsReading)
	if (g_lReaderCount > 0)
	{
		_tprintf(_T("\n  Hey Writer! Please wait. "));
		printf("Someone is reading.\n");
	}
	//if (TRUE == bIsWriting)
	if (g_lWriterCount > 0)
	{
		_tprintf(_T("\n  Hey Writer! Please wait. "));
		printf("Someone is writing.\n");
	}

	while (bFlag)
	{
		//dwRetVal = WaitForSingleObject(g_hReaderEvent,INFINITE);
		//if (WAIT_FAILED == dwRetVal)
		//{
		//	_tprintf(_T("\n Wait for g_hReaderEvent failed."));
		//}

		if (0 == g_lReaderCount)
		{
			break;
		}
	}

	//if (WAIT_OBJECT_0 == dwRetVal)
	//{
	dwRetVal = WaitForSingleObject(g_hWriterMutex, INFINITE);
	dwRetVal1 = WaitForSingleObject(g_hReaderEvent, INFINITE);

	if ((WAIT_OBJECT_0 == dwRetVal) && (WAIT_OBJECT_0 == dwRetVal1))
	{
		InterlockedExchangeAdd(&g_lWriterCount, 1);
		_tprintf(_T("  Writer is writing... "));
		_tprintf(_T("(%d Readers and %d Writers available.)\n"), g_iProcessCount - g_lReaderCount, g_iProcessCount - g_lWriterCount);
		//bIsWriting = TRUE;
		Sleep(20000);
		_tprintf(_T("  Writing completed.\n"));
		//bIsWriting = FALSE;
		InterlockedExchangeAdd(&g_lWriterCount, -1);
	}

	ReleaseMutex(g_hWriterMutex);
	SetEvent(g_hReaderEvent);
	//}
}


void main()
{
	int iChoice;;
	DWORD dwErrorCode;
	BOOLEAN bContinue;

	bContinue = TRUE;

	InterlockedExchangeAdd(&g_iProcessCount, 1);
	_tprintf(_T(" The open process ID is: %d\n"), GetCurrentProcessId());

	g_hReaderEvent = CreateEvent(NULL, TRUE, TRUE, _T("ReaderEvent"));
	dwErrorCode = GetLastError();
	if (NULL == g_hReaderEvent)
	{
		printf(" CreateEvent() failed\n");

		return;
	}

	if (ERROR_ALREADY_EXISTS == dwErrorCode)
	{
		_tprintf(_T(" g_hReaderEvent Already Exists\n"));
	}

	g_hWriterMutex = CreateMutex(NULL, FALSE, _T("WriterMutex"));
	dwErrorCode = GetLastError();
	if (NULL == g_hWriterMutex)
	{
		printf(" CreateMutex() failed.\n");
		CloseHandle(g_hReaderEvent);

		return;
	}

	if (ERROR_ALREADY_EXISTS == dwErrorCode)
	{
		_tprintf(_T(" g_hWriterMutex Already Exists\n"));
	}

	while (bContinue)
	{
		_tprintf(_T("\n 1.Read\n 2.Write\n 3.Exit\n\n"));
		_tprintf(_T(" Enter your choice: "));
		_tscanf_s(_T("%d"), &iChoice);

		switch (iChoice)
		{
		case 1:
			ReaderFunction();
			break;

		case 2:
			WriterFunction();
			break;

		case 3:
			return;
		}
	}

	getchar();

	CloseHandle(g_hReaderEvent);
	CloseHandle(g_hWriterMutex);
}



