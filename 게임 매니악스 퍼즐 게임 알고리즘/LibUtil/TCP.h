#ifndef TCPH
#define TCPH

#include <winsock.h>
#include <string>
#include <vector>
using namespace std;


//==============================================================
// TCP 소켓의 기본 기능
class CTCPBaseSocket {
protected:
	SOCKET Socket;
public:
	CTCPBaseSocket(SOCKET s);
	virtual ~CTCPBaseSocket();
};


//==============================================================
// TCP 소켓
class CTCPSocket : CTCPBaseSocket {
protected:
	wstring LineBuffer;
	vector<wstring> LineSeparator;
public:
	// 생성자
	CTCPSocket(SOCKET s=INVALID_SOCKET);

	// 접속
	bool Connect(wstring host, int port);

	// 버퍼의 송수신
	int Send(const char* buffer, int length);
	int Recv(char* buffer, int length);

	// Line 송수신
	void SetLineSeparator(vector<wstring> ls);
	void SendLine(wstring s);
	wstring RecvLine();
};


//==============================================================
// TCP 서버소켓
class CTCPServerSocket : CTCPBaseSocket {
public:
	// 생성자
	CTCPServerSocket();

	// 포트번호 설정, 접속 받아들이기
	bool Bind(int port);
	CTCPSocket* Accept();
};


#endif

