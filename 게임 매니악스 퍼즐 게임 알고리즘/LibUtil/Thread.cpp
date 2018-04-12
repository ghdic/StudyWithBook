#include "Thread.h"


//==============================================================
// ������

// �������� �⵿ �Լ�
static DWORD WINAPI ThreadProc(LPVOID thread) {
	return ((CThread*)thread)->Run();
}

// ������
CThread::CThread(DWORD stackSize) {
	Thread=CreateThread(NULL, stackSize, ThreadProc, (LPVOID)this, CREATE_SUSPENDED, &ID);
}

// �Ҹ���
CThread::~CThread() {
	CloseHandle(Thread);
}

