//==============================================================
// 아이템의 위치를 기억하기

// 스테이지
class CMemorizingItemPositionStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, State, Time;
	bool PrevLever, PrevButton;

	CMemorizingItemPositionStage() : CStage(L"MEMORIZING ITEM POSITION") {
		Cell = new CCell();
	}
	virtual ~CMemorizingItemPositionStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"++++++++++++++++"
			"################"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"################"
		);
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				if (Cell->Get(x, y) == ' ' && Rand.Int31() % 10 == 0) {
					Cell->Set(x, y, 'I');
				}
			}
		}
		CX = 1;
		CY = 2;
		Time = 0;
		State = 0;
		PrevLever = true;
		PrevButton = true;
	}

	virtual bool Move(const CInputState* is) {
		if (State == 0) {
			Time++;
			if (Time == 60) {
				State = 1;
			}
		}
		if (State == 1) {
			if (!PrevLever) {
				int cx = CX, cy = CY;
				if (is->Left) cx--;
				if (is->Right) cx++;
				if (is->Up) cy--;
				if (is->Down) cy++;
				if (Cell->Get(cx, cy) != '#') {
					CX = cx;
					CY = cy;
				}
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			if (!PrevButton) {
				if (is->Button[0] && Cell->Get(CX, CY) == 'I') {
					Cell->Set(CX, CY, 'J');
				}
				else
					if (is->Button[1]) {
						Time = 0;
						State = 0;
					}
			}
			PrevButton = is->Button[0] || is->Button[1];
		}
		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		Game->Texture[TEX_FILL]->Draw(
			CX*sw, CY*sh, sw, sh,
			0, 0, 1, 1, COL_LGRAY);
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				int texture = TEX_VOID;
				switch (Cell->Get(x, y)) {
				case '#':
					texture = TEX_FLOOR;
					break;
				case 'I':
					if (State == 0) {
						texture = TEX_ITEM;
					}
					break;
				case 'J':
					texture = TEX_ITEM;
					break;
				}
				Game->Texture[texture]->Draw(
					x*sw, y*sh, sw, sh,
					0, 0, 1, 1, COL_BLACK);
			}
		}
	}
};

//==============================================================
// 지도에 의지해 아이템 찾기

// 스테이지
class CTreasureMapStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, IX, IY, State;
	bool PrevLever, PrevButton;

	CTreasureMapStage() : CStage(L"TREASURE MAP") {
		Cell = new CCell();
	}
	virtual ~CTreasureMapStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"                "
			"                "
			"################"
			"#      #       #"
			"# ### ## # ### #"
			"# #   #      # #"
			"#   ###  ###   #"
			"# #      #   # #"
			"# ### # ## ### #"
			"#     #        #"
			"################"
		);
		do {
			IX = Rand.Int31() % 14 + 1;
			IY = Rand.Int31() % 7 + 4;
		} while (Cell->Get(IX, IY) != ' ');
		CX = 1;
		CY = 4;
		State = 0;
		PrevLever = true;
		PrevButton = true;
	}

	virtual bool Move(const CInputState* is) {
		if (!PrevLever) {
			int cx = CX, cy = CY;
			if (is->Left) cx--;
			if (is->Right) cx++;
			if (is->Up) cy--;
			if (is->Down) cy++;
			if (Cell->Get(cx, cy) != '#') {
				CX = cx;
				CY = cy;
			}
		}
		PrevLever = is->Left || is->Right || is->Up || is->Down;

		if (!PrevButton && is->Button[0] && CX == IX && CY == IY) {
			State = 1;
		}
		PrevButton = is->Button[0] || is->Button[1];

		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				if (Cell->Get(x, y) == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, COL_BLACK);
				}
			}
		}
		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh,
			0, 0, 1, 1, COL_BLACK);
		if (State == 1) {
			Game->Texture[TEX_ITEM]->Draw(
				IX*sw, IY*sh, sw, sh,
				0, 0, 1, 1, COL_BLACK);
		}

		for (int y = -2; y<3; y++) {
			for (int x = -4; x<4; x++) {
				float dx = (x + 16)*sw / 2, dy = (y + 3)*sh / 2;
				if (Cell->Get(IX + x, IY + y) == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						dx, dy, sw / 2, sh / 2,
						0, 0, 1, 1, COL_BLACK);
				}
				if (x == 0 && y == 0) {
					Game->Texture[TEX_ITEM]->Draw(
						dx, dy, sw / 2, sh / 2,
						0, 0, 1, 1, COL_BLACK);
				}
			}
		}
	}
};

//==============================================================
// 물체를 지정된 장소로 운반하기

#define DELIVERING_LOAD_TYPE 4

// 스테이지
class CDeliveringLoadStage : public CStage {
public:
	CCell * Cell;
	int CX, CY;
	char Type;
	bool PrevLever, PrevButton;

	CDeliveringLoadStage() : CStage(L"DELIVERING LOAD") {
		Cell = new CCell();
	}
	virtual ~CDeliveringLoadStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"################"
			"#AA          BB#"
			"#AA###    ###BB#"
			"#  22   #3333  #"
			"#  2##  #      #"
			"#  2#  ##  #0  #"
			"#      #  ##0  #"
			"#  1111#   00  #"
			"#DD###    ###CC#"
			"#DD          CC#"
			"################"
		);
		CX = 7;
		CY = 2;
		PrevLever = true;
		PrevButton = true;
	}

	virtual bool Move(const CInputState* is) {
		if (!PrevLever) {
			int cx = CX, cy = CY;
			if (is->Left) cx--;
			if (is->Right) cx++;
			if (is->Up) cy--;
			if (is->Down) cy++;
			if (Cell->Get(cx, cy) != '#') {
				CX = cx;
				CY = cy;
			}
		}
		PrevLever = is->Left || is->Right || is->Up || is->Down;

		if (!PrevButton && is->Button[0]) {
			char c = Cell->Get(CX, CY);
			if (c == ' ' || ('0' <= c && c<'0' + DELIVERING_LOAD_TYPE)) {
				Cell->Set(CX, CY, Type);
				Type = c;
			}
			else
				if ('A' <= c && c<'A' + DELIVERING_LOAD_TYPE && Type - '0' == c - 'A') {
					Cell->Set(CX, CY, 'a' + Type - '0');
					Type = ' ';
				}
		}
		PrevButton = is->Button[0] || is->Button[1];

		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		Game->Texture[TEX_FILL]->Draw(
			CX*sw, CY*sh, sw, sh,
			0, 0, 1, 1, COL_LGRAY);
		if ('0' <= Type && Type<'0' + DELIVERING_LOAD_TYPE) {
			Game->Texture[TEX_BALL0 + Type - '0']->Draw(
				CX*sw, CY*sh, sw, sh,
				0, 0, 1, 1, COL_MGRAY);
		}
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				if (c == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh,
						0, 0, 1, 1, COL_BLACK);
				}
				else
					if ('0' <= c && c<'0' + DELIVERING_LOAD_TYPE) {
						Game->Texture[TEX_BALL0 + c - '0']->Draw(
							x*sw, y*sh, sw, sh,
							0, 0, 1, 1, COL_BLACK);
					}
					else
						if ('A' <= c && c<'A' + DELIVERING_LOAD_TYPE) {
							Game->Texture[TEX_ORB0 + c - 'A']->Draw(
								x*sw, y*sh, sw, sh,
								0, 0, 1, 1, COL_LGRAY);
						}
						else
							if ('a' <= c && c <= 'a' + DELIVERING_LOAD_TYPE) {
								Game->Texture[TEX_ORB0 + c - 'a']->Draw(
									x*sw, y*sh, sw, sh,
									0, 0, 1, 1, COL_BLACK);
							}
			}
		}
	}
};

//==============================================================
// 발판을 만들어 나아가기

// 스테이지
class CMakingFloorStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, VX;
	bool PrevLever, PrevButton;

	CMakingFloorStage() : CStage(L"MAKING FLOOR") {
		Cell = new CCell();
	}
	virtual ~CMakingFloorStage() {
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
		CX = xs / 2;
		CY = ys - 2;
		VX = -1;
		PrevLever = true;
		PrevButton = true;
	}

	virtual bool Move(const CInputState* is) {
		if (Cell->Get(CX, CY + 1) == ' ') {
			CY++;
		}
		else
			if (!PrevLever) {
				if (is->Left) {
					if (Cell->Get(CX - 1, CY) == ' ') {
						CX--;
						VX = -1;
					}
					else
						if (Cell->Get(CX - 1, CY) == '#' && Cell->Get(CX - 1, CY - 1) == ' ' && Cell->Get(CX, CY - 1) == ' ') {
							CX--;
							CY--;
							VX = -1;
						}
				}
				else
					if (is->Right) {
						if (Cell->Get(CX + 1, CY) == ' ') {
							CX++;
							VX = 1;
						}
						else
							if (Cell->Get(CX + 1, CY) == '#' && Cell->Get(CX + 1, CY - 1) == ' ' && Cell->Get(CX, CY - 1) == ' ') {
								CX++;
								CY--;
								VX = 1;
							}
					}
			}
		PrevLever = is->Left || is->Right;

		if (!PrevButton && is->Button[0]) {
			if (Cell->Get(CX + VX, CY) == ' ') {
				Cell->Set(CX + VX, CY, '#');
			}
			else
				if (Cell->Get(CX + VX, CY) == '#') {
					Cell->Set(CX + VX, CY, ' ');
				}
		}
		PrevButton = is->Button[0];

		return true;
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				int texture = TEX_VOID;
				switch (Cell->Get(x, y)) {
				case '#': texture = TEX_FLOOR; break;
				case '=': texture = TEX_DROP_FLOOR; break;
				}
				Game->Texture[texture]->Draw(
					x*sw, y*sh, sw, sh,
					0, 0, 1, 1, COL_BLACK);
			}
		}
		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh,
			0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// 주사위를 모아 없애기

#define DICE_TYPE 6

// 스테이지
class CDiceStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, State, Time;
	char Type;
	bool PrevLever, PrevButton;

	CDiceStage() : CStage(L"DICE") {
		Cell = new CCell();
	}

	virtual ~CDiceStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"++++++++++++++++"
			"################"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"#              #"
			"################"
		);
		CX = 1;
		CY = 2;
		PrevLever = true;
		PrevButton = true;
		State = 0;
	}

	virtual bool Move(const CInputState* is) {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		if (State == 0) {
			Type = '1' + Rand.Int31() % DICE_TYPE;
			State = 1;
		}

		if (State == 1) {
			if (!PrevLever) {
				int cx = CX, cy = CY;
				if (is->Left) cx--; else
					if (is->Right) cx++; else
						if (is->Up) cy--; else
							if (is->Down) cy++;
				if (Cell->Get(cx, cy) != '#') {
					CX = cx;
					CY = cy;
				}
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			if (!PrevButton && is->Button[0] && Cell->Get(CX, CY) == ' ') {
				Cell->Set(CX, CY, Type);
				State = 2;
			}
			PrevButton = is->Button[0];
		}

		if (State == 2) {
			State = 0;
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					char c = Cell->Get(x, y);
					if (
						'1'<c && c<'1' + DICE_TYPE &&
						Count(x, y, c) >= c - '0'
						) {
						Erase(x, y, c | 0x80);
						Time = 0;
						State = 3;
					}
				}
			}
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		if (State == 3) {
			Time++;
			if (Time == 30) {
				for (int x = 0; x<xs; x++) {
					for (int y = 0; y<ys; y++) {
						if (Cell->Get(x, y) & 0x40) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				State = 0;
			}
		}

		return true;
	}

	int Count(int x, int y, char c) {
		if (Cell->Get(x, y) == c) {
			Cell->Set(x, y, c | 0x80);
			return
				1 +
				Count(x - 1, y, c) +
				Count(x + 1, y, c) +
				Count(x, y - 1, c) +
				Count(x, y + 1, c);
		}
		return 0;
	}

	void Erase(int x, int y, char c) {
		char d = Cell->Get(x, y);
		if (d == c) {
			Cell->Set(x, y, d | 0x40);
			Erase(x - 1, y, c);
			Erase(x + 1, y, c);
			Erase(x, y - 1, c);
			Erase(x, y + 1, c);
		}
		else
			if (d == '1') {
				Cell->Set(x, y, d | 0x40);
			}
	}

	virtual void Draw() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;

		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y), d = c & 0x3f;
				if (c == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
				else
					if ('1' <= d && d<'1' + DICE_TYPE) {
						D3DCOLOR color = COL_BLACK;
						if (c & 0x40) {
							float f = (float)Time / 30;
							color = D3DXCOLOR(f, f, f, 1);
						}
						Game->Texture[TEX_DICE1 + d - '1']->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, color);
					}
			}
		}

		if (State == 1) {
			Game->Texture[TEX_DICE1 + Type - '1']->Draw(
				CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 건물 세우기

#define BUILDING_TYPE 4

// 스테이지
class CBuildingStage : public CStage {
public:
	CCell * Cell;
	int CX, CY;
	bool PrevLever, PrevButton;

	CBuildingStage() : CStage(L"BUILDING") {
		Cell = new CCell(12, 9);
	}
	virtual ~CBuildingStage() {
		delete Cell;
	}

	virtual void Init() {
		Cell->Init((char*)
			"++++++++++++"
			"############"
			"#          #"
			"#          #"
			"#          #"
			"#          #"
			"#          #"
			"#          #"
			"############"
		);
		CX = 1;
		CY = 2;
		Time = 60;
		PrevLever = true;
		PrevButton = true;
	}

	virtual bool Move(const CInputState* is) {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		if (!PrevLever) {
			int cx = CX, cy = CY;
			if (is->Left) cx--;
			if (is->Right) cx++;
			if (is->Up) cy--;
			if (is->Down) cy++;
			if (Cell->Get(cx, cy) != '#') {
				CX = cx;
				CY = cy;
			}
		}
		PrevLever = is->Left || is->Right || is->Up || is->Down;

		if (!PrevButton && is->Button[0]) {
			if (Cell->Get(CX, CY) == ' ') {
				Cell->Set(CX, CY, '0');
			}
			else {
				Cell->Set(CX, CY, ' ');
			}
		}
		PrevButton = is->Button[0];

		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				if (Cell->Get(x, y) == ' ') {
					static const int
						vx[] = { -1, 1, 0, 0 },
						vy[] = { 0, 0, -1, 1 };
					int type = 0;
					for (int i = 0; i<BUILDING_TYPE; i++) {
						int count = 0;
						for (int j = 0; j<4; j++) {
							if (Cell->Get(x + vx[j], y + vy[j]) == '0' + i) {
								count++;
							}
						}
						if (count >= 2) {
							type = i + count - 1;
						}
					}
					if (type>0) {
						Cell->Set(x, y, '0' + min(type, BUILDING_TYPE - 1));
					}
				}
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
			CX*sw, CY*sh, sw, sh,
			0, 0, 1, 1, COL_LGRAY);
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = Cell->Get(x, y);
				int texture = TEX_VOID;
				if (c == '#') {
					texture = TEX_FLOOR;
				}
				else
					if ('0' <= c && c<'0' + BUILDING_TYPE) {
						texture = TEX_BUILDING0 + c - '0';
					}
				Game->Texture[texture]->Draw(
					x*sw, y*sh, sw, sh,
					0, 0, 1, 1, COL_BLACK);
			}
		}
	}
};

