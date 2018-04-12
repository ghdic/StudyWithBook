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


// 게임 본체 클래스
class CGame {
protected:

	// 악셀러레이터 키
	HACCEL HAccel;

	// 윈도우 핸들러
	HWND HWnd;

	// 그래픽
	CGraphics* Graphics;

	// 입력
	CInput* Input;
	MOUSE_STATE Mouse;

	// 실행파일의 경로, 어플리케이션 이름, 도움말 URL
	wstring ExePath, AppName, HelpURL;

	// 속도 조정
	LARGE_INTEGER LastPerfCounter;
	DWORD LastTickCount;
	double Elapsed;
	void ResetTime();

	// 일시정지
	bool Pause, MenuPause;

	// 메뉴
	bool UseMenu;
	HMENU MenuBar;

	// 종료확인
	bool ConfirmExit;

	// 화면크기, Refresh rate
	int WindowWidth, WindowHeight, FullScreenWidth, FullScreenHeight,
		RefreshRate;
	bool FullScreen;
	
	void DrawScene();

public:

	// 생성자, 소멸자
	CGame(const wchar_t* app_name, bool zbuffer, bool use_menu, bool fixed_size);
	~CGame();

	// 메시지 핸들러
	virtual LRESULT WINAPI WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

	// 게임 실행
	void Run();

	// 각종 설정
	double FPS;
	bool DropFrames, PauseInTheBackground;

	// 이동, 그리기
	// 서브 클래스에서 오버라이드
	virtual void Move() {}
	virtual void Draw() {}
	
	// Direct3D 리소스 관리
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}

	// 메뉴
	HMENU GetMenuBar() { return MenuBar; }
	HMENU AddMenu(wstring text);
	HMENU AddSubMenu(HMENU menu, wstring text);
	void AddMenuItem(HMENU menu, wstring text, int id);
	void AddMenuSeparator(HMENU menu);
	void CheckMenuItem(HMENU menu, int id, bool check);

	// 각종 정보의 취득
	CGraphics* GetGraphics() { return Graphics; }
	CInput* GetInput() { return Input; }
	const MOUSE_STATE& GetMouseState() { return Mouse; }

	// 종료 확인
	void SetConfirmExit(bool ce) { ConfirmExit=ce; }

	// 화면 모드의 변경
	void ResetScreen();
	void ToggleFullScreen();
};


#endif
