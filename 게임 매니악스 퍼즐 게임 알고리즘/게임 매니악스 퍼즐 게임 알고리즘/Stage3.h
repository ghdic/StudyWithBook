//==============================================================
// ���� �����ϱ�

//ĳ���͸� �����Ű�� ���� ����
static const int
CONNECTED_RAIL_TIME = 60,	//ĳ���Ͱ� 1���� ���� ����ϴ� �ð�
CONNECTED_RAIL_VX[] = { 1, 0, -1, 0 },	//�̵� ����(0-3)��
CONNECTED_RAIL_VY[] = { 0, 1, 0, -1 },	//X����� Y������ ��ǥ�� ��ȯ�ϱ� ���� ���̺�
//���ο� �� ���� �̵�����(0-3)��
//���ο��� �������� ���� �̵� �������� ��ȯ�ϱ� ���� ���̺�
//(4�� �̵� �������� ���� �� �� ���� ���� ǥ���Ѵ�)
//���տ��� ������ ��ġ�� arr[dir]���� ������ ���ö��� �����Ѵ�(dir�� ����)  
CONNECTED_RAIL_DIR[][4] = {
	{ 4, 4, 4, 4 },
{ 4, 4, 1, 0 },{ 1, 4, 4, 2 },
{ 3, 2, 4, 4 },{ 4, 0, 3, 4 },
{ 0, 4, 2, 4 },{ 4, 1, 4, 3 },
{ 0, 1, 2, 3 }
};

// ���
// 	3
// 	��
//2��O��0
//	��
//	1
//Dir��
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
	//ĳ������ �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� �ð��� ����� ������ Ÿ�̸Ӹ� �����Ѵ�.
		if (Time<CONNECTED_RAIL_TIME) {
			Time++;
		}
		//���� �ð��� ����ϸ� �̵� ������ �����Ѵ�.
		else {
			//�̵� ������ �� ��ǥ�� ����Ѵ�.
			int
				cx = CX + CONNECTED_RAIL_VX[Dir],
				cy = CY + CONNECTED_RAIL_VY[Dir];
			//�̵� ������ �������� �������� Ȯ���Ѵ�.
			if (
				cx >= 0 && cx<Cell->GetXSize() &&
				cy >= 0 && cy<Cell->GetYSize()
				) {
				//�̵� ������ ���ο� ������ �̵� �������� ������ ��쿡 ����
				//���ο� �̵� ������ ��´�.
				int dir = CONNECTED_RAIL_DIR[Cell->Get(cx, cy) - '0'][Dir];
				//���ο� ������ �� �ִ����� �˻��Ѵ�.
				if (dir != 4) {
					//������ �� ������ ����ǥ�� �̵� ������ �����ϰ�
					//Ÿ�̸Ӹ� �����Ѵ�.
					CX = cx;
					CY = cy;
					Dir = dir;
					Time = 0;
				}
			}
		}

		//���� ĳ���Ͱ� �ִ� ���� ��´�.
		char c = Cell->Get(CX, CY) - '0' - 1;
		//���� ������ ���
		if (0 <= c && c <= 3) {
			//ĳ���Ͱ� ���ư��� �ϱ� ���� ����
			static const int
				//ȸ���߽��� �����ǥ
				rx[] = { 1, 0, 0, 1 },
				ry[] = { 1, 1, 0, 0 },
				//ȸ���� �����ϴ� ����
				rad[][4] = {
					{ 2, 3, 4, 4 },{ 4, 3, 0, 4 },
			{ 4, 4, 0, 1 },{ 2, 4, 4, 1 }
			},
				//ȸ������
				vrad[][4] = {
					{ 1, -1, 0, 0 },{ 0, 1, -1, 0 },
			{ 0, 0, 1, -1 },{ -1, 0, 0, 1 }
			};
			//Ÿ�̸��� ���� ����
			//���θ� ���� �ε巴�� ���ư�����
			//ĳ������ �׸���ǥ�� �����Ѵ�.
			float f = (rad[c][Dir] + vrad[c][Dir] * (float)Time / CONNECTED_RAIL_TIME)*D3DX_PI / 2;
			X = CX + rx[c] - 1.0f / 6 + cosf(f) / 2;
			Y = CY + ry[c] - 1.0f / 6 + sinf(f) / 2;
		}
		//������ ���� �Ǵ� ������ ������ ���
		else {
			//Ÿ�̸��� ���� ����
			//���θ� ���� �ε巴�� �����ϵ���
			//ĳ������ �׸���ǥ�� �����Ѵ�.
			float f = (float)Time / CONNECTED_RAIL_TIME - 0.5f;
			X = CX + 1.0f / 3 + CONNECTED_RAIL_VX[Dir] * f;
			Y = CY + 1.0f / 3 + CONNECTED_RAIL_VY[Dir] * f;
		}
		return true;
	}

	virtual void Draw() {}
};

// ��������
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

	//���������� �ʱ�ȭ ó��
	virtual void Init() {
		Cell->Init(
			(char*)
			"152152"
			"457366"
			"157213"
			"453430"
		);
		//Ŀ���� ��ǥ�� �����Ѵ�.
		CX = Cell->GetXSize() - 1;
		CY = Cell->GetYSize() - 1;
		//���¸� ��ȯ�Ѵ�.
		State = 0;
		//ĳ���͸� �����Ѵ�.
		Man = new CConnectedRailMan(Cell, 1, 0, 0);
	}

	//���������� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//�Է� ������ ó��
		if (State == 0) {
			//������ �������� �Է��� ��쿡
			//Ŀ���� ��ġ�� ���������� ���� ���� �ƴ϶��
			//Ŀ���� �������� �̵���Ų��.
			if (is->Left && CX>0) {
				//�̵� ����� Ÿ�̸Ӹ� �����ϰ�
				//�̵� ���·� ��ȯ�Ѵ�.
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
		//�̵� ������ ó��
		if (State == 1) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			//Ÿ�̸Ӱ� ���� ���� �����ϸ�
			//Ŀ���� ������ ��ġ�� �ٲ� �ִ´�.
			if (Time == 10) {
				//�ٲ� ���� ������ ���� ĳ���Ͱ� �ִٸ�
				//ĳ���͵� ���ο� ��ġ�� �����Ѵ�.
				if (Man->CX == CX + VX && Man->CY == CY + VY) {
					Man->CX = CX;
					Man->CY = CY;
				}
				//Ŀ���� ������ ���� �ٲ� �ִ´�.
				Cell->Swap(CX, CY, CX + VX, CY + VY);
				//Ŀ���� ��ǥ�� �����Ѵ�.
				CX += VX;
				CY += VY;
				//�Է� ���·� ��ȯ�Ѵ�.
				State = 0;
			}
		}
		return true;
	}

	//���������� �׸� ó��
	virtual void Draw() {
		//���ο��� ��(���ο����� ��)�� 0���� �ʱ�ȭ�Ѵ�.
		PathCell->Clear(0);
		//ĳ������ ���� ��ġ�� �̵� ������
		//���ο����� �ʱ� ��ġ �� �ʱ� �������� �����Ѵ�.
		int cx = Man->CX, cy = Man->CY, dir = Man->Dir;
		//������ ���� ������ �̵� �������� ����� ������ ���ο����� ����Ѵ�.
		while (!(PathCell->Get(cx, cy)&(1 << dir))) {
			//������ ���� ������ �̵� ������ ��ִ´�.
			PathCell->Set(cx, cy, PathCell->Get(cx, cy) | (1 << dir));
			//���� ���� �̵��Ѵ�.
			cx += CONNECTED_RAIL_VX[dir],
				cy += CONNECTED_RAIL_VY[dir];
			//���������� ������ ������ �Ǹ� ���ο����� �����Ѵ�.
			if (
				cx<0 || cx >= Cell->GetXSize() ||
				cy<0 || cy >= Cell->GetYSize()
				) break;
			//������ ������ ���� �̵� ������ ��ȭ��Ų��.
			//���ο� ���� �� �� ���� ��쿡�� ���ο����� �����Ѵ�.
			int ndir = CONNECTED_RAIL_DIR[Cell->Get(cx, cy) - '0'][dir];
			if (ndir == 4) break;
			dir = ndir;
		}

		//���� ������ ȭ���� �ػ󵵿� ����
		//�׸��� �׸� ũ�⸦ �����Ѵ�.
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / (ys + 1);

		//���������� ��� ���� ���� ó���� �Ѵ�.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				//���� ������ ��´�.
				char c = Cell->Get(x, y) - '0';
				//�׸���ǥ�� ����Ѵ�.
				float
					dx = x, dy = y + 1,
					f = Time * 0.1f, vx = VX * f, vy = VY * f;
				//�̵����¶��
				//Ŀ���� �ε巴�� �����̱� ����
				//�׸���ǥ�� �����Ѵ�.
				if (State == 1 && x == CX && y == CY) {
					dx += vx;
					dy += vy;
				}
				//�̵����¶��
				//���θ� �ε巴�� �����̱� ���Ͽ�
				//�׸���ǥ�� �����Ѵ�.
				else
					if (State == 1 && x == CX + VX && y == CY + VY) {
						dx -= vx;
						dy -= vy;
					}
				//�� ��ǥ�� ȭ����� ��ǥ�� ��ȯ�Ѵ�.
				dx *= sw;
				dy *= sh;
				//���ο��� ���� �˻��Ѵ�.
				char pc = PathCell->Get(x, y);
				//���� ������ ������ ��쿡��
				//���θ� �׸��� ���� ȸ���� �簢���� �׷��� ������ �������� ��Ÿ����.
				if (pc) {
					//�������� ���θ� ���� ����(������ �Ʒ��� �Ǵ� �Ʒ����� ����)���θ�
					//ĳ���Ͱ� ����ϴ� ����� �׸� ó��
					if (c == 7 && !(pc & 0x05)) {
						Game->Texture[TEX_FILL]->Draw(
							dx + sw / 3, dy, sw / 3, sh,
							0, 0, 1, 1, COL_LGRAY);
					}
					//������ ���θ� ���� ����(���ʿ��� ������ �Ǵ� �����ʿ��� ����)���θ�
					//ĳ���Ͱ� ����ϴ� ����� �׸� ó��
					else
						if (c == 7 && !(pc & 0x0a)) {
							Game->Texture[TEX_FILL]->Draw(
								dx, dy + sh / 3, sw, sh / 3,
								0, 0, 1, 1, COL_LGRAY);
						}
					//ĳ���Ͱ� ���� ���� �Ǵ� ������ ���θ� ����ϴ� ���,
					//�Ǵ� �������� ���θ� ���ο� ���� �������� ����ϴ� ����� �׸� ó��
						else {
							Game->Texture[TEX_FILL]->Draw(
								dx, dy, sw, sh,
								0, 0, 1, 1, COL_LGRAY);
						}
				}
				//���� �Ǵ� Ŀ���� �̹����� �׸���.
				//ȸ���� ���ο� ���ļ� ������ �׷����� �׸���.
				Game->Texture[TEX_RAIL0 + c]->Draw(
					dx, dy, sw, sh,
					0, 0, 1, 1, COL_BLACK);
			}
		}
		//ĳ������ �׸���ǥ
		float dx = Man->X, dy = Man->Y + 1;
		//Ŀ���� �̵������̰�
		//ĳ���Ͱ� �����̰� �ִ� ������ ���� �ִ� ��쿡��
		//���ο� �Բ� �����̵��� �׸���ǥ�� �����Ѵ�.
		if (State == 1 && Man->CX == CX + VX && Man->CY == CY + VY) {
			dx -= VX * Time*0.1f;
			dy -= VY * Time*0.1f;
		}
		//ĳ���� �̹����� �׸���.
		Game->Texture[TEX_MAN]->Draw(
			dx*sw, dy*sh, sw / 3, sh / 3, 0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// ������ �����ϱ�

#define CONNECTED_PIPE_COUNT 7

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//������ �Է¿� ���� Ŀ���� �̵���Ų��.
		int cx = CX, cy = CY;
		if (!PrevLever) {
			//������ �Է¹��⿡ ���� ���ο� ��ǥ�� ���Ѵ�.
			if (is->Left) cx--; else
				if (is->Right) cx++; else
					if (is->Up) cy--; else
						if (is->Down) cy++;
			//���ο� ��ǥ�� ���� ���̳� ��ü�� �������� �ƴ϶��
			//Ŀ���� ���ο� ��ǥ�� �̵���Ų��.
			char c = Cell->Get(cx, cy);
			if (c != '#' && c != '+') {
				CX = cx;
				CY = cy;
			}
		}
		//������ ������ �Է��� �����Ѵ�.
		PrevLever = is->Left | is->Right | is->Up | is->Down;
		//��ư�Է��� �ִ� ��쿡��
		//Ŀ���� ��ġ�� ���ο� �������� ���´�.
		if (!PrevButton && is->Button[0]) {
			//���� ���ο� ������ ������ ��ִ´�.
			Cell->Set(cx, cy, '0' + NewPipe);
			//���� �������� ������ �������� �����Ѵ�.
			NewPipe = Rand.Int31() % CONNECTED_PIPE_COUNT + 1;
		}
		//������ ��ư�Է��� �����Ѵ�.
		PrevButton = is->Button[0];
		return true;
	}

	//���������� �׸� ó��
	virtual void Draw() {
		//���� ũ��
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//���ο��� ���� Ŭ�����Ѵ�.
		PathCell->Clear(0);
		//�������� �ȿ� �ִ� ��� ��ü�� �������� ����
		//�������������� �������� Ÿ�� �帣�� ��ü�� ���θ� �����Ѵ�.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				//��ü�� ������ ���� ã�´�.
				if (Cell->Get(x, y) == '+') {
					//��� ����(�����¿�)�� ���ؼ�
					//���θ� �����Ѵ�.
					for (int i = 0; i<4; i++) {
						//�ʱ� ��ġ�� �̵� ������ �����Ѵ�.
						int cx = x, cy = y, dir = i;
						//������ ���� ������ �̵� �������� ����� ������
						//������ ������ ����Ѵ�.
						while (!(PathCell->Get(cx, cy)&(1 << dir))) {
							//������ ���� ���� �̵� ������ ����´�.
							PathCell->Set(cx, cy, PathCell->Get(cx, cy) | (1 << dir));
							//���� ���� �̵��Ѵ�.
							cx += CONNECTED_RAIL_VX[dir],
								cy += CONNECTED_RAIL_VY[dir];
							char c = Cell->Get(cx, cy) - '0';
							//������ �̿��� ��(��, ��ü�� ������, ������ ��)��
							//���� ��쿡�� ������ ������ �����Ѵ�.
							if (c<0 || c >= CONNECTED_PIPE_COUNT + 1) break;
							//�������� ������ ���� �̵� ������ ��ȭ��Ų��.
							//�������� ���� �� ���� ��쿡�� ������ ������ �����Ѵ�.
							int ndir = CONNECTED_RAIL_DIR[c][dir];
							if (ndir == 4) break;
							dir = ndir;
						}
					}
				}
			}
		}
		//�׸��� ũ�⸦ ����Ѵ�.
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		//���������� ��� ���� �׸���.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				//���� ������ ��´�.
				char c = Cell->Get(x, y);
				//���� ������ ���� �׷���(��ü�� ������, ��)�� �����Ѵ�.
				int t = TEX_VOID;
				if (c == '+') {
					t = TEX_CONVEYOR1;
				}
				else
					if (c == '#') {
						t = TEX_FLOOR;
					}
					else
						//���� �������� ���
						if ('0' <= c && c<'0' + CONNECTED_PIPE_COUNT + 1) {
							//�������� ���¿� ���� �׷����� �����Ѵ�.
							t = TEX_RAIL0 + c - '0';
							//���ο��� ���� �����Ѵ�.
							char pc = PathCell->Get(x, y);
							//���� ������ ������ ��쿡��
							//�������� �׸��� ���� ȸ�� �簢���� �׷�
							//������ ���� ǥ���Ѵ�.
							if (pc) {
								//������ �������� ����(������ �Ʒ� �Ǵ� �Ʒ����� ��)�θ�
								//������ ���
								if (c == '7' && !(pc & 0x05)) {
									Game->Texture[TEX_FILL]->Draw(
										x*sw + sw / 3, y*sh, sw / 3, sh,
										0, 0, 1, 1, COL_LGRAY);
								}
								//������ �������� ����(���ʿ��� ������ �Ǵ� �����ʿ��� ����)���θ�
								//������ ���
								else
									if (c == '7' && !(pc & 0x0a)) {
										Game->Texture[TEX_FILL]->Draw(
											x*sw, y*sh + sh / 3, sw, sh / 3,
											0, 0, 1, 1, COL_LGRAY);
									}
								//���� ������, ������ �������� �������� ���
								//�Ǵ� ������ �������� ���ο� ���η� ��� ������ ���
									else {
										Game->Texture[TEX_FILL]->Draw(
											x*sw, y*sh, sw, sh,
											0, 0, 1, 1, COL_LGRAY);
									}
							}
						}
				//�������� �׸���.
				Game->Texture[t]->Draw(x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
			}
		}
		//���ο� �������� �׸���.
		Game->Texture[TEX_RAIL0 + NewPipe]->Draw(CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
	}
};

//==============================================================
// �����Ͽ� ���� �����

// ����
class CLinkedShapeMolecule : public CMover {
public:
	//�յڷ� ������ �ִ� ��ü
	CLinkedShapeMolecule * Prev;
	CLinkedShapeMolecule* Next;
	//�ӵ�
	float VX, VY;

	//������
	CLinkedShapeMolecule(float x, float y)
		: Prev(NULL), Next(NULL)
	{
		//�׷����� �����Ѵ�.
		Texture = Game->Texture[TEX_MOLECULE1];
		//��ǥ�� �������� �����Ѵ�.
		X = Rand.Int31() % 2 * (MAX_X + 1) - 1;
		Y = Rand.Int31() % 2 * (MAX_Y + 1) - 1;
		//��ü�� ���ذ��� �ӵ��� �����Ѵ�.
		VX = x - X;
		VY = y - Y;
		float v = sqrtf(VX*VX + VY * VY) / 0.02f;
		if (v>0) {
			VX /= v;
			VY /= v;
		}
	}

	//�̵�ó��
	virtual bool Move(const CInputState* is) {
		//��ü�� �ٸ� ��ü�� ���յǾ� ���� ���� ����� ó��
		if (!Prev || Prev == this) {
			//�̸� ������ �ӵ��� �����Ѵ�.
			X += VX;
			Y += VY;
			//�׷����� ȸ����Ų��.
			Angle += 0.01f;
			//ȭ�� ������ ������ �����ϴ�.
			return X >= -1 && X <= MAX_X && Y >= -1 && Y <= MAX_Y;
		}
		//��ü�� �ٸ� ��ü�� ���յǾ� �ִ� ���ȿ��� �������� �ʴ´�.
		return true;
	}
};

// Ŀ��
class CLinkedShapeCursor : public CLinkedShapeMolecule {
public:
	//������ ��ư ����
	bool PrevButton;

	//������
	CLinkedShapeCursor()
		: CLinkedShapeMolecule(0, 0), PrevButton(true)
	{
		//�׷����� �˻��Ѵ�.
		Texture = Game->Texture[TEX_MOLECULE0];
		//��ǥ�� ȭ���� �߽����� �����.
		X = (MAX_X - 1) / 2;
		Y = (MAX_Y - 1) / 2;
		//��ü�� �ٸ� ��ü�� �������� �ʵ��� �Ѵ�.
		//(��ü �ڽſ��� ������ �ִ� ������ �����Ѵ�.)
		Prev = this;
	}

	//�̵�ó��
	virtual bool Move(const CInputState* is) {
		//�̵��ӵ�
		static const float speed = 0.1f;
		//������ �Է¿� ���� �ӵ��� �����Ѵ�.
		VX = VY = 0;
		if (is->Left) VX = -speed;
		if (is->Right) VX = speed;
		if (is->Up) VY = -speed;
		if (is->Down) VY = speed;
		//ȭ�� ������ ���� ������ �ӵ��� 0���� �����Ѵ�.
		if (X + VX<0 || X + VX>MAX_X - 1) VX = 0;
		if (Y + VY<0 || Y + VY>MAX_Y - 1) VY = 0;

		//���������� ������ ��ü�� ã�´�.
		CLinkedShapeMolecule* tail = this;
		while (tail->Next) tail = tail->Next;

		//��ü�� ���յǾ� �ִ� ��� ��ü�� ���Ͽ� ó���Ѵ�.
		for (CLinkedShapeMolecule* m = this; m; m = m->Next) {
			//��ü�� �Բ� �����δ�.
			m->X += VX;
			m->Y += VY;
			//���� ���յ��� ���� ��ü���� ������ �˻��Ѵ�.
			for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
				//ȭ��� �ִ� ��ü �߿��� 1���� ��´�.
				CLinkedShapeMolecule* n = (CLinkedShapeMolecule*)i.Next();
				//���������� ũ��, ���� ���� �Ÿ�
				static const float hit = 1.0f, link = 0.6f;
				//� ��ü�� ���� ������ ���� �ʰ�
				//��ü �Ǵ� ������ ��ü�κ����� �Ÿ��� ���� �� �̸��̶��
				//�� ��ü�� ���ս�Ų��.
				if (
					!n->Prev &&
					abs(n->X - m->X)<hit && abs(n->Y - m->Y)<hit
					) {
					//������ ���⿡ ����
					//���� ��, ���� �Ʒ�, ������ ��, ������ �Ʒ� �� �������� ���ս�Ų��.
					n->X = m->X + (n->X<m->X ? -link : link);
					n->Y = m->Y + (n->Y<m->Y ? -link : link);
					//ȸ���� �ʱ�ȭ�Ѵ�.
					n->Angle = 0;
					//���������� ������ ��ü�� ����Ѵ�.
					n->Prev = tail;
					tail->Next = n;
					tail = n;
				}
			}
		}

		//��ư�� �Է��� ����� ó��
		//��ư�� ������ ���� ��
		//����ؼ� ��ü�� �и��Ǿ� ������ �ʵ���
		//������ ��ư �Է��� ���¸� �˻��Ѵ�.
		if (!PrevButton && is->Button[0]) {
			//��ü�� 1�� �̻��� ��ü�� ������ ���� ���� ó��
			if (tail->Prev) {
				//�и��� ��ü�� �ӵ���
				//��ü�κ��� �־������� �����Ѵ�.
				float
					vx = tail->X - X, vy = tail->Y - Y,
					v = sqrtf(vx*vx + vy * vy) / 0.12f;
				if (v>0) {
					tail->VX = vx / v;
					tail->VY = vy / v;
				}
				//��ü�� ����Ʈ���� �����Ѵ�.
				tail->Prev->Next = NULL;
				//(��ü �ڽſ��� ������ �ִ� ������ �����Ѵ�)
				tail->Prev = tail;
			}
		}
		//��ư�� �Է� ���¸� �����Ѵ�.
		PrevButton = is->Button[0];

		return true;
	}
};

// ��������
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
// ������ �ѷ��� ĥ�ϱ�
//# ���ؼ�, + ���ο� ��, * ĥ�� ����

// ��
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
	//���� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//�̵� ������ ���� �˻��Ͽ�
		//������� ������(�� �Ǵ� ĥ���� ����)
		//�̵� ������ �������� ��ȭ ��Ų��.
		if (Cell->Get(CX + VX, CY + VY) != ' ') {
			//�̵� ������ ���� ��, ���� �Ʒ�, ������ ��, ������ �Ʒ� �߿���
			//�������� �����Ѵ�.
			VX = Rand.Int31() % 2 * 2 - 1;
			VY = Rand.Int31() % 2 * 2 - 1;
		}
		else {
			//�̵� ������ ���� ��������� �̵� �������� �����δ�.
			CX += VX;
			CY += VY;
		}
		return true;
	}

	virtual void Draw() {}
};

// Ŀ��
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

	//Ŀ���� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//������ �Է¿� ���� �̵� ������ �����Ѵ�.
		int vx = 0, vy = 0;
		if (is->Left) vx = -1; else
			if (is->Right) vx = 1; else
				if (is->Up) vy = -1; else
					if (is->Down) vy = 1;
		//1ĭ ���� ���� 2ĭ ���� ���� ��´�.
		char
			c1 = Cell->Get(CX + vx, CY + vy),
			c2 = Cell->Get(CX + vx * 2, CY + vy * 2);
		//��ư�� ������ ���� ���� ó��
		if (is->Button[0]) {
			//1ĭ ���� ���� �����̰�
			//2ĭ ���� ���� ������ ��쿡�� ���� �ߴ´�.
			if (c1 == ' ' && c2 == ' ') {
				//1ĭ ���� ���� 2ĭ �տ� ���� ������ �����Ѵ�
				Cell->Set(CX + vx, CY + vy, '+');
				Cell->Set(CX + vx * 2, CY + vy * 2, '+');
			}
			else
				//1ĭ ���� ���� �����̰�
				//2ĭ ���� ���� ���ؼ��� ��쿡 ������ �ݴ´�.
				if (c1 == ' ' && c2 == '#') {
					//1ĭ ���� ���� ������ �����Ѵ�.
					Cell->Set(CX + vx, CY + vy, '+');
					//���� �ִ� ������ %���� ĥ�Ѵ�.
					Paint(Enemy->CX, Enemy->CY);
					//ĥ���� ������ ��������
					//���� ���� ������ ĥ�Ѵ�
					//���� ���ο� ���� ���ؼ����� �����Ѵ�.
					for (int y = 3; y<Cell->GetYSize() - 3; y++) {
						for (int x = 3; x<Cell->GetXSize() - 3; x++) {
							switch (Cell->Get(x, y)) {
								//���� ���� ����(���� ��)��
								//*�� �����Ѵ�.
							case ' ':
								Cell->Set(x, y, '*');
								break;
								//���� �ִ� ����%�� �������� �����Ѵ�.
							case '%':
								Cell->Set(x, y, ' ');
								break;
								//���ο+�� ���ؼ�#���� �����Ѵ�.
							case '+':
								Cell->Set(x, y, '#');
								break;
							}
						}
					}
					//������ ���� ������ ���� ���ؼ��� ĥ�Ѵ�.
					for (int y = 2; y<Cell->GetYSize() - 2; y++) {
						for (int x = 2; x<Cell->GetXSize() - 2; x++) {
							//���ؼ��� ���� ó��
							if (Cell->Get(x, y) == '#') {
								//�� ����(8����)�� �ִ�
								//���� ���� ������ ����.
								int count = 0;
								for (int i = -1; i <= 1; i++) {
									for (int j = -1; j <= 1; j++) {
										if (Cell->Get(x + i, y + j) == ' ') {
											count++;
										}
									}
								}
								//���� ���� �ϳ��� ���� ������
								//���ؼ��� ĥ�Ѵ�.
								if (count == 0) {
									Cell->Set(x, y, '*');
								}
							}
						}
					}
				}
			//�� ���� ��쿡�� Ŀ���� �̵����� �ʴ´�.
				else {
					vx = vy = 0;
				}
		}
		else {
			//��ư�� ������ ���� ���� ���� ó��
			//1ĭ ���� ���� 2ĭ ���� ����
			//��� ���ؼ��� ��� �̿ܿ���
			//Ŀ���� �̵����� �ʴ´�.
			if (c1 != '#' || c2 != '#') {
				vx = vy = 0;
			}
		}
		//Ŀ���� �� ��ǥ�� �����Ѵ�.
		//������ �������� �ʵ��� �ϱ� ����
		//���� 2ĭ�� �̵��Ѵ�.
		CX += vx * 2;
		CY += vy * 2;
		return true;
	}

	//ĥ�ϱ� ó��
	void Paint(int x, int y) {
		//���� ���� �����̶��
		//ĥ�ϴ� ó���� �Ѵ�.
		if (Cell->Get(x, y) == ' ') {
			int lx, rx;
			//���� ���� ��� �� ������
			//������ ���κ��� �������� �̵��Ѵ�.
			for (lx = x; Cell->Get(lx - 1, y) == ' '; lx--);
			//���� ���� ��ӵ� ������
			//���� ���κ��� ���������� �̵��Ѵ�.
			for (rx = x; Cell->Get(rx + 1, y) == ' '; rx++);
			//���� ���� %�� ĥ�Ѵ�.
			for (x = lx; x <= rx; x++) {
				Cell->Set(x, y, '%');
			}
			//ĥ�� ���� ���� ���Ʒ� ���� ���Ͽ�
			//ĥ�ϱ� ó���� ��������� �����Ѵ�.
			for (x = lx; x <= rx; x++) {
				Paint(x, y - 1);
				Paint(x, y + 1);
			}
		}
	}

	virtual void Draw() {}
};

// ��������
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
// �� �� �׸���� ������ ȸ���ϱ�

// ���
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

	//ĳ������ �̵�ó��
	virtual bool Move(const CInputState* is) {
		//�Է� ����
		if (State == 0) {
			//������ �Է¿� ����
			//�����¿�� �̵� ������ �����Ѵ�.
			VX = VY = 0;
			if (is->Left) VX = -1; else
				if (is->Right) VX = 1; else
					if (is->Up) VY = -1; else
						if (is->Down) VY = 1;
			//������ �Էµ� ����� ó��
			if (VX != 0 || VY != 0) {
				//�̵� ������ ���� �˻��Ѵ�.
				char c = Cell->Get(CX + VX, CY + VY);
				//�̵� ������ ���� �����̳� �������̸�
				if (c == ' ' || c == 'I') {
					//ĳ���Ͱ� ������ ������ ����´�.
					Cell->Set(CX, CY, '+');
					//�� ��ǥ�� �����Ѵ�.
					CX += VX;
					CY += VY;
					//�������� ����� ����
					//���� �������� �����Ѵ�.
					Cell->Set(CX, CY, ' ');
					//Ÿ�̸Ӹ� �����ϰ�
					//�̵����·� ��ȯ�Ѵ�.
					State = 1;
					Time = 0;
				}
			}
		}
		//�̵� ����
		if (State == 1) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			//ȭ��󿡼� ĳ���Ͱ� �ε巴�� �����̵���
			//�׸���ǥ�� ���ݾ� ��ȭ��Ų��.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			//���� �ð��� ����ϸ� �Է»��·� ��ȯ�Ѵ�.
			if (Time == 10) {
				State = 0;
			}
		}
		return true;
	}
};

// ��������
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
// �ܾ� �����

#define CROSSWORD_KANA_COUNT 5
#define CROSSWORD_MIN_LENGTH 2
#define CROSSWORD_MAX_LENGTH 4

// ��������
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

	//���������� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//���� ũ��
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//�ʱ� ����
		if (State == 0) {
			//���ο� ���ڸ� �����Ѵ�.
			Kana = Rand.Int31() % CROSSWORD_KANA_COUNT;
			//�Է»��·� ��ȯ�Ѵ�.
			State = 1;
		}
		//�Է»���
		if (State == 1) {
			//������ �Է¿� ����
			//Ŀ���� �����¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && CX>0) CX--; else
					if (is->Right && CX<xs - 1) CX++; else
						if (is->Up && CY>0) CY--; else
							if (is->Down && CY<ys - 1) CY++;
			}
			//������ ������ �Է��� �����Ѵ�.
			PrevLever = is->Left || is->Right || is->Up || is->Down;
			//Ŀ���� ���ڰ� ���� ���� ���� ��
			//��ư�� ������
			//Ŀ���� ��ġ�� ���ο� ���ڸ� ���´�.
			if (
				!PrevButton &&
				is->Button[0] &&
				Cell->Get(CX, CY) == CROSSWORD_KANA_COUNT
				) {
				//���ο� ���ڸ� ���� ��ִ´�.
				Cell->Set(CX, CY, Kana);
				//���� ������ �ܾ ã�� ���� �غ�
				//Ž�� ��ǥ�� ����, �ܾ��� ���� ���� �����Ѵ�.
				X = CX;
				Y = max(0, CY - CROSSWORD_MAX_LENGTH + 1);
				VX = 0;
				VY = 1;
				Length = CROSSWORD_MIN_LENGTH;
				//Ž�����·� ��ȯ�Ѵ�.
				State = 2;
			}
			PrevButton = is->Button[0];
		}
		//�ܾ� ����
		static const wchar_t* dictionary[] = {
			L"����", L"��������", L"����", L"����", L"������",
			L"������", L"����", L"����", L"����", L"������", L"����",
			L"����", L"����", L"����",
			L"����", L"����",
			L"����", L"����", L"����", L"������", L"������",
			NULL
		};
		//Ž������
		if (State == 2) {
			//������������ �о�� �ܾ�
			wchar_t word[CROSSWORD_MAX_LENGTH + 1];
			//������ ������ Ž���Ѵ�.
			for (; X <= CX && Y <= CY; X += VX, Y += VY, Length = CROSSWORD_MIN_LENGTH) {
				//������������ ������ �ʰ����� �ʴ� ���� �� �ܾ��� ���� ���� ���Ѵ�.
				int max_length = min(CROSSWORD_MAX_LENGTH, (xs - X)*VX + (ys - Y)*VY);
				//���� ª�� ���� ������ ���� �� ���� �������� �˻��Ѵ�.
				for (; Length <= max_length; Length++) {
					//���������� ������ ���ڸ� �о�
					//�ܾ��� ���ڿ��� �����Ѵ�.
					int i, x, y;
					for (i = 0, x = X, y = Y; i<Length; i++, x += VX, y += VY) {
						word[i] = L"����������?"[Cell->Get(x, y)];
					}
					word[i] = L'\0';
					//���� ������ ���ڸ� �����ϰ� �ִ� ����� ó��
					if (x>CX || y>CY) {
						//������ �ܾ�� ��ġ�ϴ��� �˻��Ѵ�.
						for (i = 0; dictionary[i]; i++) {
							if (wcscmp(word, dictionary[i]) == 0) break;
						}
						//������ �ܾ�� ��ġ�ϸ� ������ Ż���Ѵ�.
						if (dictionary[i]) break;
					}
				}
				//������ �ܾ�� ��ġ�� ��쿡�� �� ������ Ż���Ѵ�.
				if (Length <= max_length) break;
			}
			//���� ������ Ž���� �Ϸ��ϸ�
			if (Y>CY) {
				//���� �������� �ܾ ã�� ���� �غ�
				//Ž�� ��ǥ�� ����, �ܾ��� ���� ���� �����Ѵ�.
				X = max(0, CX - CROSSWORD_MAX_LENGTH + 1);
				Y = CY;
				VX = 1;
				VY = 0;
				Length = CROSSWORD_MIN_LENGTH;
			}
			else
				//���� ������ Ž���� �Ϸ��ϸ�
				if (X>CX) {
					//�ʱ���·� ��ȯ�Ѵ�.
					State = 0;
				}
			//���� �ܾ�� ��ġ�� ����� ó��
				else {
					//��ġ�� �κ��� �ٸ� ������ ǥ���ϱ� ���Ͽ�
					//���� ǥ�ø� �Ѵ�.
					for (int i = 0, x = X, y = Y; i<Length; i++, x += VX, y += VY) {
						Cell->Set(x, y, Cell->Get(x, y) | 0x80);
					}
					//���ݴ� �� �ܾ ã�´�.
					Length++;
					//Ÿ�̸Ӹ� �����ϰ� ǥ�û��·� ��ȯ�Ѵ�.
					Time = 0;
					State = 3;
				}
		}
		//ǥ�û���
		if (State == 3) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			if (Time == 30) {
				//��ġ�� �κ��� ǥ�ø� �����ϱ� ���Ͽ�
				//���� ǥ�ø� �����.
				for (int i = 0, x = X, y = Y; i<Length; i++, x += VX, y += VY) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
				//Ž�����·� ��ȯ�Ѵ�.
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




