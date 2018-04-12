#ifndef SHOWH
#define SHOWH

#include <DShow.h>
#include <string>
using namespace std;


//==============================================================
// ������� ���� ���
class CMedia {
private:

	// GraphBuilder(�̵�� �ҽ��� ���ڴ��� ����)
	// MediaControl(����� ����)
	// MediaEvent(��� ���� �̺�Ʈ ó��)
	// MediaSeeking(��� ��ġ�� ���� ����)
	IGraphBuilder* Graph;
	IMediaControl* Control;
	IMediaEvent* Event;
	IMediaSeeking* Seeking;
	IBasicAudio* Audio;

    void CleanUp();

public:

	// ������, �Ҹ���
	CMedia();
	~CMedia();
	
	// ���, �Ͻ�����, ����
	bool LoadFromFile(string file_name);
	void Play();
	void Pause();
	void Stop();

	// ���� (0.0 ~ 1.0)
	float GetVolume();
	void SetVolume(float vol);

	// �뷱�� (-1.0 ~ 1.0)
	float GetBalance();
	void SetBalance(float bal);

	// ����� �����Ͽ��°�?
	bool IsFinished();

	// ��� ���� ��ġ, ��ü�� ����(�ʴ���)
	float GetPos();
	float GetLength();
	void MovePos(float offset);
	void SetPos(float pos);
};


#endif

