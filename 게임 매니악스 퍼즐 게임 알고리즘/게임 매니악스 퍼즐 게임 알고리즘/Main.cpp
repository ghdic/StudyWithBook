#define STAGE_ORDER 1

#include "Main.h"
#include "Mover.h"

#include <string>
using namespace std;

// 게임 본체
CPuzzleGame* Game;

// Texture 파일명
static const wchar_t* TEX_NAME[] = {
	L"Void", L"Coin", L"Enemy0", L"Enemy1", L"Fish",
	L"Floor", L"IceFloor", L"Item", L"Lift",
	L"Man", L"Swimmer", L"Tree", L"Loop",
	L"StampBoard", L"Rope", L"Ladder",
	L"Trampoline0", L"Trampoline1", L"Trampoline2",
	L"DropFloor", L"RevolvingDoor",
	L"Door0", L"Door1", L"Door2", L"Door3",
	L"Elevator0", L"Elevator1",
	L"MovingFloor", L"Conveyor0", L"Conveyor1",
	L"AirCurrent0", L"AirCurrent1",
	L"WarpGate", L"Rock", L"Clay", L"Object",
	L"Ice", L"Hole", L"Foothold", L"Panel",
	L"Pile", L"Crack",
	L"Skateboard", L"Car", L"Animal",
	L"Seesaw0", L"Seesaw1", L"Crouch",
	L"Sword", L"Pepper", L"Bomb", L"Light",
	L"Shuriken", L"Fill", L"Ball", L"Fire", L"Blast",
	L"Missile", L"Bullet", L"Boomerang", L"Shield",
	L"Bubble", L"Medicine", L"Heart", L"Mushroom", L"Jewel",
	L"Ball0", L"Ball1", L"Ball2", L"Ball3", L"Ball4",
	L"Ball5", L"Ball6", L"Ball7", L"Ball8", L"Ball9",
	L"Driller", L"Load", L"CursorFill", L"CursorLine",
	L"Rail0", L"Rail1", L"Rail2", L"Rail3", L"Rail4",
	L"Rail5", L"Rail6", L"Rail7",
	L"Molecule0", L"Molecule1",
	L"Kana0", L"Kana1", L"Kana2", L"Kana3", L"Kana4",
	L"Block0", L"Block1",
	L"BrokenBlock0", L"BrokenBlock1",
	L"Orb", L"Orb0", L"Orb1", L"Orb2", L"Orb3", L"Orb4",
	L"Building0", L"Building1", L"Building2", L"Building3",
	L"Dice0", L"Dice1", L"Dice2", L"Dice3",
	L"Dice4", L"Dice5", L"Dice6",
};

// Mesh 파일명
static const wchar_t* MESH_NAME[] = {
	L"CubeWhite", L"CubeGray",
	L"BallGray", L"BallBlack",
};

// 생성자
CPuzzleGame::CPuzzleGame()
	: CGame(L"서적[게임 매니악스 : 퍼즐게임 알고리즘](마쯔우라 켄이치로/츠카사 유키 저) 예제 프로그램", true, false, true),
	Paused(false), PrevInput(false),
	StageIndex(0), StageActive(false)
{
	// 도움말, 종료, 커서에 관한 설정
	HelpURL = ExePath + L"..\\Help\\index.html";
	ConfirmExit = false;
	ShowCursor(TRUE);
	FPS = 60;
	PauseInTheBackground = true;
	DropFrames = false;

	// 폰트 초기화
	static const wchar_t fontchar[] = L"~|0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.(){}<>!?+-x*/='\\\"#$%&@[]";
	Font = new CFont(Graphics->GetDevice(), ExePath + L"..\\Font\\font.png", NULL, fontchar, 16.0f / 256, 32.0f / 256, 16, 32, 2, 2);

	// Texture 초기화
	LPDIRECT3DDEVICE9 device = Graphics->GetDevice();
	for (int i = 0; i<TEX_COUNT; i++) {
		wstring path = ExePath + L"..\\Texture\\" + TEX_NAME[i] + L".png";
		Texture.push_back(new CTexture(Graphics->GetDevice(), path, NULL));
	}

	// Mesh 초기화
	for (int i = 0; i<MESH_COUNT; i++) {
		wstring path = ExePath + L"..\\Mesh\\" + MESH_NAME[i] + L".x";
		Mesh.push_back(new CMesh(Graphics->GetDevice(), path, NULL));
	}

	// Task 리스트 초기화
	MoverList = new CTaskList(200, 1000);
}

// 소멸자
CPuzzleGame::~CPuzzleGame() {
	delete Font;
	for (size_t i = 0, n = Texture.size(); i<n; i++) delete Texture[i];
	for (size_t i = 0, n = Mesh.size(); i<n; i++) delete Mesh[i];
	delete MoverList;
	for (size_t i = 0, n = Stage.size(); i<n; i++) delete Stage[i];
}

// 스테이지 설정
void CPuzzleGame::SetStage(int stage) {
	MoverList->DeleteTask();
	StageIndex = (stage + Stage.size()) % Stage.size();
	Stage[StageIndex]->Init();
}

// 동작
void CPuzzleGame::Move() {

	// 입력
	Input->UpdateState();
	const CInputState* is = Input->GetState(0);

	// 일시 정지(스크린샷용)
	if (!PrevInput && is->Button[5]) Paused = !Paused;

	// 스테이지의 동작
	if (!Paused) {
		if (StageActive) {
			StageActive = Stage[StageIndex]->Move(is);
			for (CTaskIter i(MoverList); i.HasNext(); ) {
				CMover* mover = (CMover*)i.Next();
				if (!mover->Move(is)) i.Remove();
			}
			if (!PrevInput && is->Button[4]) StageActive = false;
		}
		else {
			if (!PrevInput) {
				if (is->Button[4]) {
					StageActive = true;
					SetStage(StageIndex);
				}
				else
					if (is->Left) SetStage(StageIndex - STAGE_ORDER); else
						if (is->Right) SetStage(StageIndex + STAGE_ORDER); else
							if (is->Up) SetStage(StageIndex - 10 * STAGE_ORDER); else
								if (is->Down) SetStage(StageIndex + 10 * STAGE_ORDER);
			}
		}
	}

	// 이전 입력
	PrevInput = is->Button[4] || is->Button[5];
	if (!StageActive) PrevInput = PrevInput || is->Left || is->Right || is->Up || is->Down;
}

// 그리기
void CPuzzleGame::Draw() {
	LPDIRECT3DDEVICE9 device = Graphics->GetDevice();
	Graphics->Clear(COL_WHITE);
	CStage* stage = Stage[StageIndex];
	stage->Draw();
	for (int order = 0, count = 0; count<MoverList->GetNumActiveTask(); order++) {
		for (CTaskIter i(MoverList); i.HasNext(); ) {
			CMover* mover = (CMover*)i.Next();
			if (order == mover->DrawOrder) {
				mover->Draw();
				count++;
			}
		}
	}
	Font->DrawText(wstring(L"\"") + stage->Name + L"\"", 0, 0, COL_BLACK);
	if (!StageActive) {
		Font->DrawText(L"PUSH BUTTON4 OR [B] TO START", 0, 32, COL_BLACK);
	}
}

// 디바이스 복귀
void CPuzzleGame::OnResetDevice() {
	LPDIRECT3DDEVICE9 device = Graphics->GetDevice();

	// Alpha blending의 설정
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 렌더링 설정
	device->SetRenderState(D3DRS_DITHERENABLE, FALSE);
	device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_AMBIENT, 0x00FFFFFF);
	device->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
	device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	// Texture 처리방법의 설정
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// World matrix
	D3DXMATRIX mat_world;
	D3DXMatrixIdentity(&mat_world);
	device->SetTransform(D3DTS_WORLD, &mat_world);

	// View matrix
	D3DXMATRIX mat_view;
	D3DXVECTOR3 vec_from = D3DXVECTOR3(0, 0, -10);
	D3DXVECTOR3 vec_lookat = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 vec_up = D3DXVECTOR3(0, 1, 0);
	D3DXMatrixLookAtLH(&mat_view, &vec_from, &vec_lookat, &vec_up);
	device->SetTransform(D3DTS_VIEW, &mat_view);

	// Projection matrix
	D3DXMATRIX mat_proj;
	D3DXMatrixPerspectiveLH(&mat_proj, 1, (float)Graphics->GetHeight() / Graphics->GetWidth(), 1, 100);
	device->SetTransform(D3DTS_PROJECTION, &mat_proj);
}

// Main 루틴
INT WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, INT) {
	Game = new CPuzzleGame();
	MakeStage();
	Game->SetStage((STAGE_ORDER - 1) / 2);
	Game->Run();
	delete Game;
	return 0;
}

