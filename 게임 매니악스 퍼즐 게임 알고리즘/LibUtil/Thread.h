#pragma once
#ifndef THREADH
#define THREADH

#include <windows.h>


//==============================================================
// 스레드
class CThread {
protected:
	HANDLE Thread;
	DWORD ID;
public:

	// 생성자
	CThread(DWORD stackSize=0);
	
	// 소멸자
	virtual ~CThread();
	
	// 스레드 시작
	inline void Start() { ResumeThread(Thread); }

	// ID 취득
	inline DWORD GetID() { return ID; }
	
	// 우선순위 취득
	inline int GetPriority() { GetThreadPriority(Thread); }

	// 우선순위 설정
	inline void SetPriority(int priority) { SetThreadPriority(Thread, priority); }

	// 종료 대기
	inline bool Join(DWORD millisecond=INFINITE) {
		return WaitForSingleObject(Thread, millisecond)==WAIT_OBJECT_0;
	}

	// 종료 코드 취득
	inline DWORD GetExitCode() { DWORD i; GetExitCodeThread(Thread, &i); return i; }

	// 스레드의 처리를 정의하는 함수
	virtual DWORD Run() { return 0; }
};


//==============================================================
// 크리티컬 섹션
class CCriticalSection {
protected:
	CRITICAL_SECTION CriticalSection;
public:

	// 생성자
	CCriticalSection() { InitializeCriticalSection(&CriticalSection); }

	// 소멸자
	~CCriticalSection() { DeleteCriticalSection(&CriticalSection); }

	// 크리티컬 섹션 개시
	inline void Enter() { EnterCriticalSection(&CriticalSection); }

	// 크리티컬 섹션의 개시를 시험
	// inline BOOL TryEnter() { return TryEnterCriticalSection(&CriticalSection); }

	// 크리티컬 섹션 종료
	inline void Leave() { LeaveCriticalSection(&CriticalSection); }
};


#endif
