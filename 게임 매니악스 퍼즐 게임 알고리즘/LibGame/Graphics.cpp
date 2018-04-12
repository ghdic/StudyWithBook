#pragma warning(disable:4819)

#include "Graphics.h"
#include <d3dx9.h>


//==============================================================
// 생성자
CGraphics::CGraphics(HWND hwnd)
:	HWnd(hwnd), FullScreen(false), RefreshRate(60),
	Device(NULL), IsGDISurface(true),
	DepthStencilFormat(D3DFMT_D16)
{
	// 윈도우 스타일을 보존
	WindowStyle=GetWindowLong(hwnd, GWL_STYLE);

	// 클라이언트 영역의 크기를 취득
	RECT r;
	GetClientRect(hwnd, &r);
	Width=r.right-r.left;
	Height=r.bottom-r.top;

	// Direct3D 인터페이스를 작성
	D3D=Direct3DCreate9(D3D_SDK_VERSION);

	// Direct3D 디바이스를 작성
	ResetDevice();
}


//==============================================================
// 소멸자
CGraphics::~CGraphics() {
	if (Device) Device->Release();
	if (D3D) D3D->Release();
}


//==============================================================
// 디바이스

// 디바이스 리셋
bool CGraphics::ResetDevice() {

	// 윈도우 스타일과 크기 설정
	// 디바이스의 리셋보다 먼저 실행하지 않으면 윈도우 크기가 바뀐다.
	if (FullScreen) {
		SetWindowLong(HWnd, GWL_STYLE, WS_VISIBLE);
	} else {
		SetWindowLong(HWnd, GWL_STYLE, WindowStyle);
		RECT r={0, 0, Width, Height};
		AdjustWindowRect(&r, WindowStyle, GetMenu(HWnd)!=NULL);
		SetWindowPos(HWnd, HWND_NOTOPMOST, 
			0, 0, r.right-r.left, r.bottom-r.top, SWP_SHOWWINDOW);
	}

	// 디바이스의 파라미터 설정
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow=HWnd;
	d3dpp.BackBufferWidth=Width;
	d3dpp.BackBufferHeight=Height;
	d3dpp.Windowed=!FullScreen;
	d3dpp.FullScreen_RefreshRateInHz=FullScreen?RefreshRate:0;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_ONE;
	d3dpp.EnableAutoDepthStencil=TRUE;
	d3dpp.AutoDepthStencilFormat=DepthStencilFormat;

	// 디바이스 작성 또는 리셋
	// GeForce series에서 스키닝을 정상적으로 동작시키기 위해서는
	// D3DCREATE_MIXED_VERTEXPROCESSING 으로 할 필요가 있다.
	if (!Device) {
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&d3dpp, &Device))
		) 
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &Device))
		)
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &Device))
		)
		if (FAILED(D3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, HWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &Device))
		) return false;
	} else {
		if (FAILED(Device->Reset(&d3dpp))) return false;
	}

	// 폭과 높이 취득
	LPDIRECT3DSURFACE9 back_buffer;
	if (SUCCEEDED(Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer))) {
		D3DSURFACE_DESC desc;
		back_buffer->GetDesc(&desc);
		Width=desc.Width;
		Height=desc.Height;
		back_buffer->Release();
	} else {
		Width=Height=0;
	}

	// Refresh rate을 취득
	D3DDISPLAYMODE mode;
	if (SUCCEEDED(Device->GetDisplayMode(0, &mode))) {
		RefreshRate=mode.RefreshRate;
	} else {
		RefreshRate=0;
	}

	IsGDISurface=true;
	return true;
}


//==============================================================
// 화면(Back buffer) 삭제
void CGraphics::Clear(D3DCOLOR color) {
	Device->Clear(0, NULL, 
		D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, color, 1, 0 );
}


//==============================================================
// 화면 갱신
bool CGraphics::Present() {
	if (FAILED(Device->Present(0, 0, 0, 0))) return false;
	IsGDISurface=!IsGDISurface;
	return true;
}


//==============================================================
// 라이트 초기화
void CGraphics::SetLight(
	int id, D3DLIGHTTYPE type, 
	D3DXVECTOR3 pos, D3DXVECTOR3 dir, D3DXCOLOR color, 
	float range
) {
	// 라이트 작성
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type=type;
	light.Position=pos;
	D3DXVec3Normalize(&dir, &dir);
	light.Direction=dir;
	light.Diffuse=color, 
	light.Range=range;

	// 라이트 설정
	Device->SetLight(id, &light);
	Device->LightEnable(id, TRUE);
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CGraphics::SetLight(
	int id, D3DLIGHTTYPE type, 
	float x, float y, float z,
	float range
) {
	SetLight(
		id, type, 
		D3DXVECTOR3(x, y, z), D3DXVECTOR3(x, y, z), 
		D3DXCOLOR(1, 1, 1, 1), range
	);
}

//==============================================================
// Alpha 가산 모드의 설정
void CGraphics::SetAddAlpha(bool add_alpha) {
	if (add_alpha) {
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	} else {
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

//==============================================================
// Effect 로드
ID3DXEffect* CGraphics::LoadEffect(wstring file) {
	ID3DXEffect* effect=NULL;
	LPD3DXBUFFER error;

	// Effect 파일의 유무 확인
	WIN32_FIND_DATA fd;
	if (FindFirstFile(file.c_str(), &fd)==INVALID_HANDLE_VALUE) {
		MessageBox(NULL, file.c_str(), L"Shader File Not Found", MB_OK);
		PostQuitMessage(1);
		return NULL;
	}

	// Effect 파일을 로드
	D3DXCreateEffectFromFile(
		Device, file.c_str(), 
		NULL, NULL, 0, NULL, &effect, &error);

	// 컴파일 에러의 경우에는 에러를 표시하고 종료
	if (!effect) {
		MessageBox(
			NULL, 
			error?(LPCTSTR)error->GetBufferPointer():NULL, 
			L"Shader Compile Error", MB_OK);
		PostQuitMessage(1);
		return NULL;
	}
	return effect;
}

//==============================================================
// Texture 로드
LPDIRECT3DTEXTURE9 CGraphics::LoadTexture(wstring file) {
	LPDIRECT3DTEXTURE9 texture=NULL;
	D3DXCreateTextureFromFile(Device, file.c_str(), &texture);
	return texture;
}

