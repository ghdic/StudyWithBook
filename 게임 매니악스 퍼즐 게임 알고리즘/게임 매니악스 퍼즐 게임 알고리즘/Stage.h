#ifndef STAGEH
#define STAGEH

#include "..\LibGame\Task.h"
#include "Mover.h"

//스테이지 기본 클래스
class CStage {
public:
	const wchar_t* Name;
	int Score, Time;
	CStage(const wchar_t* name) : Name(name) {}
	virtual void Init() {}
	virtual bool Move(const CInputState* is) { return true; }
	virtual void Draw() {}
};

//모든 스테이지의 작성
extern void MakeStage();

#endif