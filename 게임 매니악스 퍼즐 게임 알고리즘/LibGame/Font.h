#ifndef FONTH
#define FONTH

#include "..\LibUtil\Archive.h"
#include "Texture.h"
#include <string>
using namespace std;


//==============================================================
// ��Ʈ
class CFont {
protected:

	// Texture
	CTexture* Texture;

	// �� ������ ��ǥ
	float X[256], Y[256];

	// Texture���� ũ��, ǥ�� ũ��, �׸����� ��ġ
	float SW, SH, DW, DH, ShadeX, ShadeY;

public:

	// ������
	CFont(LPDIRECT3DDEVICE9 device, 
		wstring file, CArchiveLoader* archive, 
		wstring chars, float sw, float sh, float dw, float dh, 
		float shadex, float shadey);
	
	// �׸���
	void DrawChar(unsigned char c, float x, float y, D3DCOLOR color);
	void DrawChar(unsigned char c, float x, float y, D3DCOLOR color, D3DCOLOR shade);
	void DrawText(wstring s, float x, float y, D3DCOLOR color);
	void DrawText(wstring s, float x, float y, D3DCOLOR color, D3DCOLOR shade);
	void DrawText(wstring s, float x, float y, float sx, float sy, D3DCOLOR color);

	// ũ��
	float GetTextW(wstring s) { return s.length()*DW; }
	float GetTextH() { return DH; }
};


#endif