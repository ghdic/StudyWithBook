#ifndef UTILH
#define UTILH

#pragma warning(disable:4786)
#include <vector>
#include <string>
using namespace std;

//==============================================================
// 기타
#define SJIS 932
#define JIS 50220
#define EUC 50932
#define UTF8 65001
wstring MultiToWide(string s, int encoding);
string WideToMulti(wstring ws, int encoding);
double GetPreciseTime();

//==============================================================
// 파일 관련
wstring GetExePath();
wstring ExtractFilePath(wstring file);
wstring ExtractFileName(wstring file);
bool FileExists(wstring file);
bool RegFileExt(wstring ext, wstring inst, wstring command, bool force);
wstring GetSpecialFolderPath(int csidl);
wstring GetDesktopFolderPath();
int GetWebFile(wstring url, char** data, wstring app_name);
void OpenURL(wstring url);

//==============================================================
// 문자열 관련
int Find(wstring s, wstring pattern);
int RFind(wstring s, wstring pattern);
class EConvertError {};
int ToInt(wstring s);
wstring ToStr(int i);
wstring TrimLeft(wstring s);
wstring TrimRight(wstring s);
wstring Trim(wstring s);
wstring ConvertEscapes(wstring s);
bool StartsWith(wstring s, wstring pattern);
bool EndsWith(wstring s, wstring pattern);
wstring ToUpper(wstring s);
wstring ToLower(wstring s);

wstring TrimLeft(wstring s, wstring pattern);
wstring TrimRight(wstring s, wstring pattern);
wstring Trim(wstring s, wstring pattern);

wstring GetToken(wstring s, int index, wstring separator);
int GetTokenCount(wstring s, wstring separator);

wstring Replace(wstring s, wstring from, wstring to);
wstring Replace2(wstring s, int n, wstring fromto[][2]);
wstring ToHan123(wstring s);
wstring ToHanABC(wstring s);
float ToFloat(wstring s);

//==============================================================
// CStrings클래스 : 복수행의 문자열을 취급
class CStrings {
private:
	vector<wstring> Strings;
public:
	CStrings();
	void Add(wstring s);
	int GetCount();
	wstring GetString(int index);
	inline wstring Get(int index) { return GetString(index); }
	void SetString(int index, wstring str);
	wstring GetText();
	void SetText(wstring text);
	void Clear();
	void Assign(CStrings* source);

	bool LoadFromFile(wstring file_name);
	bool SaveToFile(wstring file_name);
	bool LoadFromWeb(wstring url, wstring app_name);

	wstring GetName(int index);
	wstring GetValue(int index);
	int FindName(wstring name);
	wstring GetValue(wstring name);
	void SetValue(wstring name, wstring value);
	void SetValue(wstring name, int value);
};


//==============================================================
// 문자 판정용 서브루틴
bool IsAlphabet(char c);
bool IsDigit(char c);


//==============================================================
// 문자코드 관련
string JISToSJIS(string s);
string EUCToSJIS(string s);
string ToSJIS(string s);
string FromBase64(string s);
string ToBase64(string s);


#endif

