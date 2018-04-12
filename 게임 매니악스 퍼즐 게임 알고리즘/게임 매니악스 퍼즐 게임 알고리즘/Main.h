#ifndef MAINH
#define MAINH

#pragma warning(disable:4244 4786 4819 4996)

#include "..\LibGame\LibGame.h"
#include "..\LibUtil\LibUtil.h"
#include "Stage.h"

enum {
	TEX_VOID, TEX_COIN, TEX_ENEMY0, TEX_ENEMY1, TEX_FISH,
	TEX_FLOOR, TEX_ICE_FLOOR, TEX_ITEM, TEX_LIFT,
	TEX_MAN, TEX_SWIMMER, TEX_TREE, TEX_LOOP,
	TEX_STAMP_BOARD, TEX_ROPE, TEX_LADDER,
	TEX_TRAMPOLINE0, TEX_TRAMPOLINE1, TEX_TRAMPOLINE2,
	TEX_DROP_FLOOR, TEX_REVOLVING_DOOR,
	TEX_DOOR0, TEX_DOOR1, TEX_DOOR2, TEX_DOOR3,
	TEX_ELEVATOR0, TEX_ELEVATOR1,
	TEX_MOVING_FLOOR, TEX_CONVEYOR0, TEX_CONVEYOR1,
	TEX_AIR_CURRENT0, TEX_AIR_CURRENT1,
	TEX_WARP_GATE, TEX_ROCK, TEX_CLAY, TEX_OBJECT,
	TEX_ICE, TEX_HOLE, TEX_FOOTHOLD, TEX_PANEL,
	TEX_PILE, TEX_CRACK,
	TEX_SKATEBOARD, TEX_CAR, TEX_ANIMAL,
	TEX_SEESAW0, TEX_SEESAW1, TEX_CROUCH,
	TEX_SWORD, TEX_PEPPER, TEX_BOMB, TEX_LIGHT,
	TEX_SHURIKEN, TEX_FILL, TEX_BALL, TEX_FIRE, TEX_BLAST,
	TEX_MISSILE, TEX_BULLET, TEX_BOOMERANG, TEX_SHIELD,
	TEX_BUBBLE, TEX_MEDICINE, TEX_HEART, TEX_MUSHROOM, TEX_JEWEL,
	TEX_BALL0, TEX_BALL1, TEX_BALL2, TEX_BALL3, TEX_BALL4,
	TEX_BALL5, TEX_BALL6, TEX_BALL7, TEX_BALL8, TEX_BALL9,
	TEX_DRILLER, TEX_LOAD, TEX_CURSOR_FILL, TEX_CURSOR_LINE,
	TEX_RAIL0, TEX_RAIL1, TEX_RAIL2, TEX_RAIL3, TEX_RAIL4,
	TEX_RAIL5, TEX_RAIL6, TEX_RAIL7,
	TEX_MOLECULE0, TEX_MOLECULE1,
	TEX_KANA0, TEX_KANA1, TEX_KANA2, TEX_KANA3, TEX_KANA4,
	TEX_BLOCK0, TEX_BLOCK1,
	TEX_BROKEN_BLOCK0, TEX_BROKEN_BLOCK1,
	TEX_ORB, TEX_ORB0, TEX_ORB1, TEX_ORB2, TEX_ORB3, TEX_ORB4,
	TEX_BUILDING0, TEX_BUILDING1, TEX_BUILDING2, TEX_BUILDING3,
	TEX_DICE0, TEX_DICE1, TEX_DICE2, TEX_DICE3,
	TEX_DICE4, TEX_DICE5, TEX_DICE6,
	TEX_COUNT
};

enum {
	MESH_CUBE_WHITE, MESH_CUBE_GRAY,
	MESH_BALL_GRAY, MESH_BALL_BLACK,
	MESH_COUNT
};

//============================================================
//	���� ��ü Ŭ����
class CPuzzleGame : public CGame {
protected:
	bool Paused, PrevInput;
public:

	CPuzzleGame();
	~CPuzzleGame();
	//	����̽� �ʱ�ȭ, �̵�, �׸���
	virtual void OnResetDevice();
	virtual void Move();
	virtual void Draw();

	//	��Ʈ
	CFont *Font;
	
	//	Texture
	vector<CTexture*> Texture;

	//	Model
	vector<CMesh*> Mesh;

	//	Task ����Ʈ
	CTaskList *MoverList;
	void MoveTask(CTaskList *list);
	void DrawTask(CTaskList *list);

	//	��������
	vector<CStage*> Stage;
	int StageIndex;
	bool StageActive;
	void SetStage(int stage);

	//	����
	CRand Rand;
};

//=====================================================================
//	�۷ι� ����, ����

//	���� ��ü
extern CPuzzleGame *Game;

//	Color
#define COL_WHITE D3DCOLOR_ARGB(255, 255, 255, 255)
#define COL_LGRAY D3DCOLOR_ARGB(255, 192, 192, 192)
#define COL_MGRAY D3DCOLOR_ARGB(255, 128, 128, 128)
#define COL_DGRAY D3DCOLOR_ARGB(255, 64, 64, 64)
#define COL_BLACK D3DCOLOR_ARGB(255, 0, 0, 0)
#define COL_SHADE D3DCOLOR_ARGB(128, 0, 0, 0)

//	��ǥ
#define MAX_X 16
#define MAX_Y 12

#endif