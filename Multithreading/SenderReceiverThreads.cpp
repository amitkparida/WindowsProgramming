#include <iostream>
#include <windows.h>
using namespace std;

/* 
Use the event kernel object and critical section to enable the communication among two threads.

Sender should send the messages to receiver only when the receiver is ready to receive.
*/

#define MSGEXIT 4


HANDLE hSender = NULL;
HANDLE hReceiver = NULL;
HANDLE hEvent = NULL;
CRITICAL_SECTION cs;
DWORD thIDSender;
DWORD thIDReceiver;

// It should not be able to send any message until the Reciever says that I'm OK.
void Sender() {
	DWORD dw = WaitForSingleObject(hEvent, INFINITE);
	if (dw != WAIT_FAILED && dw != WAIT_ABANDONED && dw != WAIT_TIMEOUT) {
		for (int i = 0; i < 5; i++) {
			EnterCriticalSection(&cs);
			cout << endl << "Sending the message...";
			Sleep(500);
			PostThreadMessage(thIDReceiver, i, 0, 0);
			LeaveCriticalSection(&cs);
		}
	}
}

void Receiver() {
	MSG m;
	SetEvent(hEvent);
	while (GetMessage(&m, NULL, 0, 0)) {
		EnterCriticalSection(&cs);
		if (m.message == MSGEXIT) {
			cout << endl<<"The message to exit has been received..";
			LeaveCriticalSection(&cs);
			break;
		}
		cout << endl << "The message received! " << m.message;
		Sleep(500);
		LeaveCriticalSection(&cs);
	}
}

int main() {
	hEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("Evt1"));
	InitializeCriticalSection(&cs);
	hSender = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Sender, NULL, 0, &thIDSender);
	hReceiver = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Receiver, NULL, 0, &thIDReceiver);

	HANDLE hArr[2] = {hSender, hReceiver};

	WaitForMultipleObjects(2, hArr, TRUE, INFINITE);
	DeleteCriticalSection(&cs);
	return 0;
}