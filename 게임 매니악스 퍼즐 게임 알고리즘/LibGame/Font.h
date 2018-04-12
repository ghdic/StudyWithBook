#ifndef FONTH
#define FONTH

#include "..\LibUtil\Archive.h"
#include "Texture.h"
#include <string>
using namespace std;


//==============================================================
// 폰트
class CFont {
protected:

	// Texture
	CTexture* Texture;

	// 각 문자의 좌표
	float X[256], Y[256];

	// Texture상의 크기, 표시 크기, 그림자의 위치
	float SW, SH, DW, DH, ShadeX, ShadeY;

public:

	// 생성자
	CFont(LPDIRECT3DDEVICE9 device, 
		wstring file, CArchiveLoader* archive, 
		wstring chars, float sw, float sh, float dw, float dh, 
		float shadex, float shadey);
	
	// 그리기
	void DrawChar(unsigned char c, float x, float y, D3DCOLOR color);
	void DrawChar(unsigned char c, float x, float y, D3DCOLOR color, D3DCOLOR shade);
	void DrawText(wstring s, float x, float y, D3DCOLOR color);
	void DrawText(wstring s, float x, float y, D3DCOLOR color, D3DCOLOR shade);
	void DrawText(wstring s, float x, float y, float sx, float sy, D3DCOLOR color);

	// 크기
	float GetTextW(wstring s) { return s.length()*DW; }
	float GetTextH() { return DH; }
};


#endif