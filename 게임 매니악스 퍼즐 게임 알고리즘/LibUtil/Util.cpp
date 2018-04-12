#include "LibUtil.h"
#include <fstream>
#include <sstream>
#include <windows.h>
#include <assert.h>
#include <mbstring.h>
#include <shlobj.h>
#include <wininet.h>

//==================================================================
// 문자코드 변환

wstring MultiToWide(string s, int encoding) {
	int wsize=MultiByteToWideChar(encoding, 0, s.c_str(), s.length(), NULL, 0);
	if (wsize>0) {
		wchar_t* ws=new wchar_t[wsize];
		if (MultiByteToWideChar(encoding, 0, s.c_str(), s.length(), ws, wsize)==wsize) {
			return wstring(ws);
		}
	}
	return L"";
}

string WideToMulti(wstring ws, int encoding) {
	int size=WideCharToMultiByte(encoding, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
	if (size>0) {
		char* s=new char[size];
		if (WideCharToMultiByte(encoding, 0, ws.c_str(), -1, s, size, NULL, NULL)==size) {
			return string(s);
		}
	}
	return "";
}



double GetPreciseTime() {
	LARGE_INTEGER freq, count;
	if (QueryPerformanceFrequency(&freq) && 
		QueryPerformanceCounter(&count)) {
		return (double)count.QuadPart/freq.QuadPart;
	} else {
		return (double)GetTickCount()/1000;
	}
}

void OpenURL(wstring url) {
	ShellExecute(NULL, NULL, url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

wstring GetSpecialFolderPath(int csidl) {
	wchar_t buf[MAX_PATH];
	SHGetSpecialFolderPath(NULL, buf, csidl, false);
	return wstring(buf);
}

wstring GetDesktopFolderPath() {
	return GetSpecialFolderPath(CSIDL_DESKTOP);
}


// 확장자 등록
bool RegFileExt(wstring ext, wstring inst, wstring command, bool force) {
	HKEY key;
	DWORD disp;
	wstring cls=ext+L"_auto_file";
	
	// 확장자 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (wstring(L".")+ext).c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	if (!force && disp==REG_OPENED_EXISTING_KEY) {
		RegCloseKey(key);
		return false;
	}
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)cls.c_str(), cls.length());
	RegCloseKey(key);

	// 설명 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, cls.c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)inst.c_str(), inst.length());
	RegCloseKey(key);

	// 커맨드 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (cls+L"\\shell\\open\\command").c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	wstring cmd=wstring(L"\"")+command+L"\" \"%1\"";
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)cmd.c_str(), cmd.length());
	RegCloseKey(key);

	// 아이콘 등록
	if (ERROR_SUCCESS!=RegCreateKeyEx(
		HKEY_CLASSES_ROOT, (cls+L"\\DefaultIcon").c_str(), NULL, L"", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp)) {
		return false;
	}
	wstring icon=command+L",0";
	RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)icon.c_str(), icon.length());
	RegCloseKey(key);

	return true;
}


wstring TrimLeft(wstring s, wstring pattern) {
	int pl=pattern.length();
	while (StartsWith(s, pattern)) s=s.substr(pl);
	return s;
}

wstring TrimRight(wstring s, wstring pattern) {
	int sl=s.length(), pl=pattern.length();
	while (EndsWith(s, pattern)) s=s.substr(sl-pl);
	return s;
}

wstring Trim(wstring s, wstring pattern) {
	s=TrimLeft(s, pattern);
	s=TrimRight(s, pattern);
	return s;
}

int Find(wstring s, wstring pattern) {
	for (int i=0, n=s.length(), pl=pattern.length(); i<n; i++) {
		if (s.compare(i, pl, pattern)==0) return i;
	}
	return wstring::npos;
}

int RFind(wstring s, wstring pattern) {
	for (int i=s.length()-1, pl=pattern.length(); i>=0; i--) {
		if (s.compare(i, pl, pattern)==0) return i;
	}
	return wstring::npos;
}

wstring GetToken(wstring s, int index, wstring separator) {
	int sl=separator.length();
	for (int i=0; i<index; i++) {
		int pos=Find(s, separator);
		if (pos==wstring::npos) return L"";
		s=s.substr(pos+sl);
	}
	return s.substr(0, Find(s, separator));
}

int GetTokenCount(wstring s, wstring separator) {
	int sl=separator.length(), i;
	for (i=1; ; i++) {
		int pos=Find(s, separator);
		if (pos==wstring::npos) break;
		s=s.substr(pos+sl);
	}
	return i;
}

wstring ToHan123(wstring s) {
	wstring t;
	for (int i=0, n=s.length(); i<n; i++) {
		if (L'0'<=s[i] && s[i]<=L'9') t+=s[i]+L'괥'; else t+=s[i];
	}
	return t;
}

wstring ToHanABC(wstring s) {
	wstring t;
	for (int i=0, n=s.length(); i<n; i++) {
		if (L'a'<=s[i] && s[i]<=L'z') t+=s[i]+L'굙'; 
		if (L'A'<=s[i] && s[i]<=L'Z') t+=s[i]+L'?'; else t+=s[i];
	}
	return t;
}

wstring Replace(wstring s, wstring from, wstring to) {
	if (Find(s, from)==wstring::npos) return s;
	wstring t;
	int fl=from.length();
	while (!s.empty()) {
		if (StartsWith(s, from)) {
			t+=to;
			s=s.substr(fl);
		} else {
			t+=s[0];
			s=s.substr(1);
		}
	}
	return t;
}

wstring Replace2(wstring s, int n, wstring fromto[][2]) {
	int i;
	for (i=0; i<n; i++) {
		if (Find(s, fromto[i][0])!=wstring::npos) break;
	}
	if (i==n) return s;
	wstring t;
	while (!s.empty()) {
		for (i=0; i<n; i++) {
			if (StartsWith(s, fromto[i][0])) {
				t+=fromto[i][1];
				s=s.substr(fromto[i][0].length());
				break;
			}
		}
		if (i==n) {
			t+=s[0];
			s=s.substr(1);
		}
	}
	return t;
}


float ToFloat(wstring s) {
	wistringstream iss(s.c_str());
	float f=0;
	iss>>f;
	return f;
}

int ToInt(wstring s) {
	wistringstream iss(s.c_str());
	int i=0;
	iss>>i;
	return i;
}




//==============================================================
// 파일 관련


// 실행 파일의 경로를 반환
wstring GetExePath() {
	wchar_t buf[MAX_PATH+1];
	GetModuleFileName(NULL, buf, MAX_PATH);
	return ExtractFilePath(buf);
}


// [경로+파일명]에서 경로 부분을 꺼내기
wstring ExtractFilePath(wstring file) {
	unsigned long pos=RFind(file, L"\\");
	if (pos==wstring::npos) return L"";
	return file.substr(0, pos+1);
}


// [경로+파일명]에서 파일명 부분을 꺼내기
wstring ExtractFileName(wstring file) {
	unsigned long pos=RFind(file, L"\\");
	if (pos==wstring::npos) return file;
	return file.substr(pos+1);
}

// 파일이 존재한다면 true를 반환
bool FileExists(wstring file) {
	WIN32_FIND_DATA fd;
	return FindFirstFile(file.c_str(), &fd)!=INVALID_HANDLE_VALUE;
}

// Web상의 파일을 취득
// url은 URL의 문자열, data는 취득한 데이터
int GetWebFile(wstring url, char** data, wstring app_name) {
	HINTERNET inet, file;
	DWORD data_size=0, read_size;
	char* data_buf=NULL;

	// Web에 접속한다
	inet=InternetOpen(
		app_name.c_str(), INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if (inet) {

		// URL에서 지정된 파일을 열기
		file=InternetOpenUrl(
			inet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (file) {

			// 읽을 수 있는 파일의 크기를 조사
			if (InternetQueryDataAvailable(
				file, &read_size, 0, 0)) {

				// 파일 읽기
				do {
					
					// 읽기 위한 버퍼를 확장
					data_buf=(char*)realloc(data_buf, data_size+read_size+1);

					// 파일의 일보를 읽기
					InternetReadFile(
						file, data_buf+data_size, read_size, &read_size);

					// 읽은 크기를 더한다
					data_size+=read_size;

				} while (read_size>0);
				data_buf[data_size]='\0';

			}

			// 파일을 닫는다
			InternetCloseHandle(file);
		}
		
		// 접속을 종료한다
		InternetCloseHandle(inet);
	}

	*data=data_buf;
	return (int)data_size;
}


//==============================================================
// 문자열 관련

// 대문자로 변환
wstring ToUpper(wstring s) {
	wstring out=s;
	for (int i=0, n=out.size(); i<n; i++) {
		out[i]=toupper(out[i]);
	}
	return out;
}


// 소문자로 변환
wstring ToLower(wstring s) {
	wstring out=s;
	for (int i=0, n=out.size(); i<n; i++) {
		out[i]=tolower(out[i]);
	}
	return out;
}

// 문자열의 앞부분이 지정한 문자열인지를 검사
bool StartsWith(wstring s, wstring pattern) {
	return s.compare(0, pattern.length(), pattern)==0;
}


// 문자열의 마지막 부분이 지정한 문자열인지를 검사
bool EndsWith(wstring s, wstring pattern) {
	int len=pattern.length();
	int pos=s.length()-len;
	if (pos<0 || s.compare(pos, len, pattern)!=0) return false;
	return true;
}


// 정수를 문자열로 변환
wstring ToStr(int i) {
	wostringstream oss;
	oss<<i<<ends;
	return oss.str();
}


// 문자열 중의 escape 문자를 변환
// \n는 개행으로, \t는 탭으로 변환
// 그 외의 [\문자]는 [문자]를 그대로 출력한다
wstring ConvertEscapes(wstring s) {
	wstring ret;
	int i, n;
	for (i=0, n=s.length(); i<n; i++) {
		if (s[i]=='\\' && i+1<n) {
			switch (s[i+1]) {
				case 'n': ret+='\n'; break;
				case 't': ret+='\t'; break;
				case '\n': break;
				default: ret+=s[i+1]; break;
			}
			i++;
		} else {
			ret+=s[i];
		}
	}
	return ret;
}


// 문자열의 왼쪽에 있는 공백, 탭, 개행을 삭제한다
wstring TrimLeft(wstring s) {
	int i, n;
	for (i=0, n=s.length(); i<n ; i++)
		if (s[i]!=' ' && s[i]!='\t' && 
			s[i]!='\n' && s[i]!='\r') break;
	return s.substr(i, n-i);
}


// 문자열의 오른쪽에 있는 공백, 탭, 개행을 삭제한다
wstring TrimRight(wstring s) {
	int i;
	for (i=s.length()-1; i>=0; i--)
		if (s[i]!=' ' && s[i]!='\t' &&
			s[i]!='\n' && s[i]!='\r') break;
	return s.substr(0, i+1);
}


// 문자열의 양쪽에 있는 공백, 탭, 개행을 삭제한다
wstring Trim(wstring s) {
	return TrimRight(TrimLeft(s));
}




//==============================================================
// 문자 판정용 서브루틴


// c가 알파벳이라면 알파벳을 반환
bool IsAlphabet(char c) {
	return 'a'<=c && c<='z' || 'A'<=c && c<='Z';
}


// c가 숫자라면 숫자를 반환
bool IsDigit(char c) {
	return '0'<=c && c<='9';
}



//==============================================================
// CStrings클래스 : 복수행의 문자열을 취급


// 생성자
CStrings::CStrings() {
	Strings.clear();
}


// 마지막에 문자열을 추가
void CStrings::Add(wstring s) {
	Strings.push_back(s);
}


// 모든 문자열을 삭제
void CStrings::Clear() {
	Strings.clear();
}


// 복사
void CStrings::Assign(CStrings* source) {
	Strings.assign(source->Strings.begin(), source->Strings.end());
}


// 행의 수를 반환
int CStrings::GetCount() {
	return Strings.size();
}


// 지정한 행을 반환
wstring CStrings::GetString(int index) {
	if (index<0 || (int)Strings.size()<=index) return L"";
	return Strings[index];
}


// 지정한 행에 문자열을 설정
void CStrings::SetString(int index, wstring str) {
	if (index<0 || (int)Strings.size()<=index) return;
	Strings[index]=str;
}


// 복수행의 문자열을 개행을 더해 연결하고, 단일 문자열로 반환
wstring CStrings::GetText() {
	wstring s;
	int i, n;
	for (i=0, n=Strings.size(); i<n; i++) {
		s+=Strings[i];
		s+=L'\n';
	}
	return s;
}


// 복수행의 문자열을 개행으로 나누어 복수 개의 문자열로 만든다
void CStrings::SetText(wstring text) {
	Clear();
	int pos;
	while ((pos=Find(text, L"\n"))!=wstring::npos) {
		if (pos<1 || text[pos-1]!=L'\r') {
			Strings.push_back(text.substr(0, pos));
		} else {
			Strings.push_back(text.substr(0, pos-1));
		}
		text=text.substr(pos+1);
	}
	Strings.push_back(text);
}

// 파일에서 복수행의 문자열을 읽는다
bool CStrings::LoadFromFile(wstring file_name) {
	wifstream fin;
	static const int BUFSIZE=1024;
	static wchar_t buf[BUFSIZE];
	Clear();
	fin.open(file_name.c_str());
	if (fin.fail()) return false;
	for (fin.getline(buf, BUFSIZE); !fin.fail(); fin.getline(buf, BUFSIZE))
		Strings.push_back(buf);
	fin.close();
	return true;
}

// 파일에 복수행의 문자열을 써넣는다
bool CStrings::SaveToFile(wstring file_name) {
	wofstream fout;
	fout.open(file_name.c_str());
	if (fout.fail()) return false;
	fout<<GetText();
	fout.close();
	return true;
}

// Web상의 파일에서 복수행의 문자열을 읽는다
bool CStrings::LoadFromWeb(wstring url, wstring app_name) {
	char* buffer;
	if (!GetWebFile(url, &buffer, app_name)) return false;
	SetText(MultiToWide(ToSJIS(buffer), 932));
	free(buffer);
	return true;
}


//==============================================================
static const wchar_t* copyright=
	L"Copyright(C) 2004 Higpen Works(마쯔우라 켄이치로/츠카사 유키)";


//==============================================================
// CStrings클래스 중에서
// 문자열을 [이름=값] 형식으로 해석하는 함수들

// 지정한 행의 이름 부분을 반환
wstring CStrings::GetName(int index) {
	if (index<0 || (int)Strings.size()<=index) return L"";
	wstring s=Strings[index];
	int pos=Find(s, L"=");
	if (pos<0) return L"";
	return s.substr(0, pos);
}

// 지정한 행의 값 부분을 반환
wstring CStrings::GetValue(int index) {
	if (index<0 || (int)Strings.size()<=index) return L"";
	wstring s=Strings[index];
	int pos=Find(s, L"=");
	if (pos<0) return L"";
	return s.substr(pos+1, s.length()-pos-1);
}

// 지정한 이름을 가진 최초의 행 번호를 반환
int CStrings::FindName(wstring name) {
	int i, n;
	for (i=0, n=Strings.size(); i<n; i++) {
		if (GetName(i)==name) return i;
	}
	return -1;
}

// 지정한 이름을 가진 최초의 행을 찾아서 값을 반환 :
// 이름이 찾아지지 않으면 공백문자열을 반환
wstring CStrings::GetValue(wstring name) {
	int i=FindName(name);
	if (i<0) return L"";
	return GetValue(i);
}

// 지정한 이름을 가진 최초의 행을 찾아서 문자열 값을 설정
void CStrings::SetValue(wstring name, wstring value) {
	int i=FindName(name);
	if (i>=0) Strings[i]=name+L"="+value;
	else Strings.push_back(name+L"="+value);
}

// 지정한 이름을 가진 최초의 행을 찾아서 정수 값을 설정
void CStrings::SetValue(wstring name, int value) {
	SetValue(name, ToStr(value));
}


string Encode(string str) {
	string s=str;
	for (int i=0, n=s.length(), rnd=0xaa; i<n; i++) {
		s[i]=s[i]^(char)rnd;
		rnd=214013*rnd+2531011;
	}
	return ToBase64(s);
}

string Decode(string str) {
	string s=FromBase64(str);
	for (int i=0, n=s.length(), rnd=0xaa; i<n; i++) {
		s[i]=s[i]^(char)rnd;
		rnd=214013*rnd+2531011;
	}
	return s;
}


//==============================================================
// 문자코드 관련


// 문자열의 코드를 JIS에서 Shift JIS로 변환한다 :
// 입력문자열을 JIS에서 Shift JIS로 변환한 문자열을 반환
string JISToSJIS(string s) {
	string ret;
	unsigned char c0, c1;
	bool kanji=false;

	for (int i=0, n=(int)s.length(); i<n; i++) {
	
		// Escape sequence의 판별
		if (s[i]==0x1b) {
			i++;
			if (i>=n) break;
			c0=s[i++];
			if (c0==0x24) kanji=true; else
			if (c0==0x28) kanji=false;
			continue;
		}

		// 한자
		if (kanji) {
			c0=s[i++];
			if (i>=n) break;
			c1=s[i];
			ret+=(char)(((c0+1)>>1)+(c0<0x5f ? 0x70 : 0xb0));
			ret+=(char)(c1+(c0%2 ? (c1>0x5f ? 0x20 : 0x1f) : 0x7e));
		} else

		// ASCII문자
		{
			ret+=s[i];
		}
	}
	return ret;
}


// 문자열의 코드를 EUC에서 Shift JIS로 변환한다 :
// 입력문자열을 EUC에서 Shift JIS로 변환한 문자열을 반환
string EUCToSJIS(string s) {
	string ret;
	int i;
	unsigned char c0, c1;
	for (i=0; i<(int)s.length(); i++) {

		// 1바이트 문자는 그대로 복사
		c0=s[i];
		if (c0<=0xa0 || c0==0xff || 
			(unsigned long)i==s.length()-1) {
			ret+=c0;
			continue;
		} 
		
		// 2바이트 문자의 2바이트째 문자를 변환
		c1=s[i+1];
		if (c0%2) {
			c1-=0x61;
			if (c1>0x7e) c1++;
		} else {
			c1-=0x02;
		}

		// 2바이트 문자의 1바이트째 문자를 변환
		if (c0>=0xdf) {
			c0=(c0+1)/2+0x70;
		} else {
			c0=(c0+1)/2+0x30;
		}
		ret+=c0;
		ret+=c1;
		i++;
	}
	return ret;
}


// 문자열의 코드를 Shift JIS로 변환한다 :
// 입력 문자열의 문자 코드를 판별하여 EUC의 경우에는 Shift JIS로 변환한다
// 원래 Shift JIS인 경우에는 아무런 처리를 하지 않는다
string ToSJIS(string s) {
	int i;
	unsigned char c;
	for (i=0; i<(int)s.length(); i++) {

		// Shift JIS인 문자를 발견하면
		// 변환하지 않는다
		c=s[i];
		if (0x81<=c && c<=0x9f) return s;
	}
	
	// Shift JIS인 문자가 발견되지 않았기 때문에
	// EUC라고 판단하여 변환한다
	return EUCToSJIS(s);
}


// BASE64형식의 문자열을 디코딩한다
string FromBase64(string s) {
	string ret;
	unsigned char c[4];
	for (int i=0, n=s.length(); i<n; i+=4) {
		int j;
		
		// 'A'~'Z','a'~'z','0'~'9','+','/'를
		// 0~63의 값으로 변환한다
		for (j=0; j<4 && i+j<n; j++) {
			c[j]=s[i+j];
			if ('A'<=c[j] && c[j]<='Z') c[j]-='A'; else
			if ('a'<=c[j] && c[j]<='z') c[j]-='a'-26; else
			if ('0'<=c[j] && c[j]<='9') c[j]-='0'-52; else
			if (c[j]=='+') c[j]=62; else
			if (c[j]=='/') c[j]=63; else break;
		}
		
		// 6비트 값 4개를 8비트 값 3개로 변환
		if (j>1) ret+=(c[0]<<2 | c[1]>>4);
		if (j>2) ret+=(c[1]<<4 | c[2]>>2);
		if (j>3) ret+=(c[2]<<6 | c[3]);
	}
	return ret;
}


// 문자열을 BASE64형식으로 인코딩한다
string ToBase64(string s) {
	string ret;
	unsigned char c[3], d[4];
	for (int i=0, n=s.length(); i<n; i+=3) {
		int j;

		// 3문자를 꺼내서
		for (j=0; j<3; j++) c[j]=i+j<n?s[i+j]:0;
		
		// 8비트 값 3개를 6비트 값 4개로 변환
		d[0]=(c[0]>>2)&63;
		d[1]=(c[0]<<4|c[1]>>4)&63;
		d[2]=(c[1]<<2|c[2]>>6)&63;
		d[3]=c[2]&63;

		// 0~63의 값을
		// 'A'~'Z','a'~'z','0'~'9','+','/'로 변환
		for (j=0; j<4; j++) {
			if (d[j]<=25) d[j]+='A'; else
			if (26<=d[j] && d[j]<=51) d[j]+='a'-26; else
			if (52<=d[j] && d[j]<=61) d[j]+='0'-52; else
			if (d[j]==62) d[j]='+'; else
			if (d[j]==63) d[j]='/'; else d[j]='A';
		}

		// 결과를 문자열에 추가한다 :
		// 문자 수가 4의 배수가 되도록
		// 필요에 따라 마지막에 '='를 추가한다.
		ret+=d[0];
		ret+=d[1];
		ret+=i+1<n?d[2]:'=';
		ret+=i+2<n?d[3]:'=';
	}
	return ret;
}



