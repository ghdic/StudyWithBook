//========================================================
// 미로 걷기

// 캐릭터 클라스
class CMazeWalkerMan : public CMover {
public:
	//스테이지를 나타내는 셀
	CCell * Cell;
	//셀 상의 좌표와 이동 방향
	int CX, CY, VX, VY;
	//캐릭터의 상태와 타이머
	int State, Time;
	
	//생성자
	//좌표, 상태, 타이머, 그래픽의 형태 등을 설정한다.
	CMazeWalkerMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}

	//이동
	//레버의 입력에 따라 캐릭터를 상하좌우로 움직인다.
	//이 처리는 일정 시간(1/60초)마다 호출된다.
	virtual bool Move(const CInputState* is) {
		//정지상태(캐릭터가 정지하고 있는 상태)
		if (State = 0) {
			//레버의 입력에 따라 이동 방향을 설정한다.
			VX = VY = 0;
			if (is->Left)VX = -1;
			else if (is->Right)VX = 1;
			else if (is->Up)VY = -1;
			else if (is->Down)VY = 1;

			//레버의 입력이 있는 경우의 처리
			if (VX != 0 || VY != 0) {
				//이동 방향에 있는 셀이 공백인지 검사한다.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//이동 방향이 공백인 경우에는 캐릭터를 이동시킨다.
					//현재 캐릭터가 있는 셀을 공백으로 설정하고
					//이동 방향의 셀을 캐릭터(문자 M)으로 설정한다.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					//셀 상의 캐릭터 좌표를 갱신한다.
					CX += VX;
					CY += VY;
					//타이머를 설정하고 이동상태로 전환한다.
					State = 1;
					Time = 10;
				}
			}
		}

		//이동상태(캐릭터가 이동중인 상태)
		if (State == 1) {
			//타이머를 감소시킨다.
			Time--;

			//타이머를 사용해 화면상의 캐릭터 좌표를 조금씩 변화시켜
			//캐릭터를 부드럽게 이동시킨다.
			X = CX - VX * Time*0.1f;
			Y = CY - VY * Time*01.f;
			//타이머가 0이 되면 이동 방향을 초기화하고 정자상태로 돌아간다.
			if (Time == 0) {
				VX = VY = 0;
				State = 0;
			}
		}

		//캐릭터를 삭제하지 않는 경우에는 true를 반환하고
		//캐릭터를 삭제하는 경우에는 false를 반환한다.
		return true;
	}
};


//스테이지 클래스
class CMazeWalkerStage : public CStage{
	//스테이지를 나타내는 셀
	CCell* Cell;
public:
	//생성자
	//스테이지의 이름을 설정하고 셀을 나타내는 객체를 생성한다.
	CMazeWalkerStage() : CStage(L"MAZE WALKER") {
		Cell = new CCell();
	}

	//소멸자
	//셀의 객체를 삭제한다.
	virtual ~CMazeWalkerStage() {
		delete Cell;
	}

	//스테이지의 초기화
	//이 처리는 스테이지가 시작될 때마다 호출된다.
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
		//모든 셀을 검사하여
		//캐릭터 셀(문자 M)의 위치에 캐릭터의 객체를 생성한다.
		for (int y=0; y<MAX_Y; y++) {
			for (int x=0; x<MAX_X; x++) {
				if (Cell->Get(x, y)=='M') {
					new CMazeWalkerMan(Cell, x, y);
				}
			}
		}
	}

	//스테이지 그리기
	//이 처리는 스테이지를 화면에 그릴 때 호출된다.
	virtual void Draw() {
		//화면의 해상도에 따라 그릴 크기를 변화시키기 위한 처리
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		//모든 셀을 검사하여
		//벽 셀(문자 #)의 위치에 벽의 이미지를 그린다.
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
};

//======================================================
//물체 밀기

//물체를 밀 때에 호출 되는 처리
class CLoadPusherLoad : public CMover {
public:
	int CX, CY, VX, VY;
	int State, Time;

	CLoadPusherLoad(int x, int y)
		: CX(x), CY(y), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_LOAD];
		X = x;
		Y = y;
	}

	void Push(int vx, int vy) {
		//물체가 정지상태이면 이동상태로 전환한다.
		if (State == 0) {
			//이동방향을 설정
			VX = vx;
			VY = vy;
			//셀 좌표를 갱신
			CX += vx;
			CY += vy;
			//상태와 타이머 설정
			State = 1;
			Time = 10;
		}
	}

	//물체의 이동 거리
	virtual bool Move(const CInputState* is) {
		//물체가 이동상태일 때의 처리
		if (State == 1) {

			//타이머를 갱신한다.
			Time--;

			//그림좌표를 갱신한다.
			//타이머를 사용해 조금씩 변화시킨다.
			X = CX - VX * Time*0.1f;
			Y = CY - VX * Time*0.1f;

			//타이머가 0이 되면 정지상태로 전환한다.
			if (Time == 0) {
				State = 0;
			}
		}
		return true;
	}
};

//사람
class CLoadPusherMan : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;

	CLoadPusherMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), CY(y), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}
	//캐릭터의 이동 처리
	virtual bool Move(const CInputState* is) {
		//정지 상태의 처리
		if (State == 0) {
			//레버 입력에 따라 이동 방향을 설정한다.
			VX = VY = 0;
			if (is->Left)VX = -1;
			else if (is->Right)VX = 1;
			else if (is->Up)VX = -1;
			else if (is->Down)VY = 1;

			//이동하는 경우의 처리
			if (VX != 0 || VY != 0) {
				//캐릭터의 이동 방향 셀이 공백인 경우 처리
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//캐릭터의 현재 위치와 셀과 이동 방향의 셀을 갱신한다.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					//셀 좌표를 갱신하다.
					CX += VX;
					CY += VY;
					//상태와 타이머를 설정한다.
					State = 1;
					Time = 10;
				}
				//이동 방향에 물체가 있고
				//또한 물체의 이동 방향이 공백인 경우 처리
				else if (Cell->Get(CX + VX, CY + VY) == 'L' &&
					Cell->Get(CX + VX * 2, CY + VY * 2) == ' ') {
					//캐릭터와 물체에 대하여
					//현재 위치의 셀과 이동 방향의 셀을 갱신한다.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					Cell->Set(CX + VX * 2, CY + VY * 2, 'L');
					//셀 좌표를 갱신한다.
					CX += VX;
					CY += VY;

					//상태와 타이머를 설정한다.
					State = 1;
					Time = 10;

					//물체의 객체를 찾는다.
					for (CTaskIter i(Game->MoverList); i.HasNext();) {
						CMover* mover = (CMover*)i.Next();
						//물체를 밀기 위한 처리를 호출한다.
						if ((int)mover->X == CX && (int)mover->Y == CY) {
							((CLoadPusherLoad*)mover)->Push(VX, VY);
						}
					}
				}
			}
		}
		//이동 상태 처리
		if (State == 1) {
			//타이머를 갱신한다.
			Time--;

			//그림좌표를 갱신한다.
			//타이머를 사용해 조금씩 변화시킨다.
			X = CX - VX * Time*0.1f;
			Y = CY - VY * Time*0.1f;

			//타이머가 0이 되면 정지상태로 전환시킨다.
			if (Time == 0) {
				State = 0;
			}
		}
		return true;
	}
};

//스테이지
class CLoadPusherStage : public CStage {
	CCell* Cell;
public:
	CLoadPusherStage() : CStage(L"LOAD PUSHER") {
		Cell = new CCell();
	}
	virtual ~CLoadPusherStage() {
		delete Cell;
	}
	virtual void Init() {
		Cell->Init((char*)
			"                "
			"################"
			"#              #"
			"#      LL      #"
			"#  ###    ###  #"
			"#  #   M    #  #"
			"#     L  L     #"
			"#  #        #  #"
			"#  ###    ###  #"
			"#      LL      #"
			"#              #"
			"################"
		);
		for (int y = 0; y < MAX_Y; y++) {
			for (int x = 0; x < MAX_X; x++) {
				switch (Cell->Get(x, y)) {
				case 'M': new CLoadPusherMan(Cell, x, y); break;
				case 'L': new CLoadPusherLoad(x, y); break;
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
};

//=================================================
//미끄러지는 물체 밀기

//미끄러지는 물체 클라스
class CSlidingLoadPusherLoad : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;

	CSlidingLoadPusherLoad(CCell* cell, int x, int y)
		: Cell(cell), CX(x), CY(y), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_ICE];
		X = x;
		Y = y;
	}
	//물체를 밀 때 호출하는 처리
	void Push(int vx, int vy) {
		//정지 상태에서 이동 상태로 전환한다.
		if (State == 0) {
			//이동방향을 정한다.
			VX = vx;
			VY = vy;
			//타이머와 상태를 설정한다.
			Time = 1;
			State = 1;
		}
	}

	//물체의 이동 처리
	virtual bool Move(const CInputState* is) {
		//이동상태 처리
		if (State == 1) {
			//타이머를 갱신한다.
			Time--;

			//그림 좌표를 갱신한다.
			X = CX - VX * Time*0.2f;
			Y = CY - VY * Time*0.2f;
			//타이머가 0이 되었을 경우의 처리
			if (Time == 0) {
				//이동 방향의 셀이 공백인지를 검사한다.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//이동 방향의 셀이 공백인 경우에는 계속해서 이동한다.
					//현재 위치와 이동할 위치의 셀을 갱신한다.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'L');
					//셀 좌표를 갱신한다.
					CX += VX;
					CY += VY;
					//타이머를 설정한다.
					Time = 5;
				}
				else {
					//이동 방향 셀이 공백이 아닌 경우에는 정지한다.
					//정지사태로 전환한다.
					State = 0;
				}
			}
		}
		return true;
	}
};

//사람
class CSlidingLoadPusherMan : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;
	
	CSlidingLoadPusherMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), CY(y), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}

	//캐리터의 이동 처리
	virtual bool Move(const CInputState* is) {
		//정지 상태의 처리
		if (State == 0) {
			//레버의 입력에 따라 이동 방향을 설정한다.
			if (is->Left) {
				VX = 1; VY = 0;
			}
			else if (is->Right) {
				VX = 1; VY = 0;
			}
			else if (is->Up) {
				VY = -1; VX = 0;
			}
			else if (is->Down) {
				VY = 1; VY = 0;
			}

			//레버의 입력이 있는 경우의 처리
			if (is->Left || is->Right || is->Up || is->Down) {
				//이동 방향의 셀이 공백인지 검사하여
				//공백인 경우에는 이동한다.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//이동 전의 위치와 이동할 위치의 셀을 갱신한다.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					//셀 좌표를 갱신한다.
					CX += VX;
					CY += VY;
					//상태와 타이머를 설정한다.
					//이동상태로 전환한다.
					State = 1;
					Time = 10;

				}
			}
			//물체를 향한 채로 버튼을 누른 경우의 처리
			if (
				is->Button[0] &&
				Cell->Get(CX + VX, CY + VY) == 'L'
				) {
				//물체의 객체를 찾는다.
				for (CTaskIter i(Game->MoverList); i.HasNext();) {
					CMover* mover = (CMover*)i.Next();
					//물체를 미는 처리를 호출한다.
					if ((int)mover->X == CX + VX && (int)mover->Y == CY + VY) {
						((CSlidingLoadPusherLoad*)mover)->Push(VX, VY);
					}
				}
			}
			//이동 상태의 처리
			if (State == 1) {
				//타이머를 갱신한다.
				Time--;
				//그림좌표를 갱신한다.
				X = CX - VX * Time*0.1f;
				Y = CY - VY * Time*0.1f;
				//타이머가 0이 되면 정지상태로 전환한다.
				if (Time == 0) {
					State = 0;
				}
			}
		}
		return true;
	}
};

//스테이지
class CSlidingLoadPusherStage : public CStage {
	CCell* Cell;
public:
	CSlidingLoadPusherStage() : CStage(L"SLIDING LOAD PUSHER") {
		Cell = new CCell();
	}
	virtual ~CSlidingLoadPusherStage() {
		delete Cell;
	}
	virtual void Init() {
		Cell->Init((char*)
			"                "
			"################"
			"#      L       #"
			"#  L L    L L  #"
			"#    L M  L    #"
			"#  L #L  L# L  #"
			"# LL        LL #"
			"#  L #L  L# L  #"
			"#    L    L    #"
			"#  L L    L L  #"
			"#       L      #"
			"################"
		);
		for (int y = 0; y < MAX_Y; y++) {
			for (int x = 0; x < MAX_X; x++) {
				switch (Cell->Get(x, y)) {
				case 'M': new CSlidingLoadPusherMan(Cell, x, y); break;
				case 'L': new CSlidingLoadPusherLoad(Cell, x, y); break;
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
				if (Cell->Get(x, y) == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}

			}
		}
	}
	
};

//==============================================================
// 중력에 의해 떨어지는 물체 밀기

// 물체
class CLoadPusherInGravityLoad : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;

	CLoadPusherInGravityLoad(CCell* cell, int x, int y)
		: Cell(cell), CX(x), CY(y), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_OBJECT];
		X = x;
		Y = y;
	}
	//물체를 밀 때 호출하는 처리
	void Push(int vx, int vy) {
		if (State == 0) {
			VX = vx;
			VY = 0;
			CX += vx;
			CY += vy;
			State = 1;
			Time = 10;
		}
	}

	//물체의 이동 처리
	virtual bool Move(const CInputState* is) {
		if (State == 0) {
			//아래쪽 방향의 셀이 공백이면 이동상태로 전환한다.
			if (Cell->Get(CX, CY + 1) == ' ') {
				//이동 방향을 설정한다(아래로)
				VX = 0;
				VY = 1;
				Cell->Set(CX, CY, ' ');
				Cell->Set(CX, CY + VY, 'L');
				CX += VX;
				CY += VY;
				State = 1;
				Time = 10;
			}
		}
		if (State == 1) {
			Time--;
			X = CX - VX * Time*0.1f;
			Y = CY - VY * Time*0.1f;
			if (Time == 0) {
				State = 0;
			}
		}
		return true;
	}
};

// 사람
class CLoadPusherInGravityMan : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;

	CLoadPusherInGravityMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), CY(y), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}
	//캐릭터의 이동처리
	virtual bool Move(const CInputState* is) {
		if (State == 0) {
			//레버 입력에 따라 이동 방향을 설정한다.
			VX = VY = 0;
			if (is->Left) VX = -1; else
				if (is->Right) VX = 1; else
					if (is->Up) VY = -1; else
						if (is->Down) VY = 1;
			//이동 하는 경우 처리
			if (VX != 0 || VY != 0) {
				//이동 방향의 셀이 공백이면 이동한다.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					CX += VX;
					CY += VY;
					State = 1;
					Time = 10;
				}
				else
					//이동 방향의 셀이 물체이고
					//그 물체의 이동 방향이 공백이면
					//물체를 민다.
					if (
						Cell->Get(CX + VX, CY) == 'L' &&
						Cell->Get(CX + VX * 2, CY) == ' ' &&
						Cell->Get(CX + VX, CY + 1) != ' '
						) {
						Cell->Set(CX, CY, ' ');
						Cell->Set(CX + VX, CY, 'M');
						Cell->Set(CX + VX * 2, CY, 'L');
						CX += VX;
						CY += VY;
						State = 1;
						Time = 10;
						for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
							CMover* mover = (CMover*)i.Next();
							if ((int)mover->X == CX && (int)mover->Y == CY) {
								((CLoadPusherInGravityLoad*)mover)->Push(VX, VY);
							}
						}
					}
			}
		}
		if (State == 1) {
			Time--;
			X = CX - VX * Time*0.1f;
			Y = CY - VY * Time*0.1f;
			if (Time == 0) {
				State = 0;
			}
		}
		return true;
	}
};

// 스테이지
class CLoadPusherInGravityStage : public CStage {
	CCell* Cell;
public:
	CLoadPusherInGravityStage() : CStage(L"LOAD PUSHER IN GRAVITY") {
		Cell = new CCell();
	}
	virtual ~CLoadPusherInGravityStage() {
		delete Cell;
	}
	virtual void Init() {
		Cell->Init((char*)
			"                "
			"################"
			"#              #"
			"#    L    L    #"
			"#  ###    ###  #"
			"#  #        #  #"
			"#      M       #"
			"#  #  L  L  #  #"
			"#  ####  ####  #"
			"#              #"
			"#     L  L     #"
			"################"
		);
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				switch (Cell->Get(x, y)) {
				case 'M': new CLoadPusherInGravityMan(Cell, x, y); break;
				case 'L': new CLoadPusherInGravityLoad(Cell, x, y); break;
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
				if (Cell->Get(x, y) == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}

			}
		}
	}
};

//==============================================================
// 자율적으로 움직이는 캐릭터

// 사람
class CSelfDirectiveCharacterMan : public CMover {
public:
	CCell * Cell;
	int CX, CY, VX, VY;
	int State, Time;
	bool Digger;

	CSelfDirectiveCharacterMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), CY(y), VX(1), VY(1), State(0), Time(0), Digger(false)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}
	//캐릭터에게 명령을 내릴때 호출되는 처리
	void Dig() {
		//굴착 캐릭터 플래그를 설정한다.
		Digger = true;
		//그래픽을 굴착 캐릭터로 변경한다.
		Texture = Game->Texture[TEX_DRILLER];
	}

	//캐릭터의 이동 처리
	virtual bool Move(const CInputState* is) {
		if (State == 0) {
			//아래쪽 셀이 공백인 경우에는 낙하 상태로 전환한다.
			if (Cell->Get(CX, CY + 1) == ' ') {
				CY += VY;
				Time = 20;
				State = 1;
			}
			else
				//굴착 캐릭터 플래그가 설정되어 있고,
				//아래쪽 방향의 셀이 벽인 겨우에는 굴착상태로 전환한다.
				if (Digger && Cell->Get(CX, CY + 1) == '#') {
					CY += VY;
					Time = 40;
					State = 2;
				}
				else
					//좌우 방향의 셀이 공백인 경우에는 보행상태로 전환한다.
					if (Cell->Get(CX + VX, CY) == ' ') {
						CX += VX;
						Time = 20;
						State = 3;
					}
					else {
						//좌우 방향이 공백이 아닐 때에는
						//이동 방향을 반대로 설정한다.
						VX = -VX;
					}
		}
		//낙하 상태의 처리
		if (State == 1) {
			Time--;
			Y = CY - VY * Time*0.05f;
			if (Time == 0) {
				State = 0;
			}
		}
		//굴착 상태의 처리
		if (State == 2) {
			Time--;
			Y = CY - VY * Time*0.025f;
			//타이머가 0이 되면
			//파인 바닥의 셀을 공백으로 변경하고
			//정지상태로 전환한다.
			if (Time == 0) {
				Cell->Set(CX, CY, ' ');
				State = 0;
			}
		}
		//보행처리
		if (State == 3) {
			Time--;
			X = CX - VX * Time*0.05f;
			if (Time == 0) {
				State = 0;
			}
		}
		return true;
	}

	virtual void Draw() {
		if (State == 2) {
			float
				sw = Game->GetGraphics()->GetWidth() / MAX_X,
				sh = Game->GetGraphics()->GetHeight() / MAX_Y;
			Game->Texture[TEX_FILL]->Draw(
				X*sw, CY*sh, sw, sh*(1 - CY + Y), 0, 0, 1, 1, COL_WHITE
			);
		}
		CMover::Draw();
	}
};

// 커서
class CSelfDirectiveCharacterCursor : public CMover {
public:
	CSelfDirectiveCharacterCursor(float x, float y)
	{
		Texture = Game->Texture[TEX_SHURIKEN];
		X = x;
		Y = y;
	}
	
	//커서의 이동 처리
	virtual bool Move(const CInputState* is) {
		//커서의 속도와 접촉판정의 크기를 설정한다.
		float speed = 0.1f, hit = 0.5f;
		//레버의 입력에 따라 커서의 좌표를 갱신한다.
		if (is->Left && X>0) X -= speed;
		if (is->Right && X<MAX_X - 1) X += speed;
		if (is->Up && Y>0) Y -= speed;
		if (is->Down && Y<MAX_Y - 1) Y += speed;

		//모든 캐릭터에 대해
		//커서와의 접촉판정 처리를 수행한다.
		CSelfDirectiveCharacterMan* man = NULL;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover = (CMover*)i.Next();
			//커서와 접촉해있는 캐릭터를 발견하면
			//루프를 탈출한다.
			if (
				mover != this &&
				mover->X<X + hit && X<mover->X + hit &&
				mover->Y<Y + hit && Y<mover->Y + hit
				) {
				man = (CSelfDirectiveCharacterMan*)mover;
				break;
			}
		}
		//커서와 접촉해있는 캐릭터가 있는 경우의 처리
		if (man) {
			//커서를 원형으로 표시한다.
			Texture = Game->Texture[TEX_PILE];
			//버튼 입력이 있으면
			//커서와 접촉해있는 캐릭터에게 명령을 내린다.
			if (is->Button[0]) {
				man->Dig();
			}
		}
		else {
			//커서와 접촉해 있는 캐릭터가 없는경우
			//커서를 십자 형태로 표시한다.
			Texture = Game->Texture[TEX_SHURIKEN];
		}
		return true;
	}
};

// 스테이지
class CSelfDirectiveCharacterStage : public CStage {
	CCell* Cell;
public:
	CSelfDirectiveCharacterStage() : CStage(L"SELF DIRECTIVE CHARACTER") {
		Cell = new CCell();
	}
	virtual ~CSelfDirectiveCharacterStage() {
		delete Cell;
	}
	virtual void Init() {
		Cell->Init((char*)
			"                "
			"################"
			"#  M        M  #"
			"#  #  M  M  #  #"
			"#  ##########  #"
			"#  #        #  #"
			"# M   M  M   M #"
			"# # M #  # M # #"
			"# #####  ##### #"
			"#              #"
			"#     M  M     #"
			"================"
		);
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				switch (Cell->Get(x, y)) {
				case 'M':
					new CSelfDirectiveCharacterMan(Cell, x, y);
					Cell->Set(x, y, ' ');
					break;
				}
			}
		}
		new CSelfDirectiveCharacterCursor((MAX_X - 1)*0.5f, (MAX_Y - 1)*0.5f);
	}
	virtual void Draw() {
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				if (Cell->Get(x, y) == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				else
					if (Cell->Get(x, y) == '=') {
						Game->Texture[TEX_DROP_FLOOR]->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
						);
					}
			}
		}
	}
};

//==============================================================
// 뒤따라오는 커서

// 선두 커서
#define FOLLOWING_CURSOR_LENGTH 101
//커서가 이전에 통과한 좌표를 저장하기 위한 배열
float
FollowingCursorX[FOLLOWING_CURSOR_LENGTH],
FollowingCursorY[FOLLOWING_CURSOR_LENGTH];

class CFollowingCursorHeadCursor : public CMover {
public:
	CCell * Cell;
	bool PrevButton;

	CFollowingCursorHeadCursor(CCell* cell, float x, float y)
		: Cell(cell), PrevButton(true)
	{
		Texture = Game->Texture[TEX_CURSOR_FILL];
		X = x;
		Y = y;

		float *fx = FollowingCursorX, *fy = FollowingCursorY;
		for (int i = 0; i<FOLLOWING_CURSOR_LENGTH; i++) {
			fx[i] = x;
			fy[i] = y;
		}
	}
	//선두 커서의 이동 처리
	virtual bool Move(const CInputState* is) {
		//커서의 이동 속도
		float speed = 0.1f;
		//레버의 입력에 따라 커서의 그림좌표를 변화시킨다.
		if (is->Left && X>0) X -= speed;
		if (is->Right && X<MAX_X - 1) X += speed;
		if (is->Up && Y>0) Y -= speed;
		if (is->Down && Y<MAX_Y - 1) Y += speed;

		//버튼을 입력한 경우의 처리
		//선두 커서가 가리키는 구역과
		//후미에 따라오는 커서가 가리키는 구역간에
		//셀을 바꿔 넣는다.
		float *fx = FollowingCursorX, *fy = FollowingCursorY;
		if (!PrevButton && is->Button[0]) {
			//커서가 가리키는 구역의 셀 좌표를 계산한다.
			int
				x = ((int)X / 5) * 5,
				y = (((int)Y - 1) / 5) * 5 + 1,
				tx = ((int)fx[FOLLOWING_CURSOR_LENGTH - 1] / 5) * 5,
				ty = (((int)fy[FOLLOWING_CURSOR_LENGTH - 1] - 1) / 5) * 5 + 1;
			//구역 내의 셀을 바꿔 넣는다.
			for (int i = 1; i <= 4; i++) {
				for (int j = 1; j <= 4; j++) {
					Cell->Swap(x + i, y + j, tx + i, ty + j);
				}
			}
		}
		//버튼을 누른 채로 있을 때
		//바꿔 넣기가 계속해서 반복되지 않도록 하기 위하여
		//이전의 버튼 입력을 저장한다.
		PrevButton = is->Button[0];


		//커서의 좌표를 저장하기 위하여
		//오래된 좌표를 배열의 뒤로 옮긴다.
		//원형 deque를 이용하면 좋지 않을까싶음
		for (int i = FOLLOWING_CURSOR_LENGTH - 1; i>0; i--) {
			fx[i] = fx[i - 1];
			fy[i] = fy[i - 1];
		}
		fx[0] = X;
		fy[0] = Y;

		return true;
	}
};

// 추종 커서
class CFollowingCursorTailCursor : public CMover {
public:
	CCell * Cell;
	int Delay;

	CFollowingCursorTailCursor(int delay, CTexture* texture)
		: Delay(delay)
	{
		Texture = texture;
		float *fx = FollowingCursorX, *fy = FollowingCursorY;
		X = fx[delay];
		Y = fy[delay];
	}
	//추종 커서의 이동처리
	virtual bool Move(const CInputState* is) {
		//선두 커서의 이전 좌표를 배열에서 읽어
		//추종 커서의 좌표로 설정한다.
		float *fx = FollowingCursorX, *fy = FollowingCursorY;
		X = fx[Delay];
		Y = fy[Delay];
		return true;
	}
};

// 스테이지
class CFollowingCursorStage : public CStage {
	CCell* Cell;
public:
	CFollowingCursorStage() : CStage(L"FOLLOWING CURSOR") {
		Cell = new CCell();
	}
	virtual ~CFollowingCursorStage() {
		delete Cell;
	}
	virtual void Init() {
		Cell->Init((char*)
			"                "
			"################"
			"#    #    #2222#"
			"#0000#    #2222#"
			"#0000#1111#2222#"
			"#0000#1111#2222#"
			"################"
			"#    #    #    #"
			"#    #3333#    #"
			"#    #3333#    #"
			"#    #3333#4444#"
			"################"
		);
		float x = (MAX_X - 1)*0.5f, y = (MAX_Y - 1)*0.5f;
		new CFollowingCursorHeadCursor(Cell, x, y);
		for (int i = 10; i<FOLLOWING_CURSOR_LENGTH - 1; i += 10) {
			new CFollowingCursorTailCursor(i, Game->Texture[TEX_CURSOR_LINE]);
		}
		new CFollowingCursorTailCursor(FOLLOWING_CURSOR_LENGTH - 1, Game->Texture[TEX_CURSOR_FILL]);
	}
	virtual void Draw() {
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				char c = Cell->Get(x, y);
				if (c == '#') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				else
					if ('0' <= c && c <= '4') {
						Game->Texture[TEX_BALL0 + (int)(c - '0')]->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_MGRAY
						);
					}
			}
		}
	}
};
