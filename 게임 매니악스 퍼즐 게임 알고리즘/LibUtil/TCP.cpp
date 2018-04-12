#include "LibUtil.h"


//==============================================================
// Winsock 초기화와 종료

// 사용중인 소켓 수
static int SocketCount=0;

// Winsock 초기화
static void InitializeWinsock() {
	if (SocketCount==0) {
		WSADATA data;
		WSAStartup(MAKEWORD(2,0), &data);
	}
	SocketCount++;
}

// Winsock 종료
static void FinalizeWinsock() {
	SocketCount--;
	if (SocketCount==0) {
		WSACleanup();
	}
}


//==============================================================
// 소켓의 기본 기능

// 생성자
CTCPBaseSocket::CTCPBaseSocket(SOCKET s)
:	Socket(s)
{
	InitializeWinsock();
	if (Socket==INVALID_SOCKET) {
		Socket=socket(AF_INET, SOCK_STREAM, 0);
	}
}

// 소멸자
CTCPBaseSocket::~CTCPBaseSocket() {
	closesocket(Socket);
	FinalizeWinsock();
}


//==============================================================
// TCP 소켓

// 생성자
CTCPSocket::CTCPSocket(SOCKET s)
:	CTCPBaseSocket(s)
{
	LineSeparator.push_back(L"\r\n");
	LineSeparator.push_back(L"\r");
	LineSeparator.push_back(L"\n");
}

// 접속 : 
// 성공시에는 true를 반환
bool CTCPSocket::Connect(wstring host, int port) {

	// 호스트명을 어드레스로 변환
	LPHOSTENT hostent=gethostbyname(WideToMulti(host, SJIS).c_str());
	if (!hostent ||
		hostent->h_addrtype!=AF_INET ||
		hostent->h_length!=4 ||
		!hostent->h_addr_list[0]) return false;

	// 어드레스와 포트 등록
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_addr.s_addr=*(unsigned long*)(hostent->h_addr_list[0]);
	sa.sin_port=htons(port);

	// 접속 실행
	return connect(Socket, (sockaddr*)&sa, sizeof(sa))==0;
}

// 버퍼 송신
int CTCPSocket::Send(const char* buffer, int length) {
	return send(Socket, buffer, length, 0);
}

// 버퍼 수신
int CTCPSocket::Recv(char* buffer, int length) {
	return recv(Socket, buffer, length, 0);
}

// Line separator 설정
void CTCPSocket::SetLineSeparator(vector<wstring> ls) {
	LineSeparator.clear();
	for (size_t i=0, n=ls.size(); i<n; i++) {
		LineSeparator.push_back(ls[i]);
	}
}

// Line 송신
void CTCPSocket::SendLine(wstring s) {
	if (!LineSeparator.empty()) s+=LineSeparator[0];
	Send(WideToMulti(s, SJIS).c_str(), s.length());
}

// Line 수신
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
// TCP 서버소켓

// 생성자
CTCPServerSocket::CTCPServerSocket()
:	CTCPBaseSocket(INVALID_SOCKET)
{}

// 포트번호 설정
// 성공시에는 true를 반환
bool CTCPServerSocket::Bind(int port) {
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(port);
	return bind(Socket, (sockaddr*)&sa, sizeof(sa))==0;
}

// 접속을 받아들이기
// 정상적으로 접속되면 CTCPSocket의 새로운 인스턴스를 반환
CTCPSocket* CTCPServerSocket::Accept() {

	// 접속 대기
	if (listen(Socket, SOMAXCONN)!=0) return NULL;

	// 접속 받아들이기
	SOCKET socket=accept(Socket, NULL, NULL);
	if (socket==INVALID_SOCKET) return NULL;

	// CTCPSocket의 생성
	return new CTCPSocket(socket);
}


