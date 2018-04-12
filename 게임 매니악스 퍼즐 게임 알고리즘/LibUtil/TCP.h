#ifndef TCPH
#define TCPH

#include <winsock.h>
#include <string>
#include <vector>
using namespace std;


//==============================================================
// TCP ������ �⺻ ���
class CTCPBaseSocket {
protected:
	SOCKET Socket;
public:
	CTCPBaseSocket(SOCKET s);
	virtual ~CTCPBaseSocket();
};


//==============================================================
// TCP ����
class CTCPSocket : CTCPBaseSocket {
protected:
	wstring LineBuffer;
	vector<wstring> LineSeparator;
public:
	// ������
	CTCPSocket(SOCKET s=INVALID_SOCKET);

	// ����
	bool Connect(wstring host, int port);

	// ������ �ۼ���
	int Send(const char* buffer, int length);
	int Recv(char* buffer, int length);

	// Line �ۼ���
	void SetLineSeparator(vector<wstring> ls);
	void SendLine(wstring s);
	wstring RecvLine();
};


//==============================================================
// TCP ��������
class CTCPServerSocket : CTCPBaseSocket {
public:
	// ������
	CTCPServerSocket();

	// ��Ʈ��ȣ ����, ���� �޾Ƶ��̱�
	bool Bind(int port);
	CTCPSocket* Accept();
};


#endif

