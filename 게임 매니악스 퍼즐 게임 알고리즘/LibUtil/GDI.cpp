#include "LibUtil.h"


//==============================================================
// ������
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


// �������� ��ġ�� ��´�
void GetWindowXY(HWND hwnd, int* x, int* y) {
	RECT r;
	GetWindowRect(hwnd, &r);
	*x=r.left;
	*y=r.top;
}

// �������� ��ġ �����Ѵ�
void SetWindowXY(HWND hwnd, int x, int y) {
	int w, h;
	GetWindowWH(hwnd, &w, &h);
	MoveWindow(hwnd, x, y, w, h, TRUE);
}

// �������� ���� ���̸� ��´�
void GetWindowWH(HWND hwnd, int* w, int* h) {
	RECT r;
	GetWindowRect(hwnd, &r);
	*w=r.right-r.left;
	*h=r.bottom-r.top;
}

// �������� ���� ���̸� �����Ѵ�
void SetWindowWH(HWND hwnd, int w, int h) {
	int x, y;
	GetWindowXY(hwnd, &x, &y);	
	MoveWindow(hwnd, x, y, w, h, TRUE);
}

// Ŭ���̾�Ʈ ������ ���� ���̸� ��´�
void GetClientWH(HWND hwnd, int* w, int* h) {
	RECT r;
	GetClientRect(hwnd, &r);
	*w=r.right-r.left;
	*h=r.bottom-r.top;
}

// Ŭ���̾�Ʈ ������ ���� ���̸� �����Ѵ�
void SetClientWH(HWND hwnd, int w, int h) {
	RECT wr, cr;
	GetWindowRect(hwnd, &wr);
	GetClientRect(hwnd, &cr);
	MoveWindow(hwnd, wr.left, wr.top, 
		(wr.right-wr.left)+w-(cr.right-cr.left),
		(wr.bottom-wr.top)+h-(cr.bottom-cr.top),
		TRUE);
}

// Region�� �����Ѵ�
HRGN CloneRgn(HRGN rgn) {
	HRGN ret;
	ret=CreateRectRgn(0, 0, 0, 0);
	CombineRgn(ret, rgn, ret, RGN_COPY);
	return ret;
}

