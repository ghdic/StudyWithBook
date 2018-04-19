//==============================================================
// 블록 떨어뜨리기

#define DROPPING_BLOCK_CELL_SIZE 4
#define DROPPING_BLOCK_PATTERN_COUNT 7
#define DROPPING_BLOCK_TURN_COUNT 4
#define DROPPING_BLOCK_FIELD_LEFT 1
#define DROPPING_BLOCK_FIELD_RIGHT 11
#define DROPPING_BLOCK_FIELD_TOP 1
#define DROPPING_BLOCK_FIELD_BOTTOM 11
#define DROPPING_BLOCK_NEXT_LEFT 12

// 블록의 패턴
// 블록의 형태마다 4개의 패턴을 준비한다.
char* DroppingBlockPattern[DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT] = {
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	"####"
	"    "
	"    "
	,
	(char*)
	" #  "
	" #  "
	" #  "
	" #  "
	,
	(char*)
	"    "
	"####"
	"    "
	"    "
	,
	(char*)
	" #  "
	" #  "
	" #  "
	" #  "
	,
	(char*)
	"    "
	"##  "
	" ## "
	"    "
	,
	(char*)
	"  # "
	" ## "
	" #  "
	"    "
	,
	(char*)
	"    "
	"##  "
	" ## "
	"    "
	,
	(char*)
	"  # "
	" ## "
	" #  "
	"    "
	,
	(char*)
	"    "
	" ## "
	"##  "
	"    "
	,
	(char*)
	" #  "
	" ## "
	"  # "
	"    "
	,
	(char*)
	"    "
	" ## "
	"##  "
	"    "
	,
	(char*)
	" #  "
	" ## "
	"  # "
	"    "
	,
	(char*)
	"  # "
	"### "
	"    "
	"    "
	,
	(char*)
	" #  "
	" #  "
	" ## "
	"    "
	,
	(char*)
	"    "
	"### "
	"#   "
	"    "
	,
	(char*)
	"##  "
	" #  "
	" #  "
	"    "
	,
	(char*)
	"#   "
	"### "
	"    "
	"    "
	,
	(char*)
	" ## "
	" #  "
	" #  "
	"    "
	,
	(char*)
	"    "
	"### "
	"  # "
	"    "
	,
	(char*)
	" #  "
	" #  "
	"##  "
	"    "
	,
	(char*)
	" #  "
	"### "
	"    "
	"    "
	,
	(char*)
	" #  "
	" ## "
	" #  "
	"    "
	,
	(char*)
	"    "
	"### "
	" #  "
	"    "
	,
	(char*)
	" #  "
	"##  "
	" #  "
	"    "
};

// 블록
class CDroppingBlockBlock : public CMover {
public:
	CCell * StageCell;
	CCell** BlockCell;
	CCell** CurrentBlock;
	CCell** NextBlock;
	int CX, CY, VX, VY;
	int State, Time, DropTime, Turn;
	bool PrevButton, PrevDown;

	//블록의 초기화 처리
	void Init() {
		//결정해 놓은 다음 블록의 현재의 블록으로 설정한다.
		CurrentBlock = NextBlock;
		//난수를 사용하여 다음 블록을 결정한다.
		NextBlock = BlockCell + Rand.Int31() % DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT;
		//현재 블록의 셀 좌표를 설정한다.
		CX = (DROPPING_BLOCK_FIELD_LEFT + DROPPING_BLOCK_FIELD_RIGHT - DROPPING_BLOCK_CELL_SIZE) / 2;
		CY = DROPPING_BLOCK_FIELD_TOP;
		//그림좌표를 설정한다.
		X = CX;
		Y = CY;
		//상태, 낙하 타이머, 회전각도를 설정한다.
		State = 0;
		DropTime = 0;
		Turn = 0;

		//현재의 블록이 나타날 위치에
		//다른 블록이 있는 경우에는
		//게임을 종료한다.
		if (StageCell->Hit(CX, CY, CurrentBlock[Turn])) {
			State = 99999;
		}
		PrevDown = true;
	}
	
	CDroppingBlockBlock(CCell* stage_cell, CCell** block_cell)
		: StageCell(stage_cell), BlockCell(block_cell), PrevButton(true)
	{
		Texture = Game->Texture[TEX_OBJECT];
		NextBlock = BlockCell;
		Init();
		Init();
	}

	//블록의 이동 처리
	virtual bool Move(const CInputState* is) {
		//입력 상태의 처리
		if (State == 0) {
			//낙하 타이머를 갱신한다.
			DropTime++;
			//레버를 아래로 입력하거나
			//낙하 타이머가 일정값에 도달하면 블록을 낙하 시킨다.
			if ((is->Down && !PrevDown) || DropTime == 60) {
				//셀 좌표를 갱신한 경우
				//블록이 스테이지의 벽이나 다른 블록에
				//접촉하는지 검사한다.
				if (StageCell->Hit(CX, CY + 1, CurrentBlock[Turn])) {
					//접촉하는 경우에는
					//블록 셀을 스테이지 셀에 합성한다.
					StageCell->Merge(CX, CY, CurrentBlock[Turn]);
					//착지상태로 전환한다.
					State = 2;
				}
				else {
					//접촉하지 않는 경우에는
					//낙하 타이머나 속도를 설정하고
					//셀 좌표를 갱신한다.
					DropTime = 0;
					CY++;
					VX = 0;
					VY = 1;
					//이동 상태로 전환한다.
					Time = 0;
					State = 1;
				}
			}
			else
				//레버를 왼쪽으로 입력한 경우
				//블록의 이동 방향의 스테이지와 접촉하지 않으면
				//블록을 왼쪽으로 이동시킨다.
				//오른쪽도 이하 동문
				if (is->Left && !StageCell->Hit(CX - 1, CY, CurrentBlock[Turn])) {
					CX--;
					VX = -1;
					VY = 0;
					Time = 0;
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CurrentBlock[Turn])) {
						CX++;
						VX = 1;
						VY = 0;
						Time = 0;
						State = 1;
					}
					else
						//버튼이 입력되고, 직전의 버튼 상태가 눌리지 않은 상태이며,
						//회전 후에 블록과 스테이지가 접촉하지 않으면 블록을 회전시킨다.
						if (is->Button[0] && !PrevButton && !StageCell->Hit(CX, CY, CurrentBlock[(Turn + 1) % DROPPING_BLOCK_TURN_COUNT])) {
							//블록을 다음의 패턴으로 변화시킨다.
							//마지막 패턴의 다음에는 다시 첫 번째 패턴으로 돌아간다.
							Turn = (Turn + 1) % DROPPING_BLOCK_TURN_COUNT;
						}
			//직전에 버튼을 눌렀는지 아닌지 저장해둔다.
			PrevButton = is->Button[0];
			//레버가 계속 아래로 입력되고 있는 경우에
			//블록이 계속해서 떨어지지 않도록 하기 위한 처리
			if (!is->Down) PrevDown = false;
		}
		//이동 상태의 처리
		if (State == 1) {
			//타이머를 갱신한다.
			//여기에서는 타이머를 증가시키고 있지만
			//감소시키는 처리를 해도 좋다.
			Time++;
			//타이머를 사용해 그림좌표를 조금씩 갱신한다.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			//타이머가 일정 값에 도달하면 입력상태로 전환한다.
			if (Time == 10) {
				State = 0;
			}
		}
		//착지상태의 처리
		if (State == 2) {
			//재출현상태로 전환한다.
			//사라질 블록이 있는 경우에는
			//삭제상태로 전환한다.
			State = 4;
			//블록이 빈틈없이 채워진 단을 찾는다.
			for (int y = DROPPING_BLOCK_FIELD_TOP; y<DROPPING_BLOCK_FIELD_BOTTOM; y++) {
				int x;
				for (x = DROPPING_BLOCK_FIELD_LEFT; x<DROPPING_BLOCK_FIELD_RIGHT; x++) {
					//공백인 셀이 있는 경우에는 루프를 탈출한다.
					if (StageCell->Get(x, y) == ' ') break;
				}
				//블록이 채워진 단이 있는 경우의 처리
				if (x >= DROPPING_BLOCK_FIELD_RIGHT) {
					for (x = DROPPING_BLOCK_FIELD_LEFT; x<DROPPING_BLOCK_FIELD_RIGHT; x++) {
						//채워진 단의 블록을 사라질 블록(+)으로 변경한다.
						StageCell->Set(x, y, '+');
					}
					//삭제상태로 전환한다.
					Time = 0;
					State = 3;
				}
			}
		}
		//삭제 상태의 처리
		if (State == 3) {
			//타이머를 갱신한다.
			Time++;
			//타이머가 일정 값에 도달하면 블록을 없앤다.
			if (Time == 20) {
				//사라질 블록(+)이 있는 단을 찾는다.
				for (int i = DROPPING_BLOCK_FIELD_TOP; i<DROPPING_BLOCK_FIELD_BOTTOM; i++) {
					//사라질 블록을 찾은 경우의 처리
					if (StageCell->Get(DROPPING_BLOCK_FIELD_LEFT, i) == '+') {
						//사라질 단의 위에 있는 모든 단을
						//한 단씩 아래로 이동한다.
						for (int y = i; y >= DROPPING_BLOCK_FIELD_TOP; y--) {
							for (int x = DROPPING_BLOCK_FIELD_LEFT; x<DROPPING_BLOCK_FIELD_RIGHT; x++) {
								StageCell->Set(x, y, StageCell->Get(x, y - 1));
							}
						}
					}
				}
				//재출현상태로 전환한다.
				State = 4;
			}
		}
		//재출현 상태의 처리
		if (State == 4) {
			//새로운 블록을 출현시킨다.
			Init();
		}
		return true;
	}
	//블록의 그림 처리
	virtual void Draw() {
		//화면 해상도를 얻는다.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		//낙하 예상위치를 계산한다.
		//블록이 바닥이나 다른 블록과 접촉할 때까지
		//Y좌표를 증가시킨다.
		int ey;
		for (ey = CY; ey<DROPPING_BLOCK_FIELD_BOTTOM; ey++) {
			//접촉하는 위치가 낙하 예상 위치가 된다.
			if (StageCell->Hit(CX, ey + 1, CurrentBlock[Turn])) break;
		}
		//블록 그리기
		for (int y = 0; y<DROPPING_BLOCK_CELL_SIZE; y++) {
			for (int x = 0; x<DROPPING_BLOCK_CELL_SIZE; x++) {
				if (State<2 && CurrentBlock[Turn]->Get(x, y) == '#') {
					//낙하 예상위치에 회색으로 블록을 그린다.
					Texture->Draw(
						(CX + x)*sw, (ey + y)*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY
					);
					//현재 위치에 검은색으로 블록을 그린다.
					Texture->Draw(
						(X + x)*sw, (Y + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				//다음에 나타날 블록을 그린다.
				if (NextBlock[0]->Get(x, y) == '#') {
					Texture->Draw(
						(DROPPING_BLOCK_NEXT_LEFT + x)*sw, (1 + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
			}
		}
		//타이머 값에 따라 색을 결정한다.
		float f = Time * 0.05f;
		//스테이지의 모든 셀에 대해서 처리한다.
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				switch (StageCell->Get(x, y)) {
				case '#':
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
					break;
				case '+':
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1)
					);
					break;
				case '=':
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
					break;
				}
			}
		}
	}
};

// 스테이지
class CDroppingBlockStage : public CStage {
	CCell* StageCell;
	CCell* BlockCell[DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT];
public:
	CDroppingBlockStage() : CStage(L"DROPPING BLOCK") {
		StageCell = new CCell();
		for (int i = 0; i<DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT; i++) {
			BlockCell[i] = new CCell(DROPPING_BLOCK_CELL_SIZE, DROPPING_BLOCK_CELL_SIZE);
			BlockCell[i]->Init(DroppingBlockPattern[i]);
		}
	}
	virtual ~CDroppingBlockStage() {
		delete StageCell;
		for (int i = 0; i<DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT; i++) {
			delete BlockCell[i];
		}
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"================"
		);
		new CDroppingBlockBlock(StageCell, BlockCell);
	}
};


//==============================================================
// 보석 떨어뜨리기

#define DROPPING_JEWEL_CELL_SIZE 3
#define DROPPING_JEWEL_FIELD_LEFT 1
#define DROPPING_JEWEL_FIELD_RIGHT 11
#define DROPPING_JEWEL_FIELD_TOP 1
#define DROPPING_JEWEL_FIELD_BOTTOM 11
#define DROPPING_JEWEL_NEXT_LEFT 13
#define DROPPING_JEWEL_COLOR_COUNT 5
#define DROPPING_JEWEL_ERASING_COUNT 3

// 보석
class CDroppingJewelJewel : public CMover {
public:
	CCell * StageCell;
	CCell* CurrentJewel;
	CCell* NextJewel;
	int CX, CY, VX, VY;
	int State, Time, DropTime;
	bool PrevButton, PrevDown;

	void Init() {
		for (int y = 0; y<DROPPING_JEWEL_CELL_SIZE; y++) {
			CurrentJewel->Set(0, y, NextJewel->Get(0, y));
			NextJewel->Set(0, y, '0' + Rand.Int31() % DROPPING_JEWEL_COLOR_COUNT);
		}
		CX = (DROPPING_JEWEL_FIELD_LEFT + DROPPING_JEWEL_FIELD_RIGHT - DROPPING_JEWEL_CELL_SIZE) / 2;
		CY = DROPPING_JEWEL_FIELD_TOP;
		X = CX;
		Y = CY;
		State = 0;
		DropTime = 0;
		if (StageCell->Hit(CX, CY, CurrentJewel)) {
			State = 99999;
		}
		PrevDown = true;
	}

	CDroppingJewelJewel(CCell* stage_cell, CCell* current_jewel, CCell* next_jewel)
		: StageCell(stage_cell), CurrentJewel(current_jewel), NextJewel(next_jewel), PrevButton(true)
	{
		Texture = Game->Texture[TEX_BALL0];
		Init();
		Init();
	}
	//보석의 이동 처리
	virtual bool Move(const CInputState* is) {
		//입력상태
		if (State == 0) {
			//낙하 타이머를 갱신한다.
			DropTime++;

			//레버를 아래로 입력하거나
			//낙하 타이머가 일정 값에 도달하면
			//보석을 낙하시킨다.
			if ((is->Down && !PrevDown) || DropTime == 60) {
				if (StageCell->Hit(CX, CY + 1, CurrentJewel)) {
					StageCell->Merge(CX, CY, CurrentJewel);
					State = 2;
				}
				else {
					DropTime = 0;
					CY++;
					VX = 0;
					VY = 1;
					Time = 0;
					State = 1;
				}
			}
			else
				if (is->Left && !StageCell->Hit(CX - 1, CY, CurrentJewel)) {
					CX--;
					VX = -1;
					VY = 0;
					Time = 0;
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CurrentJewel)) {
						CX++;
						VX = 1;
						VY = 0;
						Time = 0;
						State = 1;
					}
			//버튼을 입력할 경우
			//직전의 버튼 상태가 눌리지 않은 상태이면 보석의 순서를 바꾼다.
					else
						if (is->Button[0] && !PrevButton) {
							//가장 위쪽과 가운데 보석을 바꿔 넣는다.
							CurrentJewel->Swap(0, 0, 0, 1);
							//가장 위쪽과 가장 아래쪽 보석을 바꿔 넣는다.
							CurrentJewel->Swap(0, 0, 0, 2);
						}
			//직전의 버튼 입력상태를 저장한다.
			PrevButton = is->Button[0];
			//레버가 계속 아래로 입력되고 있을 때, 보석이 계속해서 떨어지지 않도록 처리한다.
			if (!is->Down) PrevDown = false;
		}
		//이동 상태
		if (State == 1) {
			Time++;
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			if (Time == 10) {
				State = 0;
			}
		}
		//착지상태
		if (State == 2) {
			//보석을 없애지 못한 경우에는
			//재출현 상태로 전환한다.
			State = 4;
			//스테이지 안의 모든 셀에 대하여 검사한다.
			for (int y = DROPPING_JEWEL_FIELD_TOP; y<DROPPING_JEWEL_FIELD_BOTTOM; y++) {
				for (int x = DROPPING_JEWEL_FIELD_LEFT; x<DROPPING_JEWEL_FIELD_RIGHT; x++) {
					//셀을 취득한다
					//셀의 종류를 비교하기 위해
					//사라질 보석의 표시(최상위 비트)를 클리어한다.
					char c = StageCell->Get(x, y) & 0x7f;
					//셀이 공백인 경우에는 다음 셀로 넘어간다.
					if (c == ' ') continue;
					//보석이 나열된 방향
					static const int
						vx[] = { 0, 1, -1, 1 },
						vy[] = { 1, 0, 1, 1 };
					//가로, 세로, 대각선(4방향)에 대해
					//보석이 나란히 이어져 있는짖 검사한다.
					for (int v = 0; v<4; v++) {
						//이어져 있는 보석의 개수
						int count = 0;
						//한 방향으로
						//같은 종류의 보석 셀이 몇개 이어져 있는지 검사한다.
						for (
							int i = x, j = y;
							DROPPING_JEWEL_FIELD_LEFT <= i &&
							i<DROPPING_JEWEL_FIELD_RIGHT &&
							DROPPING_JEWEL_FIELD_TOP <= j &&
							j<DROPPING_JEWEL_FIELD_BOTTOM;
							i += vx[v], j += vy[v], count++
							) {
							//같은 종류의 보석이 이어져 있는 한 계속해서 루프를 수행한다.
							if (c != (StageCell->Get(i, j) & 0x7f)) break;
						}
						//이어져 있는 보석의 개수가 정해진 개수(3개) 이상이라면
						//사라질 보석의 표시(최상위 비트)를 세트한다.
						if (count >= DROPPING_JEWEL_ERASING_COUNT) {
							for (int i = x, j = y, k = 0; k<count; i += vx[v], j += vy[v], k++) {
								StageCell->Set(i, j, StageCell->Get(i, j) | 0x80);
							}
							Time = 0;
							//삭제상태로 전환한다.
							State = 3;
						}
					}
				}
			}
		}
		//삭제상태
		if (State == 3) {
			//타이머를 갱신한다.
			Time++;

			//타이머가 일정 값에 도달하면
			//표시된 보석을 삭제한다.
			if (Time == 20) {
				//스테이지 안의 모든 셀에 대하여 처리한다.
				for (int x = DROPPING_JEWEL_FIELD_LEFT; x<DROPPING_JEWEL_FIELD_RIGHT; x++) {
					for (int y = DROPPING_JEWEL_FIELD_TOP; y<DROPPING_JEWEL_FIELD_BOTTOM; y++) {
						//사라질 보석을 찾으면 삭제하고
						//그 보석의 위에 있는 모든 보석을
						//1단씩 아래로 이동한다.
						if (StageCell->Get(x, y) & 0x80) {
							for (int i = y; i >= DROPPING_JEWEL_FIELD_TOP; i--) {
								StageCell->Set(x, i, StageCell->Get(x, i - 1));
							}
						}
					}
				}
				//보석이 사라지면서
				//새로운 보석이 다시 이어질 가능성이 있기 때문에
				//착지상태로 전환한다.
				State = 2;
			}
		}
		//재출현 상태
		if (State == 4) {
			Init();
		}
		return true;
	}

	//보석의 그림 처리;
	virtual void Draw() {
		//화면의 해상도를 얻는다.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<DROPPING_JEWEL_CELL_SIZE; y++) {
			if (State<2) {
				Game->Texture[TEX_BALL0 + (CurrentJewel->Get(0, y) - '0')]->Draw(
					X*sw, (Y + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
				);
			}
			Game->Texture[TEX_BALL0 + (NextJewel->Get(0, y) - '0')]->Draw(
				DROPPING_JEWEL_NEXT_LEFT*sw, (DROPPING_JEWEL_FIELD_TOP + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
			);
		}
		//스테이지의 모든 셀에 대하여 처리한다.
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				//셀의 종류를 얻는다.
				char c = StageCell->Get(x, y);
				//사라질 보석
				if (c & 0x80) {
					//타이머에 따라 색을 결정한다.
					float f = Time * 0.05f;
					//시간 결과에 따라 조금씩 흐려지게 그린다.
					Game->Texture[TEX_BALL0 + ((c & 0x7f) - '0')]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1)
					);
				}
				else
					//보통의 보석
					if ('0' <= c && c<'0' + DROPPING_JEWEL_COLOR_COUNT) {
						Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
						);
					}
					else
						//벽
						if (c == '=') {
							Game->Texture[TEX_DROP_FLOOR]->Draw(
								x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
							);
						}
			}
		}
	}
};

// 스테이지
class CDroppingJewelStage : public CStage {
	CCell* StageCell;
	CCell* JewelCell[2];
public:
	CDroppingJewelStage() : CStage(L"DROPPING JEWEL") {
		StageCell = new CCell();
		for (int i = 0; i<2; i++) {
			JewelCell[i] = new CCell(1, DROPPING_JEWEL_CELL_SIZE);
		}
	}
	virtual ~CDroppingJewelStage() {
		delete StageCell;
		for (int i = 0; i<2; i++) {
			delete JewelCell[i];
		}
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"================"
		);
		new CDroppingJewelJewel(StageCell, JewelCell[0], JewelCell[1]);
	}
};

//==============================================================
// 볼 떨어뜨리기
//플레이어2 J K L ;
//플레이어1 화살표 스페이스바

#define DROPPING_BALL_CELL_SIZE 3
#define DROPPING_BALL_COLOR_COUNT 5
#define DROPPING_BALL_FIELD_TOP 1
#define DROPPING_BALL_FIELD_BOTTOM 11
#define DROPPING_BALL_TURN_COUNT 4

// 볼
class CDroppingBallBall : public CMover {
public:
	CCell * StageCell;
	CCell* CurrentBall;
	CCell* NextBall;
	int PlayerID, ErasingCount;
	int FieldLeft, FieldRight, NextLeft;
	int CX, CY, VX, VY;
	int State, Time, DropTime, Turn, ChainCount, AttackCount;
	bool PrevButton, PrevDown;
	CDroppingBallBall* Opponent;

	void Init() {
		CurrentBall->Set(0, 0, NextBall);
		NextBall->Clear();
		MakeNextBall();
		CX = (FieldLeft + FieldRight - DROPPING_BALL_CELL_SIZE) / 2;
		CY = DROPPING_BALL_FIELD_TOP;
		X = CX;
		Y = CY;
		State = 0;
		DropTime = 0;
		Turn = 0;
		if (StageCell->Hit(CX, CY, CurrentBall)) {
			State = 99999;
		}
		PrevDown = true;
	}

	virtual void MakeNextBall() {
		NextBall->Set(1, 0, '0' + Rand.Int31() % DROPPING_BALL_COLOR_COUNT);
		NextBall->Set(1, 1, '0' + Rand.Int31() % DROPPING_BALL_COLOR_COUNT);
	}

	CDroppingBallBall(
		CCell* stage_cell, CCell* current_ball, CCell* next_ball,
		int player_id, int Erasing_count,
		int field_left, int field_right, int next_left)
		: StageCell(stage_cell), CurrentBall(current_ball), NextBall(next_ball),
		PlayerID(player_id), ErasingCount(Erasing_count),
		FieldLeft(field_left), FieldRight(field_right), NextLeft(next_left),
		AttackCount(0), PrevButton(true)
	{
		Texture = Game->Texture[TEX_BALL0];
		NextBall->Clear();
		Init();
		Init();
	}
	//볼의 이동 처리
	virtual bool Move(const CInputState*) {
		//2인 플레이어에 대응하기 위하여
		//볼을 조작하고 있는 플레이어의 입력을 얻는다.
		const CInputState* is = Game->GetInput()->GetState(PlayerID);
		//입력상태
		if (State == 0) {
			//낙하 타이머를 갱신한다.
			DropTime++;
			//레버를 아래로 입력하거나
			//낙하 타이머가 일정 값에 도달하면
			//볼을 낙하 시킨다.
			if ((is->Down && !PrevDown) || DropTime == 120) {
				//스테이지에 접촉하면 볼을 착지시킨다.
				if (StageCell->Hit(CX, CY + 1, CurrentBall)) {
					//스테이지 셀에 볼의 셀을 합성한다.
					StageCell->Merge(CX, CY, CurrentBall);
					//연속 카운트(연속해서 없앤 횟수)를 초기화한다.
					ChainCount = 0;
					//낙하판정 상태로 전환한다.
					State = 2;
				}
				else {
					//스테이지에 접촉하지 않은 경우에는
					//볼을 1단아래로 낙하시킨다.
					DropTime = 0;
					//셀 좌표를 갱신한다.
					CY++;
					//속도, 타이머를 설정한다.
					VX = 0;
					VY = 1;
					Time = 0;
					//이동상태로 전환한다.
					State = 1;
				}
			}
			else
				//레버를 왼쪽으로 입력한 경우
				//스테이지에 접촉하지 않으면
				//왼쪽으로 이동한다.
				if (is->Left && !StageCell->Hit(CX - 1, CY, CurrentBall)) {
					//셀 좌표를 갱신한다.
					CX--;
					//속도, 타이머를 설정한다.
					VX = -1;
					VY = 0;
					Time = 0;
					//이동상태로 전환한다.
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CurrentBall)) {
						CX++;
						VX = 1;
						VY = 0;
						Time = 0;
						State = 1;
					}
					else
						//버튼이 입력되고
						//직전에 버튼이 눌리지 않은 상태라면 볼을 회전시킨다.
						if (is->Button[0] && !PrevButton) {
							//셀 안에서 볼이 위로 옮겨져 있는 경우에는
							//회전하기 전에 볼을 아래로 되돌린다.
							if (Turn == 2 && CurrentBall->Get(1, 2) == ' ') {
								//아래쪽과 가운데의 셀을 바꾸어 넣는다.
								CurrentBall->Swap(1, 2, 1, 1);
								//가운데외 위쪽 셀을 바꾸어 넣는다.
								CurrentBall->Swap(1, 1, 1, 0);
							}
							//회전각도(0-3)을 갱신한다.
							Turn = (Turn + 1) % DROPPING_BALL_TURN_COUNT;
							//볼을 회전 시킨다.
							//위와 왼쪽, 왼쪽과 아래, 아래와 오른쪽의 순서로 셀을 바꾸어 넣는다.
							CurrentBall->Swap(1, 0, 0, 1);
							CurrentBall->Swap(0, 1, 1, 2);
							CurrentBall->Swap(1, 2, 2, 1);
							//회전 후의 볼이 스테이지에 접촉한 경우의 처리
							if (StageCell->Hit(CX, CY, CurrentBall)) {
								//오른쪽에 장애물이 있고 왼쪽이 비어있는 경우에는 왼쪽으로 피한다.
								if (Turn == 1 && !StageCell->Hit(CX - 1, CY, CurrentBall)) {
									CX--;
									X = CX;
								}
								//왼쪽에 장애물이 있고 오른쪽이 비어있는 경우에는 오른쪽으로 피한다.
								else
									if (Turn == 3 && !StageCell->Hit(CX + 1, CY, CurrentBall)) {
										CX++;
										X = CX;
									}
									else
										//아래에 장애물이 있고, 위쪽이 비어있는 경우에는 셀 안에서 볼을 위로 옮긴다.
										if (Turn == 2 && !StageCell->Hit(CX, CY - 1, CurrentBall)) {
											//가운데와 위쪽의 셀을 바꾸어 넣는다.
											CurrentBall->Swap(1, 1, 1, 0);
											//아래쪽과 가운데의 셀을 바꾸어 넣는다.
											CurrentBall->Swap(1, 2, 1, 1);
										}
										else {
											//장애물을 피할 수 없는 경우에는 회전을 취소한다.
											//회전각도를 원래대로 되돌린다.
											Turn = (Turn - 1 + DROPPING_BALL_TURN_COUNT) % DROPPING_BALL_TURN_COUNT;
											//볼의 회전을 취소한다
											//처음에 회전시킬 때와 반대로
											//아래와 오른쪽, 왼쪽과 아래쪽, 위쪽과 왼쪽 순서로 셀을 바꿔 넣는다.
											CurrentBall->Swap(1, 2, 2, 1);
											CurrentBall->Swap(0, 1, 1, 2);
											CurrentBall->Swap(1, 0, 0, 1);
										}
							}
						}
			//직전의 버튼 입력상태를 저장한다.
			PrevButton = is->Button[0];
			//레버가 계속 아래로 입력되고 있는 경우에 볼이 계속 떨어지지 않도록 처리한다.
			if (!is->Down) PrevDown = false;
		}
		//이동상태
		if (State == 1) {
			//타이머를 갱신한다.
			Time++;
			//그림좌표를 갱신한다.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			//타이머가 일정 값에 도달하면
			//입력상태로 전환한다.
			if (Time == 10) {
				State = 0;
			}
		}
		//낙하판정상태
		if (State == 2) {
			//떨어질 볼이 하나도 없는 경우에는 삭제판정상태로 전환한다.
			State = 4;
			//상대방의 공격을 받고 있는 경우에는 공격 볼을 발생시킨다.
			Attack();
			//스테이지 전체의 셀을 아래에서 위로 검사한다.
			for (int x = FieldLeft; x<FieldRight; x++) {
				for (int y = DROPPING_BALL_FIELD_BOTTOM - 1; y >= DROPPING_BALL_FIELD_TOP; y--) {
					//아래에 아무것도 없는 위치의 볼을 찾는다.
					if (StageCell->Get(x, y) == ' ' && StageCell->Get(x, y - 1) != ' ') {
						//아래에 아무것도 없는 위치의 볼이 있으면
						//그 볼과, 그 볼의 위에 있는 모든 볼을
						//떨어질 볼로 표시한다.
						//(셀의 최상위 비트를 1로 설정한다.)
						for (; y >= DROPPING_BALL_FIELD_TOP; y--) {
							//셀이 볼인지를 검사하여
							//볼인 경우에는 떨어질 볼로 표시하고
							//한 단씩 아래로 이동시킨다.
							char c = StageCell->Get(x, y - 1);
							if (c != ' ') {
								StageCell->Set(x, y, c | 0x80);
							}
							else {
								StageCell->Set(x, y, ' ');
							}
						}
						//타이머를 설정하고 낙하상태로 전환한다.
						Time = 0;
						State = 3;
					}
				}
			}
		}
		//낙하상태
		if (State == 3) {
			//타이머를 갱신한다.
			Time++;

			//타이머가 일정 값에 도달하면 떨어질 볼의 표시를 삭제한다.
			if (Time == 10) {
				//스테이지상의 모든 셀에 대하여 처리한다.
				for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
					for (int x = FieldLeft; x<FieldRight; x++) {
						//떨어질 볼의 표시를 삭제한다.
						//(셀의 최상위 비트를 클리어한다.)
						StageCell->Set(x, y, StageCell->Get(x, y) & 0x7f);
					}
				}
				//낙하판정 상태로 전환한다.
				//볼은 한 단씩 낙하하기 때문에
				//높은 곳에서부터 떨어지는 볼이 착지할 때까지는
				//낙하판정상태와 낙하상태를 여러 번 반복할 필요가 있다.
				State = 2;
			}
		}
		//삭제판정상태
		if (State == 4) {
			//연속카운트(연속작용이 일어날 횟수)를 증가시킨다.
			ChainCount++;
			//볼을 전혀 없애지 못한 경우에는
			//재출현상태로 전환한다.
			State = 6;
			
			//스테이지 안의 모든 셀에 대해
			//사라질 볼이 있는지를 검사한다.
			for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
				for (int x = FieldLeft; x<FieldRight; x++) {
					//셀의 종류를 얻는다.
					char c = StageCell->Get(x, y);

					//셀이 카운트 되지 않았고
					//보통의 볼(0-4)이라면
					//카운트를 시작한다.
					if (!(c & 0x80) && '0' <= c && c<'0' + DROPPING_BALL_COLOR_COUNT) {
						//같은 종류의 볼이 몇 개 연결되어 있는지를 센다.
						int count = Count(x, y, c);
						//이어진 개수가 정해진 개수(4개)이상이면 볼을 없앤다.
						if (count >= ErasingCount) {
							//상대방에게 보낼 공격 볼의 수를 증가시킨다.
							Opponent->AttackCount += (count - ErasingCount + 1)*ChainCount;
							//사라질 볼로 표시한다.
							BeginErasing(x, y, c | 0x80);
							//타이머를 설정하고
							//삭제상태로 전환한다.
							Time = 0;
							State = 5;
						}
					}
				}
			}
			//스테이지 안의 모든 볼에 대해
			//카운트 완료 표시를 삭제한다.
			for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
				for (int x = FieldLeft; x<FieldRight; x++) {
					//카운트 완료 표시를 해제한다.
					//(최상위 비트를 클리어한다.)
					StageCell->Set(x, y, StageCell->Get(x, y) & 0x7f);
				}
			}
		}
		//삭제상태
		if (State == 5) {
			//타이머를 갱신한다.
			Time++;
			//타이머가 일정 값에 도달하면
			//표시된 볼을 없앤다.
			if (Time == 10) {
				//스테이지 내의 모든 세에 대해 처리한다.
				for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
					for (int x = FieldLeft; x<FieldRight; x++) {
						EndErasing(x, y);
					}
				}
				//볼이 사라지면
				//아래에 아무것도 없는 볼이 떨어질 수 있기 때문에
				//낙하판정상태로 전환한다.
				State = 2;
			}
		}
		//재출현상태
		if (State == 6) {
			//새로운 볼을 스테이지 위쪽에 출현시킨다.
			Init();
		}
		return true;
	}

	//인접한 볼의 개수를 세는 처리
	//재귀적으로 호출한다.
	int Count(int x, int y, char ball) {
		//셀의 종류를 얻는다.
		char c = StageCell->Get(x, y);
		//셀이 같은 종류의 볼인 경우의 처리
		if (c == ball) {
			//볼에 카운트 완료 표시를 한다.
			StageCell->Set(x, y, c | 0x80);
			//상하좌우로 인접한 셀에 대해서도
			//같은 종류의 볼이 연결되어 있는지 재귀적으로 검사하여
			//인접한 볼의 개수의 합을 반환한다.
			return 1 + Count(x - 1, y, c) + Count(x + 1, y, c) + Count(x, y - 1, c) + Count(x, y + 1, c);
		}
		//셀이 같은 종류의 볼이 아닌 경우에는
		//인접한 볼의 개수를 0으로 반환한다.
		return 0;
	}

	//연결될 볼을 사라질 볼로 표시하는 처리
	//재귀적으로 호출된다.
	virtual void BeginErasing(int x, int y, char ball) {
		//셀의 종류를 얻는다.
		char c = StageCell->Get(x, y);
		//셀이 같은 종류의 볼인 경우의 처리
		if (c == ball) {
			//볼을 사라질 볼로 표시한다.
			StageCell->Set(x, y, c | 0x40);
			//상하좌우로 인접한 셀에 대해서도
			//같은 종류의 볼을 없애는 처리를 재귀적으로 수행한다.
			BeginErasing(x - 1, y, c);
			BeginErasing(x + 1, y, c);
			BeginErasing(x, y - 1, c);
			BeginErasing(x, y + 1, c);
		}
		else
			//셀이 공격 볼이 경우의 처리
			if (c == '0' + DROPPING_BALL_COLOR_COUNT) {
				//볼을 사라질 볼로 표시한다.
				StageCell->Set(x, y, c | 0x40);
				//상대방의 공격 볼의 개수를 증가시킨다.
				Opponent->AttackCount += ChainCount;
			}
	}

	//사라질 볼을 없애는 처리
	virtual void EndErasing(int x, int y) {
		//셀이 사라질 볼인 경우의 처리
		if (StageCell->Get(x, y) & 0x40) {
			//셀을 공백으로 설정한다.
			StageCell->Set(x, y, ' ');
		}
	}

	//공격 볼을 출현시키는 처리
	virtual void Attack() {
		//필드의 폭
		int n = FieldRight - FieldLeft;
		//공격 볼의 출현위치를 임의로 결정한다.
		int r = Rand.Int31() % n;
		//가로 한 단에 공격 볼을 출현시킨다.
		for (int i = 0; i<n; i++) {
			//공격 볼을 출현시킬 X좌표
			int x = (i + r) % n + FieldLeft;
			//공격 볼의 개수가 0보다 큰 동안 볼을 출현시킨다.
			if (AttackCount>0) {
				//공격 볼을 스테이지 셀에 넣는다.
				StageCell->Set(x, 0, '0' + DROPPING_BALL_COLOR_COUNT);
				//공격 볼의 개수를 감소 시킨다.
				AttackCount--;
			}
			else {
				//공격 볼의 수가 0이면 볼을 출현시키지 않는다.
				//셀을 공백으로 설정한다.
				StageCell->Set(x, 0, ' ');
			}
		}
	}
	//볼의 그림 처리
	virtual void Draw() {
		//화면의 해상도를 얻는다.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<DROPPING_BALL_CELL_SIZE; y++) {
			for (int x = 0; x<DROPPING_BALL_CELL_SIZE; x++) {
				char c;
				c = CurrentBall->Get(x, y);
				if (State<2 && c != ' ') {
					Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
						(X + x)*sw, (Y + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				c = NextBall->Get(x, y);
				if (c != ' ') {
					Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
						(NextLeft + x - DROPPING_BALL_CELL_SIZE / 2)*sw, (DROPPING_BALL_FIELD_TOP + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
			}
		}
		//스테이지의 모든 셀에 대하여 처리한다.
		for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
			for (int x = FieldLeft; x<FieldRight; x++) {
				//셀의 종류를 얻는다.
				char c = StageCell->Get(x, y);
				//떨어지는 볼 이외의 볼
				if ('0' <= c && c<'0' + DROPPING_BALL_COLOR_COUNT * 2) {
					//볼의 종류에 따라 이미지를 그린다.
					Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				else
					//떨어지는 볼
					if (c & 0x80) {
						//타이머를 사용해 좌표를 조금씩 변화시켜
						//볼이 부드럽게 떨어지는 것처럼 그린다.
						Game->Texture[TEX_BALL0 + ((c & 0x7f) - '0')]->Draw(
							x*sw, (y - 1 + Time * 0.1f)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
						);
					}
					else
						//사라질 볼
						if (c & 0x40) {
							//타이머를 사용해 그림의 색을 결정한다.
							float f = Time * 0.1f;
							//시간의 경과에 따라 흐려지는것처럼 표시한다.
							Game->Texture[TEX_BALL0 + ((c & 0x3f) - '0')]->Draw(
								x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1)
							);
						}
			}
		}
	}
};

// 스테이지
class CDroppingBallStage : public CStage {
public:
	CCell * StageCell;
	CCell* BallCell[4];
	CDroppingBallStage() : CStage(L"DROPPING BALL") {
		StageCell = new CCell();
		for (int i = 0; i<4; i++) {
			BallCell[i] = new CCell(DROPPING_BALL_CELL_SIZE, DROPPING_BALL_CELL_SIZE);
		}
	}
	virtual ~CDroppingBallStage() {
		delete StageCell;
		for (int i = 0; i<4; i++) {
			delete BallCell[i];
		}
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"================"
		);
		CDroppingBallBall* ball0 = new CDroppingBallBall(StageCell, BallCell[0], BallCell[1], 0, 4, 1, 6, 7);
		CDroppingBallBall* ball1 = new CDroppingBallBall(StageCell, BallCell[2], BallCell[3], 1, 4, 10, 15, 8);
		ball0->Opponent = ball1;
		ball1->Opponent = ball0;
	}
	virtual void Draw() {
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				char c = StageCell->Get(x, y);
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
			}
		}
	}
};


//==============================================================
// 볼 떨어뜨리기 2
//볼 떨어뜨리기 1가 똑같은데
//공격받는 셀이 한번에 없어지지는 않지만 연속반응을 만들기 쉬운 배치로
//공격볼을 역으로 이용하여 상대에게 반격을 가할수 있다.
// 볼
class CDroppingBall2Ball : public CDroppingBallBall {
public:

	CDroppingBall2Ball(
		CCell* stage_cell, CCell* current_ball, CCell* next_ball,
		int player_id, int Erasing_count,
		int field_left, int field_right, int next_left)
		: CDroppingBallBall(
			stage_cell, current_ball, next_ball,
			player_id, Erasing_count,
			field_left, field_right, next_left)
	{}

	virtual void MakeNextBall() {
		NextBall->Set(1, 0, '0' + Rand.Int31() % (DROPPING_BALL_COLOR_COUNT * 2));
		NextBall->Set(1, 1, '0' + Rand.Int31() % (DROPPING_BALL_COLOR_COUNT * 2));
	}

	virtual void BeginErasing(int x, int y, char ball) {
		char c = StageCell->Get(x, y);
		if (c == ball) {
			StageCell->Set(x, y, c | 0x40);
			BeginErasing(x - 1, y, c);
			BeginErasing(x + 1, y, c);
			BeginErasing(x, y - 1, c);
			BeginErasing(x, y + 1, c);
		}
		else
			if ('0' + DROPPING_BALL_COLOR_COUNT <= c && c<'0' + DROPPING_BALL_COLOR_COUNT * 2) {
				StageCell->Set(x, y, c | 0x40);
			}
	}

	virtual void EndErasing(int x, int y) {
		char c = StageCell->Get(x, y);
		if (c & 0x40) {
			c &= 0x3f;
			if ('0' <= c && c<'0' + DROPPING_BALL_COLOR_COUNT) {
				StageCell->Set(x, y, ' ');
			}
			else {
				StageCell->Set(x, y, c - DROPPING_BALL_COLOR_COUNT);
			}
		}
	}

	virtual void Attack() {
		for (int x = FieldLeft; x<FieldRight; x++) {
			if (AttackCount>0) {
				StageCell->Set(x, 0, '0' + DROPPING_BALL_COLOR_COUNT + (x - FieldLeft) % DROPPING_BALL_COLOR_COUNT);
				AttackCount--;
			}
			else {
				StageCell->Set(x, 0, ' ');
			}
		}
	}
};

// 스테이지
class CDroppingBall2Stage : public CDroppingBallStage {
public:
	CDroppingBall2Stage() {
		Name = L"DROPPING BALL2";
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"================"
		);
		CDroppingBall2Ball* ball0 = new CDroppingBall2Ball(StageCell, BallCell[0], BallCell[1], 0, 3, 1, 6, 7);
		CDroppingBall2Ball* ball1 = new CDroppingBall2Ball(StageCell, BallCell[2], BallCell[3], 1, 3, 10, 15, 8);
		ball0->Opponent = ball1;
		ball1->Opponent = ball0;
	}
};

//==============================================================
// 3차원 블록 떨어뜨리기

#define DROPPING_BLOCK3D_FIELD_XSIZE 7
#define DROPPING_BLOCK3D_FIELD_YSIZE 7
#define DROPPING_BLOCK3D_FIELD_ZSIZE 11
#define DROPPING_BLOCK3D_CELL_SIZE 3
#define DROPPING_BLOCK3D_PATTERN_COUNT 7

// 블록의 패턴
char* DroppingBlock3DPattern[DROPPING_BLOCK3D_PATTERN_COUNT] = {
	(char*)
	"## "
	"## "
	"   "
	,
	(char*)
	"   "
	"###"
	"   "
	,
	(char*)
	"## "
	" ##"
	"   "
	,
	(char*)
	" ##"
	"## "
	"   "
	,
	(char*)
	" # "
	"###"
	"   "
	,
	(char*)
	"#  "
	"###"
	"   "
	,
	(char*)
	"  #"
	"###"
	"   "
};

// 블록
class CDroppingBlock3DBlock : public CMover {
public:
	CCell3D * StageCell;
	CCell3D* BlockCell;
	float Z;
	int CX, CY, CZ, VX, VY, VZ;
	int State, Time, DropTime;
	bool PrevButton, PrevDown;

	void Init() {
		BlockCell->Clear();
		char* pattern = DroppingBlock3DPattern[Rand.Int31() % DROPPING_BLOCK3D_PATTERN_COUNT];
		for (int i = 0, y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
			for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++, i++) {
				BlockCell->Set(x, y, DROPPING_BLOCK3D_CELL_SIZE / 2, pattern[i]);
			}
		}
		CX = (DROPPING_BLOCK3D_FIELD_XSIZE - DROPPING_BLOCK3D_CELL_SIZE) / 2;
		CY = (DROPPING_BLOCK3D_FIELD_YSIZE - DROPPING_BLOCK3D_CELL_SIZE) / 2;
		CZ = 1;
		X = CX;
		Y = CY;
		Z = CZ;
		State = 0;
		DropTime = 0;
		if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
			State = 99999;
		}
		PrevDown = true;
	}

	CDroppingBlock3DBlock(CCell3D* stage_cell, CCell3D* block_cell)
		: StageCell(stage_cell), BlockCell(block_cell), PrevButton(true)
	{
		Init();
	}

	//블록의 이동처리
	virtual bool Move(const CInputState* is) {
		//입력 상태
		if (State == 0) {
			//낙하 타이머를 갱신한다.
			DropTime++;
			//버튼 3을 입력하거나
			//낙하 타이머가 일정 값에 도달하면 블록을 낙하시킨다.
			if ((is->Button[3] && !PrevDown) || DropTime == 60) {
				//셀 좌표를 갱신한 경우에
				//블록이 스테이지의 벽이나 다른 블록과 접촉했는지를 검사한다.
				if (StageCell->Hit(CX, CY, CZ + 1, BlockCell)) {
					//접촉한 경우에는 블록 셀을 스텡지 셀에 합성한다.
					StageCell->Merge(CX, CY, CZ, BlockCell);
					//착지상태로 전환한다.
					State = 2;
				}
				else {
					//접촉하지 않은 경우에는
					//낙하 타이머 및 속도를 설정하고 셀 좌표를 갱신한다.
					DropTime = 0;
					CZ++;
					VX = 0;
					VY = 0;
					VZ = 1;
					//이동상태로 전환한다.
					Time = 0;
					State = 1;
				}
			}
			else
				//레버를 왼쪼으로 입력한 경우에
				//블록이 이동 방향의 스테이지 셀에 접촉하지 않으면
				//블록을 왼쪽으로 이동시킨다.
				if (is->Left && !StageCell->Hit(CX - 1, CY, CZ, BlockCell)) {
					CX--;
					VX = -1;
					VY = 0;
					VZ = 0;
					Time = 0;
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CZ, BlockCell)) {
						CX++;
						VX = 1;
						VY = 0;
						VZ = 0;
						Time = 0;
						State = 1;
					}
					else
						if (is->Up && !StageCell->Hit(CX, CY + 1, CZ, BlockCell)) {
							CY++;
							VX = 0;
							VY = 1;
							VZ = 0;
							Time = 0;
							State = 1;
						}
						else
							if (is->Down && !StageCell->Hit(CX, CY - 1, CZ, BlockCell)) {
								CY--;
								VX = 0;
								VY = -1;
								VZ = 0;
								Time = 0;
								State = 1;
							}
							else
								//직전에 버튼이 눌리지 않았을 때에만 처리한다.
								if (!PrevButton) {
									//버튼0을 입력한 경우에는
									//X방향을 회전축으로 하여 블록을 회전시킨다.
									if (is->Button[0]) {
										for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++) {
											BlockCell->Swap(x, 1, 2, x, 2, 1);
											BlockCell->Swap(x, 0, 1, x, 1, 2);
											BlockCell->Swap(x, 1, 0, x, 0, 1);
											BlockCell->Swap(x, 0, 0, x, 0, 2);
											BlockCell->Swap(x, 2, 0, x, 0, 0);
											BlockCell->Swap(x, 2, 2, x, 2, 0);
										}
										//회전 후의 블록이 스테이지 셀에 접촉 할때는
										//회전을 취소한다.
										if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
											for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++) {
												BlockCell->Swap(x, 2, 2, x, 2, 0);
												BlockCell->Swap(x, 2, 0, x, 0, 0);
												BlockCell->Swap(x, 0, 0, x, 0, 2);
												BlockCell->Swap(x, 1, 0, x, 0, 1);
												BlockCell->Swap(x, 0, 1, x, 1, 2);
												BlockCell->Swap(x, 1, 2, x, 2, 1);
											}
										}
									}
									else
										//버튼1을 입력한 경우에는
										//Y방향을 회전축으로 하여 블록을 회전시킨다.
										if (is->Button[1]) {
											for (int y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
												BlockCell->Swap(2, y, 1, 1, y, 2);
												BlockCell->Swap(1, y, 0, 2, y, 1);
												BlockCell->Swap(0, y, 1, 1, y, 0);
												BlockCell->Swap(0, y, 0, 0, y, 2);
												BlockCell->Swap(2, y, 0, 0, y, 0);
												BlockCell->Swap(2, y, 2, 2, y, 0);
											}
											//회전 후의 블록이 스테이지 셀에 접촉할 때에는
											//회전을 취소한다.
											if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
												for (int y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
													BlockCell->Swap(2, y, 2, 2, y, 0);
													BlockCell->Swap(2, y, 0, 0, y, 0);
													BlockCell->Swap(0, y, 0, 0, y, 2);
													BlockCell->Swap(0, y, 1, 1, y, 0);
													BlockCell->Swap(1, y, 0, 2, y, 1);
													BlockCell->Swap(2, y, 1, 1, y, 2);
												}
											}
										}
										else
											//버튼 2를 입력한 경우에는
											//Z방향을 회전축으로 하여 블록을 회전시킨다.
											if (is->Button[2]) {
												for (int z = 0; z<DROPPING_BLOCK3D_CELL_SIZE; z++) {
													BlockCell->Swap(1, 2, z, 2, 1, z);
													BlockCell->Swap(0, 1, z, 1, 2, z);
													BlockCell->Swap(1, 0, z, 0, 1, z);
													BlockCell->Swap(0, 0, z, 0, 2, z);
													BlockCell->Swap(2, 0, z, 0, 0, z);
													BlockCell->Swap(2, 2, z, 2, 0, z);
												}
												//회전 후의 블록이 스테이지 셀에 접촉할때에는
												//회전을 취소한다.
												if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
													for (int z = 0; z<DROPPING_BLOCK3D_CELL_SIZE; z++) {
														BlockCell->Swap(2, 2, z, 2, 0, z);
														BlockCell->Swap(2, 0, z, 0, 0, z);
														BlockCell->Swap(0, 0, z, 0, 2, z);
														BlockCell->Swap(1, 0, z, 0, 1, z);
														BlockCell->Swap(0, 1, z, 1, 2, z);
														BlockCell->Swap(1, 2, z, 2, 1, z);
													}
												}
											}
								}
			//직전에 버튼이 눌렸는지를 저장해 놓는다.
			PrevButton = is->Button[0] | is->Button[1] | is->Button[2];
			//버튼을 누른 채로 있을 때 블록이 계속해서 떨어지지 않도록 하기 위한 처리
			if (!is->Button[3]) PrevDown = false;
		}
		//이동 상태의 처리
		if (State == 1) {
			//타이머를 갱신한다.
			Time++;
			//타이머를 사용해 그림좌표를 조금씩 갱신한다.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			Z = CZ - VZ * (1 - Time * 0.1f);
			//타이머가 일정 값에 도달하면 입력상태로 전환한다.
			if (Time == 10) {
				State = 0;
			}
		}
		//착지상태
		if (State == 2) {
			//사라질 블록이 하나도 없는 경우에는
			//재출현 상태로 전환한다.
			State = 4;
			//블록이 빈틈없이 채워진 단을 찾는다.
			for (int z = 1; z<DROPPING_BLOCK3D_FIELD_ZSIZE - 1; z++) {
				int x, y;
				for (y = 1; y<DROPPING_BLOCK3D_FIELD_YSIZE - 1; y++) {
					for (x = 1; x<DROPPING_BLOCK3D_FIELD_XSIZE - 1; x++) {
						//공백인 셀이 있는 경우에는 루프를 탈출한다.
						if (StageCell->Get(x, y, z) == ' ') {
							y = DROPPING_BLOCK3D_FIELD_YSIZE;
							break;
						}
					}
				}
				//블록이 채워진 단이 있는 경우의 처리
				if (y == DROPPING_BLOCK3D_FIELD_YSIZE - 1) {
					for (y = 1; y<DROPPING_BLOCK3D_FIELD_YSIZE - 1; y++) {
						for (x = 1; x<DROPPING_BLOCK3D_FIELD_XSIZE - 1; x++) {
							//단의 블록을 사라질 블록 [+]으로 변경한다.
							StageCell->Set(x, y, z, '+');
						}
					}
					//삭제상태로 전환한다.
					Time = 0;
					State = 3;
				}
			}
		}
		//삭제상태
		if (State == 3) {
			//타이머를 갱신한다.
			Time++;
			//타이머가 일정 값에 도달하면 블록을 삭제한다.
			if (Time == 20) {
				//사라질 블록[+]이 있는 단을 찾는다.
				for (int i = 1; i<DROPPING_BLOCK3D_FIELD_ZSIZE - 1; i++) {
					//사라질 블록을 찾은 경우의 처리
					if (StageCell->Get(1, 1, i) == '+') {
						//사라질 단보다 앞쪽에 있는 모든 단을
						//한단씩 안쪽으로 떨어뜨린다.
						for (int z = i; z>0; z--) {
							for (int y = 1; y<DROPPING_BLOCK3D_FIELD_YSIZE - 1; y++) {
								for (int x = 1; x<DROPPING_BLOCK3D_FIELD_XSIZE - 1; x++) {
									StageCell->Set(x, y, z, StageCell->Get(x, y, z - 1));
								}
							}
						}
					}
				}
				//재출현상태로 전환한다.
				State = 4;
			}
		}
		//재출현상태
		if (State == 4) {
			Init();
		}
		return true;
	}

	virtual void Draw() {
		for (int z = DROPPING_BLOCK3D_FIELD_ZSIZE - 1; z >= 0; z--) {
			for (int y = 0; y<DROPPING_BLOCK3D_FIELD_YSIZE; y++) {
				for (int x = 0; x<DROPPING_BLOCK3D_FIELD_XSIZE; x++) {
					float
						dx = x - DROPPING_BLOCK3D_FIELD_XSIZE * 0.5f + 0.5f,
						dy = y - DROPPING_BLOCK3D_FIELD_YSIZE * 0.5f + 0.5f,
						dz = z,
						f = 1 - Time * 0.1f;
					switch (StageCell->Get(x, y, z)) {
					case '#':
						Game->Mesh[MESH_CUBE_GRAY]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, 1, 0);
						break;
					case '+':
						Game->Mesh[MESH_CUBE_GRAY]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, f, 0);
						break;
					case '=':
						Game->Mesh[MESH_CUBE_WHITE]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, 1, 0);
						break;
					}
				}
			}
		}
		if (State<2) {
			for (int z = DROPPING_BLOCK3D_CELL_SIZE - 1; z >= 0; z--) {
				for (int y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
					for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++) {
						float
							dx = X + x - DROPPING_BLOCK3D_FIELD_XSIZE * 0.5f + 0.5f,
							dy = Y + y - DROPPING_BLOCK3D_FIELD_YSIZE * 0.5f + 0.5f,
							dz = Z + z;
						if (BlockCell->Get(x, y, z) == '#') {
							Game->Mesh[MESH_CUBE_GRAY]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, 0.5f, 0);
						}
					}
				}
			}
		}
	}
};

// 스테이지
class CDroppingBlock3DStage : public CStage {
	CCell3D* StageCell;
	CCell3D* BlockCell;
public:
	CDroppingBlock3DStage() : CStage(L"DROPPING BLOCK 3D") {
		StageCell = new CCell3D(DROPPING_BLOCK3D_FIELD_XSIZE, DROPPING_BLOCK3D_FIELD_YSIZE, DROPPING_BLOCK3D_FIELD_ZSIZE);
		BlockCell = new CCell3D(DROPPING_BLOCK3D_CELL_SIZE, DROPPING_BLOCK3D_CELL_SIZE, DROPPING_BLOCK3D_CELL_SIZE);
	}
	virtual ~CDroppingBlock3DStage() {
		delete StageCell;
		delete BlockCell;
	}
	virtual void Init() {
		StageCell->Clear();
		for (int z = 1; z<DROPPING_BLOCK3D_FIELD_ZSIZE; z++) {
			for (int y = 0; y<DROPPING_BLOCK3D_FIELD_YSIZE; y++) {
				for (int x = 0; x<DROPPING_BLOCK3D_FIELD_XSIZE; x++) {
					if (
						x == 0 || x == DROPPING_BLOCK3D_FIELD_XSIZE - 1 ||
						y == 0 || y == DROPPING_BLOCK3D_FIELD_YSIZE - 1 ||
						z == DROPPING_BLOCK3D_FIELD_ZSIZE - 1
						) {
						StageCell->Set(x, y, z, '=');
					}
				}
			}
		}
		new CDroppingBlock3DBlock(StageCell, BlockCell);
	}
};