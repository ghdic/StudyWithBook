#include "LibUtil.h"
#include <assert.h>

// �Ҹ���
CArchiveSaver::~CArchiveSaver() {
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		delete[] Entry[i].Data;
	}
}

// ��Ʈ�� �߰�
void CArchiveSaver::Add(wstring name) {
	
	// ��Ʈ�� ���� ����
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h!=INVALID_HANDLE_VALUE) {

		// ũ�� Ȯ��
		DWORD size=GetFileSize(h, NULL);
		char* data=new char[size];
		
		// ��Ʈ�� ���� �б�
		DWORD nbytes;
		ReadFile(h, data, size, &nbytes, NULL);
		
		// ��Ʈ�� ���
		ARCHIVE_ENTRY e;
		e.Name=name;
		e.Size=size;
		e.Data=data;
		Entry.push_back(e);
		
		// ��Ʈ�� ���� �ݱ�
		CloseHandle(h);

		// ���ũ�� ���
		DWORD header_size=sizeof(DWORD)*2;
		for (size_t i=0, n=Entry.size(); i<n; i++) {
			header_size+=Entry[i].Name.length()+1;
			header_size+=sizeof(DWORD)*2;
		}
		
		// �ɼ� ���
		DWORD offset=header_size;
		for (size_t i=0, n=Entry.size(); i<n; i++) {
			Entry[i].Offset=offset;
			offset+=Entry[i].Size+1;
		}
	}
}

// Save
bool CArchiveSaver::Save(wstring name) {

	// Work area Ȯ��
	if (Entry.empty()) return false;
	DWORD work_size=Entry.back().Offset+Entry.back().Size+1;
	char* work=new char[work_size];
	char* p=work+sizeof(DWORD);

	// ��Ʈ�� �� ��ֱ�
	*(DWORD*)p=Entry.size();
	p+=sizeof(DWORD);
	
	// ��Ʈ�� ��ֱ�
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		
		// �̸�
		wcscpy((wchar_t*)p, Entry[i].Name.c_str());
		p+=(Entry[i].Name.length()+1)*sizeof(wchar_t);
		
		// ũ��
		*(DWORD*)p=Entry[i].Size;
		p+=sizeof(DWORD);
		
		// �ɼ�
		*(DWORD*)p=Entry[i].Offset;
		p+=sizeof(DWORD);
	}
	
	// ������ ��ֱ�
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		
		// ������
		memcpy(p, Entry[i].Data, Entry[i].Size);
		p+=Entry[i].Size;
		
		// NULL ����
		*p++='\0';
	}
	assert(p-work==work_size);

	// üũ��
	DWORD sum=0;
	for (DWORD i=4, n=work_size-3; i<n; i+=4) {
		sum+=*(DWORD*)(work+i);
	}
	*(DWORD*)work=sum;

	// ��ũ����
	Rand.Init(Password);
	for (DWORD i=0, n=work_size-3; i<n; i+=4) {
		*(DWORD*)(work+i)^=Rand.Int32();
	}

	// ��ī�̺� ���� ����
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_WRITE, 0, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h==INVALID_HANDLE_VALUE) return false;
		
	// ��ֱ�
	DWORD nbytes;
	WriteFile(h, work, work_size, &nbytes, NULL);

	// ��ī�̺� ���� �ݱ�
	CloseHandle(h);
	
	// Work area ����
	delete[] work;
	
	return true;
}

// ������
CArchiveLoader::CArchiveLoader()
:	Work(NULL), WorkSize(0) 
{}

// �Ҹ���
CArchiveLoader::~CArchiveLoader() {
	if (Work) delete[] Work;
}

// Load
bool CArchiveLoader::Load(wstring name) {
	DWORD nbytes;

	// ��ī�̺� ���� ����
	HANDLE h=CreateFile(
		name.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h==INVALID_HANDLE_VALUE) return false;

	// ũ�� Ȯ��
	DWORD size=GetFileSize(h, NULL);
	char* data=new char[size];
	
	// Work area Ȯ��
	if (WorkSize<size) {
		if (Work) delete[] Work;
		Work=new char[size];
		WorkSize=size;
	}
	
	// ��ī�̺� ���� �б�
	ReadFile(h, Work, size, &nbytes, NULL);
	
	// ��ī�̺� ���� �ݱ�
	CloseHandle(h);

	// ��ũ���� ����
	Rand.Init(Password);
	for (DWORD i=0, n=size-3; i<n; i+=4) {
		*(DWORD*)(Work+i)^=Rand.Int32();
	}

	// üũ��
	DWORD sum=0;
	for (DWORD i=4, n=size-3; i<n; i+=4) {
		sum+=*(DWORD*)(Work+i);
	}
	if (sum!=*(DWORD*)Work) return false;

	// ��Ʈ�� Ŭ����
	Entry.clear();
	
	// ��Ʈ�� ���� �б�
	char* p=Work+sizeof(DWORD);
	DWORD num_entries=*(DWORD*)p;
	p+=sizeof(DWORD);

	// ��Ʈ�� �б�
	for (DWORD i=0; i<num_entries; i++) {
		ARCHIVE_ENTRY e;
		
		// �̸�
		e.Name=(wchar_t*)p;
		p+=(e.Name.length()+1)*sizeof(wchar_t);
		
		// ũ��
		e.Size=*(DWORD*)p;
		p+=sizeof(DWORD);

		// �ɼ�
		e.Offset=*(DWORD*)p;
		p+=sizeof(DWORD);

		// ��巹��
		e.Data=Work+e.Offset;
		
		// ��Ʈ�� ���
		Entry.push_back(e);
	}
	return true;
}

// ��Ʈ�� �˻�
ARCHIVE_ENTRY* CArchive::Find(wstring name) {
	for (size_t i=0, n=Entry.size(); i<n; i++) {
		if (name==Entry[i].Name) return &Entry[i];
	}
	return NULL;
}


