//========================================================
// �̷� �ȱ�

// ĳ���� Ŭ��
class CMazeWalkerMan : public CMover {
public:
	//���������� ��Ÿ���� ��
	CCell * Cell;
	//�� ���� ��ǥ�� �̵� ����
	int CX, CY, VX, VY;
	//ĳ������ ���¿� Ÿ�̸�
	int State, Time;
	
	//������
	//��ǥ, ����, Ÿ�̸�, �׷����� ���� ���� �����Ѵ�.
	CMazeWalkerMan(CCell* cell, int x, int y)
		: Cell(cell), CX(x), VX(0), VY(0), State(0), Time(0)
	{
		Texture = Game->Texture[TEX_MAN];
		X = x;
		Y = y;
	}

	//�̵�
	//������ �Է¿� ���� ĳ���͸� �����¿�� �����δ�.
	//�� ó���� ���� �ð�(1/60��)���� ȣ��ȴ�.
	virtual bool Move(const CInputState* is) {
		//��������(ĳ���Ͱ� �����ϰ� �ִ� ����)
		if (State = 0) {
			//������ �Է¿� ���� �̵� ������ �����Ѵ�.
			VX = VY = 0;
			if (is->Left)VX = -1;
			else if (is->Right)VX = 1;
			else if (is->Up)VY = -1;
			else if (is->Down)VY = 1;

			//������ �Է��� �ִ� ����� ó��
			if (VX != 0 || VY != 0) {
				//�̵� ���⿡ �ִ� ���� �������� �˻��Ѵ�.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//�̵� ������ ������ ��쿡�� ĳ���͸� �̵���Ų��.
					//���� ĳ���Ͱ� �ִ� ���� �������� �����ϰ�
					//�̵� ������ ���� ĳ����(���� M)���� �����Ѵ�.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					//�� ���� ĳ���� ��ǥ�� �����Ѵ�.
					CX += VX;
					CY += VY;
					//Ÿ�̸Ӹ� �����ϰ� �̵����·� ��ȯ�Ѵ�.
					State = 1;
					Time = 10;
				}
			}
		}

		//�̵�����(ĳ���Ͱ� �̵����� ����)
		if (State == 1) {
			//Ÿ�̸Ӹ� ���ҽ�Ų��.
			Time--;

			//Ÿ�̸Ӹ� ����� ȭ����� ĳ���� ��ǥ�� ���ݾ� ��ȭ����
			//ĳ���͸� �ε巴�� �̵���Ų��.
			X = CX - VX * Time*0.1f;
			Y = CY - VY * Time*01.f;
			//Ÿ�̸Ӱ� 0�� �Ǹ� �̵� ������ �ʱ�ȭ�ϰ� ���ڻ��·� ���ư���.
			if (Time == 0) {
				VX = VY = 0;
				State = 0;
			}
		}

		//ĳ���͸� �������� �ʴ� ��쿡�� true�� ��ȯ�ϰ�
		//ĳ���͸� �����ϴ� ��쿡�� false�� ��ȯ�Ѵ�.
		return true;
	}
};


//�������� Ŭ����
class CMazeWalkerStage : public CStage{
	//���������� ��Ÿ���� ��
	CCell* Cell;
public:
	//������
	//���������� �̸��� �����ϰ� ���� ��Ÿ���� ��ü�� �����Ѵ�.
	CMazeWalkerStage() : CStage(L"MAZE WALKER") {
		Cell = new CCell();
	}

	//�Ҹ���
	//���� ��ü�� �����Ѵ�.
	virtual ~CMazeWalkerStage() {
		delete Cell;
	}

	//���������� �ʱ�ȭ
	//�� ó���� ���������� ���۵� ������ ȣ��ȴ�.
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
		//��� ���� �˻��Ͽ�
		//ĳ���� ��(���� M)�� ��ġ�� ĳ������ ��ü�� �����Ѵ�.
		for (int y=0; y<MAX_Y; y++) {
			for (int x=0; x<MAX_X; x++) {
				if (Cell->Get(x, y)=='M') {
					new CMazeWalkerMan(Cell, x, y);
				}
			}
		}
	}

	//�������� �׸���
	//�� ó���� ���������� ȭ�鿡 �׸� �� ȣ��ȴ�.
	virtual void Draw() {
		//ȭ���� �ػ󵵿� ���� �׸� ũ�⸦ ��ȭ��Ű�� ���� ó��
		float
			sw = Game->GetGraphics()->GetWidth() / MAX_X,
			sh = Game->GetGraphics()->GetHeight() / MAX_Y;
		//��� ���� �˻��Ͽ�
		//�� ��(���� #)�� ��ġ�� ���� �̹����� �׸���.
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
//��ü �б�

//��ü�� �� ���� ȣ�� �Ǵ� ó��
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
		//��ü�� ���������̸� �̵����·� ��ȯ�Ѵ�.
		if (State == 0) {
			//�̵������� ����
			VX = vx;
			VY = vy;
			//�� ��ǥ�� ����
			CX += vx;
			CY += vy;
			//���¿� Ÿ�̸� ����
			State = 1;
			Time = 10;
		}
	}

	//��ü�� �̵� �Ÿ�
	virtual bool Move(const CInputState* is) {
		//��ü�� �̵������� ���� ó��
		if (State == 1) {

			//Ÿ�̸Ӹ� �����Ѵ�.
			Time--;

			//�׸���ǥ�� �����Ѵ�.
			//Ÿ�̸Ӹ� ����� ���ݾ� ��ȭ��Ų��.
			X = CX - VX * Time*0.1f;
			Y = CY - VX * Time*0.1f;

			//Ÿ�̸Ӱ� 0�� �Ǹ� �������·� ��ȯ�Ѵ�.
			if (Time == 0) {
				State = 0;
			}
		}
		return true;
	}
};

//���
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
	//ĳ������ �̵� ó��
	virtual bool Move(const CInputState* is) {
		//���� ������ ó��
		if (State == 0) {
			//���� �Է¿� ���� �̵� ������ �����Ѵ�.
			VX = VY = 0;
			if (is->Left)VX = -1;
			else if (is->Right)VX = 1;
			else if (is->Up)VX = -1;
			else if (is->Down)VY = 1;

			//�̵��ϴ� ����� ó��
			if (VX != 0 || VY != 0) {
				//ĳ������ �̵� ���� ���� ������ ��� ó��
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//ĳ������ ���� ��ġ�� ���� �̵� ������ ���� �����Ѵ�.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					//�� ��ǥ�� �����ϴ�.
					CX += VX;
					CY += VY;
					//���¿� Ÿ�̸Ӹ� �����Ѵ�.
					State = 1;
					Time = 10;
				}
				//�̵� ���⿡ ��ü�� �ְ�
				//���� ��ü�� �̵� ������ ������ ��� ó��
				else if (Cell->Get(CX + VX, CY + VY) == 'L' &&
					Cell->Get(CX + VX * 2, CY + VY * 2) == ' ') {
					//ĳ���Ϳ� ��ü�� ���Ͽ�
					//���� ��ġ�� ���� �̵� ������ ���� �����Ѵ�.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					Cell->Set(CX + VX * 2, CY + VY * 2, 'L');
					//�� ��ǥ�� �����Ѵ�.
					CX += VX;
					CY += VY;

					//���¿� Ÿ�̸Ӹ� �����Ѵ�.
					State = 1;
					Time = 10;

					//��ü�� ��ü�� ã�´�.
					for (CTaskIter i(Game->MoverList); i.HasNext();) {
						CMover* mover = (CMover*)i.Next();
						//��ü�� �б� ���� ó���� ȣ���Ѵ�.
						if ((int)mover->X == CX && (int)mover->Y == CY) {
							((CLoadPusherLoad*)mover)->Push(VX, VY);
						}
					}
				}
			}
		}
		//�̵� ���� ó��
		if (State == 1) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time--;

			//�׸���ǥ�� �����Ѵ�.
			//Ÿ�̸Ӹ� ����� ���ݾ� ��ȭ��Ų��.
			X = CX - VX * Time*0.1f;
			Y = CY - VY * Time*0.1f;

			//Ÿ�̸Ӱ� 0�� �Ǹ� �������·� ��ȯ��Ų��.
			if (Time == 0) {
				State = 0;
			}
		}
		return true;
	}
};

//��������
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
//�̲������� ��ü �б�

//�̲������� ��ü Ŭ��
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
	//��ü�� �� �� ȣ���ϴ� ó��
	void Push(int vx, int vy) {
		//���� ���¿��� �̵� ���·� ��ȯ�Ѵ�.
		if (State == 0) {
			//�̵������� ���Ѵ�.
			VX = vx;
			VY = vy;
			//Ÿ�̸ӿ� ���¸� �����Ѵ�.
			Time = 1;
			State = 1;
		}
	}

	//��ü�� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//�̵����� ó��
		if (State == 1) {
			//Ÿ�̸Ӹ� �����Ѵ�.
			Time--;

			//�׸� ��ǥ�� �����Ѵ�.
			X = CX - VX * Time*0.2f;
			Y = CY - VY * Time*0.2f;
			//Ÿ�̸Ӱ� 0�� �Ǿ��� ����� ó��
			if (Time == 0) {
				//�̵� ������ ���� ���������� �˻��Ѵ�.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//�̵� ������ ���� ������ ��쿡�� ����ؼ� �̵��Ѵ�.
					//���� ��ġ�� �̵��� ��ġ�� ���� �����Ѵ�.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'L');
					//�� ��ǥ�� �����Ѵ�.
					CX += VX;
					CY += VY;
					//Ÿ�̸Ӹ� �����Ѵ�.
					Time = 5;
				}
				else {
					//�̵� ���� ���� ������ �ƴ� ��쿡�� �����Ѵ�.
					//�������·� ��ȯ�Ѵ�.
					State = 0;
				}
			}
		}
		return true;
	}
};

//���
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

	//ĳ������ �̵� ó��
	virtual bool Move(const CInputState* is) {
		//���� ������ ó��
		if (State == 0) {
			//������ �Է¿� ���� �̵� ������ �����Ѵ�.
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

			//������ �Է��� �ִ� ����� ó��
			if (is->Left || is->Right || is->Up || is->Down) {
				//�̵� ������ ���� �������� �˻��Ͽ�
				//������ ��쿡�� �̵��Ѵ�.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					//�̵� ���� ��ġ�� �̵��� ��ġ�� ���� �����Ѵ�.
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					//�� ��ǥ�� �����Ѵ�.
					CX += VX;
					CY += VY;
					//���¿� Ÿ�̸Ӹ� �����Ѵ�.
					//�̵����·� ��ȯ�Ѵ�.
					State = 1;
					Time = 10;

				}
			}
			//��ü�� ���� ä�� ��ư�� ���� ����� ó��
			if (
				is->Button[0] &&
				Cell->Get(CX + VX, CY + VY) == 'L'
				) {
				//��ü�� ��ü�� ã�´�.
				for (CTaskIter i(Game->MoverList); i.HasNext();) {
					CMover* mover = (CMover*)i.Next();
					//��ü�� �̴� ó���� ȣ���Ѵ�.
					if ((int)mover->X == CX + VX && (int)mover->Y == CY + VY) {
						((CSlidingLoadPusherLoad*)mover)->Push(VX, VY);
					}
				}
			}
			//�̵� ������ ó��
			if (State == 1) {
				//Ÿ�̸Ӹ� �����Ѵ�.
				Time--;
				//�׸���ǥ�� �����Ѵ�.
				X = CX - VX * Time*0.1f;
				Y = CY - VY * Time*0.1f;
				//Ÿ�̸Ӱ� 0�� �Ǹ� �������·� ��ȯ�Ѵ�.
				if (Time == 0) {
					State = 0;
				}
			}
		}
		return true;
	}
};

//��������
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