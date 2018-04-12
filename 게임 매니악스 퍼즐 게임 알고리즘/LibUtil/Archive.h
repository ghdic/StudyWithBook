#ifndef ARCHIVEH
#define ARCHIVEH

#include "Rand.h"
#include <windows.h>
#include <vector>
#include <string>
using namespace std;

// 아카이브 된 파일의 엔트리
struct ARCHIVE_ENTRY {
	wstring Name;
	DWORD Size, Offset;
	char* Data;
};

// 공통부분
class CArchive {
protected:
	DWORD Version;
	wstring Password;
	vector<ARCHIVE_ENTRY> Entry;
	CRand Rand;
public:
	CArchive() {}
	void SetPassword(wstring password) { Password=password; }
	size_t Count() { return Entry.size(); }
	ARCHIVE_ENTRY* Find(wstring name);
	ARCHIVE_ENTRY* Get(size_t index) { return &Entry[index]; }
};

// 아카이브 작성
class CArchiveSaver : public CArchive {
public:
	CArchiveSaver() {}
	~CArchiveSaver();
	void Add(wstring name);
	bool Save(wstring name);
};

// 아카이브 로드
class CArchiveLoader : public CArchive {
protected:
	char* Work;
	DWORD WorkSize;
public:
	CArchiveLoader();
	~CArchiveLoader();
	bool Load(wstring name);
};

#endif
