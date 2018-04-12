#pragma once
#ifndef THREADH
#define THREADH

#include <windows.h>


//==============================================================
// ������
class CThread {
protected:
	HANDLE Thread;
	DWORD ID;
public:

	// ������
	CThread(DWORD stackSize=0);
	
	// �Ҹ���
	virtual ~CThread();
	
	// ������ ����
	inline void Start() { ResumeThread(Thread); }

	// ID ���
	inline DWORD GetID() { return ID; }
	
	// �켱���� ���
	inline int GetPriority() { GetThreadPriority(Thread); }

	// �켱���� ����
	inline void SetPriority(int priority) { SetThreadPriority(Thread, priority); }

	// ���� ���
	inline bool Join(DWORD millisecond=INFINITE) {
		return WaitForSingleObject(Thread, millisecond)==WAIT_OBJECT_0;
	}

	// ���� �ڵ� ���
	inline DWORD GetExitCode() { DWORD i; GetExitCodeThread(Thread, &i); return i; }

	// �������� ó���� �����ϴ� �Լ�
	virtual DWORD Run() { return 0; }
};


//==============================================================
// ũ��Ƽ�� ����
class CCriticalSection {
protected:
	CRITICAL_SECTION CriticalSection;
public:

	// ������
	CCriticalSection() { InitializeCriticalSection(&CriticalSection); }

	// �Ҹ���
	~CCriticalSection() { DeleteCriticalSection(&CriticalSection); }

	// ũ��Ƽ�� ���� ����
	inline void Enter() { EnterCriticalSection(&CriticalSection); }

	// ũ��Ƽ�� ������ ���ø� ����
	// inline BOOL TryEnter() { return TryEnterCriticalSection(&CriticalSection); }

	// ũ��Ƽ�� ���� ����
	inline void Leave() { LeaveCriticalSection(&CriticalSection); }
};


#endif
