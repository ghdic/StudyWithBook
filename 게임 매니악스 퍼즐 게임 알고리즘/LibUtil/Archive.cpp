#include "LibUtil.h"
#include <assert.h>

// 소멸자
CArchiveSaver::~CArchiveSaver() {
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		delete[] Entry[i].Data;
	}
}

// 엔트리 추가
void CArchiveSaver::Add(wstring name) {
	
	// 엔트리 파일 열기
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h!=INVALID_HANDLE_VALUE) {

		// 크기 확인
		DWORD size=GetFileSize(h, NULL);
		char* data=new char[size];
		
		// 엔트리 파일 읽기
		DWORD nbytes;
		ReadFile(h, data, size, &nbytes, NULL);
		
		// 엔트리 등록
		ARCHIVE_ENTRY e;
		e.Name=name;
		e.Size=size;
		e.Data=data;
		Entry.push_back(e);
		
		// 엔트리 파일 닫기
		CloseHandle(h);

		// 헤더크기 계산
		DWORD header_size=sizeof(DWORD)*2;
		for (size_t i=0, n=Entry.size(); i<n; i++) {
			header_size+=Entry[i].Name.length()+1;
			header_size+=sizeof(DWORD)*2;
		}
		
		// 옵셋 계산
		DWORD offset=header_size;
		for (size_t i=0, n=Entry.size(); i<n; i++) {
			Entry[i].Offset=offset;
			offset+=Entry[i].Size+1;
		}
	}
}

// Save
bool CArchiveSaver::Save(wstring name) {

	// Work area 확보
	if (Entry.empty()) return false;
	DWORD work_size=Entry.back().Offset+Entry.back().Size+1;
	char* work=new char[work_size];
	char* p=work+sizeof(DWORD);

	// 엔트리 수 써넣기
	*(DWORD*)p=Entry.size();
	p+=sizeof(DWORD);
	
	// 엔트리 써넣기
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		
		// 이름
		wcscpy((wchar_t*)p, Entry[i].Name.c_str());
		p+=(Entry[i].Name.length()+1)*sizeof(wchar_t);
		
		// 크기
		*(DWORD*)p=Entry[i].Size;
		p+=sizeof(DWORD);
		
		// 옵셋
		*(DWORD*)p=Entry[i].Offset;
		p+=sizeof(DWORD);
	}
	
	// 데이터 써넣기
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		
		// 데이터
		memcpy(p, Entry[i].Data, Entry[i].Size);
		p+=Entry[i].Size;
		
		// NULL 문자
		*p++='\0';
	}
	assert(p-work==work_size);

	// 체크섬
	DWORD sum=0;
	for (DWORD i=4, n=work_size-3; i<n; i+=4) {
		sum+=*(DWORD*)(work+i);
	}
	*(DWORD*)work=sum;

	// 스크램블
	Rand.Init(Password);
	for (DWORD i=0, n=work_size-3; i<n; i+=4) {
		*(DWORD*)(work+i)^=Rand.Int32();
	}

	// 아카이브 파일 열기
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_WRITE, 0, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h==INVALID_HANDLE_VALUE) return false;
		
	// 써넣기
	DWORD nbytes;
	WriteFile(h, work, work_size, &nbytes, NULL);

	// 아카이브 파일 닫기
	CloseHandle(h);
	
	// Work area 해제
	delete[] work;
	
	return true;
}

// 생성자
CArchiveLoader::CArchiveLoader()
:	Work(NULL), WorkSize(0) 
{}

// 소멸자
CArchiveLoader::~CArchiveLoader() {
	if (Work) delete[] Work;
}

// Load
bool CArchiveLoader::Load(wstring name) {
	DWORD nbytes;

	// 아카이브 파일 열기
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h==INVALID_HANDLE_VALUE) return false;

	// 크기 확인
	DWORD size=GetFileSize(h, NULL);
	char* data=new char[size];
	
	// Work area 확보
	if (WorkSize<size) {
		if (Work) delete[] Work;
		Work=new char[size];
		WorkSize=size;
	}
	
	// 마카이브 파일 읽기
	ReadFile(h, Work, size, &nbytes, NULL);
	
	// 마카이브 파일 닫기
	CloseHandle(h);

	// 스크램블 해제
	Rand.Init(Password);
	for (DWORD i=0, n=size-3; i<n; i+=4) {
		*(DWORD*)(Work+i)^=Rand.Int32();
	}

	// 체크섬
	DWORD sum=0;
	for (DWORD i=4, n=size-3; i<n; i+=4) {
		sum+=*(DWORD*)(Work+i);
	}
	if (sum!=*(DWORD*)Work) return false;

	// 엔트리 클리어
	Entry.clear();
	
	// 엔트리 수를 읽기
	char* p=Work+sizeof(DWORD);
	DWORD num_entries=*(DWORD*)p;
	p+=sizeof(DWORD);

	// 엔트리 읽기
	for (DWORD i=0; i<num_entries; i++) {
		ARCHIVE_ENTRY e;
		
		// 이름
		e.Name=(wchar_t*)p;
		p+=(e.Name.length()+1)*sizeof(wchar_t);
		
		// 크기
		e.Size=*(DWORD*)p;
		p+=sizeof(DWORD);

		// 옵셋
		e.Offset=*(DWORD*)p;
		p+=sizeof(DWORD);

		// 어드레스
		e.Data=Work+e.Offset;
		
		// 엔트리 등록
		Entry.push_back(e);
	}
	return true;
}

// 엔트리 검색
ARCHIVE_ENTRY* CArchive::Find(wstring name) {
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		if (name==Entry[i].Name) return &Entry[i];
	}
	return NULL;
}


