#include "LibUtil.h"
#include "libpng\\png.h"
extern "C" {
	#include "libjpeg\\jpeglib.h"
}

//==============================================================
// 초기화

// 초기화(DIB 섹션)
void CDIB::InitDIB()
{
	if (DIB) DeleteObject(DIB);

	// DIB 섹션의 정보를 갖는 BITMAPINFO 구조체를 만든다
	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	BITMAPINFOHEADER* bih=&bi.bmiHeader;
	bih->biSize=sizeof(BITMAPINFOHEADER);
	bih->biWidth=W;
	bih->biHeight=-H;
	bih->biPlanes=1;
	bih->biBitCount=32;
	bih->biCompression=BI_RGB;

	// DIB 섹션의 작성
	DIB=CreateDIBSection(
		DC, &bi, DIB_RGB_COLORS,
		(void**)&Pixel, NULL, 0);
	SelectObject(DC, DIB);

	// Alpha 영역의 작성
	if (Alpha) delete [] Alpha;
	Alpha=new BYTE[W*H];
}

// 초기화(Region)
// Alpha 에 기반한 영역을 작성한다.
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

// 초기화(Region) :
// 전체 영역을 작성한다.
void CDIB::InitRgnSimple() {
	if (Rgn) DeleteObject(Rgn);
	Rgn=CreateRectRgn(0, 0, W, H);
}


//==============================================================
// 생성자, 소멸자

// 생성자의 공통처리
void CDIB::Construct() {
	HDC hdc=::GetDC(NULL);
	DC=CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
}

// 생성자(Size)
CDIB::CDIB(int w, int h)
:	DIB(NULL), Alpha(NULL), Rgn(NULL), W(w), H(h)
{
	Construct();
	InitDIB();
	InitRgnSimple();
}

// 생성자(Capture)
CDIB::CDIB(HWND hwnd)
:	DIB(NULL), Alpha(NULL), Rgn(NULL)
{
	Construct();
	Capture(hwnd);
}

// 생성자(File)
CDIB::CDIB(wstring file_name)
:	DIB(NULL), Alpha(NULL), Rgn(NULL)
{
	Construct();
	LoadFromFile(file_name);
}

// 소멸자
CDIB::~CDIB() {
	if (DC) DeleteDC(DC);
	if (DIB) DeleteObject(DIB);
	if (Alpha) delete[] Alpha;
	if (Rgn) DeleteObject(Rgn);
}


//==============================================================
// 복사

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
// Clip 복사(Region을 사용)

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
// 화면 캡쳐
void CDIB::Capture(HWND hwnd)
{
	// 화상 크기를 윈도우 크기에 맞춘다
	if (!hwnd) hwnd=GetDesktopWindow();	
	GetWindowWH(hwnd, &W, &H);
	InitDIB();
	InitRgnSimple();
	
	// 화면을 캡쳐한다
	HDC hdc_win=::GetDC(NULL);
	BitBlt(DC, 0, 0, W, H, hdc_win, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hdc_win);
}


//==============================================================
// 투명색의 설정 :
// 지정한 색과 일치하는 픽셀을 투명으로 한다.
// 다른 색의 픽셀은 불투명하게 된다.
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
// 로드
void CDIB::LoadFromFile(wstring file_name) {
	if (!FileExists(file_name)) return;
	file_name=ToUpper(file_name);	
	if (EndsWith(file_name, L".BMP")) LoadBMPFile(file_name); else
	if (EndsWith(file_name, L".PNG")) LoadPNGFile(file_name);
	if (EndsWith(file_name, L".JPG") || EndsWith(file_name, L".JPEG")) LoadJPEGFile(file_name);
}

// BMP 형식의 로드
void CDIB::LoadBMPFile(wstring file_name)
{
	// BITMAP 로드
	HBITMAP hbmp=(HBITMAP)LoadImage(
		NULL, file_name.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	// DIB 초기화
	BITMAP bmp;
	GetObject(hbmp, sizeof(bmp), &bmp);
	W=bmp.bmWidth;
	H=bmp.bmHeight;
	InitDIB();
	FillMemory(Alpha, W*H, 255);
	InitRgnSimple();

	// BITMAP을 DBI에 복사
	Draw(0, 0, W, H, hbmp, 0, 0, W, H);
	DeleteObject(hbmp);
}

// PNG 형식의 로드
void CDIB::LoadPNGFile(wstring file_name)
{
	unsigned int sig_read=0;
	int bit_depth, color_type, interlace_type;
	
	// 파일 열기
	FILE* fp=_wfopen(file_name.c_str(), L"rb");
	if (!fp) return;

	// png_struct 초기화
	png_structp png_ptr=png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		return;
	}
	png_init_io(png_ptr, fp);

	// png_info 초기화
	png_infop info_ptr=png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return;
	}

	// 화상정보 취득
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, 
		(unsigned long*)&W, (unsigned long*)&H, 
		&bit_depth, &color_type, &interlace_type, NULL, NULL);

	// DIB 초기화
	InitDIB();
	
	// 변환방법 설정
	// Palette를 RGB로
	if (color_type==PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// 투명색을 Alpha로
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// Bit depth를 8bit로
	if (bit_depth<8) png_set_packing(png_ptr); else
	if (bit_depth==16) png_set_strip_16(png_ptr);
	
	// 24bit(RGB)를 32bit(RGB0)로
	if (color_type==PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);

	// RGB를 BGR로
	if (color_type==PNG_COLOR_TYPE_PALETTE ||
		color_type==PNG_COLOR_TYPE_RGB ||
		color_type==PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);

	// 화상 읽기
	png_bytep *row_pointers=new png_bytep[H];
	for (int row=0; row<H; row++) row_pointers[row]=(png_bytep)(Pixel+row*W);
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);
	delete [] row_pointers;

	// 뒷처리
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	// Alpha 값을 얻는다
	DWORD* p=Pixel;
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, p++, a++) {
			*a=(BYTE)(*p>>24);
			*p&=0x00ffffff;
		}
	}

	// Region 초기화
	InitRgn();
}

// JPEG 형식의 로드
static void JpegErrorHandler(j_common_ptr) {}

void CDIB::LoadJPEGFile(wstring file_name)
{
	// 에러 핸들러 설정
	jpeg_decompress_struct jd;
	jpeg_error_mgr jem;
	jd.err=jpeg_std_error(&jem);
	jem.error_exit=JpegErrorHandler;

	// 파일 열기
	FILE* fp=_wfopen(file_name.c_str(), L"rb");
	if (!fp) return;

	// 디코드 전처리
	jpeg_create_decompress(&jd);
	jpeg_stdio_src(&jd, fp);
	jpeg_read_header(&jd, TRUE);
	jpeg_start_decompress(&jd);

	// DIB 초기화
	W=jd.image_width;
	H=jd.image_height;
	InitDIB();
	
	// 디코드
	for (int row=0; row<H; row++) {
		JSAMPROW jsr=(JSAMPROW)(Pixel+row*W);
		jpeg_read_scanlines(&jd, &jsr, 1);

		// 24bit에서 32bit로 변환한다(libjpeg에서 수행하는 방법도 있음)
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

	// 디코드 후처리
	jpeg_finish_decompress(&jd);
	jpeg_destroy_decompress(&jd);

	// Alpha 초기화
	BYTE* a=Alpha;
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++, a++) *a=1;
	}

	// Region 초기화
	InitRgn();
}
