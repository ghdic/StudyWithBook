#pragma warning(disable:4786)
#pragma warning(disable:4819)
#pragma warning(disable:4311)

#include "Game.h"
#include "..\LibUtil\LibUtil.h"
#include <map>
#include <sstream>
using namespace std;


// ���ϸ��� ��θ� ��� ��ȯ
static wstring ExtractFilePath(wstring s) {
	int pos=s.rfind(L"\\");
	if (pos==string::npos) return L"";
	return s.substr(0, pos+1);
}

// ���� ������ ��θ� ��ȯ
static wstring GetExePath() {
	wchar_t buf[MAX_PATH+1];
	GetModuleFileName(NULL, buf, MAX_PATH);
	return ExtractFilePath(buf);
}


//==============================================================
// HWND�� CGame�ν��Ͻ��� �����ϴ� ��
typedef map<HWND, CGame*, less<HWND> > THWndCGameMap;
static THWndCGameMap HWndCGameMap;


//==============================================================
// �� �޽��� �ڵ鷯
// �׷��� ���� �ʱ�ȭ ������ �̰��� �����Ѵ�.
static LRESULT WINAPI DummyWndProc(
	HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
) {
	return DefWindowProc( hwnd, msg, wparam, lparam );
}


//==============================================================
// �޽��� �ڵ鷯
static LRESULT WINAPI WndProc(
	HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
) {
	THWndCGameMap::iterator iter=HWndCGameMap.find(hwnd);
	if (iter==HWndCGameMap.end())
		return DefWindowProc( hwnd, msg, wparam, lparam );
	CGame* game=iter->second;
	return game->WndProc(msg, wparam, lparam);
}

LRESULT WINAPI CGame::WndProc(
	UINT msg, WPARAM wparam, LPARAM lparam
) {
	wostringstream oss;
	switch(msg) {

		// �׸���
		case WM_PAINT:
			DrawScene();
			break;

		// ���콺
		case WM_MOUSEMOVE:
			Mouse.X=LOWORD(lparam);
			Mouse.Y=HIWORD(lparam);
			break;
		case WM_LBUTTONDOWN: Mouse.LButton=true; break;
		case WM_LBUTTONUP: Mouse.LButton=false; break;
		case WM_MBUTTONDOWN: Mouse.MButton=true; break;
		case WM_MBUTTONUP: Mouse.MButton=false; break;
		case WM_RBUTTONDOWN: Mouse.RButton=true; break;
		case WM_RBUTTONUP: Mouse.RButton=false; break;

		// Ÿ��Ʋ�ٸ� ����Ŭ��
		case WM_NCLBUTTONDBLCLK:
			ToggleFullScreen();
			return 0;

		// ����
		case WM_CLOSE:
			if (!ConfirmExit ||
				MessageBox(HWnd, L"�����Ͻðڽ��ϱ�?", L"����Ȯ��", MB_OKCANCEL)==IDOK) {
				DestroyWindow(HWnd);
			}
			return 0;

		// ���� Ŀ�ǵ�
		case WM_COMMAND:
			switch (LOWORD(wparam)) {

				// ����
				case IDC_EXIT:
					SendMessage(HWnd, WM_CLOSE, 0, 0);
					return 0;

				// Alt+Return���� ȭ�� ��带 ��ȯ
				case IDC_TOGGLEFULLSCREEN:
					ToggleFullScreen();
					return 0;

				// ����
				case IDC_HELPURL:
					if (HelpURL!=L"") {
						ShellExecute(HWnd, NULL, HelpURL.c_str(),
    						NULL, NULL, SW_SHOWNORMAL);					
						return 0;
					}

				// ���� ǥ��
				case IDC_VERSION:
					oss<<L"["<<AppName<<L"]\n\n";
					oss<<L"��� ����Ʈ������ ���۱��� ������ ����ġ��/��ī�� ��Ű�� �����ϰ� �ֽ��ϴ�.\n";
					oss<<L"Copyright(C) 2004 Ken-ichiro Matsuura & Yuki Tsukasa (HigPen Works).\n";
					oss<<L"All Rights Reserved.\n\n";
					oss<<L"[OK]�� ������ �Ʒ��� ����Ʈ ������Ʈ�� �̵��մϴ�.\n";
					oss<<L"(http://cgi32.plala.or.jp/higpen/gate.shtml)";
					oss<<ends;
					if (MessageBox(HWnd, oss.str().c_str(), L"���� ����", MB_OKCANCEL)==IDOK) {
						ShellExecute(HWnd, NULL, 
							L"http://cgi32.plala.or.jp/higpen/gate.shtml", 
    						NULL, NULL, SW_SHOWNORMAL);
					}
					return 0;
			}
			break;

		// �ý��� Ŀ�ǵ�
		case WM_SYSCOMMAND:
			switch (wparam) {

				// Full screen ȭ�� ���� ����
				case SC_MAXIMIZE:
					ToggleFullScreen();
					return 0;					

				// ��ũ�� ���̹��� ����� ���� off�� ����
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;

				// �޴�
				case SC_KEYMENU:
					Graphics->PresentGDISurface();
					DrawMenuBar(HWnd);
					return DefWindowProc(HWnd, msg, wparam, lparam);
			}
			break;

		// ����
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}
	return DefWindowProc(HWnd, msg, wparam, lparam);
}


//==============================================================
// <CGame>������
CGame::CGame(const wchar_t* app_name, bool zbuffer, bool use_menu, bool fixed_size)
:	Pause(false), MenuPause(false), UseMenu(use_menu), ConfirmExit(true),
	FPS(60), DropFrames(true), PauseInTheBackground(true),
	WindowWidth(800), WindowHeight(600), 
	FullScreenWidth(800), FullScreenHeight(600), 
	FullScreen(false), RefreshRate(60)
{
	// ���ø����̼Ǹ�, ���������� ���, ������ URL
	AppName=app_name;
	ExePath=GetExePath();
	HelpURL=L"";

	// ������ �б�
	HINSTANCE hinst=GetModuleHandle(NULL);
	HICON icon;
	wstring icon_file=ExePath+L"game.ico";
	if (FileExists(icon_file)) {
		icon=(HICON)LoadImage(
			hinst, icon_file.c_str(), 
			IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	} else {
		icon=LoadIcon(hinst, MAKEINTRESOURCE(1));
	}

	// ������ Ŭ���� ���:
	// ���⿡���� ���� �޽��� �ڵ鷯�� ����Ѵ�
	WNDCLASSEX wc={
		sizeof(WNDCLASSEX), CS_CLASSDC, DummyWndProc, 0L, 0L,
		hinst, icon, LoadCursor(NULL, IDC_ARROW), NULL, NULL,
		app_name, NULL
	};
	RegisterClassEx(&wc);

	// �Ǽ��������� Ű�� ����
	// [ESC] : ����
	// [Alt]+[Enter] : ȭ���� ����
	// [F1] : ����
	// [Shift]+[F1] : ���� ǥ��
	ACCEL accel[]={
		{FVIRTKEY, VK_ESCAPE, IDC_EXIT},
		{FALT|FVIRTKEY, VK_RETURN, IDC_TOGGLEFULLSCREEN},
		{FVIRTKEY, VK_F1, IDC_HELPURL},
		{FSHIFT|FVIRTKEY, VK_F1, IDC_VERSION}
	};
	HAccel=CreateAcceleratorTable(
		accel, sizeof(accel)/sizeof(ACCEL));

	// Ŀ�ǵ���� �ɼ�
	for (int i=1; i<__argc; i++) {
		char* s=__argv[i];
		if (strcmp("-w", s)==0 && ++i<__argc) {
			WindowWidth=FullScreenWidth=atoi(__argv[i]);
		} else
		if (strcmp("-h", s)==0 && ++i<__argc) {
			WindowHeight=FullScreenHeight=atoi(__argv[i]);
		} else
		if (strcmp("-ww", s)==0 && ++i<__argc) {
			WindowWidth=atoi(__argv[i]);
		} else
		if (strcmp("-wh", s)==0 && ++i<__argc) {
			WindowHeight=atoi(__argv[i]);
		} else
		if (strcmp("-fw", s)==0 && ++i<__argc) {
			FullScreenWidth=atoi(__argv[i]);
		} else
		if (strcmp("-fh", s)==0 && ++i<__argc) {
			FullScreenHeight=atoi(__argv[i]);
		}
		if (strcmp("-r", s)==0 && ++i<__argc) {
			RefreshRate=atoi(__argv[i]);
		}
		if (strcmp("-f", s)==0) {
			FullScreen=true;
		}
		if (strcmp("-nce", s)==0) {
			ConfirmExit=false;
		}
	}

	// ������ �ۼ�
	long style=WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
	RECT r={0, 0, WindowWidth, WindowHeight};
	AdjustWindowRect(&r, style, false);
	HWnd=CreateWindow(
		app_name, app_name, style, 
		0, 0, r.right-r.left, r.bottom-r.top, 
		GetDesktopWindow(), NULL, wc.hInstance, NULL);
	HWndCGameMap.insert(
		THWndCGameMap::value_type(HWnd, this));

	// �׷��� �ۼ�
	Graphics=new CGraphics(HWnd);
	if (!Graphics) {
		wstring s=
			L"�� ���α׷��� DirectX 9.0�̻��� �ʿ��մϴ�.\n"
			L"DirectX�� �ֽ��� �Ǵ� �ֽ� ����̹���\n"
			L"�ν���Ǿ� �ִ��� Ȯ���Ͻʽÿ�.\n";
		if (!fixed_size) {
			s+=
				L"�׸���, ���� �� �ɼǿ��� �ػ󵵸� �����غ��ñ� �ٶ��ϴ�.\n\n"
				L"�����찡 800x600, Ǯ��ũ�� 1024x768�� ���\n"
				L"-w 800 -h 600 -fw 1024 -fh 768";
		}
		MessageBox(HWnd, s.c_str(), app_name, MB_OK);
		exit(1);
	}
	Graphics->Clear();
	Graphics->Present();
	if (FullScreen) ResetScreen();

	// �Է� �ʱ���
	Input=new CInput(HWnd);
	Mouse.X=Mouse.Y=0;
	Mouse.LButton=Mouse.MButton=Mouse.RButton=false;

	// �ð�
	LastPerfCounter.QuadPart=0;
	LastTickCount=0;
	Elapsed=0;

	// �޴�
	if (UseMenu) {
		MenuBar=CreateMenu();
		SetMenu(HWnd, MenuBar);
	}
}


//==============================================================
static const char* copyright=
	"Copyright(C) 2005 Higpen Works(������ ����ġ��/��ī�� ��Ű)";


//==============================================================
// �޴�
HMENU CGame::AddMenu(wstring text) {
	HMENU menu=CreatePopupMenu();
	AppendMenu(MenuBar, MF_ENABLED | MF_STRING | MF_POPUP, (UINT)menu, text.c_str());
	DrawMenuBar(HWnd);
	return menu;
}
HMENU CGame::AddSubMenu(HMENU menu, wstring text) {
	HMENU sub_menu=CreatePopupMenu();
	AppendMenu(menu, MF_ENABLED | MF_STRING | MF_POPUP, (UINT)sub_menu, text.c_str());
	return sub_menu;
}
void CGame::AddMenuItem(HMENU menu, wstring text, int id) {
	AppendMenu(menu, MF_ENABLED | MF_STRING, id, text.c_str());
}
void CGame::AddMenuSeparator(HMENU menu) {
	AppendMenu(menu, MF_ENABLED | MF_SEPARATOR, 0, NULL);
}
void CGame::CheckMenuItem(HMENU menu, int id, bool check) {
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_STATE;
	mii.fState=check?MFS_CHECKED:MFS_UNCHECKED;
	SetMenuItemInfo(menu, id, FALSE, &mii);
}


//==============================================================
// <CGame> �Ҹ���
CGame::~CGame() {
	delete Input;
	delete Graphics;
}


//==============================================================
// <CGame> �ð� ����
void CGame::ResetTime() {
	LARGE_INTEGER freq, count;
	if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&count)) {
		LastPerfCounter=count;
	} else {
		LastTickCount=GetTickCount();
	}
}

// ȭ�� ����
void CGame::ResetScreen() {
	if (FullScreen) {
		Graphics->SetWidth(FullScreenWidth);
		Graphics->SetHeight(FullScreenHeight);
		Graphics->SetRefreshRate(RefreshRate);
	} else {
		Graphics->SetWidth(WindowWidth);
		Graphics->SetHeight(WindowHeight);
		Graphics->SetRefreshRate(0);
	}
	Graphics->SetFullScreen(FullScreen);
	OnLostDevice();
	if (Graphics->ResetDevice()) OnResetDevice();
}

// ������ ���� Ǯ��ũ�� ����� ��ȯ
void CGame::ToggleFullScreen() {
	FullScreen=!Graphics->IsFullScreen();
	ResetScreen();
}

//==============================================================
// <CGame> ���� ����
void CGame::Run() {

	// ����̽� ���� ���� ó���� ȣ��
	OnResetDevice();

	// ���� �޽��� �ڵ鷯��
	// ���� �޽��� �ڵ鷯�� �ٲ۴�
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::WndProc);
	
	// �޽��� ����
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ResetTime();
	while (msg.message!=WM_QUIT) {

		// Background���� �޽��� ó��
		if (PauseInTheBackground && HWnd!=GetForegroundWindow()) {
			GetMessage(&msg, NULL, 0U, 0U);
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			ResetTime();
			continue;
		}
		
		// Foreground���� �޽��� ó��
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			if (!TranslateAccelerator(HWnd, HAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			continue;
		}

		// Ǯ��ũ�� ���� �޴� ǥ��
		if (UseMenu) {
			if (Graphics->IsFullScreen()) {
				POINT p;
				GetCursorPos(&p);
				bool prev=MenuPause;
				MenuPause=(p.y<GetSystemMetrics(SM_CYMENU));
				if (!prev && MenuPause) {
					Graphics->PresentGDISurface();
					DrawMenuBar(HWnd);
				}
			} else {
				MenuPause=false;
			}
		}

		// �Ͻ�����
		if (Pause || MenuPause) continue;

		// �ð�����
		LARGE_INTEGER freq, count;
		DWORD tick;
		if (QueryPerformanceFrequency(&freq) && 
			QueryPerformanceCounter(&count)) {
			Elapsed+=FPS*(count.QuadPart-LastPerfCounter.QuadPart)/freq.QuadPart;
			LastPerfCounter=count;
		} else {
			tick=GetTickCount();
			Elapsed+=FPS*(tick-LastTickCount)/1000;
			LastTickCount=tick;
		}

		// �̵�, �׸���
		if (Graphics->GetRefreshRate()==FPS || Elapsed>=1.0) {
			if (DropFrames) {
				for (; Elapsed>=1.0; Elapsed-=1.0) Move();
			} else {
				Elapsed=0;
				Move();
			}
			DrawScene();
		}
	}

	// �޽��� �ڵ鷯�� ���̷� �ǵ�����
	SetWindowLong(HWnd, GWL_WNDPROC, (long)::DummyWndProc);
	
	// ����ó��
	UnregisterClass(AppName.c_str(), GetModuleHandle(NULL));
}

// �׸���
void CGame::DrawScene() {

	switch (Graphics->GetDevice()->TestCooperativeLevel()) {
		case D3DERR_DEVICELOST:
			Sleep(50);
			return;
		case D3DERR_DEVICENOTRESET:
			OnLostDevice();
			if (Graphics->ResetDevice()) OnResetDevice();
			return;
	}

	// �׸��� ����
	LPDIRECT3DDEVICE9 device=Graphics->GetDevice();
	Graphics->BeginScene();

	Draw();

	// �׸��� ����
	Graphics->EndScene();
	Graphics->Present();
}

