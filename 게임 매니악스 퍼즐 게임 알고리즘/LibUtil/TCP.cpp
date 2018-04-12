#include "LibUtil.h"


//==============================================================
// Winsock �ʱ�ȭ�� ����

// ������� ���� ��
static int SocketCount=0;

// Winsock �ʱ�ȭ
static void InitializeWinsock() {
	if (SocketCount==0) {
		WSADATA data;
		WSAStartup(MAKEWORD(2,0), &data);
	}
	SocketCount++;
}

// Winsock ����
static void FinalizeWinsock() {
	SocketCount--;
	if (SocketCount==0) {
		WSACleanup();
	}
}


//==============================================================
// ������ �⺻ ���

// ������
CTCPBaseSocket::CTCPBaseSocket(SOCKET s)
:	Socket(s)
{
	InitializeWinsock();
	if (Socket==INVALID_SOCKET) {
		Socket=socket(AF_INET, SOCK_STREAM, 0);
	}
}

// �Ҹ���
CTCPBaseSocket::~CTCPBaseSocket() {
	closesocket(Socket);
	FinalizeWinsock();
}


//==============================================================
// TCP ����

// ������
CTCPSocket::CTCPSocket(SOCKET s)
:	CTCPBaseSocket(s)
{
	LineSeparator.push_back(L"\r\n");
	LineSeparator.push_back(L"\r");
	LineSeparator.push_back(L"\n");
}

// ���� : 
// �����ÿ��� true�� ��ȯ
bool CTCPSocket::Connect(wstring host, int port) {

	// ȣ��Ʈ���� ��巹���� ��ȯ
	LPHOSTENT hostent=gethostbyname(WideToMulti(host, SJIS).c_str());
	if (!hostent ||
		hostent->h_addrtype!=AF_INET ||
		hostent->h_length!=4 ||
		!hostent->h_addr_list[0]) return false;

	// ��巹���� ��Ʈ ���
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=*(unsigned long*)(hostent->h_addr_list[0]);
	sa.sin_port=htons(port);

	// ���� ����
	return connect(Socket, (sockaddr*)&sa, sizeof(sa))==0;
}

// ���� �۽�
int CTCPSocket::Send(const char* buffer, int length) {
	return send(Socket, buffer, length, 0);
}

// ���� ����
int CTCPSocket::Recv(char* buffer, int length) {
	return recv(Socket, buffer, length, 0);
}

// Line separator ����
void CTCPSocket::SetLineSeparator(vector<wstring> ls) {
	LineSeparator.clear();
	for (size_t i=0, n=ls.size(); i<n; i++) {
		LineSeparator.push_back(ls[i]);
	}
}

// Line �۽�
void CTCPSocket::SendLine(wstring s) {
	if (!LineSeparator.empty()) s+=LineSeparator[0];
	Send(WideToMulti(s, SJIS).c_str(), s.length());
}

// Line ����
wstring CTCPSocket::RecvLine() {
	char buffer[1000];
	int length=0;
	do {
		buffer[length]='\0';
		LineBuffer+=MultiToWide(buffer, SJIS);
		for (size_t i=0, n=LineSeparator.size(); i<n; i++) {
			int pos;
			if ((pos=Find(LineBuffer, LineSeparator[i]))>=0) {
				wstring s=LineBuffer.substr(0, pos);
				LineBuffer=LineBuffer.substr(pos+LineSeparator[i].length());
				return s;
			}
		}
	} while ((length=Recv(buffer, sizeof(buffer)-1))>0);
	return L"";
}


//==============================================================
// TCP ��������

// ������
CTCPServerSocket::CTCPServerSocket()
:	CTCPBaseSocket(INVALID_SOCKET)
{}

// ��Ʈ��ȣ ����
// �����ÿ��� true�� ��ȯ
bool CTCPServerSocket::Bind(int port) {
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(port);
	return bind(Socket, (sockaddr*)&sa, sizeof(sa))==0;
}

// ������ �޾Ƶ��̱�
// ���������� ���ӵǸ� CTCPSocket�� ���ο� �ν��Ͻ��� ��ȯ
CTCPSocket* CTCPServerSocket::Accept() {

	// ���� ���
	if (listen(Socket, SOMAXCONN)!=0) return NULL;

	// ���� �޾Ƶ��̱�
	SOCKET socket=accept(Socket, NULL, NULL);
	if (socket==INVALID_SOCKET) return NULL;

	// CTCPSocket�� ����
	return new CTCPSocket(socket);
}


