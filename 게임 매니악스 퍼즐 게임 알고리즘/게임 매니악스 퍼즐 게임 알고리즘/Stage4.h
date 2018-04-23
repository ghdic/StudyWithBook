//==============================================================
// ����� �簢������ ����� ���ֱ�

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�Է� ����
		if (State == 0) {
			//������ �Է¿� ���� ĳ���͸� �¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && CX>0) CX--;
				if (is->Right && CX<xs - 1) CX++;
			}
			PrevLever = is->Left || is->Right;

			//��ư�� ������ ����� �߻��Ѵ�.
			if (!PrevButton && is->Button[0] && Cell->Get(CX, CY - 1) == ' ') {
				Cell->Set(CX, CY, '+');
			}
			PrevButton = is->Button[0];

			//�������� ������ ���� �������� �����Ѵ�.
			for (int x = 1; x<xs - 1; x++) {
				Cell->Set(x, 0, ' ');
			}
			//���ο� ����� ��(+)�� ���� �̵���Ų��.
			for (int y = 1; y<ys - 1; y++) {
				for (int x = 1; x<xs - 1; x++) {
					//���ο� ����� ���� �߰ߵ� ����� ó��
					if (Cell->Get(x, y) == '+') {
						char c = Cell->Get(x, y - 1);
						//���ο� ����� ���� ���� �����̶��
						//���ο� ����� ��ĭ ���� �̵��Ѵ�.
						if (c == ' ') {
							Cell->Set(x, y - 1, '+');
							Cell->Set(x, y, ' ');
						}
						else {
							//���ο� ����� ���� ���� ������ �ƴ϶��
							//���ο� ����� ������ ��ġ�� �����Ѵ�.
							Cell->Set(x, y, c);
							//����� ����� ó���� ȣ���Ѵ�.
							EraseBlock(x, y);
						}
					}
				}
			}

			//���� �ð����� ����� ���Ͻ�Ų��.
			DropTime++;
			if (DropTime == 60) {
				DropTime = 0;
				int x, y;
				//���������� �Ʒ����� ������ �ƴ϶��
				//����� ���Ͻ�Ű�� �ʴ´�.
				for (x = 1; x<xs - 1; x++) {
					if (Cell->Get(x, ys - 3) != ' ') break;
				}
				//�������� �Ʒ����� �����̶��
				//����� ���Ͻ�Ų��.
				if (x == xs - 1) {
					//��� ���� 1�ܾ� �Ʒ��� �̵��Ѵ�.
					for (y = ys - 3; y>0; y--) {
						for (x = 1; x<xs - 1; x++) {
							Cell->Set(x, y, Cell->Get(x, y - 1));
						}
					}
				}
			}
		}
		//���������� ó��
		if (State == 1) {
			//���� �ð��� ����ϸ� ����� �����.
			Time++;
			if (Time == 30) {
				//�簢������ ä���� ����� �����.
				for (int y = T; y<B; y++) {
					for (int x = L; x<R; x++) {
						Cell->Set(x, y, ' ');
					}
				}
				//�Է»��·� ��ȯ�Ѵ�.
				State = 0;
			}
		}

		return true;
	}

	//�簢���� ���·� ä���� ����� ����� ó��
	void EraseBlock(int x, int y) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//���� ��ġ�� ���� ��´�.
		char c = Cell->Get(x, y);
		//���� ��ġ�κ��� �����ǿ��
		//����� ����� ������ �˻��Ѵ�.
		for (L = x; Cell->Get(L - 1, y) == c; L--);
		for (R = x + 1; Cell->Get(R, y) == c; R++);
		for (T = y; Cell->Get(x, T - 1) == c; T--);
		for (B = y + 1; Cell->Get(x, B) == c; B++);

		//�簢���� ���� �ȿ� ������ ���� �ִ��� �˻��Ѵ�.
		for (y = T; y<B; y++) {
			for (x = L; x<R; x++) {
				if (Cell->Get(x, y) != c) break;
			}
			if (x<R) break;
		}

		//������ ���� ���� ����� ó��
		if (y == B) {
			//�簢�� ������ 1ĭ �ۿ�
			//����� ���� �ִ����� �˻��Ѵ�.
			for (x = L; x<R; x++) {
				if (Cell->Get(x, T - 1) == c || Cell->Get(x, B) == c) break;
			}
			for (y = T; y<B; y++) {
				if (Cell->Get(L - 1, y) == c || Cell->Get(R, y) == c) break;
			}
			//����� ���� ���� ����� ó��
			if (x == R && y == B) {
				//�簢���� ������ ����� ���(*)���� �����Ѵ�.
				for (int y = T; y<B; y++) {
					for (int x = L; x<R; x++) {
						Cell->Set(x, y, '*');
					}
				}
				//Ÿ�̸Ӹ� �����ϰ� �������·� ��ȯ�Ѵ�.
				Time = 0;
				State = 1;
			}
		}
	}

	//���������� �׸� ó��
	virtual void Draw() {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//�׷��� �� ũ��
		float
			sw = Game->GetGraphics()->GetWidth() / xs,
			sh = Game->GetGraphics()->GetHeight() / ys;
		//���� ��(=)�� �׸���.
		for (int y = 0; y<ys; y++) {
			for (int x = 0; x<xs; x++) {
				if (Cell->Get(x, y) == '=') {
					Game->Texture[TEX_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
				}
			}
		}
		//����� ���� �׸���.
		for (int y = 1; y<ys - 1; y++) {
			for (int x = 1; x<xs - 1; x++) {
				char c = Cell->Get(x, y);
				//���ο� ���(+)�� �׸���.
				if (c == '+') {
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY);
				}
				else
					//�� ���� ����� �׸���.
					if (c != ' ') {
						//�����¿쿡 ���� ������ �����
						//������ �ִ����� ���� �׸���ǥ�� �����ϰ�
						//����� ������ ǥ���Ѵ�
						static const float gap = 0.1f;
						float l = x, r = x + 1, t = y, b = y + 1;
						if (c != Cell->Get(x - 1, y)) l += gap;
						if (c != Cell->Get(x + 1, y)) r -= gap;
						if (c != Cell->Get(x, y - 1)) t += gap;
						if (c != Cell->Get(x, y + 1)) b -= gap;

						//����� ����� �ð��� ����� ���� ���� �帣�� �׸���
						//������ ����� ���������� �׸���.
						float f = (float)Time / 30;
						D3DCOLOR color = COL_BLACK;
						if (c == '*') color = D3DXCOLOR(f, f, f, 1);
						Game->Texture[TEX_FILL]->Draw(
							l*sw, t*sh, (r - l)*sw, (b - t)*sh, 0, 0, 1, 1, color);
					}
			}
		}
		//ĳ���͸� �׸���.
		Game->Texture[TEX_MAN]->Draw(
			CX*sw, CY*sh, sw, sh, 0, 0, 1, 1, COL_BLACK);
	}
};

//==============================================================
// ����� ������Ű��

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���������� �� ����, ����� �� ����
		int
			xs = StageCell->GetXSize(), ys = StageCell->GetYSize(),
			bxs = BlockCell->GetXSize(), bys = BlockCell->GetYSize();

		//�ʱ� ����
		if (State == 0) {
			//����� ���� �������� �����Ѵ�.
			BlockCell->Clear(' ');
			//����� ���� ���� �Ʒ� �ܿ� ���� 1���� ����� ��ִ´�.
			for (int x = 0; x<bxs; x++) {
				BlockCell->Set(x, bys - 1, '#');
			}
			//��� ���� �ʱ���ġ�� �����Ѵ�.
			CX = (xs - bxs) / 2;
			CY = -1;
			//���ο� ����� �ٸ� ��Ͽ�
			//�����ߴ����� �˻��Ѵ�.
			if (!StageCell->Hit(CX, CY, BlockCell)) {
				//�������� ���� ��쿡�� �Է»��·� ��ȯ�Ѵ�.
				DropTime = 0;
				PrevUp = true;
				State = 1;
			}
		}
		//�Է� ����
		if (State == 1) {
			//������ �Է¿� ���� ���ο� ����� �¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && !StageCell->Hit(CX - 1, CY, BlockCell)) CX--;
				if (is->Right && !StageCell->Hit(CX + 1, CY, BlockCell)) CX++;
			}
			PrevLever = is->Left | is->Right;

			//���� �ð����� ���ο� ����� ���� �ö󰡰� �Ѵ�.
			//������ ���� �Է��� ��쿡�� Ÿ�̸ӿ� ������� ��ٷ� ���� �ö󰡰� �Ѵ�.
			DropTime++;
			if (DropTime == 60 || (!PrevUp && is->Up)) {
				DropTime = 0;
				//���ο� ����� �ٸ� ��Ͽ� �������� �ʴ� ��쿡��
				//����� ���� �ö󰡰� �Ѵ�.
				if (!StageCell->Hit(CX, CY - 1, BlockCell)) {
					CY--;
				}
				else {
					//���ο� ����� �ٸ� ��Ͽ� �����ϴ� ���
					StageCell->Merge(CX, CY, BlockCell);
					//����� ������� �ʴ� ��쿡��
					//�ʱ���·� ��ȯ�Ѵ�.
					State = 0;
					//����� ��������� �˻��Ѵ�.
					for (int y = 2; y<ys - 1; y++) {
						//���� �������� �����
						//1�� ä���� �ִ��� �˻��Ѵ�.
						int x;
						for (x = 1; x<xs - 1; x++) {
							if (StageCell->Get(x, y) != '#') break;
						}
						//1�� ä���� �ִ� ����� ó��
						if (x == xs - 1) {
							//����� ����� ������� �����Ѵ�.
							for (x = 1; x<xs - 1; x++) {
								StageCell->Set(x, y, '+');
							}
							//�������·� ��ȯ�Ѵ�.
							Time = 0;
							State = 2;
						}
					}
				}
			}
			//������ �Է��� ä�� �ִ� ��쿡
			//����� �����ؼ� �̵����� �ʵ��� �ϱ� ���� ó��
			if (!is->Up) PrevUp = false;

			//��ư�� ���� ��쿡 ����� ������Ų��.
			if (!PrevButton) {
				//� ��ư�� �������� �˻��Ѵ�.
				int x;
				for (x = 0; !is->Button[x] && x<bxs; x++);
				//��ȿ�� ��ư�� ���� ����� ó��
				if (x<bxs) {
					//���������� ���� �������� ������
					//��ư�� �����ϴ� �κ��� ����� 1�� ���� �ø���.
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
			//��ư�� ���� ä�� �ִ� ��쿡
			//����� �������ؼ� �������� �ʵ��� �ϱ� ���� ó��
			PrevButton = false;
			for (int i = 0; i<bxs; i++) {
				PrevButton = PrevButton || is->Button[i];
			}
		}

		//��������
		if (State == 2) {
			//���� �ð��� ����ϸ� ����� ����� ������ �����.
			Time++;
			if (Time == 30) {
				//����� ���(+)�� ã�´�.
				for (int y = ys - 3; y >= 2; y--) {
					//����� ����� �߰ߵǸ�
					//�� �ܺ��� �Ʒ��� �ִ� ����
					//1�ܾ� ���� �̵��Ѵ�.
					if (StageCell->Get(1, y) == '+') {
						for (int x = 1; x<xs - 1; x++) {
							for (int i = y; i<ys - 2; i++) {
								StageCell->Set(x, i, StageCell->Get(x, i + 1));
							}
							//���� �Ʒ��� ���� �������� �����Ѵ�.
							StageCell->Set(x, ys - 2, ' ');
						}
					}
				}
				//�ʱ���·� ��ȯ�Ѵ�.
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
// ����� �ε��� �μ���

#define STRUCK_BLOCK_COUNT 2

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		
		//�ʱ����
		if (State == 0) {
			//���ο� ����� ��ġ
			CX = xs / 2;
			CY = 0;
			//���ο� ����� ������ ���Ƿ� �����Ѵ�.
			Type = Rand.Int31() % STRUCK_BLOCK_COUNT;
			//Ÿ�̸� ���� �ʱ�ȭ�ϰ�
			//�Է»��·� ��ȯ�Ѵ�.
			DropTime = 0;
			PrevDown = true;
			State = 1;
		}

		//�Է»���
		if (State == 1) {
			//������ �Է¿� ���� ���ο� ����� �¿�� �̵��Ѵ�.
			if (!PrevLever) {
				if (is->Left && Cell->Get(CX - 1, CY) == ' ') CX--;
				if (is->Right && Cell->Get(CX + 1, CY) == ' ') CX++;
			}
			PrevLever = is->Left | is->Right;

			//���� �ð����� ���ο� ����� ���Ͻ�Ų��.
			//������ �Ʒ��� �Է��ϸ� Ÿ�̸ӿ� ������� ��ٷ� ���Ͻ�Ų��.
			DropTime++;
			if (DropTime == 60 || (!PrevDown && is->Down)) {
				DropTime = 0;

				//1�� �Ʒ��� ���� ������ ��쿡��
				//���ο� ����� �Ʒ��� �̵��Ѵ�.
				if (Cell->Get(CX, CY + 1) == ' ') {
					CY++;
				}
				else {
					//1�� �Ʒ��� ���� ������ �ƴ� ��쿡��
					//���ο� ����� �����Ѵ�.
					//Ÿ�̸Ӹ� �ʱ�ȭ �Ѵ�.
					Time = 0;
					//����� ������� �ʴ� ��쿡��
					//�ʱ���·� ��ȯ�Ѵ�.
					State = 0;
					//����� �����Ѵ�.
					Cell->Set(CX, CY, '0' + Type);
					//����� �ε�ġ�� ó���� ȣ���Ѵ�.
					Strike(CX, CY + 1, '0' + Type);
				}
			}
			if (!is->Down) PrevDown = false;
		}
		//�ı�����
		if (State == 2) {
			//���� �ð��� ����ϸ�
			//�μ��� ��Ͽ� ���� ����� �ϰų� �����Ѵ�.
			Time++;
			if (Time == 30) {
				//����� ����� ������
				//�ʱ���·� ��ȯ�Ѵ�.
				State = 0;
				//�μ��� ����� ã�´�.
				for (int y = 0; y<ys; y++) {
					for (int x = 0; x<xs; x++) {
						char c = Cell->Get(x, y);
						//�μ��� ����� �߰ߵǸ�
						//���� ����� �ϰų� �����Ѵ�.
						if (c & 0x80) {
							c &= 0x7f;
							if ('0' <= c && c<'0' + STRUCK_BLOCK_COUNT) {
								Cell->Set(x, y, c + STRUCK_BLOCK_COUNT);
							}
							//�̹� ���� ���� �����
							//�����Ѵ�.
							else {
								Cell->Set(x, y, ' ');
								//���ϻ��·� ��ȯ�Ѵ�.
								Time = 0;
								State = 3;
							}
						}
					}
				}
			}
		}

		//���ϻ���
		if (State == 3) {
			//���� �ð��� ����ϸ�
			//����� ����� ���� �ִ� ����� ���Ͻ�Ų��.
			Time++;
			if (Time == 30) {
				//������ ���� ã�´�.
				for (int x = 1; x<xs - 1; x++) {
					for (int i = 1; i<ys - 1; i++) {
						//������ ���� �߰ߵǸ�
						//���� �ִ� ��� ���� 1�ܾ� �Ʒ��� �̵��Ѵ�.
						if (Cell->Get(x, i) == ' ') {
							for (int y = i; y>0; y--) {
								Cell->Set(x, y, Cell->Get(x, y - 1));
							}
							//���� ���� ���� ���� �������� �����Ѵ�.
							Cell->Set(x, 0, ' ');
						}
					}
				}
				//�ʱ���·� ��ȯ�Ѵ�.
				State = 0;
			}
		}

		return true;
	}

	//����� �ε�ġ�� ó��
	void Strike(int x, int y, char c) {
		//���� ��ġ�� ���� ��´�.
		char d = Cell->Get(x, y);
		//���� ��ġ�� ����
		//������ ������ ���(���� ����/����)�� ����� ó��
		if (d == c || d == c + STRUCK_BLOCK_COUNT) {
			//����� �μ��� ������� �����Ѵ�.
			//��Ʈ������ ����� ��Ͽ� ǥ���Ѵ�.
			Cell->Set(x, y, d | 0x80);
			//�����¿� �� �밢��(8����)�� ��Ͽ� ���ؼ���
			//����� �ε�ġ�� ó���� ��������� �����Ѵ�.
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) {
						Strike(x + i, y + j, c);
					}
				}
			}
			//�ı����·� ��ȯ�Ѵ�.
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
// ���������� ȸ����Ű��

#define ROTATED_STAGE_SIZE 8

// ��������
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
	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//���������� ������ ���� ���� ��ǥ, �������� ������ ũ��
		int rx = 4, ry = 2, rs = 8;

		//�Է»���
		if (State == 0) {
			//������ �������� �Է��ϸ� ���������� �������� ȸ����Ų��.
			if (!PrevLever && is->Left) {
				//���������� 1/4������ ���ؼ� ���� �ٲ� �ִ´�.
				for (int y = 0; y<rs / 2; y++) {
					for (int x = 0; x<rs / 2; x++) {
						Cell->Swap(rx + x, ry + y, rx + rs - 1 - y, ry + x);
						Cell->Swap(rx + rs - 1 - y, ry + x, rx + rs - 1 - x, ry + rs - 1 - y);
						Cell->Swap(rx + rs - 1 - x, ry + rs - 1 - y, rx + y, ry + rs - 1 - x);
					}
				}
				//���ϻ��·� ��ȯ�Ѵ�.
				Time = 0;
				State = 1;
			}
			else
				//������ ���������� �Է��ϸ� ���������� ���������� ȸ����Ų��.
				if (!PrevLever && is->Right) {
					//������������ 1/4������ ���ؼ� ���� �ٲ� �ִ´�.
					//(���� ȸ������ �������� ���� �ٲ� �ִ´�)
					for (int y = 0; y<rs / 2; y++) {
						for (int x = 0; x<rs / 2; x++) {
							Cell->Swap(rx + rs - 1 - x, ry + rs - 1 - y, rx + y, ry + rs - 1 - x);
							Cell->Swap(rx + rs - 1 - y, ry + x, rx + rs - 1 - x, ry + rs - 1 - y);
							Cell->Swap(rx + x, ry + y, rx + rs - 1 - y, ry + x);
						}
					}
					//���ϻ��·� ��ȯ�Ѵ�.
					Time = 0;
					State = 1;
				}
			//������ �Է��� ä�� �ִ� ��쿡
			//����ؼ� ȸ�������ʵ��� �ϱ� ���� ó��
			PrevLever = is->Left || is->Right;
		}

		//���ϻ���
		if (State == 1) {
			//���� �ð��� ����ϸ� ����� ���Ͻ�Ų��.
			Time++;
			if (Time == 30) {
				//������ ���� ã�´�.
				for (int x = rx; x<rx + rs; x++) {
					for (int y = ry; y<ry + rs; y++) {
						//������ ���� ������ ���� �ִ� ���� 1�ܾ� �Ʒ��� �̵��Ѵ�.
						if (Cell->Get(x, y) == ' ') {
							for (int i = y; i>ry; i--) {
								Cell->Set(x, i, Cell->Get(x, i - 1));
							}
							//���� ���� ���� ���� �������� �����Ѵ�.
							Cell->Set(x, ry, ' ');
						}
					}
				}
				//�Է»��·� ��ȯ�Ѵ�.
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
// ���� ��� ���ֱ�
//��(3)>����(1), ���(2)>�����(0)

#define FOOD_BLOCK_COUNT 2

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�ʱ� ����
		if (State == 0) {
			//���ο� ����� ��ġ
			CX = xs / 2;
			CY = 0;
			//���ο� ����� ������ ���Ƿ� �����Ѵ�.
			Type = Rand.Int31() % (FOOD_BLOCK_COUNT * 2);
			//Ÿ�̸� ���� �ʱ�ȭ�ϰ� �Է»��·� ��ȯ�Ѵ�.
			DropTime = 0;
			PrevDown = true;
			State = 1;
		}
		//�Է� ����
		if (State == 1) {
			//������ �Է¿� ���� ���ο� ����� �¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && Cell->Get(CX - 1, CY) == ' ') CX--;
				if (is->Right && Cell->Get(CX + 1, CY) == ' ') CX++;
			}
			PrevLever = is->Left || is->Right;
			//���� �ð����� ���ο� ����� ���Ͻ�Ų��.
			DropTime++;
			if (DropTime == 60 || (!PrevDown && is->Down)) {
				DropTime = 0;
				//�Ʒ����� ���� �����̸� ���Ͻ�Ų��.
				if (Cell->Get(CX, CY + 1) == ' ') {
					CY++;
				}
				//�Ʒ����� ���� ������ �ƴϸ�
				//����� ������Ű�� �������� ���·� ��ȯ�Ѵ�.
				else {
					Cell->Set(CX, CY, '0' + Type);
					State = 2;
				}
			}
			if (!is->Down) PrevDown = false;
		}

		//���� ��������
		if (State == 2) {
			//Ÿ�̸Ӹ� �ʱ�ȭ�Ѵ�.
			Time = 0;
			//����� ������� �ʴ� ��쿡�� �ʱ���·� ��ȯ�Ѵ�.
			State = 0;
			//���� ���� �߰��ϸ�
			//������ �ִ� ���̸� �Դ´�.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					char eater = Cell->Get(x, y);
					//������ ������ ���̰� ������
					//���̸� �Դ� ó���� ȣ���Ѵ�.
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
							//������ ����� ������� ǥ���Ѵ�.
							Cell->Set(x, y, eater | 0x80);
							//������ ���̸� �Դ´�.
							Eat(x - 1, y, food);
							Eat(x + 1, y, food);
							Eat(x, y - 1, food);
							Eat(x, y + 1, food);
							//�������·� ��ȯ�Ѵ�.
							State = 3;
						}
					}
				}
			}
		}

		//���� ����
		if (State == 3) {
			//���� �ð��� ����ϸ� ����� ����� ������ �����Ѵ�.
			Time++;
			if (Time == 30) {
				for (int y = 0; y<ys; y++) {
					for (int x = 0; x<xs; x++) {
						//����� ����� �߰��ϸ�
						//���� �������� �����Ѵ�.
						if (Cell->Get(x, y) & 0x80) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//���ϻ��·� ��ȯ�Ѵ�.
				Time = 0;
				State = 4;
			}
		}

		//���ϻ���
		if (State == 4) {
			//���� �ð��� ����ϸ� �Ʒ��� �ƹ��͵� ���� ��ġ�� ����� ���Ͻ�Ų��.
			Time++;
			if (Time == 30) {
				//������ ���� ã�´�.
				for (int x = 0; x<xs; x++) {
					for (int i = 0; i<ys; i++) {
						//������ ���� �߰��ϸ�
						//���� �ִ� ���� 1�ܾ� �Ʒ��� �̵��Ѵ�.
						if (Cell->Get(x, i) == ' ') {
							for (int y = i; y>0; y--) {
								Cell->Set(x, y, Cell->Get(x, y - 1));
							}
							//���� ���� ���� ���� �������� �����Ѵ�.
							Cell->Set(x, 0, ' ');
						}
					}
				}
				//�������� ���·� ��ȯ�Ѵ�.
				State = 2;
			}
		}

		return true;
	}

	//���̸� �Դ� ó��
	void Eat(int x, int y, char food) {
		//���� ��ġ�� ���� ��´�.
		char c = Cell->Get(x, y);
		//���� ������ ������ ����� ó��
		if (c == food) {
			//�����¿��� ���� ���ؼ���
			//���̸� �Դ� ó���� ��������� �����Ѵ�.
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
// ������� �ѷ��� ���ֱ�

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�ʱ� ����
		if (State == 0) {
			//���ο� ����� ��ǥ
			CX = xs / 2;
			CY = 0;

			//���ο� ����� ����(��� �Ǵ� ������)��
			//���Ƿ� �����Ѵ�.
			Type = (Rand.Int31() % 2) ? '#' : '+';
			//Ÿ�̸� ���� �ʱ�ȭ�ϰ�
			//�Է»��·� ��ȯ�Ѵ�.
			DropTime = 0;
			PrevDown = true;
			State = 1;
		}

		//�Է»���
		if (State == 1) {
			//������ �Է¿� ���� ���ο� ����� �¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && Cell->Get(CX - 1, CY) == ' ') CX--;
				if (is->Right && Cell->Get(CX + 1, CY) == ' ') CX++;
			}
			PrevLever = is->Left || is->Right;

			//�����ð����� ���ο� ����� ���Ͻ�Ų��.
			DropTime++;
			if (DropTime == 60 || (!PrevDown && is->Down)) {
				DropTime = 0;
				//�Ʒ����� ���� �����̶�� ���Ͻ�Ų��.
				if (Cell->Get(CX, CY + 1) == ' ') {
					CY++;
				}
				//�Ʒ����� ���� ������ �ƴ϶��
				//����� ������Ű�� �������� ���·� ��ȯ�Ѵ�.
				else {
					Cell->Set(CX, CY, Type);
					State = 2;
				}
			}
			if (!is->Down) PrevDown = false;
		}

		//�������� ����
		if (State == 2) {
			//Ÿ�̸Ӹ� �ʱ�ȭ�Ѵ�.
			Time = 0;
			//����� ������� �ʴ� ������
			//�ʱ���·� ��ȯ�Ѵ�.
			State = 0;
			//������ ���� ã�´�.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					//������ ���� �߰��ϸ�
					//������ �ִ� ������ ���� ���� ����.
					if (
						Cell->Get(x, y) == '+' &&
						Count(x, y) == 0
						) {
						//������ ���� ������
						//�������� ����� ���������� �����ϰ�
						//�������·� ��ȯ�Ѵ�.
						Erase(x, y);
						State = 3;
					}
				}
			}
			//ī��Ʈ �Ϸ� ǥ�ø� �����Ѵ�.
			for (int y = 0; y<ys; y++) {
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//��������
		if (State == 3) {
			//���� �ð��� ����ϸ� ����� �������� ������ �����Ѵ�.
			Time++;
			if (Time == 30) {
				//����� �������� ã�´�.
				for (int y = 0; y<ys; y++) {
					for (int x = 0; x<xs; x++) {
						//����� �������� �߰��ϸ�
						//���� �������� �����Ѵ�.
						if (Cell->Get(x, y) & 0x40) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//���ϻ��·� ��ȯ�Ѵ�.
				Time = 0;
				State = 4;
			}
		}

		//���ϻ���
		if (State == 4) {
			//���� �ð��� ����ϸ� �Ʒ��� �ƹ��͵� ���� ��ġ�� ���� ���Ͻ�Ų��.
			Time++;
			if (Time == 30) {
				//������ ���� ã�´�.
				for (int x = 0; x<xs; x++) {
					for (int i = 0; i<ys; i++) {
						//������ ���� �߰��ϸ�
						//���� �ִ� ���� 1�ܾ� ���Ͻ�Ų��.
						if (Cell->Get(x, i) == ' ') {
							for (int y = i; y>0; y--) {
								Cell->Set(x, y, Cell->Get(x, y - 1));
							}
							//���� ���� ���� ���� �������� �����Ѵ�.
							Cell->Set(x, 0, ' ');
						}
					}
				}
				//�������� ���·� ��ȯ�Ѵ�.
				State = 2;
			}
		}

		return true;
	}

	//�������� ������ �ִ� ������ ���� ���� ���� ó��
	int Count(int x, int y) {
		//���� ��ġ�� ���� ��´�.
		char c = Cell->Get(x, y);

		//���� �������� ����� ó��
		if (c == '+') {
			//�������� ī��Ʈ�� �Ϸ�� ������ ǥ���Ѵ�.
			Cell->Set(x, y, c | 0x80);
			//�����¿��� ���� ���ؼ��� ��������� �˻��Ѵ�.
			return Count(x - 1, y) + Count(x + 1, y) + Count(x, y - 1) + Count(x, y + 1);
		}
		else
			//���� �����̶��
			//������ ���� ����(1)�� ��ȯ�Ѵ�.
			if (c == ' ') {
				return 1;
			}
		//�� �̿��� ���(���� ����̳� ���� ���)����
		//������ ���� ����(0)�� ��ȯ�Ѵ�.
			else {
				return 0;
			}
	}

	//������ �������� ���ִ� ó��
	void Erase(int x, int y) {
		//���� ��ġ���� ���� ��´�.
		char c = Cell->Get(x, y);

		//���� �������� ����� ó��
		if (c == (char)('+' | 0x80)) {
			//�������� ����� ���������� ǥ���Ѵ�.
			Cell->Set(x, y, c | 0x40);

			//�����¿��� ���� ���ؼ���
			//�������� ���ִ� ó���� ��������� �����Ѵ�.
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
// ����� ��� ���ֱ�

#define CONNECTED_BLOCK_COUNT 5
#define CONNECTED_BLOCK_ERASE 2

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�Է� ����
		if (State == 0) {
			//������ �Է¿� ���� Ŀ���� �����¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && CX>1) CX--; else
					if (is->Right && CX<xs - 2) CX++; else
						if (is->Up && CY>2) CY--; else
							if (is->Down && CY<ys - 2) CY++;
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			//��ư�� ������ Ŀ�� ��ġ�� �ִ� ����� ���ش�.
			if (!PrevButton && is->Button[0]) {
				//Ŀ�� ��ġ�� �ִ� ���� ��´�.
				char c = Cell->Get(CX, CY);
				//���� ������ �ƴϰ�
				//���� ������ ���� ������ ����(2��)�̻� ������ ������
				//����� �����Ѵ�.
				if (
					c != ' ' &&
					Erase(CX, CY, c, 0x80) >= CONNECTED_BLOCK_ERASE
					) {
					//����� �����Ѵ�.
					Erase(CX, CY, c | 0x80, 0x40);
					//�������·� ��ȯ�Ѵ�.
					Time = 0;
					State = 1;
				}
			}
			PrevButton = is->Button[0];

			//���� ī��Ʈ �Ϸ� ǥ�ø� �����.
			for (int y = 2; y<ys - 1; y++) {
				for (int x = 1; x<xs - 1; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//���� ����
		if (State == 1) {
			//���� �ð��� ����ϸ� ����� ����� ������ �����Ѵ�.
			Time++;
			if (Time == 30) {
				//����� ����� ã�´�.
				for (int x = 1; x<xs - 1; x++) {
					for (int y = 2; y<ys - 1; y++) {
						//����� ����� �߰����� ���� ó��
						if (Cell->Get(x, y) & 0x40) {
							//����� ��Ϻ��� ���� �ִ� ���� 
							//1�ܾ� �Ʒ��� �̵��Ѵ�.
							for (int i = y; i>2; i--) {
								Cell->Set(x, i, Cell->Get(x, i - 1));
							}
							
							//���� ���� ���� ���� �������� �����Ѵ�.
							Cell->Set(x, 2, ' ');
						}
					}
				}
				//Ⱦ �̵����·� ��ȯ�Ѵ�.
				Time = 0;
				State = 2;
			}
		}

		//Ⱦ �̵�����
		if (State == 2) {
			//���� �ð��� ����ϸ�
			//������ ������ ������ ���� �������� �̵��Ѵ�.
			Time++;
			if (Time == 30) {
				//������ ���� ã�´�.
				for (int x = xs - 2; x >= 1; x--) {
					//���� ������ �Ʒ��� ������
					//��� ���� ���������� �˻��Ѵ�.
					int y;
					for (y = 2; y<ys - 1 && Cell->Get(x, y) == ' '; y++);
					//��� ���� ������ ����� ó��
					if (y == ys - 1) {
						//������ ������ �������� ����
						//1���� �������� �̵��Ѵ�.
						for (int i = x; i<xs - 2; i++) {
							for (int j = 2; j<ys - 1; j++) {
								Cell->Set(i, j, Cell->Get(i + 1, j));
							}
						}
						//������ ���� ���� �������� �����Ѵ�.
						for (int j = 2; j<ys - 1; j++) {
							Cell->Set(xs - 2, j, ' ');
						}
					}
				}
				//�Է»��·� ��ȯ�Ѵ�.
				State = 0;
			}
		}

		return true;
	}

	//����� ���ִ� ó��
	//������ ���� ������ ����� ������ ���� ó���� �Բ� �Ѵ�.
	int Erase(int x, int y, char c, int mask) {
		//���� ��ġ�� ���� ������ ������ ���̶��
		//���� ǥ���Ѵ�.
		if (Cell->Get(x, y) == c) {
			//������ ��Ʈ�� �����Ͽ�
			//���� ǥ�ø� �Ѵ�.
			Cell->Set(x, y, c | mask);
			//�����¿��� ���� ���ؼ��� ��������� �˻��Ͽ�
			//������ ���� ������ ���� ������ ��ȯ�Ѵ�.
			return
				1 +
				Erase(x - 1, y, c, mask) +
				Erase(x + 1, y, c, mask) +
				Erase(x, y - 1, c, mask) +
				Erase(x, y + 1, c, mask);
		}
		//���� ��ġ�� ���� ������ ������ ���� �ƴϱ� ������
		//������ ���� ������ ���� ������ 0�� ��ȯ�Ѵ�.
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
// ����� ������ �߻��ϱ�

#define DRAWN_AND_SHOT_BLOCK_COUNT 5
#define DRAWN_AND_SHOT_BLOCK_ERASE 3

// ��������
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

	//���������� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�Է»���
		if (State == 0) {
			//������ �Է¿� ���� ĳ���͸� ���Ʒ��� �����δ�.
			if (!PrevLever) {
				if (is->Up && CY>2) CY--; else
					if (is->Down && CY<ys - 2) CY++;
			}
			PrevLever = is->Up || is->Down;

			//��ư�� ������ ����� ������ų� �߻��Ѵ�.
			if (!PrevButton && is->Button[0]) {
				//����� ������ ���� ���� ������ �������.
				if (Block == ' ') {
					int x;
					for (x = 3; x<xs - 1 && Cell->Get(x, CY) == ' '; x++);
					//ĳ������ �������� ����
					//��� ���� ã�´�.
					if (x<xs - 1) {
						//���� ��� ����� ������ ��´�.
						Block = Cell->Get(x, CY);
						//���� �������� �����Ѵ�.
						Cell->Set(x, CY, ' ');
						//������ ����� ��ǥ�� �����Ѵ�.
						BX = x;
						//�����·� ��ȯ�Ѵ�.
						State = 1;
					}
				}
				else {
					//����� ������ ���� ������ �߻��Ѵ�.
					int x;
					//ĳ������ �������� ������ ������
					//����� �߻��Ѵ�.
					for (x = 3; x<xs - 1 && Cell->Get(x, CY) != ' '; x++);
					if (x<xs - 1) {
						//�߻� ���·� ��ȯ�Ѵ�.
						State = 2;
					}
				}
			}
			PrevButton = is->Button[0];
		}

		//������
		if (State == 1) {
			//����� ĳ������ ������ ���� �����ϸ�
			//���ϻ��·� ��ȯ�Ѵ�.
			if (BX == CX + 1) {
				State = 3;
			}
			//ĳ������ ������ ���� ������ ��������
			//����� �������� �̵��Ѵ�.
			else {
				BX--;
			}
		}

		//�߻����
		if (State == 2) {
			//����� ������ ���� ������ �ƴ϶��
			//����� ���� ��ġ�� �����Ѵ�.
			if (Cell->Get(BX + 1, CY) != ' ') {
				//���� ����� ��ִ´�.
				Cell->Set(BX, CY, Block);
				//������ �ִ� ����� �������� �����Ѵ�.
				Block = ' ';
				//���ϻ��·� ��ȯ�Ѵ�.
				State = 3;
			}
			//����� ������ ���� ������ �� ����ؼ�
			//����� ���������� �̵��Ѵ�.
			else {
				BX++;
			}
		}

		//���ϻ���
		if (State == 3) {
			//�������� ���� ������ ���� ã�´�.
			for (int x = 3; x<xs - 1; x++) {
				for (int y = 2; y<ys - 1; y++) {
					//������ ���� ã�� ����� ó��
					if (Cell->Get(x, y) == ' ') {
						//���� �ִ� ���� 1�ܰ� �Ʒ��� �̵��Ѵ�.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//���� ���� ���� ���� �������� �����Ѵ�.
						Cell->Set(x, 2, ' ');
					}
				}
			}
			//������ ���� ã�´�.
			for (int x = 3; x<xs - 1; x++) {
				//���� ������ �Ʒ��� ������ ���� ���������� �˻��Ѵ�.
				int y;
				for (y = 2; y<ys - 1 && Cell->Get(x, y) == ' '; y++);
				//���� ������ ���� ó��
				if (y == ys - 1) {
					//������ ������ ������ ���� ���������� �̵��Ѵ�.
					for (int i = x; i>3; i--) {
						for (int j = 2; j<ys - 1; j++) {
							Cell->Set(i, j, Cell->Get(i - 1, j));
						}
					}
					//���� ���� ���� �������� �����Ѵ�.
					for (int j = 2; j<ys - 1; j++) {
						Cell->Set(3, j, ' ');
					}
				}
			}
			//�������� ���·� ��ȯ�Ѵ�.
			State = 4;
		}

		//���� ���� ����
		if (State == 4) {
			//����� ������� �ʾ��� ������
			//�ʱ���·� ��ȯ�Ѵ�.
			State = 0;
			//������ ���� ������ ����� ã�´�.
			for (int y = 2; y<ys - 1; y++) {
				for (int x = 3; x<xs - 1; x++) {
					char c = Cell->Get(x, y);
					//���� ������ ����� ������ ���� �̻� ������ ������
					//����� ���ش�.
					if (
						!(c & 0x80) &&
						c != ' ' &&
						Erase(x, y, c, 0x80) >= DRAWN_AND_SHOT_BLOCK_ERASE
						) {
						//����� ����� ������� ǥ���ϳ�.
						Erase(x, y, c | 0x80, 0x40);
						//�������·� ��ȯ�Ѵ�.
						Time = 0;
						State = 5;
					}
				}
			}
			//ī��Ʈ �Ϸ� ǥ�ø� �����.
			for (int y = 2; y<ys - 1; y++) {
				for (int x = 3; x<xs - 1; x++) {
					Cell->Set(x, y, Cell->Get(x, y) & 0x7f);
				}
			}
		}

		//���� ����
		if (State == 5) {
			//���� �ð��� ����ϸ� ����� ����� ������ �����ϴ�.
			Time++;
			if (Time == 30) {
				//����� ����� ã�´�.
				for (int x = 3; x<xs - 1; x++) {
					for (int y = 2; y<ys - 1; y++) {
						//����� ����� ã����
						//���� �������� �����Ѵ�.
						if (Cell->Get(x, y) & 0x40) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//���ϻ��·� ��ȯ�Ѵ�.
				State = 3;
			}
		}

		return true;
	}

	//����� ���ִ� ó��
	//������ ���� ������ ����� ������ ���� ó���� ���� �Ѵ�.
	int Erase(int x, int y, char c, int mask) {
		//���� ��ġ�� ���� ������ ������ ���̶��
		//���� ǥ���Ѵ�.
		if (Cell->Get(x, y) == c) {
			//������ ��Ʈ�� �����Ͽ� ���� ǥ�ø� �Ѵ�.
			Cell->Set(x, y, c | mask);
			//�����¿��� ���� ���ؼ��� ��������� �˻��Ͽ�
			//������ ���� ������ ���� ������ ��ȯ�Ѵ�.
			return
				1 +
				Erase(x - 1, y, c, mask) +
				Erase(x + 1, y, c, mask) +
				Erase(x, y - 1, c, mask) +
				Erase(x, y + 1, c, mask);
		}
		//���� ��ġ�� ���� ������ ������ ���� �ƴϱ� ������
		//������ ���� ������ ���� ������ 0�� ��ȯ�Ѵ�.
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
// ����� �о ����߷� ������

#define PUSHED_AND_COLLECTED_BLOCK_COUNT 5
#define PUSHED_AND_COLLECTED_BLOCK_ERASE 3

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();
		//�Է»���
		if (State == 0) {
			//������ �Է¿� ���� ĳ���͸� ���Ʒ��� �����δ�.
			if (!PrevLever) {
				if (is->Up && CY>2) CY--; else
					if (is->Down && CY<ys - 4) CY++;
			}
			PrevLever = is->Up || is->Down;

			//��ư�� ������
			//ĳ���Ϳ� ���� ���̿� �ִ� ����� �о� ����߸���.
			if (!PrevButton && is->Button[0]) {
				//ĳ���Ϳ� ���� ���̿� �ִ� �� �߿���
				//���� ���� ���� ��´�.
				char c = Cell->Get(1, CY);
				//���� ������ �ƴ϶�� ����� �о� ����߸���.
				if (c != ' ') {
					//���� ����� ������ ����.
					int x;
					for (x = 1; Cell->Get(x, ys - 2) != ' '; x++);
					//���� ����� ������ ����
					//���ο� ����� ��ġ�Ѵ�.
					Cell->Set(x, ys - 2, c);
					//����� �о� ����߸� ���� ����
					//�������� 1���� �̵��Ѵ�.
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

			//������ ���� ã�´�.
			for (int x = 1; x<xs - 1; x++) {
				for (int y = 2; y<ys - 3; y++) {
					//���鸸 ���� ã���� ���� �ִ� ���� ����߸���.
					if (Cell->Get(x, y) == ' ') {
						//������ ���� ���� �ִ� ����
						//1�ܾ� �Ʒ��� �̵��Ѵ�.
						for (int i = y; i>2; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//���� ���� ���� ���� �������� �����Ѵ�.
						Cell->Set(x, 2, ' ');
					}
				}
			}
		}

		//��������
		if (State == 1) {
			//���� �ð��� ����ϸ� ���� ����� �˻��Ѵ�.
			Time++;
			if (Time == 30) {
				//���� ����� ��� ���� ���������� �˻��Ѵ�.
				char c = Cell->Get(1, ys - 2);
				int x, y;
				for (x = 2; Cell->Get(x, ys - 2) == c; x++);

				//��� ���� ������ �ƴ϶��
				//���� ����� ���������� ���� �������� �ǵ�����.
				if (x<1 + PUSHED_AND_COLLECTED_BLOCK_ERASE) {
					//���������� ���� 1�ܾ� ���� �̵��Ѵ�.
					for (y = 2; y<ys - 4; y++) {
						for (x = 1; x<xs - 1; x++) {
							Cell->Set(x, y, Cell->Get(x, y + 1));
						}
					}
					//���� �Ʒ� ������ ���� ����� �̵��Ѵ�.
					for (x = 1; x<xs - 1; x++) {
						Cell->Set(x, ys - 4, Cell->Get(x, ys - 2));
					}
				}
				//����� ������ ������ �������� �����Ѵ�.
				for (x = 1; x<xs - 1; x++) {
					Cell->Set(x, ys - 2, ' ');
				}
				//�Է»��·� ��ȯ�Ѵ�.
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
// �������� ����� �޾Ƽ� �ױ�

#define CAUGHT_AND_PILED_BLOCK_COUNT 5
#define CAUGHT_AND_PILED_BLOCK_ERASE 3

// ��������
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

	//���������� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize();

		//�Է»���
		if (State == 0) {
			//������ �Է¿� ���� ĳ���͸� �¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && CX>1) CX--; else
					if (is->Right && CX<xs - 2) CX++;
			}
			PrevLever = is->Left || is->Right;

			//����� ������ ���� �� ��ư�� ������
			//ĳ������ �ٷ� �Ʒ��� ����� �״´�.
			if (!PrevButton && is->Button[0] && Block != ' ') {
				//ĳ������ �ٷ� �Ʒ��� ���� �����̸�
				//����� ���� ��
				//���� ���·� ��ȯ�Ѵ�.
				if (Cell->Get(CX, ys - 4) == ' ') {
					Cell->Set(CX, ys - 4, Block);
					Block = ' ';
					State = 1;
				}
			}
			PrevButton = is->Button[0];
			//�����ð����� ����� ���Ͻ�Ų��.
			DropTime++;
			if (DropTime == 60) {
				DropTime = 0;
				//ĳ���Ͱ� ����� ������ ���� ���� ��
				//�ٷ� ���� ����� ������
				//����� �޴´�.
				if (Block == ' ' && Cell->Get(CX, CY - 1) != ' ') {
					Block = Cell->Get(CX, CY - 1);
				}
				//�������� ���� ���� 1�ܾ� �Ʒ��� �̵��Ѵ�.
				for (int y = CY - 1; y>2; y--) {
					for (int x = 1; x<xs - 1; x++) {
						Cell->Set(x, y, Cell->Get(x, y - 1));
					}
				}
				//���� ���� ���� ���� �������� �����Ѵ�.
				for (int x = 1; x<xs - 1; x++) {
					Cell->Set(x, 2, ' ');
				}
				//���� ���� ���� ���� ���ο� ����� 1�� ��ִ´�.
				//�¿��� ��ġ�� �����  ������ ���Ƿ� �����Ѵ�.
				Cell->Set(Rand.Int31() % (xs - 2) + 1, 2, '0' + Rand.Int31() % CAUGHT_AND_PILED_BLOCK_COUNT);
			}
		}

		//���ϻ���
		if (State == 1) {
			//�������� ������ ���������� ������ ���� ã�´�.
			for (int x = 1; x<xs - 1; x++) {
				for (int y = CY + 1; y<ys - 1; y++) {
					//������ ���� ã���� ���� �ִ� ���� ����߸���.
					if (Cell->Get(x, y) == ' ') {
						//���� �ִ� ���� 1�ܾ� �Ʒ��� �̵��Ѵ�.
						for (int i = y; i>CY + 1; i--) {
							Cell->Set(x, i, Cell->Get(x, i - 1));
						}
						//���� ���� ���� ���� �������� �����Ѵ�.
						Cell->Set(x, CY + 1, ' ');
					}
				}
			}

			//����� �������� �ʾ����� �Է»��·� ��ȯ�Ѵ�.
			State = 0;
			//���������� �Ʒ��� ���� ���Ͽ�
			//���� ������ ����� ����Ǿ� �ִ����� �˻��Ѵ�.
			for (int y = CY + 1; y<ys - 1; y++) {
				for (int x = 1; x<xs - 1; x++) {
					//������ ���� �˻����� �ʴ´�.
					char c = Cell->Get(x, y) & 0x7f;
					if (c == ' ') continue;
					//����� ������ �˻��ϴ� ����
					//(�Ʒ���, ������, ���� �Ʒ�, ������ �Ʒ�)
					static const int
						vx[] = { 0, 1, -1, 1 },
						vy[] = { 1, 0, 1, 1 };
					//�� ���⿡ ����
					//���� ������ ����� ����Ǿ� �ִ����� �˻��Ѵ�.
					for (int v = 0; v<4; v++) {
						//����Ǿ� �ִ� ���� ������ ����� ������ �˻��Ѵ�.
						int count = 0;
						for (
							int i = x, j = y;
							1 <= i && i<xs - 1 && CY + 1 <= j && j<ys - 1;
							i += vx[v], j += vy[v], count++
							) {
							if (c != (Cell->Get(i, j) & 0x7f)) break;
						}
						//������ ���� �̻��� ����� ����Ǿ� ������
						//����� ������� ǥ���Ѵ�.
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

		//��������
		if (State == 2) {
			//���� �ð��� ����ϸ�
			//����� ����� ������ �����Ѵ�.
			Time++;
			if (Time == 30) {
				//����� ����� ã�´�.
				for (int y = CY + 1; y<ys - 1; y++) {
					for (int x = 1; x<xs - 1; x++) {
						//ã�� ���� �������� �����Ѵ�.
						if (Cell->Get(x, y) & 0x80) {
							Cell->Set(x, y, ' ');
						}
					}
				}
				//���� ���·� ��ȯ�Ѵ�.
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
// �ٴڿ� ǥ���Ͽ� ��� ���ֱ�

// ��������
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
	//���������� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//���� ����
		int xs = Cell->GetXSize(), ys = Cell->GetYSize(), zs = Cell->GetZSize();

		//�Է� ����
		if (State == 0) {
			//������ �Է¿� ���� Ŀ���� ���� �¿�� �����δ�.
			if (!PrevLever) {
				if (is->Left && CX>0) CX--;
				if (is->Right && CX<xs - 1) CX++;
				if (is->Up && CZ<zs - 1) CZ++;
				if (is->Down && CZ>0) CZ--;
			}
			PrevLever = is->Left || is->Right || is->Up || is->Down;

			//��ư�� ������ ���� ó��
			if (!PrevButton) {
				//��ư 0�� ������ Ŀ���� �Ʒ��� �ִ� �ٴ��� ǥ���Ѵ�.
				if (is->Button[0]) {
					switch (Cell->Get(CX, 0, CZ)) {
						//�ٴڿ� ǥ�ð� �Ǿ� ���� ������ ǥ�ø� �Ѵ�.
					case '=': Cell->Set(CX, 0, CZ, '-'); break;
						//�ٴڿ� ǥ�ð� �Ǿ� ������ ǥ�ø� ���ش�.
					case '-': Cell->Set(CX, 0, CZ, '='); break;
					}
				}
				else
					//��ư 1�� ������
					//ǥ�õ� �ٴ��� ���� �ִ� ����� ���߷� ���ش�.
					if (is->Button[1]) {
						//ǥ�õ� ����� ã�´�.
						for (int z = 0; z<zs; z++) {
							for (int x = 0; x<xs; x++) {
								if (Cell->Get(x, 1, z) == '#' && Cell->Get(x, 0, z) == '-') {
									//����� ����� ������� �����Ѵ�.
									Cell->Set(x, 1, z, '+');
									//�������·� ��ȯ�Ѵ�.
									Time = 0;
									State = 1;
								}
							}
						}
					}
			}
			PrevButton = is->Button[0] || is->Button[1];
			//�����ð����� ����� �������� �̵��Ѵ�.
			DropTime++;
			if (DropTime == 60) {
				DropTime = 0;
				//�������� ���� ���� �������� �̵��Ѵ�.
				for (int z = 0; z<zs - 1; z++) {
					for (int x = 0; x<xs; x++) {
						Cell->Set(x, 1, z, Cell->Get(x, 1, z + 1));
					}
				}
				//���� ������ ���� �������� �����Ѵ�.
				for (int x = 0; x<xs; x++) {
					Cell->Set(x, 1, zs - 1, ' ');
				}
				//���� ������ ���� ����� 1�� ������Ų��.
				//�¿� ������ ��ġ�� ���Ƿ� �����Ѵ�.
				Cell->Set(Rand.Int31() % xs, 1, zs - 1, '#');
			}
		}

		//��������
		if (State == 1) {
			//���� �ð��� ����ϸ� ����� ����� ������ ���ش�.
			Time++;
			if (Time == 30) {
				//����� ����� ã�´�.
				for (int z = 0; z<zs; z++) {
					for (int x = 0; x<xs; x++) {
						//����� ����� �߰��ϸ� �������� �����Ѵ�.
						if (Cell->Get(x, 1, z) == '+') {
							Cell->Set(x, 1, z, ' ');
						}
					}
				}
				//�Է»��·� ��ȯ�Ѵ�.
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

