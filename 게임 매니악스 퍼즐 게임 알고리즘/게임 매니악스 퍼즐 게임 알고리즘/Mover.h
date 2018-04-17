#ifndef MOVERH
#define MOVERH

#include "..\LibGame\Task.h"
#include "..\LibGame\Texture.h"

//==============================================================
// �̵� ��ü
class CMover : public CTask {
public:
	float X, Y, W, H, Angle, ShearX;
	int Type, DrawOrder;
	CTexture* Texture;
	D3DCOLOR Color;
	bool ReverseX;

	// ������
	CMover();
	void* operator new(size_t t);
	void operator delete(void* p);

	// �̵�, �׸���
	virtual bool Move(const CInputState* is) { return true; }
	virtual void Draw();
};

#endif

