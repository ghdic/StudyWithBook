//==============================================================
// 궤도를 따라 나아가는 볼

#define BALL_ON_RAIL_COUNT 100
#define BALL_ON_RAIL_TYPE 5
#define BALL_ON_RAIL_ERASE 3

// 볼 구조체
typedef struct {
	//종류
	int Type;
	//궤도상의 위치, 좌표, 불투명도(알파값)
	float Pos, X, Y, Alpha;
} BALL_ON_RAIL_BALL;

// 궤도 구조체
typedef struct {
	//좌표, 길이
	float X, Y, Length;
} BALL_ON_RAIL_RAIL;

//궤도의 데이터(좌표, 길이)
BALL_ON_RAIL_RAIL BallOnRailRail[] = {
	{ 18, 2, 0 },
{ 5, 2, 0 },
{ 3, 3, 0 },
{ 2, 5, 0 },
{ 2, 8, 0 },
{ 3, 10, 0 },
{ 5, 11, 0 },
{ 11, 11, 0 },
{ 13, 10, 0 },
{ 14, 8, 0 },
{ 14, 7, 0 },
{ 13, 5, 0 },
{ 11, 4, 0 },
{ 8, 4, 0 },
{ 6, 5, 0 },
{ 5, 7, 0 },
{ 5, 8, 0 },
{ 6, 9, 0 },
{ 10, 9, 0 },
{ 11, 8, 0 },
{ 11, 6, -1 },
};

// 스테이지
class CBallOnRailStage : public CStage {
public:
	BALL_ON_RAIL_BALL Ball[BALL_ON_RAIL_COUNT];
	BALL_ON_RAIL_RAIL* Rail;
	int BallCount, RailCount;
	float SX, SY, SDir, BX, BY, BVX, BVY, Speed;
	int BType, Time;
	bool PrevButton, BallReady;

	//생성자
	CBallOnRailStage() : CStage(L"BALL ON RAIL") {
		//궤도의 길이를 계산한다.
		Rail = BallOnRailRail;
		int i;
		for (i = 0; Rail[i].Length >= 0; i++) {
			float
				x = Rail[i].X - Rail[i + 1].X,
				y = Rail[i].Y - Rail[i + 1].Y;
			Rail[i].Length = sqrtf(x*x + y * y);
		}
		//궤도의 수
		RailCount = i;
		//대포의 좌표
		SX = 8;
		SY = 6;
	}

	virtual ~CBallOnRailStage() {
	}

	virtual void Init() {
		BallCount = 0;
		SDir = 0;
		Time = 0;
		Speed = 0.1f;
		PrevButton = true;
		InitBall();
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		//스테이지 안의 볼의 개수가 0개이거나
		//시작지점에서 가장 가까운 볼까지의 거리가 볼 1개 거리 이상이면
		//새로운 볼을 시작지점에 생성한다.
		if ((BallCount == 0 || Ball[0].Pos >= 1) && BallCount<BALL_ON_RAIL_COUNT) {
			//볼의 종류를 임의로 결정한다.
			int type = Rand.Int31() % BALL_ON_RAIL_TYPE;
			//같은 종류의 볼이 정해진 개수(3개)이상 연속 되지 않도록 하기위한 처리
			if (BallCount >= BALL_ON_RAIL_ERASE - 1) {
				//시작지점에 같은 종류의 볼이 정해진 개수 -1 (3-1개)만큼
				//연속해 있는지 확인한다.
				int i;
				for (i = 1; i<BALL_ON_RAIL_ERASE - 1; i++) {
					if (Ball[i].Type != Ball[0].Type) break;
				}
				//연속해 있을 경우
				//다른 종류의 볼을 생성한다.
				if (i == BALL_ON_RAIL_ERASE - 1) {
					//다른 종류의 볼을 임의로 선택한다.
					while (type == Ball[0].Type) {
						type = Rand.Int31() % BALL_ON_RAIL_TYPE;
					}
				}
			}
			//볼을 추가한다.
			AddBall(0, type, 0);
		}

		//스테이지가 시작할 때에는 볼을 빨리 나아가게 하고
		//일정 시간이 경과하면 천천히 나가게 한다.
		Time++;
		if (Time == 180) {
			Speed = 0.01f;
		}

		//가장 뒤쪽의 볼을 조금 나아가게 한다.
		Ball[0].Pos += Speed;
		//볼의 간격을 조정한다.
		for (int i = 1; i<BallCount; i++) {
			//어떤 볼과
			//바로 전(궤도의 시작지점에 가까운)의 볼에 대하여
			//두 볼 사이의 거리를 검사한다.
			BALL_ON_RAIL_BALL &b0 = Ball[i - 1], &b1 = Ball[i];
			float d = b1.Pos - b0.Pos;
			//거리가 볼 1개 미만이라면
			//볼 1개의 거리가 될 때까지 볼을 조금씩 멀어지게 한다.
			if (d<1) {
				b1.Pos = min(b1.Pos + 0.2f, b0.Pos + 1);
			}
			else
				//거리가 볼 1개보다 크다면
				//볼 1개의 거리가 될 때까지 볼을 조금씩 가까워지게 한다.
				if (d>1) {
					b1.Pos = max(b1.Pos - 0.2f, b0.Pos + 1);
				}
		}
		//볼의 좌표를 갱신한다.
		for (int i = 0; i<BallCount; i++) {
			//궤도상의 위치로부터 볼의 좌표를 계산한다.
			SetBallXY(i);
			//사라질 볼(불투명도가 1미만인 볼)을
			//시간의 경과에 따라 조금씩 사라지게 한다.
			float &a = Ball[i].Alpha;
			if (a<1) {
				//불투명도를 감소시킨다.
				a -= 0.1f;
				//완전히 투명해지면
				//볼을 삭제한다.
				if (a <= 0) {
					EraseBall(i);
					i--;
				}
			}
		}
		//레버의 입력에 따라 대포를 좌우로 회전시킨다.
		if (is->Left) SDir += 0.99f;
		if (is->Right) SDir += 0.01f;
		if (SDir>1) SDir -= (int)SDir;
		//볼이 준비된상태에서 버튼을 누르면 볼을 발사한다.
		if (BallReady) {
			if (!PrevButton && is->Button[0]) {
				//대포의 방향에 따라 볼의 속도를 결정한다.
				float rad = SDir * D3DX_PI * 2;
				BVX = cosf(rad)*0.5f;
				BVY = sinf(rad)*0.5f;
				//볼이 준비되지 않은 상태로 전환한다.
				BallReady = false;
			}
		}
		else {
			//볼이 날아가는 동안의 처리
			//볼의 좌표를 갱신한다.
			BX += BVX;
			BY += BVY;
			//볼이 화면 밖으로 나가면 삭제하고 새로운 볼을 생성한다.
			if (
				BX<-1 || BX >= MAX_X + 1 ||
				BY<-1 || BY >= MAX_Y + 1
				) {
				InitBall();
			}
			//볼이 궤도상의 볼에 접촉했는지 검사한다.
			else {
				TryAddBall();
			}
		}
		PrevButton = is->Button[0];
		
		//궤도상에서 정해진 개수(3개) 이상의 볼이 이어지면 볼을 없앤다.
		for (int i = 0; i<BallCount; i++) {
			//같은 종류의 볼이 정해진 개수 이상 이어져 있는지 검사한다.
			int type = Ball[i].Type;
			int j;
			//같은 종류의 볼이고
			//볼사이의 거리가 볼 1개크기있다.
			for (
				j = i + 1;
				j<BallCount &&
				Ball[j].Type == type &&
				Ball[j].Pos == Ball[j - 1].Pos + 1;
				j++
				);
			//정해진 개수 이상 이어져 있으면 볼을 없앤다.
			if (j - i >= BALL_ON_RAIL_ERASE) {
				for (; i<j; i++) {
					//볼의 불투명도를 1미만으로 설정한다.
					Ball[i].Alpha = min(0.9f, Ball[i].Alpha);
				}
			}
		}

		return true;
	}

	//볼의 좌표를 계산하는 처리
	void SetBallXY(int index) {
		//볼의 위치로부터 궤도의 구역 길이를 뺀다.
		BALL_ON_RAIL_BALL &b = Ball[index];
		float pos = b.Pos;
		int i;
		for (i = 0; i<RailCount && pos >= Rail[i].Length; i++) {
			pos -= Rail[i].Length;
		}
		//볼의 위치보다 구역의 길이가 커지면
		//볼이 어느 구역에 있는지 알 수 있기 때문에
		//구역 안의 좌표를 계산한다.
		if (i<RailCount) {
			//구역과 다음 구역의 좌표를
			//볼의 위치에서 비례적으로 배분하여
			//볼의 좌표를 계산한다.
			BALL_ON_RAIL_RAIL &r0 = Rail[i], &r1 = Rail[i + 1];
			b.X = r0.X + (r1.X - r0.X)*pos / r0.Length;
			b.Y = r0.Y + (r1.Y - r0.Y)*pos / r0.Length;
		}
		else {
			//볼이 어느 구역상에도 없을 때에는
			//볼을 삭제한다.
			EraseBall(index);
		}
	}

	//발사할 볼을 생성하는 처리
	void InitBall() {
		//좌표는 대포의 좌표와 같게한다.
		BX = SX;
		BY = SY;
		//종류는 임의로 결정한다.
		BType = Rand.Int31() % BALL_ON_RAIL_TYPE;
		//볼이 준비된 상태로 변경한다.
		BallReady = true;
	}

	//볼을 궤도에 끼워넣는 처리
	void TryAddBall() {
		//궤도상의 모든 볼에 대하여 검사한다.
		for (int i = 0; i<BallCount; i++) {
			//발사한 볼과 궤도상의 볼 간의 거리가 볼 1개 크기 미만이라면
			//볼이 접촉한 것으로 판단하고
			//볼을 궤도상에 추가한다.
			BALL_ON_RAIL_BALL &b = Ball[i];
			float dx = BX - b.X, dy = BY - b.Y;
			if (dx*dx + dy * dy<1) {
				//궤도상의 볼이 있는 구역을 찾는다.
				float pos = b.Pos;
				int j;
				for (j = 0; j<RailCount && pos >= Rail[j].Length; j++) {
					pos -= Rail[j].Length;
				}
				if (j<RailCount) {
					//궤도의 구역의 벡터와
					//발사한 볼과 궤도상의 볼을 연결한 벡터에 대하여
					//내적을 계산하여
					//발사한 볼을 궤도상의 볼의 앞뒤 어느 쪽에
					//추가할 것인지 결정한다.
					BALL_ON_RAIL_RAIL &r0 = Rail[j], &r1 = Rail[j + 1];
					float rx = r1.X - r0.X, ry = r1.Y - r0.Y;
					//궤도상의 볼의 앞쪽에 추가한다.
					if (dx*rx + dy * ry>0) {
						AddBall(i + 1, BType, b.Pos + 0.5f);
					}
					//궤도상에 볼의 뒤쪽에 추가한다.
					else {
						AddBall(i, BType, b.Pos - 0.5f);
					}
					//발사할 볼을 생성한다.
					InitBall();
				}
			}
		}
	}
	//볼을 추가하는 처리
	void AddBall(int index, int type, float pos) {
		//볼이 최대 개수까지 도달하지 않았다면 볼을 추가한다.
		if (BallCount<BALL_ON_RAIL_COUNT) {
			//배열사에 추가할 위치보다 뒤쪽에 있는 볼을
			//1개씩 뒤로 이동한다.
			for (int i = BallCount; i>index; i--) {
				Ball[i] = Ball[i - 1];
			}
			//볼의 개수를 증가시킨다
			BallCount++;
			//볼의 종류, 위치, 불투명도를 설정한다.
			BALL_ON_RAIL_BALL &b = Ball[index];
			b.Type = type;
			b.Pos = pos;
			b.Alpha = 1;
			//볼의 좌표를 계산한다.
			SetBallXY(index);
		}
	}

	//볼을 삭제하는 처리
	void EraseBall(int index) {
		//볼의 개수를 감소시킨다.
		BallCount--;
		//배열에서 사라질 볼보다 뒤쪽의 볼을
		//1개씩 앞으로 이동한다.
		for (int i = index; i<BallCount; i++) {
			Ball[i] = Ball[i + 1];
		}
	}

	//그림 처리
	virtual void Draw() {
		//화면의 해상도에 따라 그릴 크기를 조정한다.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;

		//궤도를 그린다.
		for (int i = 0; i<RailCount; i++) {
			BALL_ON_RAIL_RAIL
				&r0 = Rail[i],
				&r1 = Rail[i + 1];
			float
				dx = (r0.X - r1.X) / r0.Length*0.2f,
				dy = (r0.Y - r1.Y) / r0.Length*0.2f;
			//두꺼운 선을 그린다.
			Game->Texture[TEX_FILL]->Draw(
				(r0.X - dy)*sw, (r0.Y + dx)*sh, COL_BLACK, 0, 0,
				(r0.X + dy)*sw, (r0.Y - dx)*sh, COL_BLACK, 0, 1,
				(r1.X - dy)*sw, (r1.Y + dx)*sh, COL_BLACK, 1, 0,
				(r1.X + dy)*sw, (r1.Y - dx)*sh, COL_BLACK, 1, 1
			);
		}
		//볼을 그린다.
		for (int i = 0; i<BallCount; i++) {
			BALL_ON_RAIL_BALL &b = Ball[i];
			float
				x = (b.X - 0.5f)*sw,
				y = (b.Y - 0.5f)*sh;
			//볼의 배경색을 흰색으로 칠한다.
			Game->Texture[TEX_ORB]->Draw(
				x, y, sw, sh, 0, 0, 1, 1, D3DXCOLOR(1, 1, 1, b.Alpha));
			//볼의 종류에 따른 이미지를 그린다.
			Game->Texture[TEX_ORB0 + b.Type]->Draw(
				x, y, sw, sh, 0, 0, 1, 1, D3DXCOLOR(0, 0, 0, b.Alpha));
		}

		//대포를 그린다.
		float
			rad = SDir * D3DX_PI * 2,
			c = cosf(rad), s = sinf(rad),
			f0 = 2.0f, f1 = 1.0f, f2 = 0.5f;
		Game->Texture[TEX_FILL]->Draw(
			(SX + c * f0)*sw, (SY + s * f0)*sh, COL_LGRAY, 0, 0,
			(SX + c * f0)*sw, (SY + s * f0)*sh, COL_LGRAY, 0, 1,
			(SX - c * f1 + s * f2)*sw, (SY - s * f1 - c * f2)*sh, COL_LGRAY, 1, 0,
			(SX - c * f1 - s * f2)*sw, (SY - s * f1 + c * f2)*sh, COL_LGRAY, 1, 1
		);
		//발사할 볼을 그린다.
		Game->Texture[TEX_ORB0 + BType]->Draw(
			(BX - 0.5f)*sw, (BY - 0.5f)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// 매달린 볼

#define HANGING_BALL_TYPE 5
#define HANGING_BALL_ERASE 3

// 스테이지
class CHangingBallStage : public CStage {
public:
	CCell * StageCell;
	CCell* BallCell;
	float SX, SY, SDir, BX, BY, BVX, BVY;
	int BType, State, Time;
	bool PrevButton;

	CHangingBallStage() : CStage(L"HANGING BALL") {
		StageCell = new CCell();
		BallCell = new CCell();
	}

	virtual ~CHangingBallStage() {
		delete StageCell;
		delete BallCell;
	}

	virtual void Init() {
		//스테이지의 셀의 초기화
		StageCell->Init((char*)
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
			"=              ="
		);
		//볼 셀의 초기화
		BallCell->Init((char*)
			"++++++++++++++++"
			"+01234012340123+"
			"+1234012340123++"
			"+              +"
			"+             ++"
			"+              +"
			"+             ++"
			"+              +"
			"+             ++"
			"+              +"
			"+             ++"
			"++++++++++++++++"
		);
		SX = 8;
		SY = 10.5f;
		SDir = 0.75f;
		PrevButton = true;
		State = 0;
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		int bxs = BallCell->GetXSize(), bys = BallCell->GetYSize();

		if (State == 0) {
			BX = SX;
			BY = SY;
			int x, y;
			for (y = 0; y<bys; y++) {
				for (x = 0; x<bxs; x++) {
					if (IsBall(BallCell->Get(x, y))) break;
				}
				if (x<bxs) break;
			}
			if (y<bys) {
				do {
					BType = Rand.Int31() % HANGING_BALL_TYPE;
					for (y = 0; y<bys; y++) {
						for (x = 0; x<bxs; x++) {
							if (BallCell->Get(x, y) == '0' + BType) break;
						}
						if (x<bxs) break;
					}
				} while (y == bys);
			}
			else {
				BType = Rand.Int31() % HANGING_BALL_TYPE;
			}
			State = 1;
		}
		//입력상태
		if (State == 1) {
			//레버를 좌우로 입력하면 대포를 좌우로 회전시킨다.
			if (is->Left && SDir>0.52f) SDir -= 0.005f;
			if (is->Right && SDir<0.98f) SDir += 0.005f;

			//대포의 각도로부터 발사할 볼의 속도를 계산한다.
			float rad = SDir * D3DX_PI * 2;
			BVX = cosf(rad)*0.5f;
			BVY = sinf(rad)*0.5f;

			//버튼을 누르면 발사상태로 전환한다.
			if (!PrevButton && is->Button[0]) {
				State = 2;
			}
			PrevButton = is->Button[0];
		}

		//발사상태
		if (State == 2) {
			//발사할 볼을 이동한다.
			MoveBall();
			//발사한 볼이 매달린 볼에
			//접촉했는지를 검사한다.
			int x, y;
			for (y = 0; y<bys; y++) {
				for (x = 0; x<bxs; x++) {
					//발사한 볼과 매달린 볼의
					//중심 간의 거리를 구한다.
					float
						dx = BX - GetBallX(x, y),
						dy = BY - GetBallY(y);
					//중심 간의 거리가
					//볼 지름의 80%미만이라면
					//접촉한 것으로 판정한다.
					if (
						IsBall(BallCell->Get(x, y)) &&
						dx*dx + dy * dy<0.8f
						) break;
				}
				if (x<bxs) break;
			}
			//발사한 볼이 매달린 볼에 접촉하면
			//발사한 볼을 매단다
			if (y<bys) {
				//매달린 볼에 인접한 공백인 셀 중에서
				//발사한 볼에 가장 가까운 셀을 선태한다.
				float d = 1000;
				int nx = -1, ny = -1;
				for (int i = 0; i<6; i++) {
					//인접한 셀의 좌표를 얻는다.
					int
						cx = GetAdjacentCX(x, y, i),
						cy = GetAdjacentCY(x, y, i);
					//셀과 발사한 볼의
					//중심간의 거리를 계산한다.
					float
						ex = BX - GetBallX(cx, cy),
						ey = BY - GetBallY(cy),
						e = sqrtf(ex*ex + ey * ey);
					//셀이 공백이고
					//거리가 다른 셀보다 짧으면
					//셀의 좌표를 저장한다.
					if (
						BallCell->Get(cx, cy) == ' ' &&
						d>e
						) {
						d = e;
						nx = cx;
						ny = cy;
					}
				}
				//가장 가까운 셀을 찾으면
				//발사한 볼을 그 셀에 고정한다.
				if (nx >= 0) {
					//볼을 고정한다.
					BallCell->Set(nx, ny, '0' + BType);
					//초기상태로 전환한다.
					State = 0;

					//같은 종류의 볼이 이어져 있는지 검사한다.
					TryEraseBall();
				}
			}
		}
		//삭제상태
		if (State == 3) {
			Time++;
			if (Time == 30) {
				for (int y = 0; y<bys; y++) {
					for (int x = 0; x<bxs; x++) {
						if (BallCell->Get(x, y) & 0x40) {
							BallCell->Set(x, y, ' ');
						}
					}
				}
				State = 0;
			}
		}

		return true;
	}

	//사라질 볼을 찾는 처리
	void TryEraseBall() {
		//모든 볼에 대해서
		//같은 종류의 볼이 정해진 개수(3개) 이상
		//이어져 있는지를 검사한다.
		int bxs = BallCell->GetXSize(), bys = BallCell->GetYSize();
		for (int y = 0; y<bys; y++) {
			for (int x = 0; x<bxs; x++) {
				//같은 종류의 볼이 정해진 개수 이상 이어져 있으면
				//사라질 볼로 변경한다.
				char c = BallCell->Get(x, y);
				if (
					IsBall(c) &&
					EraseBall(x, y, c, 0x80) >= HANGING_BALL_ERASE
					) {
					//사라질 볼로 변경한다.
					EraseBall(x, y, c | 0x80, 0x40);
					//타이머로 설정하고
					//삭제상태로 전환한다.
					Time = 0;
					State = 3;
				}
			}
		}
		//볼의 카운트 완료 표시를 해제한다.
		for (int y = 0; y<bys; y++) {
			for (int x = 0; x<bxs; x++) {
				BallCell->Set(x, y, BallCell->Get(x, y) & 0x7f);
			}
		}
	}

	// 볼을 삭제하는 처리
	//인접한 같은 종류의 볼 개수를 세는 처리도 함께한다.
	int EraseBall(int x, int y, char c, int mask) {
		//인접한 같은 종류의 셀의 개수
		int count = 0;
		//현재 위치의 셀이 지정된 종류의 셀이면
		//셀에 표시한다.
		if (BallCell->Get(x, y) == c) {
			//지정된 비트를 세트하여
			//셀에 표시한다.
			BallCell->Set(x, y, c | mask);
			//현재 위치의 셀을
			//인접한 같은 종류의 셀의 개수에 덯나다.
			count++;
			//주위의 셀에 대해서도 재귀적으로 검사하여
			//인접한 같은 종류의 셀 개수를 계산한다.
			for (int i = 0; i<6; i++) {
				count += EraseBall(
					GetAdjacentCX(x, y, i),
					GetAdjacentCY(x, y, i),
					c, mask);
			}
		}
		//같은 종류의 셀 개수를 반환한다.
		return count;
	}

	//셀이 볼인지 판정하는 처리
	bool IsBall(char c) {
		return '0' <= c && c<'0' + HANGING_BALL_TYPE;
	}
	//인접한 셀의 X좌표를 구하는 처리
	int GetAdjacentCX(int cx, int cy, int index) {
		//셀의 짝수 단 또는 홀수 단인 경우의
		//인접한 셀의 X좌표의 테이블
		static const int ax[2][6] = {
			{ -1, 1, 0, 1, 0, 1 },
		{ -1, 1, -1, 0, -1, 0 }
		};
		//인접한 셀의 X좌표 1개를 반환한다.
		return cx + ax[cy % 2][index];
	}
	//인접한 셀의 Y좌표를 구하는 처리
	int GetAdjacentCY(int cx, int cy, int index) {
		static const int ay[] =
		{ 0, 0, -1, -1, 1, 1 };
		return cy + ay[index];
	}

	//볼 중심의 X좌표를 구하는 처리
	//셀 좌표로부터 그림좌표를 구한다.
	float GetBallX(int cx, int cy) {
		return cx + (cy + 1) % 2 * 0.5f + 0.5f;
	}
	//볼 중심으로 Y좌표를 구하는 처리
	float GetBallY(int cy) {
		return (cy - 1)*sqrtf(3)*0.5f + 2.5f;
	}
	
	//발사한 볼을 이동하는 처리
	void MoveBall() {
		//볼의 좌표를 갱신한다.
		BX += BVX;
		BY += BVY;
		//볼이 좌우의 벽에 부딪히면
		//X방향의 속도를 반전시킨다.
		if (BX<1.5f || BX >= MAX_X - 1.5f) {
			BVX = -BVX;
		}
		//볼이 위쪽 벽에 부딪히면
		//Y방향의 속도를 반전시킨다.
		if (BY<2.5f) {
			BVY = -BVY;
		}
		//볼이 스테이지의 아래쪽을 벗어나면
		//초기상태로 전환한다.
		if (BY >= MAX_Y + 0.5f) {
			State = 0;
		}
	}

	//그림 처리
	virtual void Draw() {
		int
			xs = StageCell->GetXSize(), ys = StageCell->GetYSize(),
			bxs = BallCell->GetXSize(), bys = BallCell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;

		
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char c = StageCell->Get(x, y);
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
			}
		}
		//볼의 궤도를 예측하여 표시한다.
		if (State == 1) {
			//볼의 좌표와 속도를 저장한다.
			float bx = BX, by = BY, bvx = BVX, bvy = BVY;
			//볼을 정해진 횟수만큼 움직인다.
			for (int i = 0; i<30 && State == 1; i++) {
				//볼을 움직인다.
				MoveBall();
				//볼의 좌표에 작은 원을 그려서
				//볼의 궤도를 표시한다.
				Game->Texture[TEX_ORB]->Draw(
					(BX - 0.1f)*sw, (BY - 0.1f)*sh, sw*0.2f, sh*0.2f,
					0, 0, 1, 1, COL_LGRAY);
			}
			//볼의 좌표와 속도를 원래대로 되돌린다.
			BX = bx;
			BY = by;
			BVX = bvx;
			BVY = bvy;
			State = 1;
		}

		//매달린 볼 그리기
		for (int y = 0; y<bys; y++) {
			for (int x = 0; x<bxs; x++) {
				char c = BallCell->Get(x, y);
				//셀이 볼인 경우에는 볼을 그린다.
				if (IsBall(c & 0x3f)) {
					//보통의 볼은 검정색으로 그린다.
					D3DCOLOR color = COL_BLACK;
					//사라질 볼은 점점 흐려지는 것처럼 그린다.
					if (c & 0x40) {
						float f = (float)Time / 30;
						color = D3DXCOLOR(f, f, f, 1);
					}
					//볼의 종류에 따른 이미지를 그린다.
					Game->Texture[TEX_ORB0 + (c & 0x3f) - '0']->Draw(
						(GetBallX(x, y) - 0.5f)*sw,
						(GetBallY(y) - 0.5f)*sh,
						sw, sh, 0, 0, 1, 1, color);
				}
			}
		}

		//대포를 그린다.
		float
			rad = SDir * D3DX_PI * 2,
			c = cosf(rad), s = sinf(rad),
			f0 = 2.0f, f1 = 1.0f, f2 = 0.5f;
		Game->Texture[TEX_FILL]->Draw(
			(SX + c * f0)*sw, (SY + s * f0)*sh, COL_LGRAY, 0, 0,
			(SX + c * f0)*sw, (SY + s * f0)*sh, COL_LGRAY, 0, 1,
			(SX - c * f1 + s * f2)*sw, (SY - s * f1 - c * f2)*sh, COL_LGRAY, 1, 0,
			(SX - c * f1 - s * f2)*sw, (SY - s * f1 + c * f2)*sh, COL_LGRAY, 1, 1
		);
		//발사한 볼을 그린다.
		if (State == 1 || State == 2) {
			Game->Texture[TEX_ORB0 + BType]->Draw(
				(BX - 0.5f)*sw, (BY - 0.5f)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
		}
	}
};

//==============================================================
// 볼을 획득해 모으기

#define COLLECTED_BALL_ERASE 3
#define COLLECTED_BALL_TYPE 5

// 스테이지
class CCollectedBallStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, BallCount, State, Time;
	char BallType;
	bool PrevLever, PrevButton;

	CCollectedBallStage() : CStage(L"COLLECTED BALL") {
		Cell = new CCell();
	}

	virtual ~CCollectedBallStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"                "
			"================"
			"=01234012340123="
			"=01234012340123="
			"=23401234012341="
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
		BallType = '0';
		BallCount = 0;
		PrevLever = true;
		PrevButton = true;
		State = 0;
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//입력상태
		if (State == 0) {
			//레버의 입력에 따라 캐릭터를 좌우로 움직인다.
			if (!PrevLever) {
				if (is->Left && CX>1) CX--;
				if (is->Right && CX<xs - 1) CX++;
			}
			PrevLever = is->Left || is->Right;

			//버튼을 누른 경우의 처리
			if (!PrevButton) {
				//버튼 0을 누르면 볼을 획득한다.
				if (is->Button[0]) {
					//캐릭터의 바로 위쪽에서
					//가장 가까운 공백이 아닌 셀을 찾는다.
					int y;
					for (y = CY - 2; Cell->Get(CX, y) == ' '; y--);
					//아직 볼을 가지고 있지 않는 경우에는
					//찾아낸 셀의 종류를
					//가지고 있는 볼의 종류로 저장한다.
					if (BallCount == 0) {
						BallType = Cell->Get(CX, y);
					}
					//찾아낼 셀이 볼이 경우의 처리
					if (IsBall(BallType)) {
						//같은 종류의 볼이 연속해 있는 한
						//볼을 계속 획득한다.
						for (; Cell->Get(CX, y) == BallType; y--) {
							//획득한 볼의 셀을 공백으로 변경한다.
							Cell->Set(CX, y, ' ');
							//가지고 있는 볼의 개수를 증가시킨다.
							BallCount++;
						}
					}
				}
				//버튼 1을 누르면 볼을 스테이지로 되돌린다.
				if (is->Button[1]) {
					//캐릭터의 바로 위쪽에서
					//가지고 있는 볼을 배치한다.
					int y;
					for (y = CY - 2; Cell->Get(CX, y) == ' '; y--);
					//찾아낸 셀로부터 아래쪽으로
					//가지고 있는 볼을 배치한다.
					for (y++; y<CY - 1 && BallCount>0; y++) {
						//볼을 셀에 써넣는다.
						Cell->Set(CX, y, BallType);
						//가지고 있는 볼의 개수를 감소시킨다.
						BallCount--;
					}
					//삭제판정 상태로 전환한다.
					State = 1;
				}
			}
			PrevButton = is->Button[0] || is->Button[1];
		}

		//삭제판정 상태
		if (State == 1) {
			//볼이 사라지지 않는 경우에는 입력상태로 전환한다.
			State = 0;
			//스테이지 안의 모든 볼에 대하여 검사한다.
			for (int x = 0; x<xs; x++) {
				for (int y = 0; y<ys; y++) {
					char c = Cell->Get(x, y);
					if (IsBall(c)) {
						//볼의 셀을 발견하면
						//같은 종류의 볼이 세로로 정해진 개수(3개) 이상
						//이어져 있는지를 검사한다.
						int i;
						for (i = 1; Cell->Get(x, y + i) == c; i++);
						//정해진 개수 이상으로 이어져 있으면
						//그 볼에 인접하는 같은 종류의 볼을 없앤다.
						if (i >= COLLECTED_BALL_ERASE) {
							//인접하는 같은 종류의 볼을 없앤다.
							Erase(x, y, c);
							//타이머를 설정하고
							//삭제상태로 전환한다.
							Time = 0;
							State = 2;
						}
						//없앤 볼의 다음 볼에
						//같은 종류의 볼이 인접해 있는지를
						//검사하는 처리를 계속한다.
						y += i - 1;
					}
				}
			}
		}

		//삭제상태
		if (State == 2) {
			//일정 시간이 경과하기를 기다린다.
			Time++;
			if (Time == 30) {
				//스테이지 안에서 사라질 볼을 찾는다.
				for (int x = 0; x<xs; x++) {
					for (int y = CY - 2; y >= 0; y--) {
						//사라질 볼을 찾은 경우의 처리
						if (Cell->Get(x, y) & 0x80) {
							//사라질 볼보다 아래에 있는 볼을
							//위로 옮긴다.
							for (int i = y; i<CY - 2; i++) {
								Cell->Set(x, i, Cell->Get(x, i + 1));
							}
							//스테이지 하단의 셀을 공백으로 변경한다
							Cell->Set(x, CY - 2, ' ');
						}
					}
				}
				//입력 상태로 전환한다.
				State = 0;
			}
		}

		return true;
	}

	//셀이 볼인지 판정하는 처리
	bool IsBall(char c) {
		return '0' <= c && c<'0' + COLLECTED_BALL_TYPE;
	}

	//인접한 볼을 없애는 처리
	void Erase(int x, int y, char c) {
		if (Cell->Get(x, y) == c) {
			Cell->Set(x, y, c | 0x80);
			Erase(x - 1, y, c);
			Erase(x + 1, y, c);
			Erase(x, y - 1, c);
			Erase(x, y + 1, c);
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
					if (IsBall(c & 0x7f)) {
						D3DCOLOR color = COL_BLACK;
						if (c & 0x80) {
							float f = (float)Time / 30;
							color = D3DXCOLOR(f, f, f, 1);
						}
						Game->Texture[TEX_ORB0 + (c & 0x7f) - '0']->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, color);
					}
			}
		}

		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
		if (BallCount>0) {
			Game->Texture[TEX_ORB0 + BallType - '0']->Draw(
				CX*sw, (CY - 1)*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 볼을 바꿔넣기

#define SWAPPED_BALL_TYPE 5
#define SWAPPED_BALL_ERASE 3

// 스테이지
class CSwappedBallStage : public CStage {
public:
	CCell * Cell;
	int CX, CY, State, Time;
	char BallType;
	bool PrevLever, PrevButton;

	CSwappedBallStage() : CStage(L"SWAPPED BALL") {
		Cell = new CCell();
	}

	virtual ~CSwappedBallStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"++++++++++++++++"
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
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				if (Cell->Get(x, y) == ' ') {
					Cell->Set(x, y, '0' + Rand.Int31() % SWAPPED_BALL_TYPE);
				}
			}
		}
		CX = 1;
		CY = 2;
		BallType = ' ';
		PrevLever = true;
		PrevButton = true;
		State = 0;
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//입력상태
		if (State == 0) {
			//레버의 입력에 따라 커서를 상하좌우로 움직인다.
			if (!PrevLever) {
				int cx = CX, cy = CY;
				if (is->Left) cx--; else
					if (is->Right) cx++; else
						if (is->Up) cy--; else
							if (is->Down) cy++;
				if (Cell->Get(cx, cy) != '=') {
					CX = cx;
					CY = cy;
				}
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			//버튼을 누르면 가지고 있는 볼과
			//커서 위치에 있는 볼을 바꿔 넣는다.
			if (!PrevButton && is->Button[0]) {
				//가지고 있는 볼의 종류와
				//커서 위치에 있는 볼의 종류를 바꿔 넣는다.
				char c = Cell->Get(CX, CY);
				Cell->Set(CX, CY, BallType);
				BallType = c;
				//낙하상태로 전환한다.
				State = 1;
			}
			PrevButton = is->Button[0];
		}

		//낙하상태
		if (State == 1) {
			//스테이지 안의 모든 볼에 대해서
			//아래에 공백인 셀이 있는 볼을 낙하시킨다.
			for (int x = 0; x<xs; x++) {
				for (int y = 0; y<ys; y++) {
					//공백인 셀을 발견한 경우의 처리
					if (Cell->Get(x, y) == ' ') {
						//위에 있는 볼을 낙하시킨다.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//스테이지 상단은 공백인 셀로 변경한다.
						Cell->Set(x, 2, ' ');
					}
				}
			}
			//삭제판정 상태로 전환한다.
			State = 2;
		}

		//삭제판정 상태
		if (State == 2) {
			//볼이 사라지지 않는 경우에는 입력상태로 전환한다.
			State = 0;
			//스테이지 안의 모든 볼에 대해서 검사한다.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					//같은 종류의 볼이 정해진 개수(3개)이상 이어져 있으면
					//그 볼들을 없앤다.
					char c = Cell->Get(x, y);
					if (
						IsBall(c) &&
						Erase(x, y, c, 0x80) >= SWAPPED_BALL_ERASE
						) {
						//볼을 사라질 볼로 변경한다.
						Erase(x, y, c | 0x80, 0x40);
						//타이머를 설정하고, 삭제상태로 전환한다.
						Time = 0;
						State = 3;
					}
				}
			}
			//볼의 카운트완료 표시를 해제한다.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//삭제상태
		if (State == 3) {
			//일정 시간이 경과하는 것을 기다린다.
			Time++;
			if (Time == 30) {
				//사라질 볼을 완전히 삭제한다.
				for (int x = 0; x<xs; x++) {
					for (int y = 0; y<ys; y++) {
						if (Cell->Get(x, y) & 0x40) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//낙하상태로 전환한다.
				State = 1;
			}
		}

		return true;
	}

	//셀이 볼인지를 판정하는 처리
	bool IsBall(char c) {
		return '0' <= c && c<'0' + SWAPPED_BALL_TYPE;
	}

	//인접한 볼을 없애는 처리
	int Erase(int x, int y, char c, int mask) {
		//현재의 셀이 지정된 종류인지를 검사한다.
		if (Cell->Get(x, y) == c) {
			//지정된 종류라면 셀에 표시한다.
			//(특정 비트 1로 한다)
			Cell->Set(x, y, c | mask);
			//상하좌우에 인접한 셀에 대해서도 재귀적으로 검사하여
			//같은 종류의 셀이 인접한 개수의 합계를 반환한다.
			return
				1 +
				Erase(x - 1, y, c, mask) +
				Erase(x + 1, y, c, mask) +
				Erase(x, y - 1, c, mask) +
				Erase(x, y + 1, c, mask);
		}
		//현재의 셀이 지정된 종류가 아니라면
		//같은 종류의 셀이 인접한 개수로 0을 반환한다.
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
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
				else
					if (IsBall(c & 0x3f)) {
						D3DCOLOR color = COL_BLACK;
						if (c & 0x40) {
							float f = (float)Time / 30;
							color = D3DXCOLOR(f, f, f, 1);
						}
						Game->Texture[TEX_ORB0 + (c & 0x3f) - '0']->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, color);
					}
			}
		}

		if (IsBall(BallType)) {
			Game->Texture[TEX_ORB0 + BallType - '0']->Draw(
				(CX - 0.2f)*sw, (CY - 0.2f)*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
		else {
			Game->Texture[TEX_FILL]->Draw(
				(CX - 0.2f)*sw, (CY - 0.2f)*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(0, 0, 0, 0.5f));
		}
	}
};

//==============================================================
// 볼을 뱀처럼 움직이기

#define SNAKE_BALL_COUNT 5
#define SNAKE_BALL_TYPE 5
#define SNAKE_BALL_ERASE 3

// 스테이지
class CSnakeBallStage : public CStage {
public:
	CCell * Cell;
	int CX[SNAKE_BALL_COUNT], CY[SNAKE_BALL_COUNT], State, Time;
	char BallType[SNAKE_BALL_COUNT];
	bool PrevLever, PrevButton;

	CSnakeBallStage() : CStage(L"SNAKE BALL") {
		Cell = new CCell();
	}

	virtual ~CSnakeBallStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Init((char*)
			"++++++++++++++++"
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
		PrevLever = true;
		PrevButton = true;
		State = 0;
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//초기상태
		if (State == 0) {
			//입력상태로 전환한다.
			State = 1;
			//뱀 모양의 볼의 좌표와 종류를 초기화한다.
			for (int i = 0; i<SNAKE_BALL_COUNT; i++) {
				//볼이 가로 1열로 나열되도록 좌표를 설정한다.
				CX[i] = (xs - SNAKE_BALL_COUNT) / 2 + i;
				CY[i] = 2;

				//볼의 종류는 임의로 선택한다.
				BallType[i] = '0' + Rand.Int31() % SNAKE_BALL_TYPE;
				//볼이 나타날 위치에 다른 볼이 있는 경우는
				//초기상태를 유지하고 볼을 출현시키지 않는다.
				if (Cell->Get(CX[i], CY[i]) != ' ') {
					State = 0;
				}
			}
		}

		//입력상태
		if (State == 1) {
			//레버의 입력에 따라 가장 앞쪽의 볼을 상하좌우로 움직인다.
			if (!PrevLever) {
				//레버의 입력방향에 따라서 이동방향의 좌표를 구한다.
				int cx = CX[0], cy = CY[0];
				if (is->Left) cx--; else
					if (is->Right) cx++; else
						if (is->Up) cy--; else
							if (is->Down) cy++;
				//가장 앞쪽 볼의 이동방향이 공배인 셀이라면 볼을 이동한다.
				if (Cell->Get(cx, cy) == ' ') {
					//가장 앞쪽의 볼이 뒤따르는 볼에 접촉하는지를 검사한다.
					int i;
					for (i = 0; i<SNAKE_BALL_COUNT; i++) {
						if (CX[i] == cx && CY[i] == cy) break;
					}
					//접촉하지 않는 경우에는 볼을 이동한다.
					if (i == SNAKE_BALL_COUNT) {
						//뒤따르는 볼의 좌표를 바로 앞의 볼의 좌표로 변경한다.
						for (i = SNAKE_BALL_COUNT - 1; i>0; i--) {
							CX[i] = CX[i - 1];
							CY[i] = CY[i - 1];
						}
						//가장 앞쪽 볼을 움직인다.
						CX[0] = cx;
						CY[0] = cy;
					}
				}
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			//버튼을 누르면 그 위치에 보을 고정한다.
			if (!PrevButton && is->Button[0]) {
				//볼의 종류를 셀에 써넣는다.
				for (int i = 0; i<SNAKE_BALL_COUNT; i++) {
					Cell->Set(CX[i], CY[i], BallType[i]);
				}
				//낙하상태로 전환한다.
				State = 2;
			}
			PrevButton = is->Button[0];
		}

		//낙하상태
		if (State == 2) {
			//스테이지 안의 공백인 셀을 찾는다.
			for (int x = 0; x<xs; x++) {
				for (int y = 0; y<ys; y++) {
					//공백인 셀을 찾은 경우의 처리
					if (Cell->Get(x, y) == ' ') {
						//공백인 셀의 위에 있는 셀을 1단씩 낙하시킨다.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//스테이지 상단의 셀은 공백으로 변경한다.
						Cell->Set(x, 2, ' ');
					}
				}
			}
			//삭제판정으로 전환한다.
			State = 3;
		}

		//삭제판정 상태
		if (State == 3) {
			//볼이 사라지지 않았으면 초기상태로 전환한다.
			State = 0;
			//스테이지 안의 모든 볼에 대하여 검사한다.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					//볼의 셀에 대해서
					//가로, 세로, 대각선에 같은 종류의 볼이
					//이어져 있는지를 검사한다.
					char c = Cell->Get(x, y);
					if (IsBall(c & 0x7f)) {
						//아래쪽, 오른쪽, 왼쪽 아래, 오른쪽 아래에 대해서 검사한다.
						static const int
							vx[] = { 0, 1, -1, 1 },
							vy[] = { 1, 0, 1, 1 };
						for (int v = 0; v<4; v++) {
							//같은 종류의 볼이 이어져 있는 개수를 센다.
							int count = 0;
							for (
								int i = x, j = y;
								(Cell->Get(i, j) & 0x7f) == c;
								i += vx[v], j += vy[v]
								) {
								count++;
							}
							//정해진 개수(3개) 이상의 볼이 이어져 있으면
							//볼을 사라질 볼로 변경한다.
							if (count >= SNAKE_BALL_ERASE) {
								for (
									int i = x, j = y, k = 0;
									k<count;
									i += vx[v], j += vy[v], k++
									) {
									//사라질 볼의 표시를 한다.
									Cell->Set(i, j, c | 0x80);
								}
								//타이머를 설정하고, 삭제상태로 전환한다.
								Time = 0;
								State = 4;
							}
						}
					}
				}
			}
		}

		//삭제상태
		if (State == 4) {
			//일정 시간이 경과하면
			//사라질 볼을 완전히 삭제한다.
			Time++;
			if (Time == 30) {
				//사라질 볼을 찾는다.
				for (int x = 0; x<xs; x++) {
					for (int y = 0; y<ys; y++) {
						//사라질 볼을 공백인 셀로 변경한다.
						if (Cell->Get(x, y) & 0x80) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//낙하상태로 전환한다.
				State = 2;
			}
		}

		return true;
	}

	//셀이 볼인지 판정하는 처리
	bool IsBall(char c) {
		return '0' <= c && c<'0' + SNAKE_BALL_TYPE;
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
					if (IsBall(c & 0x7f)) {
						D3DCOLOR color = COL_BLACK;
						if (c & 0x80) {
							float f = (float)Time / 30;
							color = D3DXCOLOR(f, f, f, 1);
						}
						Game->Texture[TEX_ORB0 + (c & 0x7f) - '0']->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, color);
					}
			}
		}

		if (State == 1) {
			for (int i = 0; i<SNAKE_BALL_COUNT; i++) {
				Game->Texture[TEX_ORB0 + BallType[i] - '0']->Draw(
					CX[i] * sw, CY[i] * sh, sw, sh, 0, 0, 1, 1, i == 0 ? COL_MGRAY : COL_LGRAY);
			}
		}
	}
};

//==============================================================
// 스프링으로 볼 발사하기

#define FLIPPED_BALL_TYPE 5
#define FLIPPED_BALL_ERASE 3
#define FLIPPED_BALL_POWER 11

// 스테이지
class CFlippedBallStage : public CStage {
public:
	CCell * Cell;
	float Power, BY;
	int BX, BallType, State, Time;

	CFlippedBallStage() : CStage(L"FLIPPED BALL") {
		Cell = new CCell();
	}

	virtual ~CFlippedBallStage() {
		delete Cell;
	}

	virtual void Init() {
		Cell->Init((char*)
			"++++++++++++++++"
			"================"
			"=+=            ="
			"=+=            ="
			"=+=            ="
			"=+=            ="
			"=+=            ="
			"=+=            ="
			"=+=            ="
			"=+=            ="
			"=+=            ="
			"================"
		);
		State = 0;
	}

	//이동처리
	virtual bool Move(const CInputState* is) {
		//셀의 개수
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//초기상태
		if (State == 0) {
			//스프링을 끌어당기는 힘을 설정한다.
			Power = 0;
			BX = 1;
			BallType = '0' + Rand.Int31() % FLIPPED_BALL_TYPE;
			State = 1;
		}

		//입력상태
		if (State == 1) {
			//레버를 아래로 입력하면 스프링을 끌어 당기는 힘을 증가시킨다.
			if (is->Down && Power<FLIPPED_BALL_POWER) {
				Power += 0.1f;
			}
			//레버를 아래로 입력하지 않으면 발사상태로 전환한다.
			if (!is->Down && Power>0) {
				State = 2;
			}
			//스프링을 끌어 당기는 힘에 따라 볼의 좌표를 변화시킨다.
			BY = 2 + Power * (10 - 2) / FLIPPED_BALL_POWER;
		}

		//발사상태
		if (State == 2) {
			//볼이 스테이지 상단에 도달할 때까지 위로 이동한다.
			if (BY>2) {
				BY -= 0.5f;
			}
			//스테이지의 상단에 도달하면
			//스테이지의 오른쪽 영역으로 이동한다.
			else {
				//스프링을 끌어 당기는 힘에 따라 볼의 X좌표를 설정한다.
				BX = 3 + (int)Power;
				//볼의 Y좌표는 스테이지의 상단으로 설정한다.
				BY = 2;
				State = 3;
			}
		}
		//낙하 상태
		if (State == 3) {
			//볼의 아래쪽 셀이 공백이면 볼을 낙하시킨다.
			if (Cell->Get(BX, (int)BY + 1) == ' ') {
				BY += 0.5f;
			}
			else {
				//볼의 아래쪽 셀이 공백이 아니면
				//볼을 그 위치에 고정한다.

				//볼의 종류를 셀에 써넣는다.
				Cell->Set(BX, (int)BY, BallType);
				//삭제상태로 전환한다.
				State = 4;
			}
		}

		//삭제판정 상태
		if (State == 4) {
			//볼이 사라지지 않으면 초기상태로 전환한다.
			State = 0;
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					char c = Cell->Get(x, y);
					if (
						IsBall(c) &&
						Erase(x, y, c, 0x80) >= FLIPPED_BALL_ERASE
						) {
						Erase(x, y, c | 0x80, 0x40);
						Time = 0;
						State = 5;
					}
				}
			}
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//삭제상태
		if (State == 5) {
			Time++;
			if (Time == 30) {
				for (int x = 0; x<xs; x++) {
					for (int y = 0; y<ys; y++) {
						if (Cell->Get(x, y) & 0x40) {
							for (int i = y; i>2; i--) {
								Cell->Set(x, i, Cell->Get(x, i - 1));
							}
							Cell->Set(x, 2, ' ');
						}
					}
				}
				State = 4;
			}
		}

		return true;
	}

	//셀이 볼인지를 판정하는 처리
	bool IsBall(char c) {
		return '0' <= c && c<'0' + SWAPPED_BALL_TYPE;
	}

	int Erase(int x, int y, char c, int mask) {
		if (Cell->Get(x, y) == c) {
			Cell->Set(x, y, c | mask);
			return
				1 +
				Erase(x - 1, y, c, mask) +
				Erase(x + 1, y, c, mask) +
				Erase(x, y - 1, c, mask) +
				Erase(x, y + 1, c, mask);
		}
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
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
				else
					if (IsBall(c & 0x3f)) {
						D3DCOLOR color = COL_BLACK;
						if (c & 0x40) {
							float f = (float)Time / 30;
							color = D3DXCOLOR(f, f, f, 1);
						}
						Game->Texture[TEX_ORB0 + (c & 0x3f) - '0']->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, color);
					}
			}
		}

		if (0<State && State<4) {
			Game->Texture[TEX_ORB0 + BallType - '0']->Draw(
				BX*sw, BY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
		}
	}
};

//==============================================================
// 구르는 많은 볼

// 스테이지
class CRollingBallStage : public CStage {
public:
	CCell * Cell;

	CRollingBallStage() : CStage(L"ROLLING BALL") {
		Cell = new CCell(MAX_X * 5, MAX_Y * 5);
	}

	virtual ~CRollingBallStage() {
		delete Cell;
	}

	virtual void Init() {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		Cell->Clear(' ');
		for (int y = ys / MAX_Y; y<ys; y++) {
			Cell->Set(0, y, '=');
			Cell->Set(xs - 1, y, '=');
		}
		for (int y = ys / MAX_Y * 2; y<ys; y += 3) {
			for (int x = 1; x<xs - 1; x++) {
				Cell->Set(x, y, '=');
			}
			for (int i = 0; i<Rand.Int31() % (5 * ys / MAX_Y) + 1; i++) {
				Cell->Set(Rand.Int31() % (xs - 2) + 1, y, ' ');
			}
		}
	}

	virtual bool Move(const CInputState* is) {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		if (is->Button[0] && Cell->Get(1, 1) == ' ') {
			Cell->Set(1, ys / MAX_Y, '1');
		}
		if (is->Button[1] && Cell->Get(xs - 2, 1) == ' ') {
			Cell->Set(xs - 2, ys / MAX_Y, '0');
		}

		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				char
					c = Cell->Get(x, y),
					d = c | 0x80,
					e = ('0' + '1' - c) | 0x80;
				if (c == '0') {
					Cell->Set(x, y, ' ');
					if (y<ys - 1) {
						if (Cell->Get(x, y + 1) == ' ') {
							Cell->Set(x, y + 1, d);
						}
						else
							if (Cell->Get(x - 1, y) == ' ') {
								Cell->Set(x - 1, y, d);
							}
							else
								if (Cell->Get(x + 1, y) == ' ') {
									Cell->Set(x + 1, y, e);
								}
								else {
									Cell->Set(x, y, d);
								}
					}
				}
				else
					if (c == '1') {
						Cell->Set(x, y, ' ');
						if (y<ys - 1) {
							if (Cell->Get(x, y + 1) == ' ') {
								Cell->Set(x, y + 1, d);
							}
							else
								if (Cell->Get(x + 1, y) == ' ') {
									Cell->Set(x + 1, y, d);
								}
								else
									if (Cell->Get(x - 1, y) == ' ') {
										Cell->Set(x - 1, y, e);
									}
									else {
										Cell->Set(x, y, d);
									}
						}
					}
			}
		}

		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
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
					if (c == '0' || c == '1') {
						Game->Texture[TEX_ORB]->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
					}
			}
		}
	}
};

