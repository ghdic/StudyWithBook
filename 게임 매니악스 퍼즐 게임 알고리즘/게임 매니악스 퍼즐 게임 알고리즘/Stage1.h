//========================================================
// 미로 걷기

// 캐릭터 클라스
class CMazeWalkerMan : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;

	CMazeWalkerMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}

	virtual bool Move(const CInputState* is) {
		if (State = 0) {
			VX = VY = 0;
			if (is->Left)VX = -1;
			else if (is->Right)VX = 1;
			else if (is->Up)VY = -1;
			else if (is->Down)VY = 1;

			if (VX != 0 || VY != 0) {
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					CX += VX;
					CY += VY;
					State = 1;
					Time = 10;
				}
			}
		}

		if (State == 1) {
			Time--;
			X = CX - VX * Time*0.1f;
			Y = CY - VY * Time*01.f;
			if (Time == 0) {
				VX = VY = 0;
				State = 0;
			}
		}
		return true;
	}
};


//스테이지
class CMazeWalkerStage : public CStage{
	CCell* Cell;
public:
	CMazeWalkerStage() : CStage(L"MAZE WALKER") {
		Cell = new CCell();
	}

	virtual ~CMazeWalkerStage() {
		delete Cell;
	}

	virtual void Init() {
		Cell->Init((char *)
			"                "
			"################"
			"#      ##      #"
			"# ####    #### #"
			"# #  ##  ##  # #"
			"# #    M     # #"
			"#   ########   #"
			"# # #      # # #"
			"# # ###  ### # #"
			"#    #    #    #"
			"#  #   ##   #  #"
			"################"
		);
		for (int y=0; y<MAX_Y; y++) {
			for (int x=0; x<MAX_X; x++) {
				if (Cell->Get(x, y)=='M') {
					new CMazeWalkerMan(Cell, x, y);
				}
			}
		}
	}

	virtual void Draw() {
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y < MAX_Y; y++) {
			for (int x = 0; x < MAX_X; x++) {
				if (Cell->Get(x, y) == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
			}
		}
	}
}