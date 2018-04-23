//==============================================================
// 블록을 사각형으로 만들어 없애기

// 스테이지
class CRectangleShapedBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY;
	int State, Time, DropTime;
	int L, R, T, B;
	bool PrevLever, PrevButton;

	CRectangleShapedBlockStage() : CStage(L"RECTANGLE SHAPED BLOCK") {
		Cell = new CCell();
	}

	virtual ~CRectangleShapedBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"=00000 33333   ="
			"=0 111222  3   ="
			"=  1 777244444 ="
			"=99188872554   ="
			"=9  8  7  56666="
			"=9  8     56  6="
			"=9        5    ="
			"=9        5    ="
			"=              ="
			"=              ="
			"================"
		);
		CX = xs / 2;
		CY = ys - 2;
		PrevLever = true;
		PrevButton = true;
		State = 0;
		Time = 0;
		DropTime = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//입력 상태
		if (State == 0) {
			//레버의 입력에 따라 캐릭터를 좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && CX>0) CX--;
				if (is->Right && CX<xs - 1) CX++;
			}
			PrevLever = is->Left || is->Right;

			//버튼을 누르면 블록을 발사한다.
			if (!PrevButton && is->Button[0] && Cell->Get(CX, CY - 1) == ' ') {
				Cell->Set(CX, CY, '+');
			}
			PrevButton = is->Button[0];

			//스테이지 위쪽의 셀을 공백으로 설정한다.
			for (int x = 1; x<xs - 1; x++) {
				Cell->Set(x, 0, ' ');
			}
			//새로운 블록의 셀(+)을 위로 이동시킨다.
			for (int y = 1; y<ys - 1; y++) {
				for (int x = 1; x<xs - 1; x++) {
					//새로운 블록의 셀이 발견된 경우의 처리
					if (Cell->Get(x, y) == '+') {
						char c = Cell->Get(x, y - 1);
						//새로운 블록의 위의 셀이 공백이라면
						//새로운 블록을 한칸 위로 이동한다.
						if (c == ' ') {
							Cell->Set(x, y - 1, '+');
							Cell->Set(x, y, ' ');
						}
						else {
							//새로운 블록의 위의 셀이 공백이 아니라면
							//새로운 블록을 현재의 위치에 고정한다.
							Cell->Set(x, y, c);
							//블록을 지우는 처리를 호출한다.
							EraseBlock(x, y);
						}
					}
				}
			}

			//일정 시간마다 블록을 낙하시킨다.
			DropTime++;
			if (DropTime == 60) {
				DropTime = 0;
				int x, y;
				//스테이지의 아래쪽이 공백이 아니라면
				//블록을 낙하시키지 않는다.
				for (x = 1; x<xs - 1; x++) {
					if (Cell->Get(x, ys - 3) != ' ') break;
				}
				//스테이지 아래쪽이 공백이라면
				//블록을 낙하시킨다.
				if (x == xs - 1) {
					//모든 셀을 1단씩 아래로 이동한다.
					for (y = ys - 3; y>0; y--) {
						for (x = 1; x<xs - 1; x++) {
							Cell->Set(x, y, Cell->Get(x, y - 1));
						}
					}
				}
			}
		}
		//삭제상태의 처리
		if (State == 1) {
			//일정 시간이 경과하면 블록을 지운다.
			Time++;
			if (Time == 30) {
				//사각형으로 채워진 블록을 지운다.
				for (int y = T; y<B; y++) {
					for (int x = L; x<R; x++) {
						Cell->Set(x, y, ' ');
					}
				}
				//입력상태로 전환한다.
				State = 0;
			}
		}

		return true;
	}

	//사각형의 형태로 채워진 블록을 지우는 처리
	void EraseBlock(int x, int y) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//현재 위치의 셀을 얻는다.
		char c = Cell->Get(x, y);
		//현재 위치로부터 상하좡우로
		//블록이 연결된 범위를 검사한다.
		for (L = x; Cell->Get(L - 1, y) == c; L--);
		for (R = x + 1; Cell->Get(R, y) == c; R++);
		for (T = y; Cell->Get(x, T - 1) == c; T--);
		for (B = y + 1; Cell->Get(x, B) == c; B++);

		//사각형의 범위 안에 공백인 셀이 있는지 검사한다.
		for (y = T; y<B; y++) {
			for (x = L; x<R; x++) {
				if (Cell->Get(x, y) != c) break;
			}
			if (x<R) break;
		}

		//공백의 셀이 없는 경우의 처리
		if (y == B) {
			//사각형 범위의 1칸 밖에
			//블록의 셀이 있는지를 검사한다.
			for (x = L; x<R; x++) {
				if (Cell->Get(x, T - 1) == c || Cell->Get(x, B) == c) break;
			}
			for (y = T; y<B; y++) {
				if (Cell->Get(L - 1, y) == c || Cell->Get(R, y) == c) break;
			}
			//블록의 셀이 없는 경우의 처리
			if (x == R && y == B) {
				//사각형의 범위를 사라질 블록(*)으로 설정한다.
				for (int y = T; y<B; y++) {
					for (int x = L; x<R; x++) {
						Cell->Set(x, y, '*');
					}
				}
				//타이머를 설정하고 삭제상태로 전환한다.
				Time = 0;
				State = 1;
			}
		}
	}

	//스테이지의 그림 처리
	virtual void Draw() {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//그려야 할 크기
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		//벽의 셀(=)을 그린다.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				if (Cell->Get(x, y) == '=') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
			}
		}
		//블록의 셀을 그린다.
		for (int y = 1; y<ys - 1; y++) {
			for (int x = 1; x<xs - 1; x++) {
				char c = Cell->Get(x, y);
				//새로운 블록(+)을 그린다.
				if (c == '+') {
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
				}
				else
					//그 외의 블록을 그린다.
					if (c != ' ') {
						//상하좌우에 같은 종류의 블록이
						//인접해 있는지에 따라 그림좌표를 조정하고
						//블록의 결합을 표시한다
						static const float gap = 0.1f;
						float l = x, r = x + 1, t = y, b = y + 1;
						if (c != Cell->Get(x - 1, y)) l += gap;
						if (c != Cell->Get(x + 1, y)) r -= gap;
						if (c != Cell->Get(x, y - 1)) t += gap;
						if (c != Cell->Get(x, y + 1)) b -= gap;

						//사라질 블록은 시간의 경과에 따라 점차 흐르게 그리고
						//보통의 블록은 검정색으로 그린다.
						float f = (float)Time / 30;
						D3DCOLOR color = COL_BLACK;
						if (c == '*') color = D3DXCOLOR(f, f, f, 1);
						Game->Texture[TEX_FILL]->Draw(
							l*sw, t*sh, (r - l)*sw, (b - t)*sh, 0, 0, 1, 1, color);
					}
			}
		}
		//캐릭터를 그린다.
		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// 블록을 변형시키기

// 스테이지
class CTransformedBlockStage : public CStage {
public:
	CCell * StageCell;
	CCell* BlockCell;
	int CX, CY, State, Time, DropTime;
	bool PrevLever, PrevUp, PrevButton;

	CTransformedBlockStage() : CStage(L"TRANSFORMED BLOCK") {
		StageCell = new CCell();
		BlockCell = new CCell(3, StageCell->GetYSize());
	}

	virtual ~CTransformedBlockStage() {
		delete StageCell;
		delete BlockCell;
	}

	virtual void Init() {
		int xs = StageCell->GetXSize(), ys = StageCell->GetYSize();
		StageCell->Init((char*)
			"                "
			"================"
			"=######### ####="
			"= ##  ###  # ##="
			"=  #   #   #   ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"================"
		);
		PrevLever = false;
		PrevButton = false;
		State = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//스테이지의 셀 개수, 블록의 셀 개수
		int
			xs = StageCell->GetXSize(), ys = StageCell->GetYSize(),
			bxs = BlockCell->GetXSize(), bys = BlockCell->GetYSize();

		//초기 상태
		if (State == 0) {
			//블록의 셀을 공백으로 설정한다.
			BlockCell->Clear(' ');
			//블록을 셀의 가장 아래 단에 가로 1열로 블록을 써넣는다.
			for (int x = 0; x<bxs; x++) {
				BlockCell->Set(x, bys - 1, '#');
			}
			//블록 셀의 초기위치를 설정한다.
			CX = (xs - bxs) / 2;
			CY = -1;
			//새로운 블록이 다른 블록에
			//접촉했는지를 검사한다.
			if (!StageCell->Hit(CX, CY, BlockCell)) {
				//접촉하지 않은 경우에는 입력상태로 전환한다.
				DropTime = 0;
				PrevUp = true;
				State = 1;
			}
		}
		//입력 상태
		if (State == 1) {
			//레버의 입력에 따라 새로운 블록을 좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && !StageCell->Hit(CX - 1, CY, BlockCell)) CX--;
				if (is->Right && !StageCell->Hit(CX + 1, CY, BlockCell)) CX++;
			}
			PrevLever = is->Left | is->Right;

			//일정 시간마다 새로운 블록을 위로 올라가게 한다.
			//레버를 위로 입력한 경우에는 타이머와 상관없이 곧바로 위로 올라가게 한다.
			DropTime++;
			if (DropTime == 60 || (!PrevUp && is->Up)) {
				DropTime = 0;
				//새로운 블록이 다른 블록에 접촉하지 않는 경우에는
				//블록을 위로 올라가게 한다.
				if (!StageCell->Hit(CX, CY - 1, BlockCell)) {
					CY--;
				}
				else {
					//새로운 블록이 다른 블록에 접촉하는 경우
					StageCell->Merge(CX, CY, BlockCell);
					//블록이 사라지지 않는 경우에는
					//초기상태로 전환한다.
					State = 0;
					//블록이 사라질지를 검사한다.
					for (int y = 2; y<ys - 1; y++) {
						//가로 방향으로 브록이
						//1단 채워져 있는지 검사한다.
						int x;
						for (x = 1; x<xs - 1; x++) {
							if (StageCell->Get(x, y) != '#') break;
						}
						//1단 채워져 있는 경우의 처리
						if (x == xs - 1) {
							//블록을 사라질 블록으로 변경한다.
							for (x = 1; x<xs - 1; x++) {
								StageCell->Set(x, y, '+');
							}
							//삭제상태로 전환한다.
							Time = 0;
							State = 2;
						}
					}
				}
			}
			//레버를 입력한 채로 있는 경우에
			//블록이 연속해서 이동하지 않도록 하기 위한 처리
			if (!is->Up) PrevUp = false;

			//버튼을 누른 경우에 블록을 변형시킨다.
			if (!PrevButton) {
				//어떤 버튼을 눌렀는지 검사한다.
				int x;
				for (x = 0; !is->Button[x] && x<bxs; x++);
				//유효한 버튼이 눌린 경우의 처리
				if (x<bxs) {
					//스테이지의 셀에 접촉하지 않으면
					//버튼에 대응하는 부분의 블록을 1단 위로 늘린다.
					for (int y = 0; y<bys; y++) {
						if (
							BlockCell->Get(x, y) == '#' &&
							StageCell->Get(CX + x, CY + y - 1) == ' '
							) {
							BlockCell->Set(x, y - 1, '#');
						}
					}
				}
			}
			//버튼을 누른 채로 있는 경우에
			//블록이 연ㄴ속해서 변형되지 않도록 하기 위한 처리
			PrevButton = false;
			for (int i = 0; i<bxs; i++) {
				PrevButton = PrevButton || is->Button[i];
			}
		}

		//삭제상태
		if (State == 2) {
			//일정 시간이 경과하면 사라질 블록을 완전히 지운다.
			Time++;
			if (Time == 30) {
				//사라질 블록(+)을 찾는다.
				for (int y = ys - 3; y >= 2; y--) {
					//사라질 블록이 발견되면
					//그 단보다 아래에 있는 단을
					//1단씩 위로 이동한다.
					if (StageCell->Get(1, y) == '+') {
						for (int x = 1; x<xs - 1; x++) {
							for (int i = y; i<ys - 2; i++) {
								StageCell->Set(x, i, StageCell->Get(x, i + 1));
							}
							//가장 아래쪽 단은 공백으로 변경한다.
							StageCell->Set(x, ys - 2, ' ');
						}
					}
				}
				//초기상태로 전환한다.
				State = 0;
			}
		}

		return true;
	}

	virtual void Draw() {
		int xs = StageCell->GetXSize(), ys = StageCell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				float f = (float)Time / 30;
				switch (StageCell->Get(x, y)) {
				case '=':
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
					break;
				case '#':
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
					break;
				case '+':
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 60, 0, 1, 1,
						D3DXCOLOR(f, f, f, 1));
					break;
				}
				if (State == 1 && BlockCell->Get(x - CX, y - CY) == '#') {
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 60, 0, 1, 1, COL_LGRAY);
				}
			}
		}
	}
};

//==============================================================
// 블록을 부딪쳐 부수기

#define STRUCK_BLOCK_COUNT 2

// 스테이지
class CStruckBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, Type, State, Time, DropTime;
	bool PrevLever, PrevDown;

	CStruckBlockStage() : CStage(L"STRUCK BLOCK") {
		Cell = new CCell();
	}

	virtual ~CStruckBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=      2       ="
			"=   01022      ="
			"=  010103332   ="
			"= 00111101112  ="
			"================"
		);
		PrevLever = false;
		Type = 0;
		State = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		
		//초기상태
		if (State == 0) {
			//새로운 블록의 위치
			CX = xs / 2;
			CY = 0;
			//새로운 블록의 종류를 임의로 결정한다.
			Type = Rand.Int31() % STRUCK_BLOCK_COUNT;
			//타이머 등을 초기화하고
			//입력상태로 전환한다.
			DropTime = 0;
			PrevDown = true;
			State = 1;
		}

		//입력상태
		if (State == 1) {
			//레버의 입력에 따라 새로운 블록을 좌우로 이동한다.
			if (!PrevLever) {
				if (is->Left && Cell->Get(CX - 1, CY) == ' ') CX--;
				if (is->Right && Cell->Get(CX + 1, CY) == ' ') CX++;
			}
			PrevLever = is->Left | is->Right;

			//일정 시간마다 새로운 블록을 낙하시킨다.
			//레버를 아래로 입력하면 타이머와 상관없이 곧바로 낙하시킨다.
			DropTime++;
			if (DropTime == 60 || (!PrevDown && is->Down)) {
				DropTime = 0;

				//1단 아래의 셀이 공백인 경우에는
				//새로운 블록을 아래로 이동한다.
				if (Cell->Get(CX, CY + 1) == ' ') {
					CY++;
				}
				else {
					//1단 아래의 셀이 공백이 아닌 경우에는
					//새로운 블록을 고정한다.
					//타이머를 초기화 한다.
					Time = 0;
					//블록이 사라지지 않는 경우에는
					//초기상태로 전환한다.
					State = 0;
					//블록을 고정한다.
					Cell->Set(CX, CY, '0' + Type);
					//블록을 부딪치는 처리를 호출한다.
					Strike(CX, CY + 1, '0' + Type);
				}
			}
			if (!is->Down) PrevDown = false;
		}
		//파괴상태
		if (State == 2) {
			//일정 시간이 경과하면
			//부서질 블록에 금이 생기게 하거나 삭제한다.
			Time++;
			if (Time == 30) {
				//사라질 블록이 없으면
				//초기상태로 전환한다.
				State = 0;
				//부서질 블록을 찾는다.
				for (int y = 0; y<ys; y++) {
					for (int x = 0; x<xs; x++) {
						char c = Cell->Get(x, y);
						//부서질 블록이 발견되면
						//금이 생기게 하거나 삭제한다.
						if (c & 0x80) {
							c &= 0x7f;
							if ('0' <= c && c<'0' + STRUCK_BLOCK_COUNT) {
								Cell->Set(x, y, c + STRUCK_BLOCK_COUNT);
							}
							//이미 금이 생긴 블록은
							//삭제한다.
							else {
								Cell->Set(x, y, ' ');
								//낙하상태로 전환한다.
								Time = 0;
								State = 3;
							}
						}
					}
				}
			}
		}

		//낙하상태
		if (State == 3) {
			//일정 시간이 경과하면
			//사라진 블록의 위에 있던 블록을 낙하시킨다.
			Time++;
			if (Time == 30) {
				//공백인 셀을 찾는다.
				for (int x = 1; x<xs - 1; x++) {
					for (int i = 1; i<ys - 1; i++) {
						//공백인 셀이 발견되면
						//위에 있던 모든 셀을 1단씩 아래로 이동한다.
						if (Cell->Get(x, i) == ' ') {
							for (int y = i; y>0; y--) {
								Cell->Set(x, y, Cell->Get(x, y - 1));
							}
							//가장 위쪽 단의 셀은 공백으로 변경한다.
							Cell->Set(x, 0, ' ');
						}
					}
				}
				//초기상태로 전환한다.
				State = 0;
			}
		}

		return true;
	}

	//블록을 부딪치는 처리
	void Strike(int x, int y, char c) {
		//현재 위치의 셀을 얻는다.
		char d = Cell->Get(x, y);
		//현재 위치의 셀이
		//지정된 종류의 블록(금이 있음/없음)인 경우의 처리
		if (d == c || d == c + STRUCK_BLOCK_COUNT) {
			//블록을 부서질 블록으로 변경한다.
			//비트연산을 사용해 블록에 표시한다.
			Cell->Set(x, y, d | 0x80);
			//상하좌우 및 대각선(8방향)의 블록에 대해서도
			//블록을 부딪치는 처리를 재귀적으로 수행한다.
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) {
						Strike(x + i, y + j, c);
					}
				}
			}
			//파괴상태로 전환한다.
			State = 2;
		}
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
				else
					if (c & 0x80) {
						float f = (float)Time / 30;
						Game->Texture[TEX_BLOCK0 + (c & 0x7f) - '0']->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1));
					}
					else
						if (c != ' ') {
							Game->Texture[TEX_BLOCK0 + c - '0']->Draw(
								x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
						}
			}
		}
		if (State == 1) {
			Game->Texture[TEX_BLOCK0 + Type]->Draw(
				CX*sw, CY*sh, sw, sh, 60, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 스테이지를 회전시키기

#define ROTATED_STAGE_SIZE 8

// 스테이지
class CRotatedStageStage : public CStage {
public:
	CCell * Cell;
	int State;
	bool PrevLever;

	CRotatedStageStage() : CStage(L"ROTATED STAGE") {
		Cell = new CCell();
	}

	virtual ~CRotatedStageStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"   ==========   "
			"   =        =   "
			"   =        =   "
			"   =        =   "
			"   =        =   "
			"   =        =   "
			"   =   101  =   "
			"   =  1011  =   "
			"   = 0001011=   "
			"   ==========   "
			"                "
		);
		PrevLever = false;
		State = 0;
	}
	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//스테이지의 내부의 왼쪽 위의 좌표, 스테이지 내부의 크기
		int rx = 4, ry = 2, rs = 8;

		//입력상태
		if (State == 0) {
			//레버를 왼쪽으로 입력하면 스테이지를 왼쪽으로 회전시킨다.
			if (!PrevLever && is->Left) {
				//스테이지의 1/4영역에 대해서 셀을 바꿔 넣는다.
				for (int y = 0; y<rs / 2; y++) {
					for (int x = 0; x<rs / 2; x++) {
						Cell->Swap(rx + x, ry + y, rx + rs - 1 - y, ry + x);
						Cell->Swap(rx + rs - 1 - y, ry + x, rx + rs - 1 - x, ry + rs - 1 - y);
						Cell->Swap(rx + rs - 1 - x, ry + rs - 1 - y, rx + y, ry + rs - 1 - x);
					}
				}
				//낙하상태로 전환한다.
				Time = 0;
				State = 1;
			}
			else
				//레버를 오른쪽으로 입력하면 스테이지를 오른쪽으로 회전시킨다.
				if (!PrevLever && is->Right) {
					//스스테이지의 1/4영역에 대해서 셀을 바꿔 넣는다.
					//(왼쪽 회전과는 역순으로 세을 바꿔 넣는다)
					for (int y = 0; y<rs / 2; y++) {
						for (int x = 0; x<rs / 2; x++) {
							Cell->Swap(rx + rs - 1 - x, ry + rs - 1 - y, rx + y, ry + rs - 1 - x);
							Cell->Swap(rx + rs - 1 - y, ry + x, rx + rs - 1 - x, ry + rs - 1 - y);
							Cell->Swap(rx + x, ry + y, rx + rs - 1 - y, ry + x);
						}
					}
					//낙하상태로 전환한다.
					Time = 0;
					State = 1;
				}
			//레버를 입력한 채로 있는 경우에
			//계속해서 회전하지않도록 하기 위한 처리
			PrevLever = is->Left || is->Right;
		}

		//낙하상태
		if (State == 1) {
			//일정 시간이 경과하면 블록을 낙하시킨다.
			Time++;
			if (Time == 30) {
				//공백인 세을 찾는다.
				for (int x = rx; x<rx + rs; x++) {
					for (int y = ry; y<ry + rs; y++) {
						//공백인 셀이 있으면 위에 있는 셀을 1단씩 아래로 이동한다.
						if (Cell->Get(x, y) == ' ') {
							for (int i = y; i>ry; i--) {
								Cell->Set(x, i, Cell->Get(x, i - 1));
							}
							//가장 위쪽 단의 셀은 공백으로 변경한다.
							Cell->Set(x, ry, ' ');
						}
					}
				}
				//입력상태로 전환한다.
				State = 0;
			}
		}

		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
				else
					if (c != ' ') {
						Game->Texture[TEX_BLOCK0 + c - '0']->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
					}
			}
		}
	}
};

//==============================================================
// 먹이 블록 없애기
//곰(3)>버섯(1), 펭귄(2)>물고기(0)

#define FOOD_BLOCK_COUNT 2

// 스테이지
class CFoodBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, Type, State, Time, DropTime;
	bool PrevLever, PrevDown;

	CFoodBlockStage() : CStage(L"FOOD BLOCK") {
		Cell = new CCell();
	}

	virtual ~CFoodBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=      0       ="
			"=   01030 2    ="
			"=  010101110   ="
			"= 301121011103 ="
			"================"
		);
		PrevLever = false;
		Type = 0;
		State = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//초기 상태
		if (State == 0) {
			//새로운 블록의 위치
			CX = xs / 2;
			CY = 0;
			//새로운 블록의 종류를 임의로 결정한다.
			Type = Rand.Int31() % (FOOD_BLOCK_COUNT * 2);
			//타이머 등을 초기화하고 입력상태로 전환한다.
			DropTime = 0;
			PrevDown = true;
			State = 1;
		}
		//입력 상태
		if (State == 1) {
			//레버의 입력에 따라 새로운 블록을 좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && Cell->Get(CX - 1, CY) == ' ') CX--;
				if (is->Right && Cell->Get(CX + 1, CY) == ' ') CX++;
			}
			PrevLever = is->Left || is->Right;
			//일정 시간마다 새로운 블록을 낙하시킨다.
			DropTime++;
			if (DropTime == 60 || (!PrevDown && is->Down)) {
				DropTime = 0;
				//아래쪽의 셀이 공백이면 낙하시킨다.
				if (Cell->Get(CX, CY + 1) == ' ') {
					CY++;
				}
				//아래쪽의 셀이 공백이 아니면
				//블록을 고정시키고 삭제판정 상태로 전환한다.
				else {
					Cell->Set(CX, CY, '0' + Type);
					State = 2;
				}
			}
			if (!is->Down) PrevDown = false;
		}

		//삭제 판정상태
		if (State == 2) {
			//타이머를 초기화한다.
			Time = 0;
			//블록이 사라지지 않는 경우에는 초기상태로 전환한다.
			State = 0;
			//동물 셀을 발견하면
			//주위에 있는 먹이를 먹는다.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					char eater = Cell->Get(x, y);
					//동물의 주위에 먹이가 있으면
					//먹이를 먹는 처리를 호출한다.
					if (
						'0' + FOOD_BLOCK_COUNT <= eater &&
						eater<'0' + FOOD_BLOCK_COUNT * 2
						) {
						char food = eater - FOOD_BLOCK_COUNT;
						if (
							(Cell->Get(x - 1, y) & 0x7f) == food ||
							(Cell->Get(x + 1, y) & 0x7f) == food ||
							(Cell->Get(x, y - 1) & 0x7f) == food ||
							(Cell->Get(x, y + 1) & 0x7f) == food
							) {
							//동물을 사라질 블록으로 표시한다.
							Cell->Set(x, y, eater | 0x80);
							//주위의 먹이를 먹는다.
							Eat(x - 1, y, food);
							Eat(x + 1, y, food);
							Eat(x, y - 1, food);
							Eat(x, y + 1, food);
							//삭제상태로 전환한다.
							State = 3;
						}
					}
				}
			}
		}

		//삭제 상태
		if (State == 3) {
			//일정 시간이 경과하면 사라질 블록을 완전히 삭제한다.
			Time++;
			if (Time == 30) {
				for (int y = 0; y<ys; y++) {
					for (int x = 0; x<xs; x++) {
						//사라질 블록을 발견하면
						//셀을 공백으로 변경한다.
						if (Cell->Get(x, y) & 0x80) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//낙하상태로 전환한다.
				Time = 0;
				State = 4;
			}
		}

		//낙하상태
		if (State == 4) {
			//일정 시간이 경과하면 아래에 아무것도 없는 위치의 블록을 낙하시킨다.
			Time++;
			if (Time == 30) {
				//공백인 셀을 찾는다.
				for (int x = 0; x<xs; x++) {
					for (int i = 0; i<ys; i++) {
						//공백인 셀을 발견하면
						//위에 있는 셀을 1단씩 아래로 이동한다.
						if (Cell->Get(x, i) == ' ') {
							for (int y = i; y>0; y--) {
								Cell->Set(x, y, Cell->Get(x, y - 1));
							}
							//가장 위쪽 단의 셀은 공백으로 변경한다.
							Cell->Set(x, 0, ' ');
						}
					}
				}
				//삭제판정 상태로 전환한다.
				State = 2;
			}
		}

		return true;
	}

	//먹이를 먹는 처리
	void Eat(int x, int y, char food) {
		//현재 위치의 셀을 얻는다.
		char c = Cell->Get(x, y);
		//셀이 지정된 먹이인 경우의 처리
		if (c == food) {
			//상하좌우의 셀에 대해서도
			//먹이를 먹는 처리를 재귀적으로 수행한다.
			Cell->Set(x, y, c | 0x80);
			Eat(x - 1, y, food);
			Eat(x + 1, y, food);
			Eat(x, y - 1, food);
			Eat(x, y + 1, food);
		}
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		int texture[] = { TEX_FISH, TEX_MUSHROOM, TEX_ENEMY0, TEX_ENEMY1 };
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
				else
					if (c & 0x80) {
						float f = (float)Time / 30;
						Game->Texture[texture[(c & 0x7f) - '0']]->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1));
					}
					else
						if (c != ' ') {
							Game->Texture[texture[c - '0']]->Draw(
								x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
						}
			}
		}
		if (State == 1) {
			Game->Texture[texture[Type]]->Draw(
				CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 블록으로 둘러싸 없애기

// 스테이지
class CSurroundingBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, State, Time, DropTime;
	char Type;
	bool PrevLever, PrevDown;

	CSurroundingBlockStage() : CStage(L"SURROUNDING BLOCK") {
		Cell = new CCell();
	}

	virtual ~CSurroundingBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=           +  ="
			"=    +      #  ="
			"= +# #+   # +# ="
			"= ##+#++# + ## ="
			"================"
		);
		PrevLever = false;
		Type = '#';
		State = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//초기 상태
		if (State == 0) {
			//새로운 블록의 좌표
			CX = xs / 2;
			CY = 0;

			//새로운 블록의 종류(블록 또는 아이템)를
			//임의로 결정한다.
			Type = (Rand.Int31() % 2) ? '#' : '+';
			//타이머 등을 초기화하고
			//입력상태로 전환한다.
			DropTime = 0;
			PrevDown = true;
			State = 1;
		}

		//입력상태
		if (State == 1) {
			//레버의 입력에 따라 새로운 블록을 좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && Cell->Get(CX - 1, CY) == ' ') CX--;
				if (is->Right && Cell->Get(CX + 1, CY) == ' ') CX++;
			}
			PrevLever = is->Left || is->Right;

			//일정시간마다 새로운 블록을 낙하시킨다.
			DropTime++;
			if (DropTime == 60 || (!PrevDown && is->Down)) {
				DropTime = 0;
				//아래쪽의 셀이 공백이라면 낙하시킨다.
				if (Cell->Get(CX, CY + 1) == ' ') {
					CY++;
				}
				//아래쪽의 셀이 공백이 아니라면
				//블록을 고정시키고 삭제판정 상태로 전환한다.
				else {
					Cell->Set(CX, CY, Type);
					State = 2;
				}
			}
			if (!is->Down) PrevDown = false;
		}

		//삭제판정 상태
		if (State == 2) {
			//타이머를 초기화한다.
			Time = 0;
			//블록이 사라지지 않는 때에는
			//초기상태로 전환한다.
			State = 0;
			//아이템 셀을 찾는다.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					//아이템 셀을 발견하면
					//주위에 있는 공백인 셀의 수를 센다.
					if (
						Cell->Get(x, y) == '+' &&
						Count(x, y) == 0
						) {
						//공백인 셀이 없으면
						//아이템이 사라질 아이템으로 변경하고
						//삭제상태로 전환한다.
						Erase(x, y);
						State = 3;
					}
				}
			}
			//카운트 완료 표시를 삭제한다.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//삭제상태
		if (State == 3) {
			//일정 시간이 경과하면 사라질 아이템을 완전히 삭제한다.
			Time++;
			if (Time == 30) {
				//사라질 아이템을 찾는다.
				for (int y = 0; y<ys; y++) {
					for (int x = 0; x<xs; x++) {
						//사라질 아이템을 발견하면
						//셀을 공백으로 변경한다.
						if (Cell->Get(x, y) & 0x40) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//낙하상태로 전환한다.
				Time = 0;
				State = 4;
			}
		}

		//낙하상태
		if (State == 4) {
			//일정 시간이 경과하면 아래에 아무것도 없는 위치의 셀을 낙하시킨다.
			Time++;
			if (Time == 30) {
				//공백인 셀을 찾는다.
				for (int x = 0; x<xs; x++) {
					for (int i = 0; i<ys; i++) {
						//공백인 셀을 발견하면
						//위에 있는 셀을 1단씩 낙하시킨다.
						if (Cell->Get(x, i) == ' ') {
							for (int y = i; y>0; y--) {
								Cell->Set(x, y, Cell->Get(x, y - 1));
							}
							//가장 위쪽 단의 셀을 공백으로 변경한다.
							Cell->Set(x, 0, ' ');
						}
					}
				}
				//삭제판정 상태로 전환한다.
				State = 2;
			}
		}

		return true;
	}

	//아이템의 주위에 있는 공백인 셀의 수를 세는 처리
	int Count(int x, int y) {
		//현재 위치의 셀을 얻는다.
		char c = Cell->Get(x, y);

		//셀이 아이템인 경우의 처리
		if (c == '+') {
			//아이템을 카운트가 완료된 것으로 표시한다.
			Cell->Set(x, y, c | 0x80);
			//상하좌우의 셀에 대해서도 재귀적으로 검사한다.
			return Count(x - 1, y) + Count(x + 1, y) + Count(x, y - 1) + Count(x, y + 1);
		}
		else
			//셀이 공백이라면
			//공백인 셀의 개수(1)를 반환한다.
			if (c == ' ') {
				return 1;
			}
		//그 이외의 경우(셀이 블록이나 벽인 경우)에는
		//공백인 셀의 개수(0)를 반환한다.
			else {
				return 0;
			}
	}

	//인접한 아이템을 없애는 처리
	void Erase(int x, int y) {
		//현재 위치으이 셀을 얻는다.
		char c = Cell->Get(x, y);

		//셀이 아이템인 경우의 처리
		if (c == (char)('+' | 0x80)) {
			//아이템을 사라질 아이템으로 표시한다.
			Cell->Set(x, y, c | 0x40);

			//상하좌우의 셀에 대해서도
			//아이템을 없애는 처리를 재귀적으로 수행한다.
			Erase(x - 1, y);
			Erase(x + 1, y);
			Erase(x, y - 1);
			Erase(x, y + 1);
		}
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				int texture = TEX_VOID;
				switch (c & 0x3f) {
				case '#': texture = TEX_OBJECT; break;
				case '+': texture = TEX_ITEM; break;
				case '=': texture = TEX_DROP_FLOOR; break;
				}
				if (c & 0x40) {
					float f = (float)Time / 30;
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, D3DXCOLOR(f, f, f, 1));
				}
				else {
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, COL_BLACK);
				}
			}
		}
		if (State == 1) {
			Game->Texture[Type == '#' ? TEX_OBJECT : TEX_ITEM]->Draw(
				CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 연결된 블록 없애기

#define CONNECTED_BLOCK_COUNT 5
#define CONNECTED_BLOCK_ERASE 2

// 스테이지
class CConnectedBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, State, Time;
	bool PrevLever, PrevButton;

	CConnectedBlockStage() : CStage(L"CONNECTED BLOCK") {
		Cell = new CCell();
	}

	virtual ~CConnectedBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"================"
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"================"
		);
		for (int y = 2; y<ys - 1; y++) {
			for (int x = 1; x<xs - 1; x++) {
				Cell->Set(x, y, '0' + Rand.Int31() % CONNECTED_BLOCK_COUNT);
			}
		}
		PrevLever = false;
		CX = 1;
		CY = 2;
		State = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//입력 상태
		if (State == 0) {
			//레버의 입력에 따라 커서를 상하좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && CX>1) CX--; else
					if (is->Right && CX<xs - 2) CX++; else
						if (is->Up && CY>2) CY--; else
							if (is->Down && CY<ys - 2) CY++;
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			//버튼을 누르면 커서 위치에 있는 블록을 없앤다.
			if (!PrevButton && is->Button[0]) {
				//커서 위치에 있는 셀을 얻는다.
				char c = Cell->Get(CX, CY);
				//셀이 공백이 아니고
				//같은 종류의 셀이 정해진 개수(2개)이상 인접해 있으면
				//블록을 삭제한다.
				if (
					c != ' ' &&
					Erase(CX, CY, c, 0x80) >= CONNECTED_BLOCK_ERASE
					) {
					//블록을 삭제한다.
					Erase(CX, CY, c | 0x80, 0x40);
					//삭제상태로 전환한다.
					Time = 0;
					State = 1;
				}
			}
			PrevButton = is->Button[0];

			//셀의 카운트 완료 표시를 지운다.
			for (int y = 2; y<ys - 1; y++) {
				for (int x = 1; x<xs - 1; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//삭제 상태
		if (State == 1) {
			//일정 시간이 경과하면 사라질 블록을 완전히 삭제한다.
			Time++;
			if (Time == 30) {
				//사라질 블록을 찾는다.
				for (int x = 1; x<xs - 1; x++) {
					for (int y = 2; y<ys - 1; y++) {
						//사라질 블록을 발견했을 때의 처리
						if (Cell->Get(x, y) & 0x40) {
							//사라질 블록보다 위에 있는 셀을 
							//1단씩 아래로 이동한다.
							for (int i = y; i>2; i--) {
								Cell->Set(x, i, Cell->Get(x, i - 1));
							}
							
							//가장 위쪽 단의 셀을 공백으로 변경한다.
							Cell->Set(x, 2, ' ');
						}
					}
				}
				//횡 이동상태로 전환한다.
				Time = 0;
				State = 2;
			}
		}

		//횡 이동상태
		if (State == 2) {
			//일정 시간이 경과하면
			//공백인 열보다 오른쪽 셀을 왼쪽으로 이동한다.
			Time++;
			if (Time == 30) {
				//공백인 열을 찾는다.
				for (int x = xs - 2; x >= 1; x--) {
					//위쪽 끝부터 아래쪽 끝까지
					//모든 셀이 공백인지를 검사한다.
					int y;
					for (y = 2; y<ys - 1 && Cell->Get(x, y) == ' '; y++);
					//모든 셀이 공백인 경우의 처리
					if (y == ys - 1) {
						//공백인 열보다 오른쪽의 셀을
						//1열씩 왼쪽으로 이동한다.
						for (int i = x; i<xs - 2; i++) {
							for (int j = 2; j<ys - 1; j++) {
								Cell->Set(i, j, Cell->Get(i + 1, j));
							}
						}
						//오른쪽 끝의 셀은 공백으로 변경한다.
						for (int j = 2; j<ys - 1; j++) {
							Cell->Set(xs - 2, j, ' ');
						}
					}
				}
				//입력상태로 전환한다.
				State = 0;
			}
		}

		return true;
	}

	//블록을 없애는 처리
	//인접한 같은 종류의 블록의 개수를 세는 처리도 함께 한다.
	int Erase(int x, int y, char c, int mask) {
		//현재 위치의 셀이 지정된 종류의 셀이라면
		//셀에 표시한다.
		if (Cell->Get(x, y) == c) {
			//지정된 비트를 설정하여
			//셀에 표시를 한다.
			Cell->Set(x, y, c | mask);
			//상하좌우의 셀에 대해서도 재귀적으로 검사하여
			//인접한 같은 종류의 셀의 개수를 반환한다.
			return
				1 +
				Erase(x - 1, y, c, mask) +
				Erase(x + 1, y, c, mask) +
				Erase(x, y - 1, c, mask) +
				Erase(x, y + 1, c, mask);
		}
		//현재 위치의 셀은 지정된 종류의 셀이 아니기 때문에
		//인접한 같은 종류의 셀의 개수로 0을 반환한다.
		return 0;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		Game->Texture[TEX_FILL]->Draw(
			CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				int texture = TEX_VOID;
				if (c == '=') {
					texture = TEX_DROP_FLOOR;
				}
				else
					if (c != ' ') {
						texture = TEX_BALL0 + (c & 0x3f) - '0';
					}
				if (c & 0x40) {
					float f = (float)Time / 30;
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, D3DXCOLOR(f, f, f, 1));
				}
				else {
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, COL_BLACK);
				}
			}
		}
	}
};

//==============================================================
// 블록을 끌어당겨 발사하기

#define DRAWN_AND_SHOT_BLOCK_COUNT 5
#define DRAWN_AND_SHOT_BLOCK_ERASE 3

// 스테이지
class CDrawnAndShotBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, BX, State, Time;
	char Block;
	bool PrevLever, PrevButton;

	CDrawnAndShotBlockStage() : CStage(L"DRAWN AND SHOT BLOCK") {
		Cell = new CCell();
	}

	virtual ~CDrawnAndShotBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"================"
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"================"
		);
		for (int y = 2; y<ys - 1; y++) {
			for (int x = 6; x<xs - 1; x++) {
				Cell->Set(x, y, '0' + Rand.Int31() % DRAWN_AND_SHOT_BLOCK_COUNT);
			}
		}
		PrevLever = false;
		CX = 1;
		CY = 2;
		Block = ' ';
		State = 0;
	}

	//스테이지의 이동 처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//입력상태
		if (State == 0) {
			//레버의 입력에 따라 캐릭터를 위아래로 움직인다.
			if (!PrevLever) {
				if (is->Up && CY>2) CY--; else
					if (is->Down && CY<ys - 2) CY++;
			}
			PrevLever = is->Up || is->Down;

			//버튼을 누르면 블록을 끌어당기거나 발사한다.
			if (!PrevButton && is->Button[0]) {
				//블록을 가지고 있지 않을 때에는 끌어당긴다.
				if (Block == ' ') {
					int x;
					for (x = 3; x<xs - 1 && Cell->Get(x, CY) == ' '; x++);
					//캐릭터의 오른쪽을 향해
					//블록 셀을 찾는다.
					if (x<xs - 1) {
						//끌어 당길 블록의 종류를 얻는다.
						Block = Cell->Get(x, CY);
						//셀을 공백으로 변경한다.
						Cell->Set(x, CY, ' ');
						//끌어당길 블록의 좌표를 설정한다.
						BX = x;
						//취득상태로 전환한다.
						State = 1;
					}
				}
				else {
					//블록을 가지고 있을 때에는 발사한다.
					int x;
					//캐릭터의 오른쪽이 공백일 때에는
					//블록을 발사한다.
					for (x = 3; x<xs - 1 && Cell->Get(x, CY) != ' '; x++);
					if (x<xs - 1) {
						//발사 상태로 전환한다.
						State = 2;
					}
				}
			}
			PrevButton = is->Button[0];
		}

		//취득상태
		if (State == 1) {
			//블록이 캐릭터의 오른쪽 옆에 도달하면
			//낙하상태로 전환한다.
			if (BX == CX + 1) {
				State = 3;
			}
			//캐릭터의 오른쪽 옆에 도달할 때까지는
			//블록을 왼쪽으로 이동한다.
			else {
				BX--;
			}
		}

		//발사상태
		if (State == 2) {
			//블록의 오른쪽 셀이 공백이 아니라면
			//블록을 현재 위치에 고정한다.
			if (Cell->Get(BX + 1, CY) != ' ') {
				//셀에 블록을 써넣는다.
				Cell->Set(BX, CY, Block);
				//가지고 있던 블록을 공백으로 변경한다.
				Block = ' ';
				//낙하상태로 전환한다.
				State = 3;
			}
			//블록의 오른쪽 셀이 공백인 한 계속해서
			//블록을 오른쪽으로 이동한다.
			else {
				BX++;
			}
		}

		//낙하상태
		if (State == 3) {
			//스테이지 안의 공백인 셀을 찾는다.
			for (int x = 3; x<xs - 1; x++) {
				for (int y = 2; y<ys - 1; y++) {
					//공백인 셀을 찾는 경우의 처리
					if (Cell->Get(x, y) == ' ') {
						//위에 있는 셀을 1단계 아래로 이동한다.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//가장 위쪽 단의 셀을 공백으로 변경한다.
						Cell->Set(x, 2, ' ');
					}
				}
			}
			//공백인 열을 찾는다.
			for (int x = 3; x<xs - 1; x++) {
				//위쪽 끝부터 아래쪽 끝까지 셀이 공백인지를 검사한다.
				int y;
				for (y = 2; y<ys - 1 && Cell->Get(x, y) == ' '; y++);
				//셀이 공백일 때의 처리
				if (y == ys - 1) {
					//공백인 열보다 왼쪽의 셀을 오른쪽으로 이동한다.
					for (int i = x; i>3; i--) {
						for (int j = 2; j<ys - 1; j++) {
							Cell->Set(i, j, Cell->Get(i - 1, j));
						}
					}
					//왼쪽 끝의 열을 공백으로 변경한다.
					for (int j = 2; j<ys - 1; j++) {
						Cell->Set(3, j, ' ');
					}
				}
			}
			//삭제판정 상태로 전환한다.
			State = 4;
		}

		//삭제 판정 상태
		if (State == 4) {
			//블록이 사라지지 않았을 때에는
			//초기상태로 전환한다.
			State = 0;
			//인접한 같은 종류의 블록을 찾는다.
			for (int y = 2; y<ys - 1; y++) {
				for (int x = 3; x<xs - 1; x++) {
					char c = Cell->Get(x, y);
					//같은 종류의 블록이 정해진 개수 이상 인접해 있으면
					//블록을 없앤다.
					if (
						!(c & 0x80) &&
						c != ' ' &&
						Erase(x, y, c, 0x80) >= DRAWN_AND_SHOT_BLOCK_ERASE
						) {
						//블록을 사라질 블록으로 표시하낟.
						Erase(x, y, c | 0x80, 0x40);
						//삭제상태로 전환한다.
						Time = 0;
						State = 5;
					}
				}
			}
			//카운트 완료 표시를 지운다.
			for (int y = 2; y<ys - 1; y++) {
				for (int x = 3; x<xs - 1; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//삭제 상태
		if (State == 5) {
			//일정 시간이 경과하면 사라질 블록을 완전히 삭제하다.
			Time++;
			if (Time == 30) {
				//사라질 블록을 찾는다.
				for (int x = 3; x<xs - 1; x++) {
					for (int y = 2; y<ys - 1; y++) {
						//사라질 블록을 찾으면
						//셀을 공백으로 변경한다.
						if (Cell->Get(x, y) & 0x40) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//낙하상태로 전환한다.
				State = 3;
			}
		}

		return true;
	}

	//블록을 없애는 처리
	//인접한 같은 종류의 블록의 개수를 세는 처리도 같이 한다.
	int Erase(int x, int y, char c, int mask) {
		//현재 위치의 셀이 지정된 종류의 셀이라면
		//셀에 표시한다.
		if (Cell->Get(x, y) == c) {
			//지정된 비트를 설정하여 셀에 표시를 한다.
			Cell->Set(x, y, c | mask);
			//상하좌우의 셀에 대해서도 재귀적으로 검사하여
			//인접한 같은 종류의 셀의 개수를 반환한다.
			return
				1 +
				Erase(x - 1, y, c, mask) +
				Erase(x + 1, y, c, mask) +
				Erase(x, y - 1, c, mask) +
				Erase(x, y + 1, c, mask);
		}
		//현재 위치의 셀은 지정된 종류의 셀이 아니기 때문에
		//인접한 같은 종류의 셀의 개수로 0을 반환한다.
		return 0;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				int texture = TEX_VOID;
				if (c == '=') {
					texture = TEX_DROP_FLOOR;
				}
				else
					if (c != ' ') {
						texture = TEX_BALL0 + (c & 0x3f) - '0';
					}
				if (c & 0x40) {
					float f = (float)Time / 30;
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, D3DXCOLOR(f, f, f, 1));
				}
				else {
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, COL_BLACK);
				}
			}
		}
		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
		if (Block != ' ') {
			Game->Texture[TEX_BALL0 + Block - '0']->Draw(
				BX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 블록을 밀어서 떨어뜨려 모으기

#define PUSHED_AND_COLLECTED_BLOCK_COUNT 5
#define PUSHED_AND_COLLECTED_BLOCK_ERASE 3

// 스테이지
class CPushedAndCollectedBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, BX, State, Time;
	char Block;
	bool PrevLever, PrevButton;

	CPushedAndCollectedBlockStage() : CStage(L"PUSHED AND COLLECTED BLOCK") {
		Cell = new CCell();
	}

	virtual ~CPushedAndCollectedBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"================"
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"================"
			"=              ="
			"================"
		);
		for (int y = 4; y<ys - 3; y++) {
			for (int x = 1; x<xs - 6; x++) {
				Cell->Set(x, y, '0' + Rand.Int31() % PUSHED_AND_COLLECTED_BLOCK_COUNT);
			}
		}
		PrevLever = false;
		CX = xs - 2;
		CY = 2;
		State = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//입력상태
		if (State == 0) {
			//레버의 입력에 따라 캐릭터를 위아래로 움직인다.
			if (!PrevLever) {
				if (is->Up && CY>2) CY--; else
					if (is->Down && CY<ys - 4) CY++;
			}
			PrevLever = is->Up || is->Down;

			//버튼을 누르면
			//캐릭터와 같은 높이에 있는 블록을 밀어 떨어뜨린다.
			if (!PrevButton && is->Button[0]) {
				//캐릭터와 같은 높이에 있는 셀 중에서
				//왼쪽 끝의 셀을 얻는다.
				char c = Cell->Get(1, CY);
				//셀이 공백이 아니라면 블록을 밀어 떨어뜨린다.
				if (c != ' ') {
					//모인 블록의 개수를 센다.
					int x;
					for (x = 1; Cell->Get(x, ys - 2) != ' '; x++);
					//모인 블록의 오른쪽 끝에
					//새로운 블록을 배치한다.
					Cell->Set(x, ys - 2, c);
					//블록을 밀어 떨어뜨린 단의 셀을
					//왼쪽으로 1열씩 이동한다.
					if (x == PUSHED_AND_COLLECTED_BLOCK_ERASE) {
						Time = 0;
						State = 1;
					}
					for (x = 1; x<xs - 2; x++) {
						Cell->Set(x, CY, Cell->Get(x + 1, CY));
					}
				}
			}
			PrevButton = is->Button[0];

			//공백인 셀을 찾는다.
			for (int x = 1; x<xs - 1; x++) {
				for (int y = 2; y<ys - 3; y++) {
					//공백만 셀을 찾으면 위에 있는 셀을 떨어뜨린다.
					if (Cell->Get(x, y) == ' ') {
						//공백의 셀의 위에 있는 셀을
						//1단씩 아래로 이동한다.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//가장 위쪽 단의 셀을 공백으로 변경한다.
						Cell->Set(x, 2, ' ');
					}
				}
			}
		}

		//삭제상태
		if (State == 1) {
			//일정 시간이 경과하면 모인 블록을 검사한다.
			Time++;
			if (Time == 30) {
				//모인 블록이 모두 같은 종류인지를 검사한다.
				char c = Cell->Get(1, ys - 2);
				int x, y;
				for (x = 2; Cell->Get(x, ys - 2) == c; x++);

				//모두 같은 종류가 아니라면
				//모인 블록을 스테이지의 위쪽 영역으로 되돌린다.
				if (x<1 + PUSHED_AND_COLLECTED_BLOCK_ERASE) {
					//스테이지의 셀을 1단씩 위로 이동한다.
					for (y = 2; y<ys - 4; y++) {
						for (x = 1; x<xs - 1; x++) {
							Cell->Set(x, y, Cell->Get(x, y + 1));
						}
					}
					//가장 아래 단으로 모인 블록을 이동한다.
					for (x = 1; x<xs - 1; x++) {
						Cell->Set(x, ys - 4, Cell->Get(x, ys - 2));
					}
				}
				//블록을 모으는 영역을 공백으로 변경한다.
				for (x = 1; x<xs - 1; x++) {
					Cell->Set(x, ys - 2, ' ');
				}
				//입력상태로 전환한다.
				State = 0;
			}
		}
		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				int texture = TEX_VOID;
				if (c == '=') {
					texture = TEX_DROP_FLOOR;
				}
				else
					if (c != ' ') {
						texture = TEX_BALL0 + c - '0';
					}
				Game->Texture[texture]->Draw(
					x*sw, y*sh, sw, sh,
					0, 0, 1, 1, COL_BLACK);
			}
		}
		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// 떨어지는 블록을 받아서 쌓기

#define CAUGHT_AND_PILED_BLOCK_COUNT 5
#define CAUGHT_AND_PILED_BLOCK_ERASE 3

// 스테이지
class CCaughtAndPiledBlockStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, State, Time, DropTime;
	char Block;
	bool PrevLever, PrevButton;

	CCaughtAndPiledBlockStage() : CStage(L"CAUGHT AND PILED BLOCK") {
		Cell = new CCell();
	}

	virtual ~CCaughtAndPiledBlockStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"================"
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"=              ="
			"================"
		);
		PrevLever = false;
		CX = xs / 2;
		CY = 7;
		Block = ' ';
		DropTime = 0;
		State = 0;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//입력상태
		if (State == 0) {
			//레버의 입력에 따라 캐릭터를 좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && CX>1) CX--; else
					if (is->Right && CX<xs - 2) CX++;
			}
			PrevLever = is->Left || is->Right;

			//블록을 가지고 있을 때 버튼을 누르면
			//캐릭터의 바로 아래에 블록을 쌓는다.
			if (!PrevButton && is->Button[0] && Block != ' ') {
				//캐릭터의 바로 아래쪽 셀이 공백이면
				//블록을 놓은 후
				//낙하 상태로 전환한다.
				if (Cell->Get(CX, ys - 4) == ' ') {
					Cell->Set(CX, ys - 4, Block);
					Block = ' ';
					State = 1;
				}
			}
			PrevButton = is->Button[0];
			//일정시간마다 블록을 낙하시킨다.
			DropTime++;
			if (DropTime == 60) {
				DropTime = 0;
				//캐릭터가 블록을 가지고 있지 않을 때
				//바로 위에 블록이 있으면
				//블록을 받는다.
				if (Block == ' ' && Cell->Get(CX, CY - 1) != ' ') {
					Block = Cell->Get(CX, CY - 1);
				}
				//스테이지 안의 셀을 1단씩 아래로 이동한다.
				for (int y = CY - 1; y>2; y--) {
					for (int x = 1; x<xs - 1; x++) {
						Cell->Set(x, y, Cell->Get(x, y - 1));
					}
				}
				//가장 위쪽 단의 셀은 공백으로 변경한다.
				for (int x = 1; x<xs - 1; x++) {
					Cell->Set(x, 2, ' ');
				}
				//가장 위쪽 단의 셀에 새로운 블록을 1개 써넣는다.
				//좌우의 위치와 블록의  종류는 임의로 결정한다.
				Cell->Set(Rand.Int31() % (xs - 2) + 1, 2, '0' + Rand.Int31() % CAUGHT_AND_PILED_BLOCK_COUNT);
			}
		}

		//낙하상태
		if (State == 1) {
			//스테이지 위쪽의 셀에서부터 공백인 셀을 찾는다.
			for (int x = 1; x<xs - 1; x++) {
				for (int y = CY + 1; y<ys - 1; y++) {
					//공백인 셀을 찾으면 위에 있는 셀을 떨어뜨린다.
					if (Cell->Get(x, y) == ' ') {
						//위에 있는 셀을 1단씩 아래로 이동한다.
						for (int i = y; i>CY + 1; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//가장 위쪽 단의 셀은 공백으로 변경한다.
						Cell->Set(x, CY + 1, ' ');
					}
				}
			}

			//블록이 없어지지 않았으면 입력상태로 전환한다.
			State = 0;
			//스테이지의 아래쪽 셀에 대하여
			//같은 종류의 블록이 연결되어 있는지를 검사한다.
			for (int y = CY + 1; y<ys - 1; y++) {
				for (int x = 1; x<xs - 1; x++) {
					//공백인 셀은 검사하지 않는다.
					char c = Cell->Get(x, y) & 0x7f;
					if (c == ' ') continue;
					//블록의 연결을 검사하는 방향
					//(아래쪽, 오른쪽, 왼쪽 아래, 오른쪽 아래)
					static const int
						vx[] = { 0, 1, -1, 1 },
						vy[] = { 1, 0, 1, 1 };
					//각 방향에 대해
					//같은 종류의 블록이 연결되어 있는지를 검사한다.
					for (int v = 0; v<4; v++) {
						//연결되어 있는 같은 종류의 블록의 개수를 검사한다.
						int count = 0;
						for (
							int i = x, j = y;
							1 <= i && i<xs - 1 && CY + 1 <= j && j<ys - 1;
							i += vx[v], j += vy[v], count++
							) {
							if (c != (Cell->Get(i, j) & 0x7f)) break;
						}
						//정해진 개수 이상의 블록이 연결되어 있으면
						//사라질 블록으로 표시한다.
						if (count >= CAUGHT_AND_PILED_BLOCK_ERASE) {
							for (int i = x, j = y, k = 0; k<count; i += vx[v], j += vy[v], k++) {
								Cell->Set(i, j, Cell->Get(i, j) | 0x80);
							}
							Time = 0;
							State = 2;
						}
					}
				}
			}
		}

		//삭제상태
		if (State == 2) {
			//일정 시간이 경과하면
			//사라질 블록을 완전히 삭제한다.
			Time++;
			if (Time == 30) {
				//사라질 블록을 찾는다.
				for (int y = CY + 1; y<ys - 1; y++) {
					for (int x = 1; x<xs - 1; x++) {
						//찾은 셀을 공백으로 변경한다.
						if (Cell->Get(x, y) & 0x80) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//낙하 상태로 전환한다.
				State = 1;
			}
		}

		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		Game->Texture[TEX_FILL]->Draw(
			sw, (ys - 4)*sh, sw*(xs - 2), sh * 3,
			0, 0, 1, 1, COL_LGRAY);
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				int texture = TEX_VOID;
				if (c == '=') {
					texture = TEX_DROP_FLOOR;
				}
				else
					if (c != ' ') {
						texture = TEX_BALL0 + (c & 0x3f) - '0';
					}
				if (c & 0x80) {
					float f = (float)Time / 30;
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, D3DXCOLOR(f, f, f, 1));
				}
				else {
					Game->Texture[texture]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, COL_BLACK);
				}
			}
		}
		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
		if (Block != ' ') {
			Game->Texture[TEX_BALL0 + Block - '0']->Draw(
				CX*sw, (CY - 1)*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 바닥에 표시하여 블록 없애기

// 스테이지
class CMarkedAndSunkBlockStage : public CStage {
public:
	CCell3D * Cell;
	int CX, CY, CZ, State, Time, DropTime;
	bool PrevLever, PrevButton;

	CMarkedAndSunkBlockStage() : CStage(L"MARKED AND SUNK BLOCK") {
		Cell = new CCell3D(5, 2, 12);
	}
	virtual ~CMarkedAndSunkBlockStage() {
		delete Cell;
	}
	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize(), zs = Cell->GetZSize();
		for (int z = 0; z<zs; z++) {
			for (int x = 0; x<xs; x++) {
				Cell->Set(x, 1, z, ' ');
				Cell->Set(x, 0, z, '=');
			}
		}
		CX = xs / 2;
		CY = 1;
		CZ = 0;
		PrevLever = true;
		PrevButton = true;
		DropTime = 0;
		State = 0;
	}
	//스테이지의 이동 처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize(), zs = Cell->GetZSize();

		//입력 상태
		if (State == 0) {
			//레버의 입력에 따라 커서를 전후 좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && CX>0) CX--;
				if (is->Right && CX<xs - 1) CX++;
				if (is->Up && CZ<zs - 1) CZ++;
				if (is->Down && CZ>0) CZ--;
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			//버튼을 눌렀을 때의 처리
			if (!PrevButton) {
				//버튼 0을 누르면 커서의 아래에 있는 바닥을 표시한다.
				if (is->Button[0]) {
					switch (Cell->Get(CX, 0, CZ)) {
						//바닥에 표시가 되어 있지 않으면 표시를 한다.
					case '=': Cell->Set(CX, 0, CZ, '-'); break;
						//바닥에 표시가 되어 있으면 표시를 없앤다.
					case '-': Cell->Set(CX, 0, CZ, '='); break;
					}
				}
				else
					//버튼 1을 누르면
					//표시된 바닥의 위에 있는 블록을 빠뜨려 없앤다.
					if (is->Button[1]) {
						//표시된 블록을 찾는다.
						for (int z = 0; z<zs; z++) {
							for (int x = 0; x<xs; x++) {
								if (Cell->Get(x, 1, z) == '#' && Cell->Get(x, 0, z) == '-') {
									//블록을 사라질 블록으로 변경한다.
									Cell->Set(x, 1, z, '+');
									//삭제상태로 전환한다.
									Time = 0;
									State = 1;
								}
							}
						}
					}
			}
			PrevButton = is->Button[0] || is->Button[1];
			//일정시간마답 블로을 앞쪽으로 이동한다.
			DropTime++;
			if (DropTime == 60) {
				DropTime = 0;
				//스테이지 안의 셀을 앞쪽으로 이동한다.
				for (int z = 0; z<zs - 1; z++) {
					for (int x = 0; x<xs; x++) {
						Cell->Set(x, 1, z, Cell->Get(x, 1, z + 1));
					}
				}
				//가장 안쪽의 셀을 공백으로 변경한다.
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, 1, zs - 1, ' ');
				}
				//가장 안쪽의 셀에 블록을 1개 출현시킨다.
				//좌우 방향의 위치는 임의로 결정한다.
				Cell->Set(Rand.Int31() % xs, 1, zs - 1, '#');
			}
		}

		//삭제상태
		if (State == 1) {
			//일정 시간이 경과하면 사라질 블록을 완전히 없앤다.
			Time++;
			if (Time == 30) {
				//사라질 블록을 찾는다.
				for (int z = 0; z<zs; z++) {
					for (int x = 0; x<xs; x++) {
						//사라질 블록을 발견하면 공백으로 변경한다.
						if (Cell->Get(x, 1, z) == '+') {
							Cell->Set(x, 1, z, ' ');
						}
					}
				}
				//입력상태로 전환한다.
				State = 0;
			}
		}

		return true;
	}
	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize(), zs = Cell->GetZSize();

		IDirect3DDevice9* device = Game->GetGraphics()->GetDevice();
		D3DXMATRIX mat_view, mat_view_old;
		D3DXVECTOR3 vec_from = D3DXVECTOR3(8, 10, -6);
		D3DXVECTOR3 vec_lookat = D3DXVECTOR3(xs*0.5f, ys*0.5f, zs*0.5f - 2);
		D3DXVECTOR3 vec_up = D3DXVECTOR3(0, 1, 0);
		D3DXMatrixLookAtLH(&mat_view, &vec_from, &vec_lookat, &vec_up);
		device->GetTransform(D3DTS_VIEW, &mat_view_old);
		device->SetTransform(D3DTS_VIEW, &mat_view);

		for (int z = zs - 1; z >= 0; z--) {
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					if (x == CX && y == CY && z == CZ) {
						Game->Mesh[MESH_BALL_BLACK]->Draw(CX, CY, CZ, 1, 1, 1, 0, 0, 0, TO_NONE, 1, 0);
					}
					switch (Cell->Get(x, y, z)) {
					case '#':
						Game->Mesh[MESH_CUBE_GRAY]->Draw(x, y, z, 1, 1, 1, 0, 0, 0, TO_NONE, 0.5f, 0);
						break;
					case '+':
						Game->Mesh[MESH_CUBE_GRAY]->Draw(x, y - (float)Time / 30, z, 1, 1, 1, 0, 0, 0, TO_NONE, 0.5f, 0);
						break;
					case '-':
						Game->Mesh[MESH_CUBE_GRAY]->Draw(x, y, z, 1, 1, 1, 0, 0, 0, TO_NONE, 1, 0);
						break;
					case '=':
						Game->Mesh[MESH_CUBE_WHITE]->Draw(x, y, z, 1, 1, 1, 0, 0, 0, TO_NONE, 1, 0);
						break;
					}
				}
			}
		}

		device->SetTransform(D3DTS_VIEW, &mat_view_old);
	}
};

