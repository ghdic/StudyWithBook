#ifndef MESHH
#define MESHH

#include "..\LibUtil\Archive.h"
#include <d3dx9.h>
#include <string>
using namespace std;


//==============================================================
// Mesh
typedef enum {
	TO_NONE, 
	TO_X, TO_Y, TO_Z,
	TO_XY, TO_XZ, TO_YX, TO_YZ, TO_ZX, TO_ZY,
	TO_XYZ, TO_XZY, TO_YXZ, TO_YZX, TO_ZXY, TO_ZYX,
} TURN_ORDER;

class CMesh {
private:

	// 디바이스
	LPDIRECT3DDEVICE9 Device;

	// Mesh
	ID3DXMesh* Mesh;

	// Material의 수, Material
	DWORD NumMaterials;
	D3DMATERIAL9* Materials;

	// Texture
	LPDIRECT3DTEXTURE9* Textures;

	// Color
	D3DXCOLOR ColorMultiplier, ColorAddition;

public:

	// 생성자, 소멸자
	CMesh(LPDIRECT3DDEVICE9 device, wstring file, CArchiveLoader* archive);
	~CMesh();

	// 그리기
	void SetColor(const D3DXCOLOR& multiplier, const D3DXCOLOR& addition) {
		ColorMultiplier=multiplier;
		ColorAddition=addition;
	}
	void Draw();
	void Draw(const D3DXMATRIX& mat);
	void Draw(
		const D3DXVECTOR3& pos, 
		const D3DXVECTOR3& scale, 
		const D3DXVECTOR3& turn, TURN_ORDER order);
	
	// 그리기 (오래된 함수)
	void Draw(float alpha);
	void Draw(LPD3DXMATRIX mat, float alpha);
	void Draw(
		float x, float y, float z, 
		float sx, float sy, float sz, 
		float tx, float ty, float tz, TURN_ORDER order,
		float alpha, bool alpha_add);
	void SetAddAlpha(bool add_alpha);

	// 클래스 외부에서 그리기 위한 각종 함수
	DWORD GetNumMaterials() { return NumMaterials; }
	D3DMATERIAL9* GetMaterial(DWORD i) { return &Materials[i]; }
	LPDIRECT3DTEXTURE9 GetTexture(DWORD i) { return Textures[i]; }
	void DrawSubset(DWORD i) { Mesh->DrawSubset(i); }
	D3DXVECTOR4* GetDiffuse(DWORD i) { return (D3DXVECTOR4*)&Materials[i].Diffuse; }
	D3DXVECTOR4* GetEmissive(DWORD i) { return (D3DXVECTOR4*)&Materials[i].Emissive; }

	ID3DXMesh* GetMesh() { return Mesh; }
};


#endif
