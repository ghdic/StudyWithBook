//==============================================================
// 선로 연결하기

//캐릭터를 진행시키기 위한 정수
static const int
CONNECTED_RAIL_TIME = 60,	//캐릭터가 1개의 셀을 통과하는 시간
CONNECTED_RAIL_VX[] = { 1, 0, -1, 0 },	//이동 방향(0-3)을
CONNECTED_RAIL_VY[] = { 0, 1, 0, -1 },	//X방향과 Y방향의 좌표로 변환하기 위한 테이블
//선로에 들어갈 때의 이동방향(0-3)을
//선로에서 빠져나올 때의 이동 방향으로 변환하기 위한 테이블
//(4는 이동 방향으로 진입 할 수 없는 것을 표시한다)
//집합에서 원소의 위치는 arr[dir]값이 밖으로 나올때로 설정한다(dir은 들어갈때)  
CONNECTED_RAIL_DIR[][4] = {
	{ 4, 4, 4, 4 },
{ 4, 4, 1, 0 },{ 1, 4, 4, 2 },
{ 3, 2, 4, 4 },{ 4, 0, 3, 4 },
{ 0, 4, 2, 4 },{ 4, 1, 4, 3 },
{ 0, 1, 2, 3 }
};

// 사람
// 	3
// 	↑
//2←O→0
//	↓
//	1
//Dir값
class CConnectedRailMan : public CMover {
public:
	CCell * Cell;
	int CX, CY, Dir, Time;

	CConnectedRailMan(CCell* cell, int cx, int cy, int dir)
		: Cell(cell), CX(cx), CY(cy), Dir(dir)
	{
		X = CX + 1.0f / 3;
		Y = CY + 1.0f / 3;
		Time = CONNECTED_RAIL_TIME / 2;
	}
	//캐릭터의 이동처리
	virtual bool Move(const CInputState* is) {
		//일정 시간이 경과할 때까지 타이머를 갱신한다.
		if (Time<CONNECTED_RAIL_TIME) {
			Time++;
		}
		//일정 시간이 경과하면 이동 방향을 갱신한다.
		else {
			//이동 바향의 셀 좌표를 계산한다.
			int
				cx = CX + CONNECTED_RAIL_VX[Dir],
				cy = CY + CONNECTED_RAIL_VY[Dir];
			//이동 방향이 스테이지 안쪽인지 확인한다.
			if (
				cx >= 0 && cx<Cell->GetXSize() &&
				cy >= 0 && cy<Cell->GetYSize()
				) {
				//이동 방향의 선로에 현재의 이동 방향으로 진입한 경우에 따른
				//새로운 이동 방향을 얻는다.
				int dir = CONNECTED_RAIL_DIR[Cell->Get(cx, cy) - '0'][Dir];
				//선로에 진입할 수 있는지를 검사한다.
				if (dir != 4) {
					//진입할 수 있으면 셀좌표와 이동 방향을 갱신하고
					//타이머를 설정한다.
					CX = cx;
					CY = cy;
					Dir = dir;
					Time = 0;
				}
			}
		}

		//현재 캐릭터가 있는 셀을 얻는다.
		char c = Cell->Get(CX, CY) - '0' - 1;
		//굽은 선로의 경우
		if (0 <= c && c <= 3) {
			//캐릭터가 돌아가게 하기 위한 정수
			static const int
				//회전중심의 상대좌표
				rx[] = { 1, 0, 0, 1 },
				ry[] = { 1, 1, 0, 0 },
				//회전을 시작하는 각도
				rad[][4] = {
					{ 2, 3, 4, 4 },{ 4, 3, 0, 4 },
			{ 4, 4, 0, 1 },{ 2, 4, 4, 1 }
			},
				//회전방향
				vrad[][4] = {
					{ 1, -1, 0, 0 },{ 0, 1, -1, 0 },
			{ 0, 0, 1, -1 },{ -1, 0, 0, 1 }
			};
			//타이머의 값에 따라
			//선로를 따라 부드럽게 돌아가듯이
			//캐릭터의 그림좌표를 설정한다.
			float f = (rad[c][Dir] + vrad[c][Dir] * (float)Time / CONNECTED_RAIL_TIME)*D3DX_PI / 2;
			X = CX + rx[c] - 1.0f / 6 + cosf(f) / 2;
			Y = CY + ry[c] - 1.0f / 6 + sinf(f) / 2;
		}
		//직선의 선로 또는 십자형 선로의 경우
		else {
			//타이머의 값에 따라
			//선로를 따라 부드럽게 직진하듯이
			//캐릭터의 그림좌표를 설정한다.
			float f = (float)Time / CONNECTED_RAIL_TIME - 0.5f;
			X = CX + 1.0f / 3 + CONNECTED_RAIL_VX[Dir] * f;
			Y = CY + 1.0f / 3 + CONNECTED_RAIL_VY[Dir] * f;
		}
		return true;
	}

	virtual void Draw() {}
};

// 스테이지
class CConnectedRailStage : public CStage {
public:
	CCell * Cell;
	CCell* PathCell;
	int CX, CY, VX, VY, State;
	CConnectedRailMan* Man;

	CConnectedRailStage() : CStage(L"CONNECTED RAIL") {
		Cell = new CCell(6, 4);
		PathCell = new CCell(Cell->GetXSize(), Cell->GetYSize());
	}
	virtual ~CConnectedRailStage() {
		delete Cell;
		delete PathCell;
	}

	//스테이지의 초기화 처리
	virtual void Init() {
		Cell->Init(
			(char*)
			"152152"
			"457366"
			"157213"
			"453430"
		);
		//커서의 좌표를 설정한다.
		CX = Cell->GetXSize() - 1;
		CY = Cell->GetYSize() - 1;
		//상태를 전환한다.
		State = 0;
		//캐릭터를 생성한다.
		Man = new CConnectedRailMan(Cell, 1, 0, 0);
	}

	//스테이지의 이동 처리
	virtual bool Move(const CInputState* is) {
		//입력 상태의 처리
		if (State == 0) {
			//레버를 왼쪽으로 입력한 경우에
			//커서의 위치가 스테이지의 왼쪽 끝이 아니라면
			//커서를 왼쪽으로 이동시킨다.
			if (is->Left && CX>0) {
				//이동 방향과 타이머를 설정하고
				//이동 상태로 전환한다.
				VX = -1;
				VY = 0;
				Time = 0;
				State = 1;
			}
			if (is->Right && CX<Cell->GetXSize() - 1) {
				VX = 1;
				VY = 0;
				Time = 0;
				State = 1;
			}
			if (is->Up && CY>0) {
				VX = 0;
				VY = -1;
				Time = 0;
				State = 1;
			}
			if (is->Down && CY<Cell->GetYSize() - 1) {
				VX = 0;
				VY = 1;
				Time = 0;
				State = 1;
			}
		}
		//이동 상태의 처리
		if (State == 1) {
			//타이머를 갱신한다.
			Time++;
			//타이머가 일정 값에 도달하면
			//커서와 선로의 위치를 바꿔 넣는다.
			if (Time == 10) {
				//바꿔 넣을 선로의 위에 캐릭터가 있다면
				//캐릭터도 새로운 위치로 변경한다.
				if (Man->CX == CX + VX && Man->CY == CY + VY) {
					Man->CX = CX;
					Man->CY = CY;
				}
				//커서와 선로의 셀을 바꿔 넣는다.
				Cell->Swap(CX, CY, CX + VX, CY + VY);
				//커서의 좌표를 갱신한다.
				CX += VX;
				CY += VY;
				//입력 상태로 전환한다.
				State = 0;
			}
		}
		return true;
	}

	//스테이지의 그림 처리
	virtual void Draw() {
		//진로예측 셀(진로예측용 셀)을 0으로 초기화한다.
		PathCell->Clear(0);
		//캐릭터의 현재 위치와 이동 방향을
		//진로예측의 초기 위치 및 초기 방향으로 설정한다.
		int cx = Man->CX, cy = Man->CY, dir = Man->Dir;
		//동일한 셀을 동일한 이동 방향으로 통과할 때까지 진로예측을 계속한다.
		while (!(PathCell->Get(cx, cy)&(1 << dir))) {
			//현재의 셀에 현재의 이동 방향을 써넣는다.
			PathCell->Set(cx, cy, PathCell->Get(cx, cy) | (1 << dir));
			//다음 셀로 이동한다.
			cx += CONNECTED_RAIL_VX[dir],
				cy += CONNECTED_RAIL_VY[dir];
			//스테이지의 밖으로 나가게 되면 진로예측을 종료한다.
			if (
				cx<0 || cx >= Cell->GetXSize() ||
				cy<0 || cy >= Cell->GetYSize()
				) break;
			//선로의 종류에 따라 이동 방향을 변화시킨다.
			//선로에 진입 할 수 없는 경우에는 진로예측을 종료한다.
			int ndir = CONNECTED_RAIL_DIR[Cell->Get(cx, cy) - '0'][dir];
			if (ndir == 4) break;
			dir = ndir;
		}

		//셀의 개수와 화면의 해상도에 따라
		//그림을 그릴 크기를 결정한다.
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / (ys + 1);

		//스테이지의 모든 셀에 대해 처리를 한다.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				//셀의 종류를 얻는다.
				char c = Cell->Get(x, y) - '0';
				//그림좌표를 계산한다.
				float
					dx = x, dy = y + 1,
					f = Time * 0.1f, vx = VX * f, vy = VY * f;
				//이동상태라면
				//커서를 부드럽게 움직이기 위해
				//그림좌표를 조정한다.
				if (State == 1 && x == CX && y == CY) {
					dx += vx;
					dy += vy;
				}
				//이동상태라면
				//선로를 부드럽게 움직이기 위하여
				//그림좌표를 조정한다.
				else
					if (State == 1 && x == CX + VX && y == CY + VY) {
						dx -= vx;
						dy -= vy;
					}
				//셀 좌표를 화면상의 좌표로 변환한다.
				dx *= sw;
				dy *= sh;
				//진로예측 셀을 검사한다.
				char pc = PathCell->Get(x, y);
				//셀이 예측한 진로인 경우에는
				//선로를 그리기 전에 회색으 사각형을 그려서 예측한 진로임을 나타낸다.
				if (pc) {
					//십자형의 선로를 세로 방향(위에서 아래로 또는 아래에서 위로)으로만
					//캐릭터가 통과하는 경우의 그림 처리
					if (c == 7 && !(pc & 0x05)) {
						Game->Texture[TEX_FILL]->Draw(
							dx + sw / 3, dy, sw / 3, sh,
							0, 0, 1, 1, COL_LGRAY);
					}
					//십자형 선로를 가로 방향(왼쪽에서 오른쪽 또는 오른쪽에서 왼쪽)으로만
					//캐릭터가 통과하는 경우의 그림 처리
					else
						if (c == 7 && !(pc & 0x0a)) {
							Game->Texture[TEX_FILL]->Draw(
								dx, dy + sh / 3, sw, sh / 3,
								0, 0, 1, 1, COL_LGRAY);
						}
					//캐릭터가 굽은 선로 또는 직선의 선로를 통과하는 경우,
					//또는 십자형의 선로를 가로와 세로 방향으로 통과하는 경우의 그림 처리
						else {
							Game->Texture[TEX_FILL]->Draw(
								dx, dy, sw, sh,
								0, 0, 1, 1, COL_LGRAY);
						}
				}
				//선로 또는 커서의 이미지를 그린다.
				//회색의 진로에 겹쳐서 선로의 그래픽을 그린다.
				Game->Texture[TEX_RAIL0 + c]->Draw(
					dx, dy, sw, sh,
					0, 0, 1, 1, COL_BLACK);
			}
		}
		//캐릭터의 그림좌표
		float dx = Man->X, dy = Man->Y + 1;
		//커서가 이동상태이고
		//캐릭터가 움직이고 있는 선로의 위에 있는 경우에는
		//선로와 함께 움직이도록 그림좌표를 설정한다.
		if (State == 1 && Man->CX == CX + VX && Man->CY == CY + VY) {
			dx -= VX * Time*0.1f;
			dy -= VY * Time*0.1f;
		}
		//캐릭터 이미지를 그린다.
		Game->Texture[TEX_MAN]->Draw(
			dx*sw, dy*sh, sw / 3, sh / 3, 0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// 파이프 연결하기

#define CONNECTED_PIPE_COUNT 7

// 스테이지
class CConnectedPipeStage : public CStage {
	CCell* Cell;
	CCell* PathCell;
	int CX, CY, NewPipe;
	bool PrevLever, PrevButton;
public:
	CConnectedPipeStage() : CStage(L"CONNECTED PIPE") {
		Cell = new CCell();
		PathCell = new CCell();
	}
	virtual ~CConnectedPipeStage() {
		delete Cell;
		delete PathCell;
	}

	virtual void Init() {
		Cell->Init((char*)
			"                "
			"################"
			"#+            +#"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#+            +#"
			"################"
		);
		CX = Cell->GetXSize() / 2;
		CY = Cell->GetYSize() / 2;
		PrevLever = true;
		PrevButton = true;
		NewPipe = Rand.Int31() % CONNECTED_PIPE_COUNT + 1;
	}

	//스테이지의 이동처리
	virtual bool Move(const CInputState* is) {
		//레버의 입력에 따라 커서를 이동시킨다.
		int cx = CX, cy = CY;
		if (!PrevLever) {
			//레버의 입력방향에 따라 새로운 좌표를 구한다.
			if (is->Left) cx--; else
				if (is->Right) cx++; else
					if (is->Up) cy--; else
						if (is->Down) cy++;
			//새로운 좌표의 셀이 벽이나 액체의 시작점이 아니라면
			//커서를 새로운 좌표로 이동시킨다.
			char c = Cell->Get(cx, cy);
			if (c != '#' && c != '+') {
				CX = cx;
				CY = cy;
			}
		}
		//직전의 레버의 입력을 저장한다.
		PrevLever = is->Left | is->Right | is->Up | is->Down;
		//버튼입력이 있는 경우에는
		//커서의 위치에 새로운 파이프를 놓는다.
		if (!PrevButton && is->Button[0]) {
			//셀에 새로운 파이프 종류를 써넣는다.
			Cell->Set(cx, cy, '0' + NewPipe);
			//다음 파이프의 종류를 무작위로 결정한다.
			NewPipe = Rand.Int31() % CONNECTED_PIPE_COUNT + 1;
		}
		//직전의 버튼입력을 저장한다.
		PrevButton = is->Button[0];
		return true;
	}

	//스테이지의 그림 처리
	virtual void Draw() {
		//셀의 크기
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//진로예측 셀을 클리어한다.
		PathCell->Clear(0);
		//스테이지 안에 있는 모든 액체의 시작점에 대해
		//시작점에서부터 파이프를 타고 흐르는 액체의 진로를 예측한다.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				//액체의 시작점 셀을 찾는다.
				if (Cell->Get(x, y) == '+') {
					//모든 방향(상하좌우)에 대해서
					//진로를 예측한다.
					for (int i = 0; i<4; i++) {
						//초기 위치와 이동 방향을 설정한다.
						int cx = x, cy = y, dir = i;
						//동일한 셀을 동일한 이동 방향으로 통과할 때까지
						//진로의 예측을 계속한다.
						while (!(PathCell->Get(cx, cy)&(1 << dir))) {
							//현재의 셀에 현재 이동 방향을 써놓는다.
							PathCell->Set(cx, cy, PathCell->Get(cx, cy) | (1 << dir));
							//다음 셀로 이동한다.
							cx += CONNECTED_RAIL_VX[dir],
								cy += CONNECTED_RAIL_VY[dir];
							char c = Cell->Get(cx, cy) - '0';
							//파이프 이외의 셀(벽, 액체의 시작점, 공백인 셀)에
							//닿은 경우에는 진로의 예측을 종료한다.
							if (c<0 || c >= CONNECTED_PIPE_COUNT + 1) break;
							//파이프의 종류에 따라 이동 방향을 변화시킨다.
							//파이프로 들어올 수 없는 경우에는 진로의 예측을 종료한다.
							int ndir = CONNECTED_RAIL_DIR[c][dir];
							if (ndir == 4) break;
							dir = ndir;
						}
					}
				}
			}
		}
		//그리는 크기를 계산한다.
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		//스테이지의 모든 셀을 그린다.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				//셀의 종류를 얻는다.
				char c = Cell->Get(x, y);
				//셀의 종류에 따라 그래픽(액체의 시작점, 벽)을 선택한다.
				int t = TEX_VOID;
				if (c == '+') {
					t = TEX_CONVEYOR1;
				}
				else
					if (c == '#') {
						t = TEX_FLOOR;
					}
					else
						//셀이 파이프인 경우
						if ('0' <= c && c<'0' + CONNECTED_PIPE_COUNT + 1) {
							//파이프의 형태에 따른 그래픽을 선택한다.
							t = TEX_RAIL0 + c - '0';
							//진로예측 셀을 조사한다.
							char pc = PathCell->Get(x, y);
							//셀이 예측한 진로인 경우에는
							//파이프르 그리기 적에 회색 사각형을 그려
							//진로인 것을 표시한다.
							if (pc) {
								//십자형 파이프를 세로(위에서 아래 또는 아래에서 위)로만
								//지나는 경우
								if (c == '7' && !(pc & 0x05)) {
									Game->Texture[TEX_FILL]->Draw(
										x*sw + sw / 3, y*sh, sw / 3, sh,
										0, 0, 1, 1, COL_LGRAY);
								}
								//십자형 파이프를 가로(왼쪽에서 오른쪽 또는 오른쪽에서 왼쪽)으로만
								//지나는 경우
								else
									if (c == '7' && !(pc & 0x0a)) {
										Game->Texture[TEX_FILL]->Draw(
											x*sw, y*sh + sh / 3, sw, sh / 3,
											0, 0, 1, 1, COL_LGRAY);
									}
								//굽은 파이프, 직선인 파이프를 지나가는 경우
								//또는 십자형 파이프를 가로와 세로로 모두 지나는 경우
									else {
										Game->Texture[TEX_FILL]->Draw(
											x*sw, y*sh, sw, sh,
											0, 0, 1, 1, COL_LGRAY);
									}
							}
						}
				//파이프를 그린다.
				Game->Texture[t]->Draw(x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
			}
		}
		//새로운 파이프를 그린다.
		Game->Texture[TEX_RAIL0 + NewPipe]->Draw(CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
	}
};

//==============================================================
// 결합하여 형태 만들기

// 분자
class CLinkedShapeMolecule : public CMover {
public:
	//앞뒤로 결합해 있는 물체
	CLinkedShapeMolecule * Prev;
	CLinkedShapeMolecule* Next;
	//속도
	float VX, VY;

	//생성자
	CLinkedShapeMolecule(float x, float y)
		: Prev(NULL), Next(NULL)
	{
		//그래픽을 설정한다.
		Texture = Game->Texture[TEX_MOLECULE1];
		//좌표를 무작위로 결정한다.
		X = Rand.Int31() % 2 * (MAX_X + 1) - 1;
		Y = Rand.Int31() % 2 * (MAX_Y + 1) - 1;
		//본체를 향해가는 속도를 결정한다.
		VX = x - X;
		VY = y - Y;
		float v = sqrtf(VX*VX + VY * VY) / 0.02f;
		if (v>0) {
			VX /= v;
			VY /= v;
		}
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		//본체나 다른 물체에 결합되어 있지 않은 경우의 처리
		if (!Prev || Prev == this) {
			//미리 결정된 속도로 직진한다.
			X += VX;
			Y += VY;
			//그래픽을 회전시킨다.
			Angle += 0.01f;
			//화면 밖으로 나가면 삭제하다.
			return X >= -1 && X <= MAX_X && Y >= -1 && Y <= MAX_Y;
		}
		//본체나 다른 물체에 결합되어 있는 동안에는 삭제하지 않는다.
		return true;
	}
};

// 커서
class CLinkedShapeCursor : public CLinkedShapeMolecule {
public:
	//직전의 버튼 상태
	bool PrevButton;

	//생성자
	CLinkedShapeCursor()
		: CLinkedShapeMolecule(0, 0), PrevButton(true)
	{
		//그래픽을 검사한다.
		Texture = Game->Texture[TEX_MOLECULE0];
		//좌표를 화면의 중심으로 맞춘다.
		X = (MAX_X - 1) / 2;
		Y = (MAX_Y - 1) / 2;
		//본체는 다른 물체에 결합하지 않도록 한다.
		//(본체 자신에게 결합해 있는 것으로 설정한다.)
		Prev = this;
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		//이동속도
		static const float speed = 0.1f;
		//레버의 입력에 따라 속도를 설정한다.
		VX = VY = 0;
		if (is->Left) VX = -speed;
		if (is->Right) VX = speed;
		if (is->Up) VY = -speed;
		if (is->Down) VY = speed;
		//화면 밖으로 나갈 때에는 속도를 0으로 설정한다.
		if (X + VX<0 || X + VX>MAX_X - 1) VX = 0;
		if (Y + VY<0 || Y + VY>MAX_Y - 1) VY = 0;

		//마지막으로 결합한 물체를 찾는다.
		CLinkedShapeMolecule* tail = this;
		while (tail->Next) tail = tail->Next;

		//본체에 결합되어 있는 모든 물체에 대하여 처리한다.
		for (CLinkedShapeMolecule* m = this; m; m = m->Next) {
			//본체와 함께 움직인다.
			m->X += VX;
			m->Y += VY;
			//아직 결합되지 않은 물체와의 접촉을 검사한다.
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				//화면상에 있는 물체 중에서 1개를 얻는다.
				CLinkedShapeMolecule* n = (CLinkedShapeMolecule*)i.Next();
				//접촉판정의 크기, 결합 후의 거리
				static const float hit = 1.0f, link = 0.6f;
				//어떤 물체가 아직 결합해 있지 않고
				//본체 또는 결합한 물체로부터의 거리가 일정 값 미만이라면
				//그 물체를 결합시킨다.
				if (
					!n->Prev &&
					abs(n->X - m->X)<hit && abs(n->Y - m->Y)<hit
					) {
					//접촉한 방향에 따라
					//왼쪽 위, 왼쪽 아래, 오른쪽 위, 오른쪽 아래 중 한쪽으로 결합시킨다.
					n->X = m->X + (n->X<m->X ? -link : link);
					n->Y = m->Y + (n->Y<m->Y ? -link : link);
					//회전을 초기화한다.
					n->Angle = 0;
					//마지막으로 결합한 물체로 등록한다.
					n->Prev = tail;
					tail->Next = n;
					tail = n;
				}
			}
		}

		//버튼을 입력한 경우의 처리
		//버튼을 누르고 있을 때
		//계속해서 물체가 분리되어 버리지 않도록
		//이전의 버튼 입력의 상태를 검사한다.
		if (!PrevButton && is->Button[0]) {
			//본체에 1개 이상의 물체가 결합해 있을 때의 처리
			if (tail->Prev) {
				//분리한 물체의 속도를
				//본체로부터 멀어지도록 설정한다.
				float
					vx = tail->X - X, vy = tail->Y - Y,
					v = sqrtf(vx*vx + vy * vy) / 0.12f;
				if (v>0) {
					tail->VX = vx / v;
					tail->VY = vy / v;
				}
				//물체를 리스트에서 삭제한다.
				tail->Prev->Next = NULL;
				//(물체 자신에게 결합해 있는 것으로 설정한다)
				tail->Prev = tail;
			}
		}
		//버튼의 입력 상태를 저장한다.
		PrevButton = is->Button[0];

		return true;
	}
};

// 스테이지
class CLinkedShapeStage : public CStage {
	CLinkedShapeCursor* Cursor;
	int Time;
public:
	CLinkedShapeStage() : CStage(L"LINKED SHAPE") {
	}
	virtual ~CLinkedShapeStage() {
	}
	virtual void Init() {
		Cursor = new CLinkedShapeCursor();
		Time = 0;
	}
	virtual bool Move(const CInputState* is) {
		Time++;
		if (Time == 60) {
			new CLinkedShapeMolecule(Cursor->X, Cursor->Y);
			Time = 0;
		}
		return true;
	}
};

//==============================================================
// 선으로 둘러싸 칠하기
//# 기준선, + 새로운 선, * 칠한 영역

// 적
class CEnclosedAreaEnemy : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;

	CEnclosedAreaEnemy(CCell* cell)
		: Cell(cell)
	{
		CX = Cell->GetXSize() / 2;
		CY = 3;
		VX = Rand.Int31() % 2 * 2 - 1;
		VY = Rand.Int31() % 2 * 2 - 1;
	}
	//적의 이동처리
	virtual bool Move(const CInputState* is) {
		//이동 방향의 셀을 검사하여
		//비어있지 않으면(선 또는 칠해진 영역)
		//이동 방향을 무작위로 변화 시킨다.
		if (Cell->Get(CX + VX, CY + VY) != ' ') {
			//이동 방향을 왼쪽 위, 왼쪽 아래, 오른쪽 위, 오른쪽 아래 중에서
			//무작위로 선택한다.
			VX = Rand.Int31() % 2 * 2 - 1;
			VY = Rand.Int31() % 2 * 2 - 1;
		}
		else {
			//이동 방향의 셀이 비어있으면 이동 방향으로 움직인다.
			CX += VX;
			CY += VY;
		}
		return true;
	}

	virtual void Draw() {}
};

// 커서
class CEnclosedAreaCursor : public CMover {
public:
	CCell * Cell;
	CEnclosedAreaEnemy* Enemy;
	int CX, CY;

	CEnclosedAreaCursor(CCell* cell, CEnclosedAreaEnemy* enemy)
		: Cell(cell), Enemy(enemy)
	{
		CX = Cell->GetXSize() / 2;
		CY = Cell->GetYSize() - 3;
	}

	//커서의 이동처리
	virtual bool Move(const CInputState* is) {
		//레버의 입력에 따라 이동 방향을 설정한다.
		int vx = 0, vy = 0;
		if (is->Left) vx = -1; else
			if (is->Right) vx = 1; else
				if (is->Up) vy = -1; else
					if (is->Down) vy = 1;
		//1칸 앞의 셀과 2칸 앞의 셀을 얻는다.
		char
			c1 = Cell->Get(CX + vx, CY + vy),
			c2 = Cell->Get(CX + vx * 2, CY + vy * 2);
		//버튼을 누르고 있을 때의 처리
		if (is->Button[0]) {
			//1칸 앞의 셀이 공백이고
			//2칸 앞의 셀도 공백인 경우에는 선을 긋는다.
			if (c1 == ' ' && c2 == ' ') {
				//1칸 앞의 셀과 2칸 앞에 셀을 선으로 설정한다
				Cell->Set(CX + vx, CY + vy, '+');
				Cell->Set(CX + vx * 2, CY + vy * 2, '+');
			}
			else
				//1칸 앞의 셀이 공백이고
				//2칸 앞의 셀이 기준선인 경우에 영역을 닫는다.
				if (c1 == ' ' && c2 == '#') {
					//1칸 앞의 셀을 선으로 설정한다.
					Cell->Set(CX + vx, CY + vy, '+');
					//적이 있는 영역을 %으로 칠한다.
					Paint(Enemy->CX, Enemy->CY);
					//칠해진 영역을 반전시켜
					//적이 없는 영역을 칠한다
					//또한 새로운 선을 기준선으로 변경한다.
					for (int y = 3; y<Cell->GetYSize() - 3; y++) {
						for (int x = 3; x<Cell->GetXSize() - 3; x++) {
							switch (Cell->Get(x, y)) {
								//적이 없는 영역(공백 셀)은
								//*로 설정한다.
							case ' ':
								Cell->Set(x, y, '*');
								break;
								//적이 있는 영역%은 공백으로 설정한다.
							case '%':
								Cell->Set(x, y, ' ');
								break;
								//새로운선+는 기준선#으로 설정한다.
							case '+':
								Cell->Set(x, y, '#');
								break;
							}
						}
					}
					//주위에 공백 영역이 없는 기준선을 칠한다.
					for (int y = 2; y<Cell->GetYSize() - 2; y++) {
						for (int x = 2; x<Cell->GetXSize() - 2; x++) {
							//기준선에 대한 처리
							if (Cell->Get(x, y) == '#') {
								//선 주위(8방향)에 있는
								//공백 셀의 개수를 센다.
								int count = 0;
								for (int i = -1; i <= 1; i++) {
									for (int j = -1; j <= 1; j++) {
										if (Cell->Get(x + i, y + j) == ' ') {
											count++;
										}
									}
								}
								//공백 셀이 하나도 없을 때에는
								//기준선을 칠한다.
								if (count == 0) {
									Cell->Set(x, y, '*');
								}
							}
						}
					}
				}
			//그 외의 경우에는 커서를 이동하지 않는다.
				else {
					vx = vy = 0;
				}
		}
		else {
			//버튼을 누르고 있지 않은 때의 처리
			//1칸 앞의 셀과 2칸 앞의 셀이
			//모두 기준선인 경우 이외에는
			//커서를 이동하지 않는다.
			if (c1 != '#' || c2 != '#') {
				vx = vy = 0;
			}
		}
		//커서의 셀 좌표를 갱신한다.
		//선끼리 인접하지 않도록 하기 위해
		//셀을 2칸씩 이동한다.
		CX += vx * 2;
		CY += vy * 2;
		return true;
	}

	//칠하기 처리
	void Paint(int x, int y) {
		//현재 셀이 공백이라면
		//칠하는 처리를 한다.
		if (Cell->Get(x, y) == ' ') {
			int lx, rx;
			//공백 셀이 계속 될 때까지
			//현재의 셀로부터 왼쪽으로 이동한다.
			for (lx = x; Cell->Get(lx - 1, y) == ' '; lx--);
			//공백 셀이 계속될 때까지
			//현재 셀로부터 오른쪽으로 이동한다.
			for (rx = x; Cell->Get(rx + 1, y) == ' '; rx++);
			//공백 셀을 %로 칠한다.
			for (x = lx; x <= rx; x++) {
				Cell->Set(x, y, '%');
			}
			//칠해 넣은 셀의 위아래 셀에 대하여
			//칠하기 처리를 재귀적으로 수행한다.
			for (x = lx; x <= rx; x++) {
				Paint(x, y - 1);
				Paint(x, y + 1);
			}
		}
	}

	virtual void Draw() {}
};

// 스테이지
class CEnclosedAreaStage : public CStage {
	CCell* Cell;
	CEnclosedAreaEnemy* Enemy;
	CEnclosedAreaCursor* Cursor;
public:
	CEnclosedAreaStage() : CStage(L"ENCLOSED AREA") {
		Cell = new CCell(201, 141);
	}
	virtual ~CEnclosedAreaStage() {
		delete Cell;
	}
	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				if (x<2 || x>xs - 3 || y<2 || y>ys - 3) {
					Cell->Set(x, y, '=');
				}
				else
					if (x == 2 || x == xs - 3 || y == 2 || y == ys - 3) {
						Cell->Set(x, y, '#');
					}
					else {
						Cell->Set(x, y, ' ');
					}
			}
		}
		Enemy = new CEnclosedAreaEnemy(Cell);
		Cursor = new CEnclosedAreaCursor(Cell, Enemy);
	}
	virtual void Draw() {
		static const int dy = 10;
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / (xs - 1),
			sh = Game->GetGraphics()->GetHeight() / (ys - 1);
		for (int y = 0; y<ys; y++) {
			int lx, rx;
			for (lx = rx = 0; lx<xs; lx = rx) {
				D3DCOLOR color = COL_WHITE;
				char c = Cell->Get(lx, y);
				switch (c) {
				case '#': color = COL_BLACK; break;
				case '+': color = COL_MGRAY; break;
				case '*': color = COL_LGRAY; break;
				}
				for (rx = lx + 1; rx<xs; rx++) {
					if (c != Cell->Get(rx, y)) {
						Game->Texture[TEX_FILL]->Draw(
							lx*sw, (y + dy)*sh, (rx - lx)*sw, sh,
							0, 0, 1, 1, color);
						break;
					}
				}
			}
		}
		Game->Texture[TEX_SHURIKEN]->Draw(
			(Cursor->CX - 5)*sw, (Cursor->CY - 5 + dy)*sh, sw * 11, sh * 11,
			0, 0, 1, 1, COL_BLACK);
		Game->Texture[TEX_ENEMY1]->Draw(
			(Enemy->CX - 10)*sw, (Enemy->CY - 10 + dy)*sh, sw * 21, sh * 21,
			0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// 한 붓 그리기로 아이템 회수하기

// 사람
class CTraversableRouteMan : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;

	CTraversableRouteMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), CY(y), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}

	//캐릭터의 이동처리
	virtual bool Move(const CInputState* is) {
		//입력 상태
		if (State == 0) {
			//레버의 입력에 따라
			//상하좌우로 이동 방향을 설정한다.
			VX = VY = 0;
			if (is->Left) VX = -1; else
				if (is->Right) VX = 1; else
					if (is->Up) VY = -1; else
						if (is->Down) VY = 1;
			//레버가 입력된 경우의 처리
			if (VX != 0 || VY != 0) {
				//이동 방향의 셀을 검사한다.
				char c = Cell->Get(CX + VX, CY + VY);
				//이동 방향의 셀이 공백이나 아이템이면
				if (c == ' ' || c == 'I') {
					//캐릭터가 지나온 흔적을 써놓는다.
					Cell->Set(CX, CY, '+');
					//셀 좌표를 갱신한다.
					CX += VX;
					CY += VY;
					//아이템을 지우기 위해
					//셀을 공백으로 설정한다.
					Cell->Set(CX, CY, ' ');
					//타이머를 설정하고
					//이동상태로 전환한다.
					State = 1;
					Time = 0;
				}
			}
		}
		//이동 상태
		if (State == 1) {
			//타이머를 갱신한다.
			Time++;
			//화면상에서 캐릭터가 부드럽게 움직이도록
			//그림좌표를 조금씩 변화시킨다.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			//일정 시간이 경과하면 입력상태로 전환한다.
			if (Time == 10) {
				State = 0;
			}
		}
		return true;
	}
};

// 스테이지
class CTraversableRouteStage : public CStage {
	CCell* Cell;
public:
	CTraversableRouteStage() : CStage(L"TRAVERSABLE ROUTE") {
		Cell = new CCell();
	}
	virtual ~CTraversableRouteStage() {
		delete Cell;
	}
	virtual void Init() {
		Cell->Init((char*)
			"                "
			"################"
			"#I  # I#   I   #"
			"# # #  # # ### #"
			"# #I   # #    I#"
			"# # ## M #I# ###"
			"# I  # ### # I #"
			"#### #  I    # #"
			"#   I# # ## ## #"
			"# ## #I# #I  # #"
			"#I     #   #  I#"
			"################"
		);
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				if (Cell->Get(x, y) == 'M') {
					Cell->Set(x, y, ' ');
					new CTraversableRouteMan(Cell, x, y);
				}
			}
		}
	}
	virtual void Draw() {
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				switch (Cell->Get(x, y)) {
				case '#':
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
					break;
				case 'I':
					Game->Texture[TEX_MUSHROOM]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
					break;
				case '+':
					Game->Texture[TEX_FILL]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY
					);
					break;
				}

			}
		}
	}
};

//==============================================================
// 단어 만들기

#define CROSSWORD_KANA_COUNT 5
#define CROSSWORD_MIN_LENGTH 2
#define CROSSWORD_MAX_LENGTH 4

// 스테이지
class CCrosswordStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, Kana, State;
	int X, Y, VX, VY, Index, Length, Time;
	bool PrevLever, PrevButton;

	CCrosswordStage()
		: CStage(L"CROSSWORD"),
		CX(0), CY(0), Kana(0), State(0),
		PrevLever(true), PrevButton(true)
	{
		Cell = new CCell(12, 8);
	}

	virtual ~CCrosswordStage() {
		delete Cell;
	}

	virtual void Init() {
		Cell->Clear(CROSSWORD_KANA_COUNT);
	}

	//스테이지의 이동 처리
	virtual bool Move(const CInputState* is) {
		//셀의 크기
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//초기 상태
		if (State == 0) {
			//새로운 문자를 선택한다.
			Kana = Rand.Int31() % CROSSWORD_KANA_COUNT;
			//입력상태로 전환한다.
			State = 1;
		}
		//입력상태
		if (State == 1) {
			//레버의 입력에 따라
			//커서를 상하좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && CX>0) CX--; else
					if (is->Right && CX<xs - 1) CX++; else
						if (is->Up && CY>0) CY--; else
							if (is->Down && CY<ys - 1) CY++;
			}
			//직전의 레버의 입력을 저장한다.
			PrevLever = is->Left || is->Right || is->Up || is->Down;
			//커서가 문자가 없는 셀에 있을 때
			//버튼을 누르면
			//커서의 위치에 새로운 문자를 놓는다.
			if (
				!PrevButton &&
				is->Button[0] &&
				Cell->Get(CX, CY) == CROSSWORD_KANA_COUNT
				) {
				//새로운 문자를 셀에 써넣는다.
				Cell->Set(CX, CY, Kana);
				//세로 방향의 단어를 찾기 위한 준비
				//탐색 좌표와 방향, 단어의 문자 수를 설정한다.
				X = CX;
				Y = max(0, CY - CROSSWORD_MAX_LENGTH + 1);
				VX = 0;
				VY = 1;
				Length = CROSSWORD_MIN_LENGTH;
				//탐색상태로 전환한다.
				State = 2;
			}
			PrevButton = is->Button[0];
		}
		//단어 사전
		static const wchar_t* dictionary[] = {
			L"あい", L"あいあい", L"あう", L"あお", L"あおい",
			L"いあい", L"いい", L"いう", L"いえ", L"いえい", L"いお",
			L"うい", L"うえ", L"うお",
			L"えあ", L"えい",
			L"おあ", L"おい", L"おう", L"おおい", L"おおう",
			NULL
		};
		//탐색상태
		if (State == 2) {
			//스테이지에서 읽어온 단어
			wchar_t word[CROSSWORD_MAX_LENGTH + 1];
			//정해진 범위를 탐색한다.
			for (; X <= CX && Y <= CY; X += VX, Y += VY, Length = CROSSWORD_MIN_LENGTH) {
				//스테이지에서 기준을 초과하지 않는 가장 긴 단어의 문자 수를 구한다.
				int max_length = min(CROSSWORD_MAX_LENGTH, (xs - X)*VX + (ys - Y)*VY);
				//가장 짧은 문자 수에서 가장 긴 문자 수까지를 검사한다.
				for (; Length <= max_length; Length++) {
					//스테이지의 셀에서 문자를 읽어
					//단어의 문자열을 생성한다.
					int i, x, y;
					for (i = 0, x = X, y = Y; i<Length; i++, x += VX, y += VY) {
						word[i] = L"あいうえお?"[Cell->Get(x, y)];
					}
					word[i] = L'\0';
					//새로 놓여진 문자를 포함하고 있는 경우의 처리
					if (x>CX || y>CY) {
						//사전의 단어와 일치하는지 검사한다.
						for (i = 0; dictionary[i]; i++) {
							if (wcscmp(word, dictionary[i]) == 0) break;
						}
						//사전의 단어와 일치하면 루프를 탈출한다.
						if (dictionary[i]) break;
					}
				}
				//사전의 단어와 일치한 경우에는 이 루프도 탈출한다.
				if (Length <= max_length) break;
			}
			//세로 방향의 탐색을 완료하면
			if (Y>CY) {
				//가로 방향으로 단어를 찾기 위한 준비
				//탐색 좌표와 방향, 단어의 문자 수를 설정한다.
				X = max(0, CX - CROSSWORD_MAX_LENGTH + 1);
				Y = CY;
				VX = 1;
				VY = 0;
				Length = CROSSWORD_MIN_LENGTH;
			}
			else
				//가로 방향의 탐색을 완료하면
				if (X>CX) {
					//초기상태로 전환한다.
					State = 0;
				}
			//사전 단어와 일치한 경우의 처리
				else {
					//일치한 부분을 다른 색으로 표시하기 위하여
					//셀에 표시를 한다.
					for (int i = 0, x = X, y = Y; i<Length; i++, x += VX, y += VY) {
						Cell->Set(x, y, Cell->Get(x, y) | 0x80);
					}
					//조금더 긴 단어를 찾는다.
					Length++;
					//타이머를 설정하고 표시상태로 전환한다.
					Time = 0;
					State = 3;
				}
		}
		//표시상태
		if (State == 3) {
			//타이머를 갱신한다.
			Time++;
			if (Time == 30) {
				//일치한 부분의 표시를 종료하기 위하여
				//셀의 표시를 지운다.
				for (int i = 0, x = X, y = Y; i<Length; i++, x += VX, y += VY) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
				//탐색상태로 전환한다.
				State = 2;
			}
		}
		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / (ys + 1);
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				D3DCOLOR color = (c & 0x80) ? COL_LGRAY : COL_BLACK;
				c = (c & 0x3f);
				if (0 <= c && c<CROSSWORD_KANA_COUNT) {
					Game->Texture[TEX_KANA0 + c]->Draw(
						x*sw, (y + 1)*sh, sw, sh, 0, 0, 1, 1, color);
				}
			}
		}
		if (State<2) {
			Game->Texture[TEX_KANA0 + Kana]->Draw(
				CX*sw, (CY + 1)*sh, sw, sh, 0, 0, 1, 1, COL_MGRAY);
		}
	}
};




