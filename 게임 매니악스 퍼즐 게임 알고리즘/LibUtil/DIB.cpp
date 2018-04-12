#include "LibUtil.h"
#include "libpng\\png.h"
extern "C" {
	#include "libjpeg\\jpeglib.h"
}

//==============================================================
// �ʱ�ȭ

// �ʱ�ȭ(DIB ����)
void CDIB::InitDIB()
{
	if (DIB) DeleteObject(DIB);

	// DIB ������ ������ ���� BITMAPINFO ����ü�� �����
	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	BITMAPINFOHEADER* bih=&bi.bmiHeader;
	bih->biSize=sizeof(BITMAPINFOHEADER);
	bih->biWidth=W;
	bih->biHeight=-H;
	bih->biPlanes=1;
	bih->biBitCount=32;
	bih->biCompression=BI_RGB;

	// DIB ������ �ۼ�
	DIB=CreateDIBSection(
		DC, &bi, DIB_RGB_COLORS,
		(void**)&Pixel, NULL, 0);
	SelectObject(DC, DIB);

	// Alpha ������ �ۼ�
	if (Alpha) delete [] Alpha;
	Alpha=new BYTE[W*H];
}

// �ʱ�ȭ(Region)
// Alpha �� ����� ������ �ۼ��Ѵ�.
void CDIB::InitRgn() {
	if (Rgn) DeleteObject(Rgn);
	Rgn=CreateRectRgn(0, 0, 0, 0);
	HRGN rgn;
	BYTE* a=Alpha;
	int left;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; ) {
			for (; x<W && *a==0; x++, a++) ;
			left=x;
			for (; x<W && *a!=0; x++, a++) ;
			if (x-left>0) {
				rgn=CreateRectRgn(left, y, x, y+1);
				CombineRgn(Rgn, Rgn, rgn, RGN_OR);
				DeleteObject(rgn);
			}
		}
	}
}

// �ʱ�ȭ(Region) :
// ��ü ������ �ۼ��Ѵ�.
void CDIB::InitRgnSimple() {
	if (Rgn) DeleteObject(Rgn);
	Rgn=CreateRectRgn(0, 0, W, H);
}


//==============================================================
// ������, �Ҹ���

// �������� ����ó��
void CDIB::Construct() {
	HDC hdc=::GetDC(NULL);
	DC=CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
}

// ������(Size)
CDIB::CDIB(int w, int h)
:	DIB(NULL), Alpha(NULL), Rgn(NULL), W(w), H(h)
{
	Construct();
	InitDIB();
	InitRgnSimple();
}

// ������(Capture)
CDIB::CDIB(HWND hwnd)
:	DIB(NULL), Alpha(NULL), Rgn(NULL)
{
	Construct();
	Capture(hwnd);
}

// ������(File)
CDIB::CDIB(wstring file_name)
:	DIB(NULL), Alpha(NULL), Rgn(NULL)
{
	Construct();
	LoadFromFile(file_name);
}

// �Ҹ���
CDIB::~CDIB() {
	if (DC) DeleteDC(DC);
	if (DIB) DeleteObject(DIB);
	if (Alpha) delete[] Alpha;
	if (Rgn) DeleteObject(Rgn);
}


//==============================================================
// ����

// CDIB <- CDIB
void CDIB::Draw(
	int dx, int dy, int dw, int dh, 
	CDIB* dib, int sx, int sy, int sw, int sh
) {
	StretchBlt(DC, dx, dy, dw, dh, dib->DC, sx, sy, sw, sh, SRCCOPY);
}

// CDIB <- HBITMAP
void CDIB::Draw(
	int dx, int dy, int dw, int dh, 
	HBITMAP bmp, int sx, int sy, int sw, int sh
) {
	HDC null_dc=::GetDC(NULL);
	HDC bmp_dc=CreateCompatibleDC(null_dc);
	HGDIOBJ obj=SelectObject(bmp_dc, bmp);
	StretchBlt(DC, dx, dy, dw, dh, bmp_dc, sx, sy, sw, sh, SRCCOPY);
	SelectObject(bmp_dc, obj);
	DeleteDC(bmp_dc);
	ReleaseDC(NULL, null_dc);
}

// DC <- CDIB
void CDIB::Draw(
	HDC dc, int dx, int dy, int dw, int dh,
	int sx, int sy, int sw, int sh
) {
	StretchBlt(dc, dx, dy, dw, dh, DC, sx, sy, sw, sh, SRCCOPY);
}


//==============================================================
// Clip ����(Region�� ���)

// CDIB <- CDIB
void CDIB::ClipDraw(
	int dx, int dy, int dw, int dh, CDIB* dib, int sx, int sy
) {
	SelectClipRgn(DC, dib->Rgn);
	OffsetClipRgn(DC, dx-sx, dy-sy);
	StretchBlt(DC, dx, dy, dw, dh, dib->DC, sx, sy, dw, dh, SRCCOPY);
	SelectClipRgn(DC, NULL);
}

// DC <- CDIB
void CDIB::ClipDraw(
	HDC dc, int dx, int dy, int dw, int dh, int sx, int sy
) {
	POINT p;
	GetViewportOrgEx(dc, &p);
	SelectClipRgn(dc, Rgn);
	OffsetClipRgn(dc, p.x+dx-sx, p.y+dy-sy);
	StretchBlt(dc, dx, dy, dw, dh, DC, sx, sy, dw, dh, SRCCOPY);
	SelectClipRgn(dc, NULL);
}


//==============================================================
// ȭ�� ĸ��
void CDIB::Capture(HWND hwnd)
{
	// ȭ�� ũ�⸦ ������ ũ�⿡ �����
	if (!hwnd) hwnd=GetDesktopWindow();	
	GetWindowWH(hwnd, &W, &H);
	InitDIB();
	InitRgnSimple();
	
	// ȭ���� ĸ���Ѵ�
	HDC hdc_win=::GetDC(NULL);
	BitBlt(DC, 0, 0, W, H, hdc_win, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hdc_win);
}


//==============================================================
// ������� ���� :
// ������ ���� ��ġ�ϴ� �ȼ��� �������� �Ѵ�.
// �ٸ� ���� �ȼ��� �������ϰ� �ȴ�.
void CDIB::SetClipColor(DWORD xrgb) {
	xrgb&=0x00ffffff;
	DWORD* p=Pixel;
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, p++, a++) {
			*a=(*p==xrgb?0:255);
		}
	}
	InitRgn();
}


//==============================================================
// �ε�
void CDIB::LoadFromFile(wstring file_name) {
	if (!FileExists(file_name)) return;
	file_name=ToUpper(file_name);	
	if (EndsWith(file_name, L".BMP")) LoadBMPFile(file_name); else
	if (EndsWith(file_name, L".PNG")) LoadPNGFile(file_name);
	if (EndsWith(file_name, L".JPG") || EndsWith(file_name, L".JPEG")) LoadJPEGFile(file_name);
}

// BMP ������ �ε�
void CDIB::LoadBMPFile(wstring file_name)
{
	// BITMAP �ε�
	HBITMAP hbmp=(HBITMAP)LoadImage(
		NULL, file_name.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// DIB �ʱ�ȭ
	BITMAP bmp;
	GetObject(hbmp, sizeof(bmp), &bmp);
	W=bmp.bmWidth;
	H=bmp.bmHeight;
	InitDIB();
	FillMemory(Alpha, W*H, 255);
	InitRgnSimple();

	// BITMAP�� DBI�� ����
	Draw(0, 0, W, H, hbmp, 0, 0, W, H);
	DeleteObject(hbmp);
}

// PNG ������ �ε�
void CDIB::LoadPNGFile(wstring file_name)
{
	unsigned int sig_read=0;
	int bit_depth, color_type, interlace_type;
	
	// ���� ����
	FILE* fp=_wfopen(file_name.c_str(), L"rb");
	if (!fp) return;

	// png_struct �ʱ�ȭ
	png_structp png_ptr=png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		return;
	}
	png_init_io(png_ptr, fp);

	// png_info �ʱ�ȭ
	png_infop info_ptr=png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return;
	}

	// ȭ������ ���
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, 
		(unsigned long*)&W, (unsigned long*)&H, 
		&bit_depth, &color_type, &interlace_type, NULL, NULL);

	// DIB �ʱ�ȭ
	InitDIB();
	
	// ��ȯ��� ����
	// Palette�� RGB��
	if (color_type==PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// ������� Alpha��
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// Bit depth�� 8bit��
	if (bit_depth<8) png_set_packing(png_ptr); else
	if (bit_depth==16) png_set_strip_16(png_ptr);
	
	// 24bit(RGB)�� 32bit(RGB0)��
	if (color_type==PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);

	// RGB�� BGR��
	if (color_type==PNG_COLOR_TYPE_PALETTE ||
		color_type==PNG_COLOR_TYPE_RGB ||
		color_type==PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);

	// ȭ�� �б�
	png_bytep *row_pointers=new png_bytep[H];
	for (int row=0; row<H; row++) row_pointers[row]=(png_bytep)(Pixel+row*W);
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);
	delete [] row_pointers;

	// ��ó��
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	// Alpha ���� ��´�
	DWORD* p=Pixel;
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, p++, a++) {
			*a=(BYTE)(*p>>24);
			*p&=0x00ffffff;
		}
	}

	// Region �ʱ�ȭ
	InitRgn();
}

// JPEG ������ �ε�
static void JpegErrorHandler(j_common_ptr) {}

void CDIB::LoadJPEGFile(wstring file_name)
{
	// ���� �ڵ鷯 ����
	jpeg_decompress_struct jd;
	jpeg_error_mgr jem;
	jd.err=jpeg_std_error(&jem);
	jem.error_exit=JpegErrorHandler;

	// ���� ����
	FILE* fp=_wfopen(file_name.c_str(), L"rb");
	if (!fp) return;

	// ���ڵ� ��ó��
	jpeg_create_decompress(&jd);
	jpeg_stdio_src(&jd, fp);
	jpeg_read_header(&jd, TRUE);
	jpeg_start_decompress(&jd);

	// DIB �ʱ�ȭ
	W=jd.image_width;
	H=jd.image_height;
	InitDIB();
	
	// ���ڵ�
	for (int row=0; row<H; row++) {
		JSAMPROW jsr=(JSAMPROW)(Pixel+row*W);
		jpeg_read_scanlines(&jd, &jsr, 1);

		// 24bit���� 32bit�� ��ȯ�Ѵ�(libjpeg���� �����ϴ� ����� ����)
		char *p=(char*)(Pixel+row*W)+3*(W-1), *q=(char*)(Pixel+row*W)+4*(W-1);
		do {
			q[0]=p[0];
			q[1]=p[1];
			q[2]=p[2];
			q[3]=0;
			p-=3;
			q-=4;
		} while (p!=q);
	}

	// ���ڵ� ��ó��
	jpeg_finish_decompress(&jd);
	jpeg_destroy_decompress(&jd);

	// Alpha �ʱ�ȭ
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, a++) *a=1;
	}

	// Region �ʱ�ȭ
	InitRgn();
}
