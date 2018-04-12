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
// 그래픽을 그리는 기능을 관리하는 클래스
class CGraphics {
protected:

	// Direct3D 인터페이스, 디바이스
	LPDIRECT3D9 D3D;
	LPDIRECT3DDEVICE9 Device;

	// 윈도우 핸들러
	HWND HWnd;
	
	// 초기 윈도우 스타일
	DWORD WindowStyle;

	// 폭, 높이
	int Width, Height;

	// 풀스크린 모드이면 True
	bool FullScreen;

	// Refresh rate(Hz)
	int RefreshRate;

	// GDI에 따라 그릴 수 있는 surface인가?
	bool IsGDISurface;

	D3DFORMAT DepthStencilFormat;

public:

	// 생성자, 소멸자
	CGraphics(HWND hwnd);
	~CGraphics();

	// 디바이스 리셋
	void SetWidth(int w) { Width=w; }
	void SetHeight(int h) { Height=h; }
	void SetFullScreen(bool b) { FullScreen=b; }
	void SetRefreshRate(int r) { RefreshRate=r; }
	void SetDepthStencilFormat(D3DFORMAT format) { DepthStencilFormat=format; }
	bool ResetDevice();
	bool DeviceNotReset();

	// Scene 그리기 시작과 종료
	bool BeginScene() { return SUCCEEDED(Device->BeginScene()); }
	void EndScene() { Device->EndScene(); }

	// 화면 삭제, 회면 Flip
	void Clear(D3DCOLOR color=D3DCOLOR_ARGB(255, 0, 0, 0));
	bool Present();

	// 각종 정보 취득
	LPDIRECT3DDEVICE9 GetDevice() { return Device; }
	HWND GetHWnd() { return HWnd; }
	int GetWidth() { return Width; }
	int GetHeight() { return Height; }
	bool IsFullScreen() { return FullScreen; }
	int GetRefreshRate() { return RefreshRate; }

	// GDI에 따라 그릴 수 있는 surface로 설정
	void PresentGDISurface() { if (!IsGDISurface) Present(); }

	// 라이트 설정
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

	// Alpha 가산 모드 지정
	void SetAddAlpha(bool add_alpha);

	// Effect 로드
	ID3DXEffect* LoadEffect(wstring file);
	
	// Texture 로드
	LPDIRECT3DTEXTURE9 LoadTexture(wstring file);
};


#endif



