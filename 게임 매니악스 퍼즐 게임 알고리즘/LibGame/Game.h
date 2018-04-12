#ifndef GAMEH
#define GAMEH

#include "Graphics.h"
#include "Input.h"
#include <vector>
#include <string>
using namespace std;


#define IDC_EXIT 0
#define IDC_TOGGLEFULLSCREEN 1
#define IDC_HELPURL 2
#define IDC_VERSION 3


// ���� ��ü Ŭ����
class CGame {
protected:

	// �Ǽ��������� Ű
	HACCEL HAccel;

	// ������ �ڵ鷯
	HWND HWnd;

	// �׷���
	CGraphics* Graphics;

	// �Է�
	CInput* Input;
	MOUSE_STATE Mouse;

	// ���������� ���, ���ø����̼� �̸�, ���� URL
	wstring ExePath, AppName, HelpURL;

	// �ӵ� ����
	LARGE_INTEGER LastPerfCounter;
	DWORD LastTickCount;
	double Elapsed;
	void ResetTime();

	// �Ͻ�����
	bool Pause, MenuPause;

	// �޴�
	bool UseMenu;
	HMENU MenuBar;

	// ����Ȯ��
	bool ConfirmExit;

	// ȭ��ũ��, Refresh rate
	int WindowWidth, WindowHeight, FullScreenWidth, FullScreenHeight,
		RefreshRate;
	bool FullScreen;
	
	void DrawScene();

public:

	// ������, �Ҹ���
	CGame(const wchar_t* app_name, bool zbuffer, bool use_menu, bool fixed_size);
	~CGame();

	// �޽��� �ڵ鷯
	virtual LRESULT WINAPI WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

	// ���� ����
	void Run();

	// ���� ����
	double FPS;
	bool DropFrames, PauseInTheBackground;

	// �̵�, �׸���
	// ���� Ŭ�������� �������̵�
	virtual void Move() {}
	virtual void Draw() {}
	
	// Direct3D ���ҽ� ����
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}

	// �޴�
	HMENU GetMenuBar() { return MenuBar; }
	HMENU AddMenu(wstring text);
	HMENU AddSubMenu(HMENU menu, wstring text);
	void AddMenuItem(HMENU menu, wstring text, int id);
	void AddMenuSeparator(HMENU menu);
	void CheckMenuItem(HMENU menu, int id, bool check);

	// ���� ������ ���
	CGraphics* GetGraphics() { return Graphics; }
	CInput* GetInput() { return Input; }
	const MOUSE_STATE& GetMouseState() { return Mouse; }

	// ���� Ȯ��
	void SetConfirmExit(bool ce) { ConfirmExit=ce; }

	// ȭ�� ����� ����
	void ResetScreen();
	void ToggleFullScreen();
};


#endif
