#ifndef STAGEH
#define STAGEH

#include "..\LibGame\Task.h"
#include "Mover.h"

//�������� �⺻ Ŭ����
class CStage {
public:
	const wchar_t* Name;
	int Score, Time;
	CStage(const wchar_t* name) : Name(name) {}
	virtual void Init() {}
	virtual bool Move(const CInputState* is) { return true; }
	virtual void Draw() {}
};

//��� ���������� �ۼ�
extern void MakeStage();

#endif