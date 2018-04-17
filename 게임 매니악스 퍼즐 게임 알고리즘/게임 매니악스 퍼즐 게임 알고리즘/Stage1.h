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