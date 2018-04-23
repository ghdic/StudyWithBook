//==============================================================
// �˵��� ���� ���ư��� ��

#define BALL_ON_RAIL_COUNT 100
#define BALL_ON_RAIL_TYPE 5
#define BALL_ON_RAIL_ERASE 3

// �� ����ü
typedef struct {
	//����
	int Type;
	//�˵����� ��ġ, ��ǥ, ������(���İ�)
	float Pos, X, Y, Alpha;
} BALL_ON_RAIL_BALL;

// �˵� ����ü
typedef struct {
	//��ǥ, ����
	float X, Y, Length;
} BALL_ON_RAIL_RAIL;

//�˵��� ������(��ǥ, ����)
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

// ��������
class CBallOnRailStage : public CStage {
public:
	BALL_ON_RAIL_BALL Ball[BALL_ON_RAIL_COUNT];
	BALL_ON_RAIL_RAIL* Rail;
	int BallCount, RailCount;
	float SX, SY, SDir, BX, BY, BVX, BVY, Speed;
	int BType, Time;
	bool PrevButton, BallReady;

	//������
	CBallOnRailStage() : CStage(L"BALL ON RAIL") {
		//�˵��� ���̸� ����Ѵ�.
		Rail = BallOnRailRail;
		int i;
		for (i = 0; Rail[i].Length >= 0; i++) {
			float
				x = Rail[i].X - Rail[i + 1].X,
				y = Rail[i].Y - Rail[i + 1].Y;
			Rail[i].Length = sqrtf(x*x + y * y);
		}
		//�˵��� ��
		RailCount = i;
		//������ ��ǥ
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

	//�̵�ó��
	virtual bool Move(const CInputState* is) {
		//�������� ���� ���� ������ 0���̰ų�
		//������������ ���� ����� �������� �Ÿ��� �� 1�� �Ÿ� �̻��̸�
		//���ο� ���� ���������� �����Ѵ�.
		if ((BallCount == 0 || Ball[0].Pos >= 1) && BallCount<BALL_ON_RAIL_COUNT) {
			//���� ������ ���Ƿ� �����Ѵ�.
			int type = Rand.Int31() % BALL_ON_RAIL_TYPE;
			//���� ������ ���� ������ ����(3��)�̻� ���� ���� �ʵ��� �ϱ����� ó��
			if (BallCount >= BALL_ON_RAIL_ERASE - 1) {
				//���������� ���� ������ ���� ������ ���� -1 (3-1��)��ŭ
				//������ �ִ��� Ȯ���Ѵ�.
				int i;
				for (i = 1; i<BALL_ON_RAIL_ERASE - 1; i++) {
					if (Ball[i].Type != Ball[0].Type) break;
				}
				//������ ���� ���
				//�ٸ� ������ ���� �����Ѵ�.
				if (i == BALL_ON_RAIL_ERASE - 1) {
					//�ٸ� ������ ���� ���Ƿ� �����Ѵ�.
					while (type == Ball[0].Type) {
						type = Rand.Int31() % BALL_ON_RAIL_TYPE;
					}
				}
			}
			//���� �߰��Ѵ�.
			AddBall(0, type, 0);
		}

		//���������� ������ ������ ���� ���� ���ư��� �ϰ�
		//���� �ð��� ����ϸ� õõ�� ������ �Ѵ�.
		Time++;
		if (Time == 180) {
			Speed = 0.01f;
		}

		//���� ������ ���� ���� ���ư��� �Ѵ�.
		Ball[0].Pos += Speed;
		//���� ������ �����Ѵ�.
		for (int i = 1; i<BallCount; i++) {
			//� ����
			//�ٷ� ��(�˵��� ���������� �����)�� ���� ���Ͽ�
			//�� �� ������ �Ÿ��� �˻��Ѵ�.
			BALL_ON_RAIL_BALL &b0 = Ball[i - 1], &b1 = Ball[i];
			float d = b1.Pos - b0.Pos;
			//�Ÿ��� �� 1�� �̸��̶��
			//�� 1���� �Ÿ��� �� ������ ���� ���ݾ� �־����� �Ѵ�.
			if (d<1) {
				b1.Pos = min(b1.Pos + 0.2f, b0.Pos + 1);
			}
			else
				//�Ÿ��� �� 1������ ũ�ٸ�
				//�� 1���� �Ÿ��� �� ������ ���� ���ݾ� ��������� �Ѵ�.
				if (d>1) {
					b1.Pos = max(b1.Pos - 0.2f, b0.Pos + 1);
				}
		}
		//���� ��ǥ�� �����Ѵ�.
		for (int i = 0; i<BallCount; i++) {
			//�˵����� ��ġ�κ��� ���� ��ǥ�� ����Ѵ�.
			SetBallXY(i);
			//����� ��(�������� 1�̸��� ��)��
			//�ð��� ����� ���� ���ݾ� ������� �Ѵ�.
			float &a = Ball[i].Alpha;
			if (a<1) {
				//�������� ���ҽ�Ų��.
				a -= 0.1f;
				//������ ����������
				//���� �����Ѵ�.
				if (a <= 0) {
					EraseBall(i);
					i--;
				}
			}
		}
		//������ �Է¿� ���� ������ �¿�� ȸ����Ų��.
		if (is->Left) SDir += 0.99f;
		if (is->Right) SDir += 0.01f;
		if (SDir>1) SDir -= (int)SDir;
		//���� �غ�Ȼ��¿��� ��ư�� ������ ���� �߻��Ѵ�.
		if (BallReady) {
			if (!PrevButton && is->Button[0]) {
				//������ ���⿡ ���� ���� �ӵ��� �����Ѵ�.
				float rad = SDir * D3DX_PI * 2;
				BVX = cosf(rad)*0.5f;
				BVY = sinf(rad)*0.5f;
				//���� �غ���� ���� ���·� ��ȯ�Ѵ�.
				BallReady = false;
			}
		}
		else {
			//���� ���ư��� ������ ó��
			//���� ��ǥ�� �����Ѵ�.
			BX += BVX;
			BY += BVY;
			//���� ȭ�� ������ ������ �����ϰ� ���ο� ���� �����Ѵ�.
			if (
				BX<-1 || BX >= MAX_X + 1 ||
				BY<-1 || BY >= MAX_Y + 1
				) {
				InitBall();
			}
			//���� �˵����� ���� �����ߴ��� �˻��Ѵ�.
			else {
				TryAddBall();
			}
		}
		PrevButton = is->Button[0];
		
		//�˵��󿡼� ������ ����(3��) �̻��� ���� �̾����� ���� ���ش�.
		for (int i = 0; i<BallCount; i++) {
			//���� ������ ���� ������ ���� �̻� �̾��� �ִ��� �˻��Ѵ�.
			int type = Ball[i].Type;
			int j;
			//���� ������ ���̰�
			//�������� �Ÿ��� �� 1��ũ���ִ�.
			for (
				j = i + 1;
				j<BallCount &&
				Ball[j].Type == type &&
				Ball[j].Pos == Ball[j - 1].Pos + 1;
				j++
				);
			//������ ���� �̻� �̾��� ������ ���� ���ش�.
			if (j - i >= BALL_ON_RAIL_ERASE) {
				for (; i<j; i++) {
					//���� �������� 1�̸����� �����Ѵ�.
					Ball[i].Alpha = min(0.9f, Ball[i].Alpha);
				}
			}
		}

		return true;
	}

	//���� ��ǥ�� ����ϴ� ó��
	void SetBallXY(int index) {
		//���� ��ġ�κ��� �˵��� ���� ���̸� ����.
		BALL_ON_RAIL_BALL &b = Ball[index];
		float pos = b.Pos;
		int i;
		for (i = 0; i<RailCount && pos >= Rail[i].Length; i++) {
			pos -= Rail[i].Length;
		}
		//���� ��ġ���� ������ ���̰� Ŀ����
		//���� ��� ������ �ִ��� �� �� �ֱ� ������
		//���� ���� ��ǥ�� ����Ѵ�.
		if (i<RailCount) {
			//������ ���� ������ ��ǥ��
			//���� ��ġ���� ��������� ����Ͽ�
			//���� ��ǥ�� ����Ѵ�.
			BALL_ON_RAIL_RAIL &r0 = Rail[i], &r1 = Rail[i + 1];
			b.X = r0.X + (r1.X - r0.X)*pos / r0.Length;
			b.Y = r0.Y + (r1.Y - r0.Y)*pos / r0.Length;
		}
		else {
			//���� ��� �����󿡵� ���� ������
			//���� �����Ѵ�.
			EraseBall(index);
		}
	}

	//�߻��� ���� �����ϴ� ó��
	void InitBall() {
		//��ǥ�� ������ ��ǥ�� �����Ѵ�.
		BX = SX;
		BY = SY;
		//������ ���Ƿ� �����Ѵ�.
		BType = Rand.Int31() % BALL_ON_RAIL_TYPE;
		//���� �غ�� ���·� �����Ѵ�.
		BallReady = true;
	}

	//���� �˵��� �����ִ� ó��
	void TryAddBall() {
		//�˵����� ��� ���� ���Ͽ� �˻��Ѵ�.
		for (int i = 0; i<BallCount; i++) {
			//�߻��� ���� �˵����� �� ���� �Ÿ��� �� 1�� ũ�� �̸��̶��
			//���� ������ ������ �Ǵ��ϰ�
			//���� �˵��� �߰��Ѵ�.
			BALL_ON_RAIL_BALL &b = Ball[i];
			float dx = BX - b.X, dy = BY - b.Y;
			if (dx*dx + dy * dy<1) {
				//�˵����� ���� �ִ� ������ ã�´�.
				float pos = b.Pos;
				int j;
				for (j = 0; j<RailCount && pos >= Rail[j].Length; j++) {
					pos -= Rail[j].Length;
				}
				if (j<RailCount) {
					//�˵��� ������ ���Ϳ�
					//�߻��� ���� �˵����� ���� ������ ���Ϳ� ���Ͽ�
					//������ ����Ͽ�
					//�߻��� ���� �˵����� ���� �յ� ��� �ʿ�
					//�߰��� ������ �����Ѵ�.
					BALL_ON_RAIL_RAIL &r0 = Rail[j], &r1 = Rail[j + 1];
					float rx = r1.X - r0.X, ry = r1.Y - r0.Y;
					//�˵����� ���� ���ʿ� �߰��Ѵ�.
					if (dx*rx + dy * ry>0) {
						AddBall(i + 1, BType, b.Pos + 0.5f);
					}
					//�˵��� ���� ���ʿ� �߰��Ѵ�.
					else {
						AddBall(i, BType, b.Pos - 0.5f);
					}
					//�߻��� ���� �����Ѵ�.
					InitBall();
				}
			}
		}
	}
	//���� �߰��ϴ� ó��
	void AddBall(int index, int type, float pos) {
		//���� �ִ� �������� �������� �ʾҴٸ� ���� �߰��Ѵ�.
		if (BallCount<BALL_ON_RAIL_COUNT) {
			//�迭�翡 �߰��� ��ġ���� ���ʿ� �ִ� ����
			//1���� �ڷ� �̵��Ѵ�.
			for (int i = BallCount; i>index; i--) {
				Ball[i] = Ball[i - 1];
			}
			//���� ������ ������Ų��
			BallCount++;
			//���� ����, ��ġ, �������� �����Ѵ�.
			BALL_ON_RAIL_BALL &b = Ball[index];
			b.Type = type;
			b.Pos = pos;
			b.Alpha = 1;
			//���� ��ǥ�� ����Ѵ�.
			SetBallXY(index);
		}
	}

	//���� �����ϴ� ó��
	void EraseBall(int index) {
		//���� ������ ���ҽ�Ų��.
		BallCount--;
		//�迭���� ����� ������ ������ ����
		//1���� ������ �̵��Ѵ�.
		for (int i = index; i<BallCount; i++) {
			Ball[i] = Ball[i + 1];
		}
	}

	//�׸� ó��
	virtual void Draw() {
		//ȭ���� �ػ󵵿� ���� �׸� ũ�⸦ �����Ѵ�.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;

		//�˵��� �׸���.
		for (int i = 0; i<RailCount; i++) {
			BALL_ON_RAIL_RAIL
				&r0 = Rail[i],
				&r1 = Rail[i + 1];
			float
				dx = (r0.X - r1.X) / r0.Length*0.2f,
				dy = (r0.Y - r1.Y) / r0.Length*0.2f;
			//�β��� ���� �׸���.
			Game->Texture[TEX_FILL]->Draw(
				(r0.X - dy)*sw, (r0.Y + dx)*sh, COL_BLACK, 0, 0,
				(r0.X + dy)*sw, (r0.Y - dx)*sh, COL_BLACK, 0, 1,
				(r1.X - dy)*sw, (r1.Y + dx)*sh, COL_BLACK, 1, 0,
				(r1.X + dy)*sw, (r1.Y - dx)*sh, COL_BLACK, 1, 1
			);
		}
		//���� �׸���.
		for (int i = 0; i<BallCount; i++) {
			BALL_ON_RAIL_BALL &b = Ball[i];
			float
				x = (b.X - 0.5f)*sw,
				y = (b.Y - 0.5f)*sh;
			//���� ������ ������� ĥ�Ѵ�.
			Game->Texture[TEX_ORB]->Draw(
				x, y, sw, sh, 0, 0, 1, 1, D3DXCOLOR(1, 1, 1, b.Alpha));
			//���� ������ ���� �̹����� �׸���.
			Game->Texture[TEX_ORB0 + b.Type]->Draw(
				x, y, sw, sh, 0, 0, 1, 1, D3DXCOLOR(0, 0, 0, b.Alpha));
		}

		//������ �׸���.
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
		//�߻��� ���� �׸���.
		Game->Texture[TEX_ORB0 + BType]->Draw(
			(BX - 0.5f)*sw, (BY - 0.5f)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// �Ŵ޸� ��

#define HANGING_BALL_TYPE 5
#define HANGING_BALL_ERASE 3

// ��������
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
		//���������� ���� �ʱ�ȭ
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
		//�� ���� �ʱ�ȭ
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

	//�̵�ó��
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
		//�Է»���
		if (State == 1) {
			//������ �¿�� �Է��ϸ� ������ �¿�� ȸ����Ų��.
			if (is->Left && SDir>0.52f) SDir -= 0.005f;
			if (is->Right && SDir<0.98f) SDir += 0.005f;

			//������ �����κ��� �߻��� ���� �ӵ��� ����Ѵ�.
			float rad = SDir * D3DX_PI * 2;
			BVX = cosf(rad)*0.5f;
			BVY = sinf(rad)*0.5f;

			//��ư�� ������ �߻���·� ��ȯ�Ѵ�.
			if (!PrevButton && is->Button[0]) {
				State = 2;
			}
			PrevButton = is->Button[0];
		}

		//�߻����
		if (State == 2) {
			//�߻��� ���� �̵��Ѵ�.
			MoveBall();
			//�߻��� ���� �Ŵ޸� ����
			//�����ߴ����� �˻��Ѵ�.
			int x, y;
			for (y = 0; y<bys; y++) {
				for (x = 0; x<bxs; x++) {
					//�߻��� ���� �Ŵ޸� ����
					//�߽� ���� �Ÿ��� ���Ѵ�.
					float
						dx = BX - GetBallX(x, y),
						dy = BY - GetBallY(y);
					//�߽� ���� �Ÿ���
					//�� ������ 80%�̸��̶��
					//������ ������ �����Ѵ�.
					if (
						IsBall(BallCell->Get(x, y)) &&
						dx*dx + dy * dy<0.8f
						) break;
				}
				if (x<bxs) break;
			}
			//�߻��� ���� �Ŵ޸� ���� �����ϸ�
			//�߻��� ���� �Ŵܴ�
			if (y<bys) {
				//�Ŵ޸� ���� ������ ������ �� �߿���
				//�߻��� ���� ���� ����� ���� �����Ѵ�.
				float d = 1000;
				int nx = -1, ny = -1;
				for (int i = 0; i<6; i++) {
					//������ ���� ��ǥ�� ��´�.
					int
						cx = GetAdjacentCX(x, y, i),
						cy = GetAdjacentCY(x, y, i);
					//���� �߻��� ����
					//�߽ɰ��� �Ÿ��� ����Ѵ�.
					float
						ex = BX - GetBallX(cx, cy),
						ey = BY - GetBallY(cy),
						e = sqrtf(ex*ex + ey * ey);
					//���� �����̰�
					//�Ÿ��� �ٸ� ������ ª����
					//���� ��ǥ�� �����Ѵ�.
					if (
						BallCell->Get(cx, cy) == ' ' &&
						d>e
						) {
						d = e;
						nx = cx;
						ny = cy;
					}
				}
				//���� ����� ���� ã����
				//�߻��� ���� �� ���� �����Ѵ�.
				if (nx >= 0) {
					//���� �����Ѵ�.
					BallCell->Set(nx, ny, '0' + BType);
					//�ʱ���·� ��ȯ�Ѵ�.
					State = 0;

					//���� ������ ���� �̾��� �ִ��� �˻��Ѵ�.
					TryEraseBall();
				}
			}
		}
		//��������
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

	//����� ���� ã�� ó��
	void TryEraseBall() {
		//��� ���� ���ؼ�
		//���� ������ ���� ������ ����(3��) �̻�
		//�̾��� �ִ����� �˻��Ѵ�.
		int bxs = BallCell->GetXSize(), bys = BallCell->GetYSize();
		for (int y = 0; y<bys; y++) {
			for (int x = 0; x<bxs; x++) {
				//���� ������ ���� ������ ���� �̻� �̾��� ������
				//����� ���� �����Ѵ�.
				char c = BallCell->Get(x, y);
				if (
					IsBall(c) &&
					EraseBall(x, y, c, 0x80) >= HANGING_BALL_ERASE
					) {
					//����� ���� �����Ѵ�.
					EraseBall(x, y, c | 0x80, 0x40);
					//Ÿ�̸ӷ� �����ϰ�
					//�������·� ��ȯ�Ѵ�.
					Time = 0;
					State = 3;
				}
			}
		}
		//���� ī��Ʈ �Ϸ� ǥ�ø� �����Ѵ�.
		for (int y = 0; y<bys; y++) {
			for (int x = 0; x<bxs; x++) {
				BallCell->Set(x, y, BallCell->Get(x, y) & 0x7f);
			}
		}
	}

	// ���� �����ϴ� ó��
	//������ ���� ������ �� ������ ���� ó���� �Բ��Ѵ�.
	int EraseBall(int x, int y, char c, int mask) {
		//������ ���� ������ ���� ����
		int count = 0;
		//���� ��ġ�� ���� ������ ������ ���̸�
		//���� ǥ���Ѵ�.
		if (BallCell->Get(x, y) == c) {
			//������ ��Ʈ�� ��Ʈ�Ͽ�
			//���� ǥ���Ѵ�.
			BallCell->Set(x, y, c | mask);
			//���� ��ġ�� ����
			//������ ���� ������ ���� ������ �F����.
			count++;
			//������ ���� ���ؼ��� ��������� �˻��Ͽ�
			//������ ���� ������ �� ������ ����Ѵ�.
			for (int i = 0; i<6; i++) {
				count += EraseBall(
					GetAdjacentCX(x, y, i),
					GetAdjacentCY(x, y, i),
					c, mask);
			}
		}
		//���� ������ �� ������ ��ȯ�Ѵ�.
		return count;
	}

	//���� ������ �����ϴ� ó��
	bool IsBall(char c) {
		return '0' <= c && c<'0' + HANGING_BALL_TYPE;
	}
	//������ ���� X��ǥ�� ���ϴ� ó��
	int GetAdjacentCX(int cx, int cy, int index) {
		//���� ¦�� �� �Ǵ� Ȧ�� ���� �����
		//������ ���� X��ǥ�� ���̺�
		static const int ax[2][6] = {
			{ -1, 1, 0, 1, 0, 1 },
		{ -1, 1, -1, 0, -1, 0 }
		};
		//������ ���� X��ǥ 1���� ��ȯ�Ѵ�.
		return cx + ax[cy % 2][index];
	}
	//������ ���� Y��ǥ�� ���ϴ� ó��
	int GetAdjacentCY(int cx, int cy, int index) {
		static const int ay[] =
		{ 0, 0, -1, -1, 1, 1 };
		return cy + ay[index];
	}

	//�� �߽��� X��ǥ�� ���ϴ� ó��
	//�� ��ǥ�κ��� �׸���ǥ�� ���Ѵ�.
	float GetBallX(int cx, int cy) {
		return cx + (cy + 1) % 2 * 0.5f + 0.5f;
	}
	//�� �߽����� Y��ǥ�� ���ϴ� ó��
	float GetBallY(int cy) {
		return (cy - 1)*sqrtf(3)*0.5f + 2.5f;
	}
	
	//�߻��� ���� �̵��ϴ� ó��
	void MoveBall() {
		//���� ��ǥ�� �����Ѵ�.
		BX += BVX;
		BY += BVY;
		//���� �¿��� ���� �ε�����
		//X������ �ӵ��� ������Ų��.
		if (BX<1.5f || BX >= MAX_X - 1.5f) {
			BVX = -BVX;
		}
		//���� ���� ���� �ε�����
		//Y������ �ӵ��� ������Ų��.
		if (BY<2.5f) {
			BVY = -BVY;
		}
		//���� ���������� �Ʒ����� �����
		//�ʱ���·� ��ȯ�Ѵ�.
		if (BY >= MAX_Y + 0.5f) {
			State = 0;
		}
	}

	//�׸� ó��
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
		//���� �˵��� �����Ͽ� ǥ���Ѵ�.
		if (State == 1) {
			//���� ��ǥ�� �ӵ��� �����Ѵ�.
			float bx = BX, by = BY, bvx = BVX, bvy = BVY;
			//���� ������ Ƚ����ŭ �����δ�.
			for (int i = 0; i<30 && State == 1; i++) {
				//���� �����δ�.
				MoveBall();
				//���� ��ǥ�� ���� ���� �׷���
				//���� �˵��� ǥ���Ѵ�.
				Game->Texture[TEX_ORB]->Draw(
					(BX - 0.1f)*sw, (BY - 0.1f)*sh, sw*0.2f, sh*0.2f,
					0, 0, 1, 1, COL_LGRAY);
			}
			//���� ��ǥ�� �ӵ��� ������� �ǵ�����.
			BX = bx;
			BY = by;
			BVX = bvx;
			BVY = bvy;
			State = 1;
		}

		//�Ŵ޸� �� �׸���
		for (int y = 0; y<bys; y++) {
			for (int x = 0; x<bxs; x++) {
				char c = BallCell->Get(x, y);
				//���� ���� ��쿡�� ���� �׸���.
				if (IsBall(c & 0x3f)) {
					//������ ���� ���������� �׸���.
					D3DCOLOR color = COL_BLACK;
					//����� ���� ���� ������� ��ó�� �׸���.
					if (c & 0x40) {
						float f = (float)Time / 30;
						color = D3DXCOLOR(f, f, f, 1);
					}
					//���� ������ ���� �̹����� �׸���.
					Game->Texture[TEX_ORB0 + (c & 0x3f) - '0']->Draw(
						(GetBallX(x, y) - 0.5f)*sw,
						(GetBallY(y) - 0.5f)*sh,
						sw, sh, 0, 0, 1, 1, color);
				}
			}
		}

		//������ �׸���.
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
		//�߻��� ���� �׸���.
		if (State == 1 || State == 2) {
			Game->Texture[TEX_ORB0 + BType]->Draw(
				(BX - 0.5f)*sw, (BY - 0.5f)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
		}
	}
};

//==============================================================
// ���� ȹ���� ������

#define COLLECTED_BALL_ERASE 3
#define COLLECTED_BALL_TYPE 5

// ��������
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

	//�̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�Է»���
		if (State == 0) {
			//������ �Է¿� ���� ĳ���͸� �¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && CX>1) CX--;
				if (is->Right && CX<xs - 1) CX++;
			}
			PrevLever = is->Left || is->Right;

			//��ư�� ���� ����� ó��
			if (!PrevButton) {
				//��ư 0�� ������ ���� ȹ���Ѵ�.
				if (is->Button[0]) {
					//ĳ������ �ٷ� ���ʿ���
					//���� ����� ������ �ƴ� ���� ã�´�.
					int y;
					for (y = CY - 2; Cell->Get(CX, y) == ' '; y--);
					//���� ���� ������ ���� �ʴ� ��쿡��
					//ã�Ƴ� ���� ������
					//������ �ִ� ���� ������ �����Ѵ�.
					if (BallCount == 0) {
						BallType = Cell->Get(CX, y);
					}
					//ã�Ƴ� ���� ���� ����� ó��
					if (IsBall(BallType)) {
						//���� ������ ���� ������ �ִ� ��
						//���� ��� ȹ���Ѵ�.
						for (; Cell->Get(CX, y) == BallType; y--) {
							//ȹ���� ���� ���� �������� �����Ѵ�.
							Cell->Set(CX, y, ' ');
							//������ �ִ� ���� ������ ������Ų��.
							BallCount++;
						}
					}
				}
				//��ư 1�� ������ ���� ���������� �ǵ�����.
				if (is->Button[1]) {
					//ĳ������ �ٷ� ���ʿ���
					//������ �ִ� ���� ��ġ�Ѵ�.
					int y;
					for (y = CY - 2; Cell->Get(CX, y) == ' '; y--);
					//ã�Ƴ� ���κ��� �Ʒ�������
					//������ �ִ� ���� ��ġ�Ѵ�.
					for (y++; y<CY - 1 && BallCount>0; y++) {
						//���� ���� ��ִ´�.
						Cell->Set(CX, y, BallType);
						//������ �ִ� ���� ������ ���ҽ�Ų��.
						BallCount--;
					}
					//�������� ���·� ��ȯ�Ѵ�.
					State = 1;
				}
			}
			PrevButton = is->Button[0] || is->Button[1];
		}

		//�������� ����
		if (State == 1) {
			//���� ������� �ʴ� ��쿡�� �Է»��·� ��ȯ�Ѵ�.
			State = 0;
			//�������� ���� ��� ���� ���Ͽ� �˻��Ѵ�.
			for (int x = 0; x<xs; x++) {
				for (int y = 0; y<ys; y++) {
					char c = Cell->Get(x, y);
					if (IsBall(c)) {
						//���� ���� �߰��ϸ�
						//���� ������ ���� ���η� ������ ����(3��) �̻�
						//�̾��� �ִ����� �˻��Ѵ�.
						int i;
						for (i = 1; Cell->Get(x, y + i) == c; i++);
						//������ ���� �̻����� �̾��� ������
						//�� ���� �����ϴ� ���� ������ ���� ���ش�.
						if (i >= COLLECTED_BALL_ERASE) {
							//�����ϴ� ���� ������ ���� ���ش�.
							Erase(x, y, c);
							//Ÿ�̸Ӹ� �����ϰ�
							//�������·� ��ȯ�Ѵ�.
							Time = 0;
							State = 2;
						}
						//���� ���� ���� ����
						//���� ������ ���� ������ �ִ�����
						//�˻��ϴ� ó���� ����Ѵ�.
						y += i - 1;
					}
				}
			}
		}

		//��������
		if (State == 2) {
			//���� �ð��� ����ϱ⸦ ��ٸ���.
			Time++;
			if (Time == 30) {
				//�������� �ȿ��� ����� ���� ã�´�.
				for (int x = 0; x<xs; x++) {
					for (int y = CY - 2; y >= 0; y--) {
						//����� ���� ã�� ����� ó��
						if (Cell->Get(x, y) & 0x80) {
							//����� ������ �Ʒ��� �ִ� ����
							//���� �ű��.
							for (int i = y; i<CY - 2; i++) {
								Cell->Set(x, i, Cell->Get(x, i + 1));
							}
							//�������� �ϴ��� ���� �������� �����Ѵ�
							Cell->Set(x, CY - 2, ' ');
						}
					}
				}
				//�Է� ���·� ��ȯ�Ѵ�.
				State = 0;
			}
		}

		return true;
	}

	//���� ������ �����ϴ� ó��
	bool IsBall(char c) {
		return '0' <= c && c<'0' + COLLECTED_BALL_TYPE;
	}

	//������ ���� ���ִ� ó��
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
// ���� �ٲ�ֱ�

#define SWAPPED_BALL_TYPE 5
#define SWAPPED_BALL_ERASE 3

// ��������
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

	//�̵�ó��
	virtual bool Move(const CInputState* is) {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//�Է»���
		if (State == 0) {
			//������ �Է¿� ���� Ŀ���� �����¿�� �����δ�.
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

			//��ư�� ������ ������ �ִ� ����
			//Ŀ�� ��ġ�� �ִ� ���� �ٲ� �ִ´�.
			if (!PrevButton && is->Button[0]) {
				//������ �ִ� ���� ������
				//Ŀ�� ��ġ�� �ִ� ���� ������ �ٲ� �ִ´�.
				char c = Cell->Get(CX, CY);
				Cell->Set(CX, CY, BallType);
				BallType = c;
				//���ϻ��·� ��ȯ�Ѵ�.
				State = 1;
			}
			PrevButton = is->Button[0];
		}

		//���ϻ���
		if (State == 1) {
			//�������� ���� ��� ���� ���ؼ�
			//�Ʒ��� ������ ���� �ִ� ���� ���Ͻ�Ų��.
			for (int x = 0; x<xs; x++) {
				for (int y = 0; y<ys; y++) {
					//������ ���� �߰��� ����� ó��
					if (Cell->Get(x, y) == ' ') {
						//���� �ִ� ���� ���Ͻ�Ų��.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//�������� ����� ������ ���� �����Ѵ�.
						Cell->Set(x, 2, ' ');
					}
				}
			}
			//�������� ���·� ��ȯ�Ѵ�.
			State = 2;
		}

		//�������� ����
		if (State == 2) {
			//���� ������� �ʴ� ��쿡�� �Է»��·� ��ȯ�Ѵ�.
			State = 0;
			//�������� ���� ��� ���� ���ؼ� �˻��Ѵ�.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					//���� ������ ���� ������ ����(3��)�̻� �̾��� ������
					//�� ������ ���ش�.
					char c = Cell->Get(x, y);
					if (
						IsBall(c) &&
						Erase(x, y, c, 0x80) >= SWAPPED_BALL_ERASE
						) {
						//���� ����� ���� �����Ѵ�.
						Erase(x, y, c | 0x80, 0x40);
						//Ÿ�̸Ӹ� �����ϰ�, �������·� ��ȯ�Ѵ�.
						Time = 0;
						State = 3;
					}
				}
			}
			//���� ī��Ʈ�Ϸ� ǥ�ø� �����Ѵ�.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//��������
		if (State == 3) {
			//���� �ð��� ����ϴ� ���� ��ٸ���.
			Time++;
			if (Time == 30) {
				//����� ���� ������ �����Ѵ�.
				for (int x = 0; x<xs; x++) {
					for (int y = 0; y<ys; y++) {
						if (Cell->Get(x, y) & 0x40) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//���ϻ��·� ��ȯ�Ѵ�.
				State = 1;
			}
		}

		return true;
	}

	//���� �������� �����ϴ� ó��
	bool IsBall(char c) {
		return '0' <= c && c<'0' + SWAPPED_BALL_TYPE;
	}

	//������ ���� ���ִ� ó��
	int Erase(int x, int y, char c, int mask) {
		//������ ���� ������ ���������� �˻��Ѵ�.
		if (Cell->Get(x, y) == c) {
			//������ ������� ���� ǥ���Ѵ�.
			//(Ư�� ��Ʈ 1�� �Ѵ�)
			Cell->Set(x, y, c | mask);
			//�����¿쿡 ������ ���� ���ؼ��� ��������� �˻��Ͽ�
			//���� ������ ���� ������ ������ �հ踦 ��ȯ�Ѵ�.
			return
				1 +
				Erase(x - 1, y, c, mask) +
				Erase(x + 1, y, c, mask) +
				Erase(x, y - 1, c, mask) +
				Erase(x, y + 1, c, mask);
		}
		//������ ���� ������ ������ �ƴ϶��
		//���� ������ ���� ������ ������ 0�� ��ȯ�Ѵ�.
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
// ���� ��ó�� �����̱�

#define SNAKE_BALL_COUNT 5
#define SNAKE_BALL_TYPE 5
#define SNAKE_BALL_ERASE 3

// ��������
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

	//�̵�ó��
	virtual bool Move(const CInputState* is) {
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�ʱ����
		if (State == 0) {
			//�Է»��·� ��ȯ�Ѵ�.
			State = 1;
			//�� ����� ���� ��ǥ�� ������ �ʱ�ȭ�Ѵ�.
			for (int i = 0; i<SNAKE_BALL_COUNT; i++) {
				//���� ���� 1���� �����ǵ��� ��ǥ�� �����Ѵ�.
				CX[i] = (xs - SNAKE_BALL_COUNT) / 2 + i;
				CY[i] = 2;

				//���� ������ ���Ƿ� �����Ѵ�.
				BallType[i] = '0' + Rand.Int31() % SNAKE_BALL_TYPE;
				//���� ��Ÿ�� ��ġ�� �ٸ� ���� �ִ� ����
				//�ʱ���¸� �����ϰ� ���� ������Ű�� �ʴ´�.
				if (Cell->Get(CX[i], CY[i]) != ' ') {
					State = 0;
				}
			}
		}

		//�Է»���
		if (State == 1) {
			//������ �Է¿� ���� ���� ������ ���� �����¿�� �����δ�.
			if (!PrevLever) {
				//������ �Է¹��⿡ ���� �̵������� ��ǥ�� ���Ѵ�.
				int cx = CX[0], cy = CY[0];
				if (is->Left) cx--; else
					if (is->Right) cx++; else
						if (is->Up) cy--; else
							if (is->Down) cy++;
				//���� ���� ���� �̵������� ������ ���̶�� ���� �̵��Ѵ�.
				if (Cell->Get(cx, cy) == ' ') {
					//���� ������ ���� �ڵ����� ���� �����ϴ����� �˻��Ѵ�.
					int i;
					for (i = 0; i<SNAKE_BALL_COUNT; i++) {
						if (CX[i] == cx && CY[i] == cy) break;
					}
					//�������� �ʴ� ��쿡�� ���� �̵��Ѵ�.
					if (i == SNAKE_BALL_COUNT) {
						//�ڵ����� ���� ��ǥ�� �ٷ� ���� ���� ��ǥ�� �����Ѵ�.
						for (i = SNAKE_BALL_COUNT - 1; i>0; i--) {
							CX[i] = CX[i - 1];
							CY[i] = CY[i - 1];
						}
						//���� ���� ���� �����δ�.
						CX[0] = cx;
						CY[0] = cy;
					}
				}
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			//��ư�� ������ �� ��ġ�� ���� �����Ѵ�.
			if (!PrevButton && is->Button[0]) {
				//���� ������ ���� ��ִ´�.
				for (int i = 0; i<SNAKE_BALL_COUNT; i++) {
					Cell->Set(CX[i], CY[i], BallType[i]);
				}
				//���ϻ��·� ��ȯ�Ѵ�.
				State = 2;
			}
			PrevButton = is->Button[0];
		}

		//���ϻ���
		if (State == 2) {
			//�������� ���� ������ ���� ã�´�.
			for (int x = 0; x<xs; x++) {
				for (int y = 0; y<ys; y++) {
					//������ ���� ã�� ����� ó��
					if (Cell->Get(x, y) == ' ') {
						//������ ���� ���� �ִ� ���� 1�ܾ� ���Ͻ�Ų��.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//�������� ����� ���� �������� �����Ѵ�.
						Cell->Set(x, 2, ' ');
					}
				}
			}
			//������������ ��ȯ�Ѵ�.
			State = 3;
		}

		//�������� ����
		if (State == 3) {
			//���� ������� �ʾ����� �ʱ���·� ��ȯ�Ѵ�.
			State = 0;
			//�������� ���� ��� ���� ���Ͽ� �˻��Ѵ�.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					//���� ���� ���ؼ�
					//����, ����, �밢���� ���� ������ ����
					//�̾��� �ִ����� �˻��Ѵ�.
					char c = Cell->Get(x, y);
					if (IsBall(c & 0x7f)) {
						//�Ʒ���, ������, ���� �Ʒ�, ������ �Ʒ��� ���ؼ� �˻��Ѵ�.
						static const int
							vx[] = { 0, 1, -1, 1 },
							vy[] = { 1, 0, 1, 1 };
						for (int v = 0; v<4; v++) {
							//���� ������ ���� �̾��� �ִ� ������ ����.
							int count = 0;
							for (
								int i = x, j = y;
								(Cell->Get(i, j) & 0x7f) == c;
								i += vx[v], j += vy[v]
								) {
								count++;
							}
							//������ ����(3��) �̻��� ���� �̾��� ������
							//���� ����� ���� �����Ѵ�.
							if (count >= SNAKE_BALL_ERASE) {
								for (
									int i = x, j = y, k = 0;
									k<count;
									i += vx[v], j += vy[v], k++
									) {
									//����� ���� ǥ�ø� �Ѵ�.
									Cell->Set(i, j, c | 0x80);
								}
								//Ÿ�̸Ӹ� �����ϰ�, �������·� ��ȯ�Ѵ�.
								Time = 0;
								State = 4;
							}
						}
					}
				}
			}
		}

		//��������
		if (State == 4) {
			//���� �ð��� ����ϸ�
			//����� ���� ������ �����Ѵ�.
			Time++;
			if (Time == 30) {
				//����� ���� ã�´�.
				for (int x = 0; x<xs; x++) {
					for (int y = 0; y<ys; y++) {
						//����� ���� ������ ���� �����Ѵ�.
						if (Cell->Get(x, y) & 0x80) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//���ϻ��·� ��ȯ�Ѵ�.
				State = 2;
			}
		}

		return true;
	}

	//���� ������ �����ϴ� ó��
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
// ���������� �� �߻��ϱ�

#define FLIPPED_BALL_TYPE 5
#define FLIPPED_BALL_ERASE 3
#define FLIPPED_BALL_POWER 11

// ��������
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

	//�̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�ʱ����
		if (State == 0) {
			//�������� ������� ���� �����Ѵ�.
			Power = 0;
			BX = 1;
			BallType = '0' + Rand.Int31() % FLIPPED_BALL_TYPE;
			State = 1;
		}

		//�Է»���
		if (State == 1) {
			//������ �Ʒ��� �Է��ϸ� �������� ���� ���� ���� ������Ų��.
			if (is->Down && Power<FLIPPED_BALL_POWER) {
				Power += 0.1f;
			}
			//������ �Ʒ��� �Է����� ������ �߻���·� ��ȯ�Ѵ�.
			if (!is->Down && Power>0) {
				State = 2;
			}
			//�������� ���� ���� ���� ���� ���� ��ǥ�� ��ȭ��Ų��.
			BY = 2 + Power * (10 - 2) / FLIPPED_BALL_POWER;
		}

		//�߻����
		if (State == 2) {
			//���� �������� ��ܿ� ������ ������ ���� �̵��Ѵ�.
			if (BY>2) {
				BY -= 0.5f;
			}
			//���������� ��ܿ� �����ϸ�
			//���������� ������ �������� �̵��Ѵ�.
			else {
				//�������� ���� ���� ���� ���� ���� X��ǥ�� �����Ѵ�.
				BX = 3 + (int)Power;
				//���� Y��ǥ�� ���������� ������� �����Ѵ�.
				BY = 2;
				State = 3;
			}
		}
		//���� ����
		if (State == 3) {
			//���� �Ʒ��� ���� �����̸� ���� ���Ͻ�Ų��.
			if (Cell->Get(BX, (int)BY + 1) == ' ') {
				BY += 0.5f;
			}
			else {
				//���� �Ʒ��� ���� ������ �ƴϸ�
				//���� �� ��ġ�� �����Ѵ�.

				//���� ������ ���� ��ִ´�.
				Cell->Set(BX, (int)BY, BallType);
				//�������·� ��ȯ�Ѵ�.
				State = 4;
			}
		}

		//�������� ����
		if (State == 4) {
			//���� ������� ������ �ʱ���·� ��ȯ�Ѵ�.
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

		//��������
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

	//���� �������� �����ϴ� ó��
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
// ������ ���� ��

// ��������
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

