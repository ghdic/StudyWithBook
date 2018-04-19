//==============================================================
// ��� ����߸���

#define DROPPING_BLOCK_CELL_SIZE 4
#define DROPPING_BLOCK_PATTERN_COUNT 7
#define DROPPING_BLOCK_TURN_COUNT 4
#define DROPPING_BLOCK_FIELD_LEFT 1
#define DROPPING_BLOCK_FIELD_RIGHT 11
#define DROPPING_BLOCK_FIELD_TOP 1
#define DROPPING_BLOCK_FIELD_BOTTOM 11
#define DROPPING_BLOCK_NEXT_LEFT 12

// ����� ����
// ����� ���¸��� 4���� ������ �غ��Ѵ�.
char* DroppingBlockPattern[DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT] = {
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	" ## "
	" ## "
	"    "
	,
	(char*)
	"    "
	"####"
	"    "
	"    "
	,
	(char*)
	" #  "
	" #  "
	" #  "
	" #  "
	,
	(char*)
	"    "
	"####"
	"    "
	"    "
	,
	(char*)
	" #  "
	" #  "
	" #  "
	" #  "
	,
	(char*)
	"    "
	"##  "
	" ## "
	"    "
	,
	(char*)
	"  # "
	" ## "
	" #  "
	"    "
	,
	(char*)
	"    "
	"##  "
	" ## "
	"    "
	,
	(char*)
	"  # "
	" ## "
	" #  "
	"    "
	,
	(char*)
	"    "
	" ## "
	"##  "
	"    "
	,
	(char*)
	" #  "
	" ## "
	"  # "
	"    "
	,
	(char*)
	"    "
	" ## "
	"##  "
	"    "
	,
	(char*)
	" #  "
	" ## "
	"  # "
	"    "
	,
	(char*)
	"  # "
	"### "
	"    "
	"    "
	,
	(char*)
	" #  "
	" #  "
	" ## "
	"    "
	,
	(char*)
	"    "
	"### "
	"#   "
	"    "
	,
	(char*)
	"##  "
	" #  "
	" #  "
	"    "
	,
	(char*)
	"#   "
	"### "
	"    "
	"    "
	,
	(char*)
	" ## "
	" #  "
	" #  "
	"    "
	,
	(char*)
	"    "
	"### "
	"  # "
	"    "
	,
	(char*)
	" #  "
	" #  "
	"##  "
	"    "
	,
	(char*)
	" #  "
	"### "
	"    "
	"    "
	,
	(char*)
	" #  "
	" ## "
	" #  "
	"    "
	,
	(char*)
	"    "
	"### "
	" #  "
	"    "
	,
	(char*)
	" #  "
	"##  "
	" #  "
	"    "
};

// ���
class CDroppingBlockBlock : public CMover {
public:
	CCell * StageCell;
	CCell** BlockCell;
	CCell** CurrentBlock;
	CCell** NextBlock;
	int CX, CY, VX, VY;
	int State, Time, DropTime, Turn;
	bool PrevButton, PrevDown;

	//����� �ʱ�ȭ ó��
	void Init() {
		//������ ���� ���� ����� ������ ������� �����Ѵ�.
		CurrentBlock = NextBlock;
		//������ ����Ͽ� ���� ����� �����Ѵ�.
		NextBlock = BlockCell + Rand.Int31() % DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT;
		//���� ����� �� ��ǥ�� �����Ѵ�.
		CX = (DROPPING_BLOCK_FIELD_LEFT + DROPPING_BLOCK_FIELD_RIGHT - DROPPING_BLOCK_CELL_SIZE) / 2;
		CY = DROPPING_BLOCK_FIELD_TOP;
		//�׸���ǥ�� �����Ѵ�.
		X = CX;
		Y = CY;
		//����, ���� Ÿ�̸�, ȸ�������� �����Ѵ�.
		State = 0;
		DropTime = 0;
		Turn = 0;

		//������ ����� ��Ÿ�� ��ġ��
		//�ٸ� ����� �ִ� ��쿡��
		//������ �����Ѵ�.
		if (StageCell->Hit(CX, CY, CurrentBlock[Turn])) {
			State = 99999;
		}
		PrevDown = true;
	}
	
	CDroppingBlockBlock(CCell* stage_cell, CCell** block_cell)
		: StageCell(stage_cell), BlockCell(block_cell), PrevButton(true)
	{
		Texture = Game->Texture[TEX_OBJECT];
		NextBlock = BlockCell;
		Init();
		Init();
	}

	//����� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//�Է� ������ ó��
		if (State == 0) {
			//���� Ÿ�̸Ӹ� �����Ѵ�.
			DropTime++;
			//������ �Ʒ��� �Է��ϰų�
			//���� Ÿ�̸Ӱ� �������� �����ϸ� ����� ���� ��Ų��.
			if ((is->Down && !PrevDown) || DropTime == 60) {
				//�� ��ǥ�� ������ ���
				//����� ���������� ���̳� �ٸ� ��Ͽ�
				//�����ϴ��� �˻��Ѵ�.
				if (StageCell->Hit(CX, CY + 1, CurrentBlock[Turn])) {
					//�����ϴ� ��쿡��
					//��� ���� �������� ���� �ռ��Ѵ�.
					StageCell->Merge(CX, CY, CurrentBlock[Turn]);
					//�������·� ��ȯ�Ѵ�.
					State = 2;
				}
				else {
					//�������� �ʴ� ��쿡��
					//���� Ÿ�̸ӳ� �ӵ��� �����ϰ�
					//�� ��ǥ�� �����Ѵ�.
					DropTime = 0;
					CY++;
					VX = 0;
					VY = 1;
					//�̵� ���·� ��ȯ�Ѵ�.
					Time = 0;
					State = 1;
				}
			}
			else
				//������ �������� �Է��� ���
				//����� �̵� ������ ���������� �������� ������
				//����� �������� �̵���Ų��.
				//�����ʵ� ���� ����
				if (is->Left && !StageCell->Hit(CX - 1, CY, CurrentBlock[Turn])) {
					CX--;
					VX = -1;
					VY = 0;
					Time = 0;
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CurrentBlock[Turn])) {
						CX++;
						VX = 1;
						VY = 0;
						Time = 0;
						State = 1;
					}
					else
						//��ư�� �Էµǰ�, ������ ��ư ���°� ������ ���� �����̸�,
						//ȸ�� �Ŀ� ��ϰ� ���������� �������� ������ ����� ȸ����Ų��.
						if (is->Button[0] && !PrevButton && !StageCell->Hit(CX, CY, CurrentBlock[(Turn + 1) % DROPPING_BLOCK_TURN_COUNT])) {
							//����� ������ �������� ��ȭ��Ų��.
							//������ ������ �������� �ٽ� ù ��° �������� ���ư���.
							Turn = (Turn + 1) % DROPPING_BLOCK_TURN_COUNT;
						}
			//������ ��ư�� �������� �ƴ��� �����صд�.
			PrevButton = is->Button[0];
			//������ ��� �Ʒ��� �Էµǰ� �ִ� ��쿡
			//����� ����ؼ� �������� �ʵ��� �ϱ� ���� ó��
			if (!is->Down) PrevDown = false;
		}
		//�̵� ������ ó��
		if (State == 1) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			//���⿡���� Ÿ�̸Ӹ� ������Ű�� ������
			//���ҽ�Ű�� ó���� �ص� ����.
			Time++;
			//Ÿ�̸Ӹ� ����� �׸���ǥ�� ���ݾ� �����Ѵ�.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			//Ÿ�̸Ӱ� ���� ���� �����ϸ� �Է»��·� ��ȯ�Ѵ�.
			if (Time == 10) {
				State = 0;
			}
		}
		//���������� ó��
		if (State == 2) {
			//���������·� ��ȯ�Ѵ�.
			//����� ����� �ִ� ��쿡��
			//�������·� ��ȯ�Ѵ�.
			State = 4;
			//����� ��ƴ���� ä���� ���� ã�´�.
			for (int y = DROPPING_BLOCK_FIELD_TOP; y<DROPPING_BLOCK_FIELD_BOTTOM; y++) {
				int x;
				for (x = DROPPING_BLOCK_FIELD_LEFT; x<DROPPING_BLOCK_FIELD_RIGHT; x++) {
					//������ ���� �ִ� ��쿡�� ������ Ż���Ѵ�.
					if (StageCell->Get(x, y) == ' ') break;
				}
				//����� ä���� ���� �ִ� ����� ó��
				if (x >= DROPPING_BLOCK_FIELD_RIGHT) {
					for (x = DROPPING_BLOCK_FIELD_LEFT; x<DROPPING_BLOCK_FIELD_RIGHT; x++) {
						//ä���� ���� ����� ����� ���(+)���� �����Ѵ�.
						StageCell->Set(x, y, '+');
					}
					//�������·� ��ȯ�Ѵ�.
					Time = 0;
					State = 3;
				}
			}
		}
		//���� ������ ó��
		if (State == 3) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			//Ÿ�̸Ӱ� ���� ���� �����ϸ� ����� ���ش�.
			if (Time == 20) {
				//����� ���(+)�� �ִ� ���� ã�´�.
				for (int i = DROPPING_BLOCK_FIELD_TOP; i<DROPPING_BLOCK_FIELD_BOTTOM; i++) {
					//����� ����� ã�� ����� ó��
					if (StageCell->Get(DROPPING_BLOCK_FIELD_LEFT, i) == '+') {
						//����� ���� ���� �ִ� ��� ����
						//�� �ܾ� �Ʒ��� �̵��Ѵ�.
						for (int y = i; y >= DROPPING_BLOCK_FIELD_TOP; y--) {
							for (int x = DROPPING_BLOCK_FIELD_LEFT; x<DROPPING_BLOCK_FIELD_RIGHT; x++) {
								StageCell->Set(x, y, StageCell->Get(x, y - 1));
							}
						}
					}
				}
				//���������·� ��ȯ�Ѵ�.
				State = 4;
			}
		}
		//������ ������ ó��
		if (State == 4) {
			//���ο� ����� ������Ų��.
			Init();
		}
		return true;
	}
	//����� �׸� ó��
	virtual void Draw() {
		//ȭ�� �ػ󵵸� ��´�.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		//���� ������ġ�� ����Ѵ�.
		//����� �ٴ��̳� �ٸ� ��ϰ� ������ ������
		//Y��ǥ�� ������Ų��.
		int ey;
		for (ey = CY; ey<DROPPING_BLOCK_FIELD_BOTTOM; ey++) {
			//�����ϴ� ��ġ�� ���� ���� ��ġ�� �ȴ�.
			if (StageCell->Hit(CX, ey + 1, CurrentBlock[Turn])) break;
		}
		//��� �׸���
		for (int y = 0; y<DROPPING_BLOCK_CELL_SIZE; y++) {
			for (int x = 0; x<DROPPING_BLOCK_CELL_SIZE; x++) {
				if (State<2 && CurrentBlock[Turn]->Get(x, y) == '#') {
					//���� ������ġ�� ȸ������ ����� �׸���.
					Texture->Draw(
						(CX + x)*sw, (ey + y)*sh, sw, sh, 0, 0, 1, 1, COL_LGRAY
					);
					//���� ��ġ�� ���������� ����� �׸���.
					Texture->Draw(
						(X + x)*sw, (Y + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				//������ ��Ÿ�� ����� �׸���.
				if (NextBlock[0]->Get(x, y) == '#') {
					Texture->Draw(
						(DROPPING_BLOCK_NEXT_LEFT + x)*sw, (1 + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
			}
		}
		//Ÿ�̸� ���� ���� ���� �����Ѵ�.
		float f = Time * 0.05f;
		//���������� ��� ���� ���ؼ� ó���Ѵ�.
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				switch (StageCell->Get(x, y)) {
				case '#':
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
					break;
				case '+':
					Game->Texture[TEX_OBJECT]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1)
					);
					break;
				case '=':
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
					break;
				}
			}
		}
	}
};

// ��������
class CDroppingBlockStage : public CStage {
	CCell* StageCell;
	CCell* BlockCell[DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT];
public:
	CDroppingBlockStage() : CStage(L"DROPPING BLOCK") {
		StageCell = new CCell();
		for (int i = 0; i<DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT; i++) {
			BlockCell[i] = new CCell(DROPPING_BLOCK_CELL_SIZE, DROPPING_BLOCK_CELL_SIZE);
			BlockCell[i]->Init(DroppingBlockPattern[i]);
		}
	}
	virtual ~CDroppingBlockStage() {
		delete StageCell;
		for (int i = 0; i<DROPPING_BLOCK_PATTERN_COUNT*DROPPING_BLOCK_TURN_COUNT; i++) {
			delete BlockCell[i];
		}
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"=          =    "
			"================"
		);
		new CDroppingBlockBlock(StageCell, BlockCell);
	}
};


//==============================================================
// ���� ����߸���

#define DROPPING_JEWEL_CELL_SIZE 3
#define DROPPING_JEWEL_FIELD_LEFT 1
#define DROPPING_JEWEL_FIELD_RIGHT 11
#define DROPPING_JEWEL_FIELD_TOP 1
#define DROPPING_JEWEL_FIELD_BOTTOM 11
#define DROPPING_JEWEL_NEXT_LEFT 13
#define DROPPING_JEWEL_COLOR_COUNT 5
#define DROPPING_JEWEL_ERASING_COUNT 3

// ����
class CDroppingJewelJewel : public CMover {
public:
	CCell * StageCell;
	CCell* CurrentJewel;
	CCell* NextJewel;
	int CX, CY, VX, VY;
	int State, Time, DropTime;
	bool PrevButton, PrevDown;

	void Init() {
		for (int y = 0; y<DROPPING_JEWEL_CELL_SIZE; y++) {
			CurrentJewel->Set(0, y, NextJewel->Get(0, y));
			NextJewel->Set(0, y, '0' + Rand.Int31() % DROPPING_JEWEL_COLOR_COUNT);
		}
		CX = (DROPPING_JEWEL_FIELD_LEFT + DROPPING_JEWEL_FIELD_RIGHT - DROPPING_JEWEL_CELL_SIZE) / 2;
		CY = DROPPING_JEWEL_FIELD_TOP;
		X = CX;
		Y = CY;
		State = 0;
		DropTime = 0;
		if (StageCell->Hit(CX, CY, CurrentJewel)) {
			State = 99999;
		}
		PrevDown = true;
	}

	CDroppingJewelJewel(CCell* stage_cell, CCell* current_jewel, CCell* next_jewel)
		: StageCell(stage_cell), CurrentJewel(current_jewel), NextJewel(next_jewel), PrevButton(true)
	{
		Texture = Game->Texture[TEX_BALL0];
		Init();
		Init();
	}
	//������ �̵� ó��
	virtual bool Move(const CInputState* is) {
		//�Է»���
		if (State == 0) {
			//���� Ÿ�̸Ӹ� �����Ѵ�.
			DropTime++;

			//������ �Ʒ��� �Է��ϰų�
			//���� Ÿ�̸Ӱ� ���� ���� �����ϸ�
			//������ ���Ͻ�Ų��.
			if ((is->Down && !PrevDown) || DropTime == 60) {
				if (StageCell->Hit(CX, CY + 1, CurrentJewel)) {
					StageCell->Merge(CX, CY, CurrentJewel);
					State = 2;
				}
				else {
					DropTime = 0;
					CY++;
					VX = 0;
					VY = 1;
					Time = 0;
					State = 1;
				}
			}
			else
				if (is->Left && !StageCell->Hit(CX - 1, CY, CurrentJewel)) {
					CX--;
					VX = -1;
					VY = 0;
					Time = 0;
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CurrentJewel)) {
						CX++;
						VX = 1;
						VY = 0;
						Time = 0;
						State = 1;
					}
			//��ư�� �Է��� ���
			//������ ��ư ���°� ������ ���� �����̸� ������ ������ �ٲ۴�.
					else
						if (is->Button[0] && !PrevButton) {
							//���� ���ʰ� ��� ������ �ٲ� �ִ´�.
							CurrentJewel->Swap(0, 0, 0, 1);
							//���� ���ʰ� ���� �Ʒ��� ������ �ٲ� �ִ´�.
							CurrentJewel->Swap(0, 0, 0, 2);
						}
			//������ ��ư �Է»��¸� �����Ѵ�.
			PrevButton = is->Button[0];
			//������ ��� �Ʒ��� �Էµǰ� ���� ��, ������ ����ؼ� �������� �ʵ��� ó���Ѵ�.
			if (!is->Down) PrevDown = false;
		}
		//�̵� ����
		if (State == 1) {
			Time++;
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			if (Time == 10) {
				State = 0;
			}
		}
		//��������
		if (State == 2) {
			//������ ������ ���� ��쿡��
			//������ ���·� ��ȯ�Ѵ�.
			State = 4;
			//�������� ���� ��� ���� ���Ͽ� �˻��Ѵ�.
			for (int y = DROPPING_JEWEL_FIELD_TOP; y<DROPPING_JEWEL_FIELD_BOTTOM; y++) {
				for (int x = DROPPING_JEWEL_FIELD_LEFT; x<DROPPING_JEWEL_FIELD_RIGHT; x++) {
					//���� ����Ѵ�
					//���� ������ ���ϱ� ����
					//����� ������ ǥ��(�ֻ��� ��Ʈ)�� Ŭ�����Ѵ�.
					char c = StageCell->Get(x, y) & 0x7f;
					//���� ������ ��쿡�� ���� ���� �Ѿ��.
					if (c == ' ') continue;
					//������ ������ ����
					static const int
						vx[] = { 0, 1, -1, 1 },
						vy[] = { 1, 0, 1, 1 };
					//����, ����, �밢��(4����)�� ����
					//������ ������ �̾��� �ִ�¢ �˻��Ѵ�.
					for (int v = 0; v<4; v++) {
						//�̾��� �ִ� ������ ����
						int count = 0;
						//�� ��������
						//���� ������ ���� ���� � �̾��� �ִ��� �˻��Ѵ�.
						for (
							int i = x, j = y;
							DROPPING_JEWEL_FIELD_LEFT <= i &&
							i<DROPPING_JEWEL_FIELD_RIGHT &&
							DROPPING_JEWEL_FIELD_TOP <= j &&
							j<DROPPING_JEWEL_FIELD_BOTTOM;
							i += vx[v], j += vy[v], count++
							) {
							//���� ������ ������ �̾��� �ִ� �� ����ؼ� ������ �����Ѵ�.
							if (c != (StageCell->Get(i, j) & 0x7f)) break;
						}
						//�̾��� �ִ� ������ ������ ������ ����(3��) �̻��̶��
						//����� ������ ǥ��(�ֻ��� ��Ʈ)�� ��Ʈ�Ѵ�.
						if (count >= DROPPING_JEWEL_ERASING_COUNT) {
							for (int i = x, j = y, k = 0; k<count; i += vx[v], j += vy[v], k++) {
								StageCell->Set(i, j, StageCell->Get(i, j) | 0x80);
							}
							Time = 0;
							//�������·� ��ȯ�Ѵ�.
							State = 3;
						}
					}
				}
			}
		}
		//��������
		if (State == 3) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;

			//Ÿ�̸Ӱ� ���� ���� �����ϸ�
			//ǥ�õ� ������ �����Ѵ�.
			if (Time == 20) {
				//�������� ���� ��� ���� ���Ͽ� ó���Ѵ�.
				for (int x = DROPPING_JEWEL_FIELD_LEFT; x<DROPPING_JEWEL_FIELD_RIGHT; x++) {
					for (int y = DROPPING_JEWEL_FIELD_TOP; y<DROPPING_JEWEL_FIELD_BOTTOM; y++) {
						//����� ������ ã���� �����ϰ�
						//�� ������ ���� �ִ� ��� ������
						//1�ܾ� �Ʒ��� �̵��Ѵ�.
						if (StageCell->Get(x, y) & 0x80) {
							for (int i = y; i >= DROPPING_JEWEL_FIELD_TOP; i--) {
								StageCell->Set(x, i, StageCell->Get(x, i - 1));
							}
						}
					}
				}
				//������ ������鼭
				//���ο� ������ �ٽ� �̾��� ���ɼ��� �ֱ� ������
				//�������·� ��ȯ�Ѵ�.
				State = 2;
			}
		}
		//������ ����
		if (State == 4) {
			Init();
		}
		return true;
	}

	//������ �׸� ó��;
	virtual void Draw() {
		//ȭ���� �ػ󵵸� ��´�.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<DROPPING_JEWEL_CELL_SIZE; y++) {
			if (State<2) {
				Game->Texture[TEX_BALL0 + (CurrentJewel->Get(0, y) - '0')]->Draw(
					X*sw, (Y + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
				);
			}
			Game->Texture[TEX_BALL0 + (NextJewel->Get(0, y) - '0')]->Draw(
				DROPPING_JEWEL_NEXT_LEFT*sw, (DROPPING_JEWEL_FIELD_TOP + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
			);
		}
		//���������� ��� ���� ���Ͽ� ó���Ѵ�.
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				//���� ������ ��´�.
				char c = StageCell->Get(x, y);
				//����� ����
				if (c & 0x80) {
					//Ÿ�̸ӿ� ���� ���� �����Ѵ�.
					float f = Time * 0.05f;
					//�ð� ����� ���� ���ݾ� ������� �׸���.
					Game->Texture[TEX_BALL0 + ((c & 0x7f) - '0')]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1)
					);
				}
				else
					//������ ����
					if ('0' <= c && c<'0' + DROPPING_JEWEL_COLOR_COUNT) {
						Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
							x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
						);
					}
					else
						//��
						if (c == '=') {
							Game->Texture[TEX_DROP_FLOOR]->Draw(
								x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
							);
						}
			}
		}
	}
};

// ��������
class CDroppingJewelStage : public CStage {
	CCell* StageCell;
	CCell* JewelCell[2];
public:
	CDroppingJewelStage() : CStage(L"DROPPING JEWEL") {
		StageCell = new CCell();
		for (int i = 0; i<2; i++) {
			JewelCell[i] = new CCell(1, DROPPING_JEWEL_CELL_SIZE);
		}
	}
	virtual ~CDroppingJewelStage() {
		delete StageCell;
		for (int i = 0; i<2; i++) {
			delete JewelCell[i];
		}
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"=          =   ="
			"================"
		);
		new CDroppingJewelJewel(StageCell, JewelCell[0], JewelCell[1]);
	}
};

//==============================================================
// �� ����߸���
//�÷��̾�2 J K L ;
//�÷��̾�1 ȭ��ǥ �����̽���

#define DROPPING_BALL_CELL_SIZE 3
#define DROPPING_BALL_COLOR_COUNT 5
#define DROPPING_BALL_FIELD_TOP 1
#define DROPPING_BALL_FIELD_BOTTOM 11
#define DROPPING_BALL_TURN_COUNT 4

// ��
class CDroppingBallBall : public CMover {
public:
	CCell * StageCell;
	CCell* CurrentBall;
	CCell* NextBall;
	int PlayerID, ErasingCount;
	int FieldLeft, FieldRight, NextLeft;
	int CX, CY, VX, VY;
	int State, Time, DropTime, Turn, ChainCount, AttackCount;
	bool PrevButton, PrevDown;
	CDroppingBallBall* Opponent;

	void Init() {
		CurrentBall->Set(0, 0, NextBall);
		NextBall->Clear();
		MakeNextBall();
		CX = (FieldLeft + FieldRight - DROPPING_BALL_CELL_SIZE) / 2;
		CY = DROPPING_BALL_FIELD_TOP;
		X = CX;
		Y = CY;
		State = 0;
		DropTime = 0;
		Turn = 0;
		if (StageCell->Hit(CX, CY, CurrentBall)) {
			State = 99999;
		}
		PrevDown = true;
	}

	virtual void MakeNextBall() {
		NextBall->Set(1, 0, '0' + Rand.Int31() % DROPPING_BALL_COLOR_COUNT);
		NextBall->Set(1, 1, '0' + Rand.Int31() % DROPPING_BALL_COLOR_COUNT);
	}

	CDroppingBallBall(
		CCell* stage_cell, CCell* current_ball, CCell* next_ball,
		int player_id, int Erasing_count,
		int field_left, int field_right, int next_left)
		: StageCell(stage_cell), CurrentBall(current_ball), NextBall(next_ball),
		PlayerID(player_id), ErasingCount(Erasing_count),
		FieldLeft(field_left), FieldRight(field_right), NextLeft(next_left),
		AttackCount(0), PrevButton(true)
	{
		Texture = Game->Texture[TEX_BALL0];
		NextBall->Clear();
		Init();
		Init();
	}
	//���� �̵� ó��
	virtual bool Move(const CInputState*) {
		//2�� �÷��̾ �����ϱ� ���Ͽ�
		//���� �����ϰ� �ִ� �÷��̾��� �Է��� ��´�.
		const CInputState* is = Game->GetInput()->GetState(PlayerID);
		//�Է»���
		if (State == 0) {
			//���� Ÿ�̸Ӹ� �����Ѵ�.
			DropTime++;
			//������ �Ʒ��� �Է��ϰų�
			//���� Ÿ�̸Ӱ� ���� ���� �����ϸ�
			//���� ���� ��Ų��.
			if ((is->Down && !PrevDown) || DropTime == 120) {
				//���������� �����ϸ� ���� ������Ų��.
				if (StageCell->Hit(CX, CY + 1, CurrentBall)) {
					//�������� ���� ���� ���� �ռ��Ѵ�.
					StageCell->Merge(CX, CY, CurrentBall);
					//���� ī��Ʈ(�����ؼ� ���� Ƚ��)�� �ʱ�ȭ�Ѵ�.
					ChainCount = 0;
					//�������� ���·� ��ȯ�Ѵ�.
					State = 2;
				}
				else {
					//���������� �������� ���� ��쿡��
					//���� 1�ܾƷ��� ���Ͻ�Ų��.
					DropTime = 0;
					//�� ��ǥ�� �����Ѵ�.
					CY++;
					//�ӵ�, Ÿ�̸Ӹ� �����Ѵ�.
					VX = 0;
					VY = 1;
					Time = 0;
					//�̵����·� ��ȯ�Ѵ�.
					State = 1;
				}
			}
			else
				//������ �������� �Է��� ���
				//���������� �������� ������
				//�������� �̵��Ѵ�.
				if (is->Left && !StageCell->Hit(CX - 1, CY, CurrentBall)) {
					//�� ��ǥ�� �����Ѵ�.
					CX--;
					//�ӵ�, Ÿ�̸Ӹ� �����Ѵ�.
					VX = -1;
					VY = 0;
					Time = 0;
					//�̵����·� ��ȯ�Ѵ�.
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CurrentBall)) {
						CX++;
						VX = 1;
						VY = 0;
						Time = 0;
						State = 1;
					}
					else
						//��ư�� �Էµǰ�
						//������ ��ư�� ������ ���� ���¶�� ���� ȸ����Ų��.
						if (is->Button[0] && !PrevButton) {
							//�� �ȿ��� ���� ���� �Ű��� �ִ� ��쿡��
							//ȸ���ϱ� ���� ���� �Ʒ��� �ǵ�����.
							if (Turn == 2 && CurrentBall->Get(1, 2) == ' ') {
								//�Ʒ��ʰ� ����� ���� �ٲپ� �ִ´�.
								CurrentBall->Swap(1, 2, 1, 1);
								//����� ���� ���� �ٲپ� �ִ´�.
								CurrentBall->Swap(1, 1, 1, 0);
							}
							//ȸ������(0-3)�� �����Ѵ�.
							Turn = (Turn + 1) % DROPPING_BALL_TURN_COUNT;
							//���� ȸ�� ��Ų��.
							//���� ����, ���ʰ� �Ʒ�, �Ʒ��� �������� ������ ���� �ٲپ� �ִ´�.
							CurrentBall->Swap(1, 0, 0, 1);
							CurrentBall->Swap(0, 1, 1, 2);
							CurrentBall->Swap(1, 2, 2, 1);
							//ȸ�� ���� ���� ���������� ������ ����� ó��
							if (StageCell->Hit(CX, CY, CurrentBall)) {
								//�����ʿ� ��ֹ��� �ְ� ������ ����ִ� ��쿡�� �������� ���Ѵ�.
								if (Turn == 1 && !StageCell->Hit(CX - 1, CY, CurrentBall)) {
									CX--;
									X = CX;
								}
								//���ʿ� ��ֹ��� �ְ� �������� ����ִ� ��쿡�� ���������� ���Ѵ�.
								else
									if (Turn == 3 && !StageCell->Hit(CX + 1, CY, CurrentBall)) {
										CX++;
										X = CX;
									}
									else
										//�Ʒ��� ��ֹ��� �ְ�, ������ ����ִ� ��쿡�� �� �ȿ��� ���� ���� �ű��.
										if (Turn == 2 && !StageCell->Hit(CX, CY - 1, CurrentBall)) {
											//����� ������ ���� �ٲپ� �ִ´�.
											CurrentBall->Swap(1, 1, 1, 0);
											//�Ʒ��ʰ� ����� ���� �ٲپ� �ִ´�.
											CurrentBall->Swap(1, 2, 1, 1);
										}
										else {
											//��ֹ��� ���� �� ���� ��쿡�� ȸ���� ����Ѵ�.
											//ȸ�������� ������� �ǵ�����.
											Turn = (Turn - 1 + DROPPING_BALL_TURN_COUNT) % DROPPING_BALL_TURN_COUNT;
											//���� ȸ���� ����Ѵ�
											//ó���� ȸ����ų ���� �ݴ��
											//�Ʒ��� ������, ���ʰ� �Ʒ���, ���ʰ� ���� ������ ���� �ٲ� �ִ´�.
											CurrentBall->Swap(1, 2, 2, 1);
											CurrentBall->Swap(0, 1, 1, 2);
											CurrentBall->Swap(1, 0, 0, 1);
										}
							}
						}
			//������ ��ư �Է»��¸� �����Ѵ�.
			PrevButton = is->Button[0];
			//������ ��� �Ʒ��� �Էµǰ� �ִ� ��쿡 ���� ��� �������� �ʵ��� ó���Ѵ�.
			if (!is->Down) PrevDown = false;
		}
		//�̵�����
		if (State == 1) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			//�׸���ǥ�� �����Ѵ�.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			//Ÿ�̸Ӱ� ���� ���� �����ϸ�
			//�Է»��·� ��ȯ�Ѵ�.
			if (Time == 10) {
				State = 0;
			}
		}
		//������������
		if (State == 2) {
			//������ ���� �ϳ��� ���� ��쿡�� �����������·� ��ȯ�Ѵ�.
			State = 4;
			//������ ������ �ް� �ִ� ��쿡�� ���� ���� �߻���Ų��.
			Attack();
			//�������� ��ü�� ���� �Ʒ����� ���� �˻��Ѵ�.
			for (int x = FieldLeft; x<FieldRight; x++) {
				for (int y = DROPPING_BALL_FIELD_BOTTOM - 1; y >= DROPPING_BALL_FIELD_TOP; y--) {
					//�Ʒ��� �ƹ��͵� ���� ��ġ�� ���� ã�´�.
					if (StageCell->Get(x, y) == ' ' && StageCell->Get(x, y - 1) != ' ') {
						//�Ʒ��� �ƹ��͵� ���� ��ġ�� ���� ������
						//�� ����, �� ���� ���� �ִ� ��� ����
						//������ ���� ǥ���Ѵ�.
						//(���� �ֻ��� ��Ʈ�� 1�� �����Ѵ�.)
						for (; y >= DROPPING_BALL_FIELD_TOP; y--) {
							//���� �������� �˻��Ͽ�
							//���� ��쿡�� ������ ���� ǥ���ϰ�
							//�� �ܾ� �Ʒ��� �̵���Ų��.
							char c = StageCell->Get(x, y - 1);
							if (c != ' ') {
								StageCell->Set(x, y, c | 0x80);
							}
							else {
								StageCell->Set(x, y, ' ');
							}
						}
						//Ÿ�̸Ӹ� �����ϰ� ���ϻ��·� ��ȯ�Ѵ�.
						Time = 0;
						State = 3;
					}
				}
			}
		}
		//���ϻ���
		if (State == 3) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;

			//Ÿ�̸Ӱ� ���� ���� �����ϸ� ������ ���� ǥ�ø� �����Ѵ�.
			if (Time == 10) {
				//������������ ��� ���� ���Ͽ� ó���Ѵ�.
				for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
					for (int x = FieldLeft; x<FieldRight; x++) {
						//������ ���� ǥ�ø� �����Ѵ�.
						//(���� �ֻ��� ��Ʈ�� Ŭ�����Ѵ�.)
						StageCell->Set(x, y, StageCell->Get(x, y) & 0x7f);
					}
				}
				//�������� ���·� ��ȯ�Ѵ�.
				//���� �� �ܾ� �����ϱ� ������
				//���� ���������� �������� ���� ������ ��������
				//�����������¿� ���ϻ��¸� ���� �� �ݺ��� �ʿ䰡 �ִ�.
				State = 2;
			}
		}
		//������������
		if (State == 4) {
			//����ī��Ʈ(�����ۿ��� �Ͼ Ƚ��)�� ������Ų��.
			ChainCount++;
			//���� ���� ������ ���� ��쿡��
			//���������·� ��ȯ�Ѵ�.
			State = 6;
			
			//�������� ���� ��� ���� ����
			//����� ���� �ִ����� �˻��Ѵ�.
			for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
				for (int x = FieldLeft; x<FieldRight; x++) {
					//���� ������ ��´�.
					char c = StageCell->Get(x, y);

					//���� ī��Ʈ ���� �ʾҰ�
					//������ ��(0-4)�̶��
					//ī��Ʈ�� �����Ѵ�.
					if (!(c & 0x80) && '0' <= c && c<'0' + DROPPING_BALL_COLOR_COUNT) {
						//���� ������ ���� �� �� ����Ǿ� �ִ����� ����.
						int count = Count(x, y, c);
						//�̾��� ������ ������ ����(4��)�̻��̸� ���� ���ش�.
						if (count >= ErasingCount) {
							//���濡�� ���� ���� ���� ���� ������Ų��.
							Opponent->AttackCount += (count - ErasingCount + 1)*ChainCount;
							//����� ���� ǥ���Ѵ�.
							BeginErasing(x, y, c | 0x80);
							//Ÿ�̸Ӹ� �����ϰ�
							//�������·� ��ȯ�Ѵ�.
							Time = 0;
							State = 5;
						}
					}
				}
			}
			//�������� ���� ��� ���� ����
			//ī��Ʈ �Ϸ� ǥ�ø� �����Ѵ�.
			for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
				for (int x = FieldLeft; x<FieldRight; x++) {
					//ī��Ʈ �Ϸ� ǥ�ø� �����Ѵ�.
					//(�ֻ��� ��Ʈ�� Ŭ�����Ѵ�.)
					StageCell->Set(x, y, StageCell->Get(x, y) & 0x7f);
				}
			}
		}
		//��������
		if (State == 5) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			//Ÿ�̸Ӱ� ���� ���� �����ϸ�
			//ǥ�õ� ���� ���ش�.
			if (Time == 10) {
				//�������� ���� ��� ���� ���� ó���Ѵ�.
				for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
					for (int x = FieldLeft; x<FieldRight; x++) {
						EndErasing(x, y);
					}
				}
				//���� �������
				//�Ʒ��� �ƹ��͵� ���� ���� ������ �� �ֱ� ������
				//�����������·� ��ȯ�Ѵ�.
				State = 2;
			}
		}
		//����������
		if (State == 6) {
			//���ο� ���� �������� ���ʿ� ������Ų��.
			Init();
		}
		return true;
	}

	//������ ���� ������ ���� ó��
	//��������� ȣ���Ѵ�.
	int Count(int x, int y, char ball) {
		//���� ������ ��´�.
		char c = StageCell->Get(x, y);
		//���� ���� ������ ���� ����� ó��
		if (c == ball) {
			//���� ī��Ʈ �Ϸ� ǥ�ø� �Ѵ�.
			StageCell->Set(x, y, c | 0x80);
			//�����¿�� ������ ���� ���ؼ���
			//���� ������ ���� ����Ǿ� �ִ��� ��������� �˻��Ͽ�
			//������ ���� ������ ���� ��ȯ�Ѵ�.
			return 1 + Count(x - 1, y, c) + Count(x + 1, y, c) + Count(x, y - 1, c) + Count(x, y + 1, c);
		}
		//���� ���� ������ ���� �ƴ� ��쿡��
		//������ ���� ������ 0���� ��ȯ�Ѵ�.
		return 0;
	}

	//����� ���� ����� ���� ǥ���ϴ� ó��
	//��������� ȣ��ȴ�.
	virtual void BeginErasing(int x, int y, char ball) {
		//���� ������ ��´�.
		char c = StageCell->Get(x, y);
		//���� ���� ������ ���� ����� ó��
		if (c == ball) {
			//���� ����� ���� ǥ���Ѵ�.
			StageCell->Set(x, y, c | 0x40);
			//�����¿�� ������ ���� ���ؼ���
			//���� ������ ���� ���ִ� ó���� ��������� �����Ѵ�.
			BeginErasing(x - 1, y, c);
			BeginErasing(x + 1, y, c);
			BeginErasing(x, y - 1, c);
			BeginErasing(x, y + 1, c);
		}
		else
			//���� ���� ���� ����� ó��
			if (c == '0' + DROPPING_BALL_COLOR_COUNT) {
				//���� ����� ���� ǥ���Ѵ�.
				StageCell->Set(x, y, c | 0x40);
				//������ ���� ���� ������ ������Ų��.
				Opponent->AttackCount += ChainCount;
			}
	}

	//����� ���� ���ִ� ó��
	virtual void EndErasing(int x, int y) {
		//���� ����� ���� ����� ó��
		if (StageCell->Get(x, y) & 0x40) {
			//���� �������� �����Ѵ�.
			StageCell->Set(x, y, ' ');
		}
	}

	//���� ���� ������Ű�� ó��
	virtual void Attack() {
		//�ʵ��� ��
		int n = FieldRight - FieldLeft;
		//���� ���� ������ġ�� ���Ƿ� �����Ѵ�.
		int r = Rand.Int31() % n;
		//���� �� �ܿ� ���� ���� ������Ų��.
		for (int i = 0; i<n; i++) {
			//���� ���� ������ų X��ǥ
			int x = (i + r) % n + FieldLeft;
			//���� ���� ������ 0���� ū ���� ���� ������Ų��.
			if (AttackCount>0) {
				//���� ���� �������� ���� �ִ´�.
				StageCell->Set(x, 0, '0' + DROPPING_BALL_COLOR_COUNT);
				//���� ���� ������ ���� ��Ų��.
				AttackCount--;
			}
			else {
				//���� ���� ���� 0�̸� ���� ������Ű�� �ʴ´�.
				//���� �������� �����Ѵ�.
				StageCell->Set(x, 0, ' ');
			}
		}
	}
	//���� �׸� ó��
	virtual void Draw() {
		//ȭ���� �ػ󵵸� ��´�.
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<DROPPING_BALL_CELL_SIZE; y++) {
			for (int x = 0; x<DROPPING_BALL_CELL_SIZE; x++) {
				char c;
				c = CurrentBall->Get(x, y);
				if (State<2 && c != ' ') {
					Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
						(X + x)*sw, (Y + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				c = NextBall->Get(x, y);
				if (c != ' ') {
					Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
						(NextLeft + x - DROPPING_BALL_CELL_SIZE / 2)*sw, (DROPPING_BALL_FIELD_TOP + y)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
			}
		}
		//���������� ��� ���� ���Ͽ� ó���Ѵ�.
		for (int y = DROPPING_BALL_FIELD_TOP; y<DROPPING_BALL_FIELD_BOTTOM; y++) {
			for (int x = FieldLeft; x<FieldRight; x++) {
				//���� ������ ��´�.
				char c = StageCell->Get(x, y);
				//�������� �� �̿��� ��
				if ('0' <= c && c<'0' + DROPPING_BALL_COLOR_COUNT * 2) {
					//���� ������ ���� �̹����� �׸���.
					Game->Texture[TEX_BALL0 + (c - '0')]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
				else
					//�������� ��
					if (c & 0x80) {
						//Ÿ�̸Ӹ� ����� ��ǥ�� ���ݾ� ��ȭ����
						//���� �ε巴�� �������� ��ó�� �׸���.
						Game->Texture[TEX_BALL0 + ((c & 0x7f) - '0')]->Draw(
							x*sw, (y - 1 + Time * 0.1f)*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
						);
					}
					else
						//����� ��
						if (c & 0x40) {
							//Ÿ�̸Ӹ� ����� �׸��� ���� �����Ѵ�.
							float f = Time * 0.1f;
							//�ð��� ����� ���� ������°�ó�� ǥ���Ѵ�.
							Game->Texture[TEX_BALL0 + ((c & 0x3f) - '0')]->Draw(
								x*sw, y*sh, sw, sh, 0, 0, 1, 1, D3DXCOLOR(f, f, f, 1)
							);
						}
			}
		}
	}
};

// ��������
class CDroppingBallStage : public CStage {
public:
	CCell * StageCell;
	CCell* BallCell[4];
	CDroppingBallStage() : CStage(L"DROPPING BALL") {
		StageCell = new CCell();
		for (int i = 0; i<4; i++) {
			BallCell[i] = new CCell(DROPPING_BALL_CELL_SIZE, DROPPING_BALL_CELL_SIZE);
		}
	}
	virtual ~CDroppingBallStage() {
		delete StageCell;
		for (int i = 0; i<4; i++) {
			delete BallCell[i];
		}
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"================"
		);
		CDroppingBallBall* ball0 = new CDroppingBallBall(StageCell, BallCell[0], BallCell[1], 0, 4, 1, 6, 7);
		CDroppingBallBall* ball1 = new CDroppingBallBall(StageCell, BallCell[2], BallCell[3], 1, 4, 10, 15, 8);
		ball0->Opponent = ball1;
		ball1->Opponent = ball0;
	}
	virtual void Draw() {
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		for (int y = 0; y<MAX_Y; y++) {
			for (int x = 0; x<MAX_X; x++) {
				char c = StageCell->Get(x, y);
				if (c == '=') {
					Game->Texture[TEX_DROP_FLOOR]->Draw(
						x*sw, y*sh, sw, sh, 0, 0, 1, 1, COL_BLACK
					);
				}
			}
		}
	}
};


//==============================================================
// �� ����߸��� 2
//�� ����߸��� 1�� �Ȱ�����
//���ݹ޴� ���� �ѹ��� ���������� ������ ���ӹ����� ����� ���� ��ġ��
//���ݺ��� ������ �̿��Ͽ� ��뿡�� �ݰ��� ���Ҽ� �ִ�.
// ��
class CDroppingBall2Ball : public CDroppingBallBall {
public:

	CDroppingBall2Ball(
		CCell* stage_cell, CCell* current_ball, CCell* next_ball,
		int player_id, int Erasing_count,
		int field_left, int field_right, int next_left)
		: CDroppingBallBall(
			stage_cell, current_ball, next_ball,
			player_id, Erasing_count,
			field_left, field_right, next_left)
	{}

	virtual void MakeNextBall() {
		NextBall->Set(1, 0, '0' + Rand.Int31() % (DROPPING_BALL_COLOR_COUNT * 2));
		NextBall->Set(1, 1, '0' + Rand.Int31() % (DROPPING_BALL_COLOR_COUNT * 2));
	}

	virtual void BeginErasing(int x, int y, char ball) {
		char c = StageCell->Get(x, y);
		if (c == ball) {
			StageCell->Set(x, y, c | 0x40);
			BeginErasing(x - 1, y, c);
			BeginErasing(x + 1, y, c);
			BeginErasing(x, y - 1, c);
			BeginErasing(x, y + 1, c);
		}
		else
			if ('0' + DROPPING_BALL_COLOR_COUNT <= c && c<'0' + DROPPING_BALL_COLOR_COUNT * 2) {
				StageCell->Set(x, y, c | 0x40);
			}
	}

	virtual void EndErasing(int x, int y) {
		char c = StageCell->Get(x, y);
		if (c & 0x40) {
			c &= 0x3f;
			if ('0' <= c && c<'0' + DROPPING_BALL_COLOR_COUNT) {
				StageCell->Set(x, y, ' ');
			}
			else {
				StageCell->Set(x, y, c - DROPPING_BALL_COLOR_COUNT);
			}
		}
	}

	virtual void Attack() {
		for (int x = FieldLeft; x<FieldRight; x++) {
			if (AttackCount>0) {
				StageCell->Set(x, 0, '0' + DROPPING_BALL_COLOR_COUNT + (x - FieldLeft) % DROPPING_BALL_COLOR_COUNT);
				AttackCount--;
			}
			else {
				StageCell->Set(x, 0, ' ');
			}
		}
	}
};

// ��������
class CDroppingBall2Stage : public CDroppingBallStage {
public:
	CDroppingBall2Stage() {
		Name = L"DROPPING BALL2";
	}
	virtual void Init() {
		StageCell->Init((char*)
			"                "
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"=     =  =     ="
			"================"
		);
		CDroppingBall2Ball* ball0 = new CDroppingBall2Ball(StageCell, BallCell[0], BallCell[1], 0, 3, 1, 6, 7);
		CDroppingBall2Ball* ball1 = new CDroppingBall2Ball(StageCell, BallCell[2], BallCell[3], 1, 3, 10, 15, 8);
		ball0->Opponent = ball1;
		ball1->Opponent = ball0;
	}
};

//==============================================================
// 3���� ��� ����߸���

#define DROPPING_BLOCK3D_FIELD_XSIZE 7
#define DROPPING_BLOCK3D_FIELD_YSIZE 7
#define DROPPING_BLOCK3D_FIELD_ZSIZE 11
#define DROPPING_BLOCK3D_CELL_SIZE 3
#define DROPPING_BLOCK3D_PATTERN_COUNT 7

// ����� ����
char* DroppingBlock3DPattern[DROPPING_BLOCK3D_PATTERN_COUNT] = {
	(char*)
	"## "
	"## "
	"   "
	,
	(char*)
	"   "
	"###"
	"   "
	,
	(char*)
	"## "
	" ##"
	"   "
	,
	(char*)
	" ##"
	"## "
	"   "
	,
	(char*)
	" # "
	"###"
	"   "
	,
	(char*)
	"#  "
	"###"
	"   "
	,
	(char*)
	"  #"
	"###"
	"   "
};

// ���
class CDroppingBlock3DBlock : public CMover {
public:
	CCell3D * StageCell;
	CCell3D* BlockCell;
	float Z;
	int CX, CY, CZ, VX, VY, VZ;
	int State, Time, DropTime;
	bool PrevButton, PrevDown;

	void Init() {
		BlockCell->Clear();
		char* pattern = DroppingBlock3DPattern[Rand.Int31() % DROPPING_BLOCK3D_PATTERN_COUNT];
		for (int i = 0, y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
			for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++, i++) {
				BlockCell->Set(x, y, DROPPING_BLOCK3D_CELL_SIZE / 2, pattern[i]);
			}
		}
		CX = (DROPPING_BLOCK3D_FIELD_XSIZE - DROPPING_BLOCK3D_CELL_SIZE) / 2;
		CY = (DROPPING_BLOCK3D_FIELD_YSIZE - DROPPING_BLOCK3D_CELL_SIZE) / 2;
		CZ = 1;
		X = CX;
		Y = CY;
		Z = CZ;
		State = 0;
		DropTime = 0;
		if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
			State = 99999;
		}
		PrevDown = true;
	}

	CDroppingBlock3DBlock(CCell3D* stage_cell, CCell3D* block_cell)
		: StageCell(stage_cell), BlockCell(block_cell), PrevButton(true)
	{
		Init();
	}

	//����� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//�Է� ����
		if (State == 0) {
			//���� Ÿ�̸Ӹ� �����Ѵ�.
			DropTime++;
			//��ư 3�� �Է��ϰų�
			//���� Ÿ�̸Ӱ� ���� ���� �����ϸ� ����� ���Ͻ�Ų��.
			if ((is->Button[3] && !PrevDown) || DropTime == 60) {
				//�� ��ǥ�� ������ ��쿡
				//����� ���������� ���̳� �ٸ� ��ϰ� �����ߴ����� �˻��Ѵ�.
				if (StageCell->Hit(CX, CY, CZ + 1, BlockCell)) {
					//������ ��쿡�� ��� ���� ������ ���� �ռ��Ѵ�.
					StageCell->Merge(CX, CY, CZ, BlockCell);
					//�������·� ��ȯ�Ѵ�.
					State = 2;
				}
				else {
					//�������� ���� ��쿡��
					//���� Ÿ�̸� �� �ӵ��� �����ϰ� �� ��ǥ�� �����Ѵ�.
					DropTime = 0;
					CZ++;
					VX = 0;
					VY = 0;
					VZ = 1;
					//�̵����·� ��ȯ�Ѵ�.
					Time = 0;
					State = 1;
				}
			}
			else
				//������ �������� �Է��� ��쿡
				//����� �̵� ������ �������� ���� �������� ������
				//����� �������� �̵���Ų��.
				if (is->Left && !StageCell->Hit(CX - 1, CY, CZ, BlockCell)) {
					CX--;
					VX = -1;
					VY = 0;
					VZ = 0;
					Time = 0;
					State = 1;
				}
				else
					if (is->Right && !StageCell->Hit(CX + 1, CY, CZ, BlockCell)) {
						CX++;
						VX = 1;
						VY = 0;
						VZ = 0;
						Time = 0;
						State = 1;
					}
					else
						if (is->Up && !StageCell->Hit(CX, CY + 1, CZ, BlockCell)) {
							CY++;
							VX = 0;
							VY = 1;
							VZ = 0;
							Time = 0;
							State = 1;
						}
						else
							if (is->Down && !StageCell->Hit(CX, CY - 1, CZ, BlockCell)) {
								CY--;
								VX = 0;
								VY = -1;
								VZ = 0;
								Time = 0;
								State = 1;
							}
							else
								//������ ��ư�� ������ �ʾ��� ������ ó���Ѵ�.
								if (!PrevButton) {
									//��ư0�� �Է��� ��쿡��
									//X������ ȸ�������� �Ͽ� ����� ȸ����Ų��.
									if (is->Button[0]) {
										for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++) {
											BlockCell->Swap(x, 1, 2, x, 2, 1);
											BlockCell->Swap(x, 0, 1, x, 1, 2);
											BlockCell->Swap(x, 1, 0, x, 0, 1);
											BlockCell->Swap(x, 0, 0, x, 0, 2);
											BlockCell->Swap(x, 2, 0, x, 0, 0);
											BlockCell->Swap(x, 2, 2, x, 2, 0);
										}
										//ȸ�� ���� ����� �������� ���� ���� �Ҷ���
										//ȸ���� ����Ѵ�.
										if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
											for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++) {
												BlockCell->Swap(x, 2, 2, x, 2, 0);
												BlockCell->Swap(x, 2, 0, x, 0, 0);
												BlockCell->Swap(x, 0, 0, x, 0, 2);
												BlockCell->Swap(x, 1, 0, x, 0, 1);
												BlockCell->Swap(x, 0, 1, x, 1, 2);
												BlockCell->Swap(x, 1, 2, x, 2, 1);
											}
										}
									}
									else
										//��ư1�� �Է��� ��쿡��
										//Y������ ȸ�������� �Ͽ� ����� ȸ����Ų��.
										if (is->Button[1]) {
											for (int y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
												BlockCell->Swap(2, y, 1, 1, y, 2);
												BlockCell->Swap(1, y, 0, 2, y, 1);
												BlockCell->Swap(0, y, 1, 1, y, 0);
												BlockCell->Swap(0, y, 0, 0, y, 2);
												BlockCell->Swap(2, y, 0, 0, y, 0);
												BlockCell->Swap(2, y, 2, 2, y, 0);
											}
											//ȸ�� ���� ����� �������� ���� ������ ������
											//ȸ���� ����Ѵ�.
											if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
												for (int y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
													BlockCell->Swap(2, y, 2, 2, y, 0);
													BlockCell->Swap(2, y, 0, 0, y, 0);
													BlockCell->Swap(0, y, 0, 0, y, 2);
													BlockCell->Swap(0, y, 1, 1, y, 0);
													BlockCell->Swap(1, y, 0, 2, y, 1);
													BlockCell->Swap(2, y, 1, 1, y, 2);
												}
											}
										}
										else
											//��ư 2�� �Է��� ��쿡��
											//Z������ ȸ�������� �Ͽ� ����� ȸ����Ų��.
											if (is->Button[2]) {
												for (int z = 0; z<DROPPING_BLOCK3D_CELL_SIZE; z++) {
													BlockCell->Swap(1, 2, z, 2, 1, z);
													BlockCell->Swap(0, 1, z, 1, 2, z);
													BlockCell->Swap(1, 0, z, 0, 1, z);
													BlockCell->Swap(0, 0, z, 0, 2, z);
													BlockCell->Swap(2, 0, z, 0, 0, z);
													BlockCell->Swap(2, 2, z, 2, 0, z);
												}
												//ȸ�� ���� ����� �������� ���� �����Ҷ�����
												//ȸ���� ����Ѵ�.
												if (StageCell->Hit(CX, CY, CZ, BlockCell)) {
													for (int z = 0; z<DROPPING_BLOCK3D_CELL_SIZE; z++) {
														BlockCell->Swap(2, 2, z, 2, 0, z);
														BlockCell->Swap(2, 0, z, 0, 0, z);
														BlockCell->Swap(0, 0, z, 0, 2, z);
														BlockCell->Swap(1, 0, z, 0, 1, z);
														BlockCell->Swap(0, 1, z, 1, 2, z);
														BlockCell->Swap(1, 2, z, 2, 1, z);
													}
												}
											}
								}
			//������ ��ư�� ���ȴ����� ������ ���´�.
			PrevButton = is->Button[0] | is->Button[1] | is->Button[2];
			//��ư�� ���� ä�� ���� �� ����� ����ؼ� �������� �ʵ��� �ϱ� ���� ó��
			if (!is->Button[3]) PrevDown = false;
		}
		//�̵� ������ ó��
		if (State == 1) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			//Ÿ�̸Ӹ� ����� �׸���ǥ�� ���ݾ� �����Ѵ�.
			X = CX - VX * (1 - Time * 0.1f);
			Y = CY - VY * (1 - Time * 0.1f);
			Z = CZ - VZ * (1 - Time * 0.1f);
			//Ÿ�̸Ӱ� ���� ���� �����ϸ� �Է»��·� ��ȯ�Ѵ�.
			if (Time == 10) {
				State = 0;
			}
		}
		//��������
		if (State == 2) {
			//����� ����� �ϳ��� ���� ��쿡��
			//������ ���·� ��ȯ�Ѵ�.
			State = 4;
			//����� ��ƴ���� ä���� ���� ã�´�.
			for (int z = 1; z<DROPPING_BLOCK3D_FIELD_ZSIZE - 1; z++) {
				int x, y;
				for (y = 1; y<DROPPING_BLOCK3D_FIELD_YSIZE - 1; y++) {
					for (x = 1; x<DROPPING_BLOCK3D_FIELD_XSIZE - 1; x++) {
						//������ ���� �ִ� ��쿡�� ������ Ż���Ѵ�.
						if (StageCell->Get(x, y, z) == ' ') {
							y = DROPPING_BLOCK3D_FIELD_YSIZE;
							break;
						}
					}
				}
				//����� ä���� ���� �ִ� ����� ó��
				if (y == DROPPING_BLOCK3D_FIELD_YSIZE - 1) {
					for (y = 1; y<DROPPING_BLOCK3D_FIELD_YSIZE - 1; y++) {
						for (x = 1; x<DROPPING_BLOCK3D_FIELD_XSIZE - 1; x++) {
							//���� ����� ����� ��� [+]���� �����Ѵ�.
							StageCell->Set(x, y, z, '+');
						}
					}
					//�������·� ��ȯ�Ѵ�.
					Time = 0;
					State = 3;
				}
			}
		}
		//��������
		if (State == 3) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time++;
			//Ÿ�̸Ӱ� ���� ���� �����ϸ� ����� �����Ѵ�.
			if (Time == 20) {
				//����� ���[+]�� �ִ� ���� ã�´�.
				for (int i = 1; i<DROPPING_BLOCK3D_FIELD_ZSIZE - 1; i++) {
					//����� ����� ã�� ����� ó��
					if (StageCell->Get(1, 1, i) == '+') {
						//����� �ܺ��� ���ʿ� �ִ� ��� ����
						//�Ѵܾ� �������� ����߸���.
						for (int z = i; z>0; z--) {
							for (int y = 1; y<DROPPING_BLOCK3D_FIELD_YSIZE - 1; y++) {
								for (int x = 1; x<DROPPING_BLOCK3D_FIELD_XSIZE - 1; x++) {
									StageCell->Set(x, y, z, StageCell->Get(x, y, z - 1));
								}
							}
						}
					}
				}
				//���������·� ��ȯ�Ѵ�.
				State = 4;
			}
		}
		//����������
		if (State == 4) {
			Init();
		}
		return true;
	}

	virtual void Draw() {
		for (int z = DROPPING_BLOCK3D_FIELD_ZSIZE - 1; z >= 0; z--) {
			for (int y = 0; y<DROPPING_BLOCK3D_FIELD_YSIZE; y++) {
				for (int x = 0; x<DROPPING_BLOCK3D_FIELD_XSIZE; x++) {
					float
						dx = x - DROPPING_BLOCK3D_FIELD_XSIZE * 0.5f + 0.5f,
						dy = y - DROPPING_BLOCK3D_FIELD_YSIZE * 0.5f + 0.5f,
						dz = z,
						f = 1 - Time * 0.1f;
					switch (StageCell->Get(x, y, z)) {
					case '#':
						Game->Mesh[MESH_CUBE_GRAY]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, 1, 0);
						break;
					case '+':
						Game->Mesh[MESH_CUBE_GRAY]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, f, 0);
						break;
					case '=':
						Game->Mesh[MESH_CUBE_WHITE]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, 1, 0);
						break;
					}
				}
			}
		}
		if (State<2) {
			for (int z = DROPPING_BLOCK3D_CELL_SIZE - 1; z >= 0; z--) {
				for (int y = 0; y<DROPPING_BLOCK3D_CELL_SIZE; y++) {
					for (int x = 0; x<DROPPING_BLOCK3D_CELL_SIZE; x++) {
						float
							dx = X + x - DROPPING_BLOCK3D_FIELD_XSIZE * 0.5f + 0.5f,
							dy = Y + y - DROPPING_BLOCK3D_FIELD_YSIZE * 0.5f + 0.5f,
							dz = Z + z;
						if (BlockCell->Get(x, y, z) == '#') {
							Game->Mesh[MESH_CUBE_GRAY]->Draw(dx, dy, dz, 1, 1, 1, 0, 0, 0, TO_NONE, 0.5f, 0);
						}
					}
				}
			}
		}
	}
};

// ��������
class CDroppingBlock3DStage : public CStage {
	CCell3D* StageCell;
	CCell3D* BlockCell;
public:
	CDroppingBlock3DStage() : CStage(L"DROPPING BLOCK 3D") {
		StageCell = new CCell3D(DROPPING_BLOCK3D_FIELD_XSIZE, DROPPING_BLOCK3D_FIELD_YSIZE, DROPPING_BLOCK3D_FIELD_ZSIZE);
		BlockCell = new CCell3D(DROPPING_BLOCK3D_CELL_SIZE, DROPPING_BLOCK3D_CELL_SIZE, DROPPING_BLOCK3D_CELL_SIZE);
	}
	virtual ~CDroppingBlock3DStage() {
		delete StageCell;
		delete BlockCell;
	}
	virtual void Init() {
		StageCell->Clear();
		for (int z = 1; z<DROPPING_BLOCK3D_FIELD_ZSIZE; z++) {
			for (int y = 0; y<DROPPING_BLOCK3D_FIELD_YSIZE; y++) {
				for (int x = 0; x<DROPPING_BLOCK3D_FIELD_XSIZE; x++) {
					if (
						x == 0 || x == DROPPING_BLOCK3D_FIELD_XSIZE - 1 ||
						y == 0 || y == DROPPING_BLOCK3D_FIELD_YSIZE - 1 ||
						z == DROPPING_BLOCK3D_FIELD_ZSIZE - 1
						) {
						StageCell->Set(x, y, z, '=');
					}
				}
			}
		}
		new CDroppingBlock3DBlock(StageCell, BlockCell);
	}
};