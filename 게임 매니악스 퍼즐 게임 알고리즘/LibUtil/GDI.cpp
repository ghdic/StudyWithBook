#include "LibUtil.h"


//==============================================================
// 아이콘
CIcon::CIcon()
:	HIcon(NULL)
{}

CIcon::~CIcon() {
	if (HIcon) DeleteObject(HIcon);
}

void CIcon::LoadFromFile(wstring file) {
	if (HIcon) DeleteObject(HIcon);
	LoadImage(GetModuleHandle(NULL), file.c_str(), 
		IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}


// 윈도우의 위치를 얻는다
void GetWindowXY(HWND hwnd, int* x, int* y) {
	RECT r;
	GetWindowRect(hwnd, &r);
	*x=r.left;
	*y=r.top;
}

// 윈도우의 위치 설정한다
void SetWindowXY(HWND hwnd, int x, int y) {
	int w, h;
	GetWindowWH(hwnd, &w, &h);
	MoveWindow(hwnd, x, y, w, h, TRUE);
}

// 윈도우의 폭과 높이를 얻는다
void GetWindowWH(HWND hwnd, int* w, int* h) {
	RECT r;
	GetWindowRect(hwnd, &r);
	*w=r.right-r.left;
	*h=r.bottom-r.top;
}

// 윈도우의 폭과 높이를 설정한다
void SetWindowWH(HWND hwnd, int w, int h) {
	int x, y;
	GetWindowXY(hwnd, &x, &y);	
	MoveWindow(hwnd, x, y, w, h, TRUE);
}

// 클라이언트 영역의 폭과 높이를 얻는다
void GetClientWH(HWND hwnd, int* w, int* h) {
	RECT r;
	GetClientRect(hwnd, &r);
	*w=r.right-r.left;
	*h=r.bottom-r.top;
}

// 클라이언트 영역의 폭과 높이를 설정한다
void SetClientWH(HWND hwnd, int w, int h) {
	RECT wr, cr;
	GetWindowRect(hwnd, &wr);
	GetClientRect(hwnd, &cr);
	MoveWindow(hwnd, wr.left, wr.top, 
		(wr.right-wr.left)+w-(cr.right-cr.left),
		(wr.bottom-wr.top)+h-(cr.bottom-cr.top),
		TRUE);
}

// Region을 복제한다
HRGN CloneRgn(HRGN rgn) {
	HRGN ret;
	ret=CreateRectRgn(0, 0, 0, 0);
	CombineRgn(ret, rgn, ret, RGN_COPY);
	return ret;
}

