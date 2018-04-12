#ifndef GRAPHICSH
#define GRAPHICSH
#include <d3dx9.h>
#include <string>
using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif


//==============================================================
// �׷����� �׸��� ����� �����ϴ� Ŭ����
class CGraphics {
protected:

	// Direct3D �������̽�, ����̽�
	LPDIRECT3D9 D3D;
	LPDIRECT3DDEVICE9 Device;

	// ������ �ڵ鷯
	HWND HWnd;
	
	// �ʱ� ������ ��Ÿ��
	DWORD WindowStyle;

	// ��, ����
	int Width, Height;

	// Ǯ��ũ�� ����̸� True
	bool FullScreen;

	// Refresh rate(Hz)
	int RefreshRate;

	// GDI�� ���� �׸� �� �ִ� surface�ΰ�?
	bool IsGDISurface;

	D3DFORMAT DepthStencilFormat;

public:

	// ������, �Ҹ���
	CGraphics(HWND hwnd);
	~CGraphics();

	// ����̽� ����
	void SetWidth(int w) { Width=w; }
	void SetHeight(int h) { Height=h; }
	void SetFullScreen(bool b) { FullScreen=b; }
	void SetRefreshRate(int r) { RefreshRate=r; }
	void SetDepthStencilFormat(D3DFORMAT format) { DepthStencilFormat=format; }
	bool ResetDevice();
	bool DeviceNotReset();

	// Scene �׸��� ���۰� ����
	bool BeginScene() { return SUCCEEDED(Device->BeginScene()); }
	void EndScene() { Device->EndScene(); }

	// ȭ�� ����, ȸ�� Flip
	void Clear(D3DCOLOR color=D3DCOLOR_ARGB(255, 0, 0, 0));
	bool Present();

	// ���� ���� ���
	LPDIRECT3DDEVICE9 GetDevice() { return Device; }
	HWND GetHWnd() { return HWnd; }
	int GetWidth() { return Width; }
	int GetHeight() { return Height; }
	bool IsFullScreen() { return FullScreen; }
	int GetRefreshRate() { return RefreshRate; }

	// GDI�� ���� �׸� �� �ִ� surface�� ����
	void PresentGDISurface() { if (!IsGDISurface) Present(); }

	// ����Ʈ ����
	void SetLight(
		int id, D3DLIGHTTYPE type, 
		D3DXVECTOR3 pos, D3DXVECTOR3 dir, D3DXCOLOR color, 
		float range
	);
	void SetLight(
		int id, D3DLIGHTTYPE type, 
		float x, float y, float z,
		float range=1000
	);

	// Alpha ���� ��� ����
	void SetAddAlpha(bool add_alpha);

	// Effect �ε�
	ID3DXEffect* LoadEffect(wstring file);
	
	// Texture �ε�
	LPDIRECT3DTEXTURE9 LoadTexture(wstring file);
};


#endif



