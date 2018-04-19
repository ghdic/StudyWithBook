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

//==============================================================
// �߷¿� ���� �������� ��ü �б�

// ��ü
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
	//��ü�� �� �� ȣ���ϴ� ó��
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

	//��ü�� �̵� ó��
	virtual bool Move(const CInputState* is) {
		if (State == 0) {
			//�Ʒ��� ������ ���� �����̸� �̵����·� ��ȯ�Ѵ�.
			if (Cell->Get(CX, CY + 1) == ' ') {
				//�̵� ������ �����Ѵ�(�Ʒ���)
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

// ���
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
	//ĳ������ �̵�ó��
	virtual bool Move(const CInputState* is) {
		if (State == 0) {
			//���� �Է¿� ���� �̵� ������ �����Ѵ�.
			VX = VY = 0;
			if (is->Left) VX = -1; else
				if (is->Right) VX = 1; else
					if (is->Up) VY = -1; else
						if (is->Down) VY = 1;
			//�̵� �ϴ� ��� ó��
			if (VX != 0 || VY != 0) {
				//�̵� ������ ���� �����̸� �̵��Ѵ�.
				if (Cell->Get(CX + VX, CY + VY) == ' ') {
					Cell->Set(CX, CY, ' ');
					Cell->Set(CX + VX, CY + VY, 'M');
					CX += VX;
					CY += VY;
					State = 1;
					Time = 10;
				}
				else
					//�̵� ������ ���� ��ü�̰�
					//�� ��ü�� �̵� ������ �����̸�
					//��ü�� �δ�.
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

// ��������
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
// ���������� �����̴� ĳ����

// ���
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
	//ĳ���Ϳ��� ����� ������ ȣ��Ǵ� ó��
	void Dig() {
		//���� ĳ���� �÷��׸� �����Ѵ�.
		Digger = true;
		//�׷����� ���� ĳ���ͷ� �����Ѵ�.
		Texture = Game->Texture[TEX_DRILLER];
	}

	//ĳ������ �̵� ó��
	virtual bool Move(const CInputState* is) {
		if (State == 0) {
			//�Ʒ��� ���� ������ ��쿡�� ���� ���·� ��ȯ�Ѵ�.
			if (Cell->Get(CX, CY + 1) == ' ') {
				CY += VY;
				Time = 20;
				State = 1;
			}
			else
				//���� ĳ���� �÷��װ� �����Ǿ� �ְ�,
				//�Ʒ��� ������ ���� ���� �ܿ쿡�� �������·� ��ȯ�Ѵ�.
				if (Digger && Cell->Get(CX, CY + 1) == '#') {
					CY += VY;
					Time = 40;
					State = 2;
				}
				else
					//�¿� ������ ���� ������ ��쿡�� ������·� ��ȯ�Ѵ�.
					if (Cell->Get(CX + VX, CY) == ' ') {
						CX += VX;
						Time = 20;
						State = 3;
					}
					else {
						//�¿� ������ ������ �ƴ� ������
						//�̵� ������ �ݴ�� �����Ѵ�.
						VX = -VX;
					}
		}
		//���� ������ ó��
		if (State == 1) {
			Time--;
			Y = CY - VY * Time*0.05f;
			if (Time == 0) {
				State = 0;
			}
		}
		//���� ������ ó��
		if (State == 2) {
			Time--;
			Y = CY - VY * Time*0.025f;
			//Ÿ�̸Ӱ� 0�� �Ǹ�
			//���� �ٴ��� ���� �������� �����ϰ�
			//�������·� ��ȯ�Ѵ�.
			if (Time == 0) {
				Cell->Set(CX, CY, ' ');
				State = 0;
			}
		}
		//����ó��
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

// Ŀ��
class CSelfDirectiveCharacterCursor : public CMover {
public:
	CSelfDirectiveCharacterCursor(float x, float y)
	{
		Texture = Game->Texture[TEX_SHURIKEN];
		X = x;
		Y = y;
	}
	
	//Ŀ���� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//Ŀ���� �ӵ��� ���������� ũ�⸦ �����Ѵ�.
		float speed = 0.1f, hit = 0.5f;
		//������ �Է¿� ���� Ŀ���� ��ǥ�� �����Ѵ�.
		if (is->Left && X>0) X -= speed;
		if (is->Right && X<MAX_X - 1) X += speed;
		if (is->Up && Y>0) Y -= speed;
		if (is->Down && Y<MAX_Y - 1) Y += speed;

		//��� ĳ���Ϳ� ����
		//Ŀ������ �������� ó���� �����Ѵ�.
		CSelfDirectiveCharacterMan* man = NULL;
		for (CTaskIter i(Game->MoverList); i.HasNext(); ) {
			CMover* mover = (CMover*)i.Next();
			//Ŀ���� �������ִ� ĳ���͸� �߰��ϸ�
			//������ Ż���Ѵ�.
			if (
				mover != this &&
				mover->X<X + hit && X<mover->X + hit &&
				mover->Y<Y + hit && Y<mover->Y + hit
				) {
				man = (CSelfDirectiveCharacterMan*)mover;
				break;
			}
		}
		//Ŀ���� �������ִ� ĳ���Ͱ� �ִ� ����� ó��
		if (man) {
			//Ŀ���� �������� ǥ���Ѵ�.
			Texture = Game->Texture[TEX_PILE];
			//��ư �Է��� ������
			//Ŀ���� �������ִ� ĳ���Ϳ��� ����� ������.
			if (is->Button[0]) {
				man->Dig();
			}
		}
		else {
			//Ŀ���� ������ �ִ� ĳ���Ͱ� ���°��
			//Ŀ���� ���� ���·� ǥ���Ѵ�.
			Texture = Game->Texture[TEX_SHURIKEN];
		}
		return true;
	}
};

// ��������
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
// �ڵ������ Ŀ��

// ���� Ŀ��
#define FOLLOWING_CURSOR_LENGTH 101
//Ŀ���� ������ ����� ��ǥ�� �����ϱ� ���� �迭
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
	//���� Ŀ���� �̵� ó��
	virtual bool Move(const CInputState* is) {
		//Ŀ���� �̵� �ӵ�
		float speed = 0.1f;
		//������ �Է¿� ���� Ŀ���� �׸���ǥ�� ��ȭ��Ų��.
		if (is->Left && X>0) X -= speed;
		if (is->Right && X<MAX_X - 1) X += speed;
		if (is->Up && Y>0) Y -= speed;
		if (is->Down && Y<MAX_Y - 1) Y += speed;

		//��ư�� �Է��� ����� ó��
		//���� Ŀ���� ����Ű�� ������
		//�Ĺ̿� ������� Ŀ���� ����Ű�� ��������
		//���� �ٲ� �ִ´�.
		float *fx = FollowingCursorX, *fy = FollowingCursorY;
		if (!PrevButton && is->Button[0]) {
			//Ŀ���� ����Ű�� ������ �� ��ǥ�� ����Ѵ�.
			int
				x = ((int)X / 5) * 5,
				y = (((int)Y - 1) / 5) * 5 + 1,
				tx = ((int)fx[FOLLOWING_CURSOR_LENGTH - 1] / 5) * 5,
				ty = (((int)fy[FOLLOWING_CURSOR_LENGTH - 1] - 1) / 5) * 5 + 1;
			//���� ���� ���� �ٲ� �ִ´�.
			for (int i = 1; i <= 4; i++) {
				for (int j = 1; j <= 4; j++) {
					Cell->Swap(x + i, y + j, tx + i, ty + j);
				}
			}
		}
		//��ư�� ���� ä�� ���� ��
		//�ٲ� �ֱⰡ ����ؼ� �ݺ����� �ʵ��� �ϱ� ���Ͽ�
		//������ ��ư �Է��� �����Ѵ�.
		PrevButton = is->Button[0];


		//Ŀ���� ��ǥ�� �����ϱ� ���Ͽ�
		//������ ��ǥ�� �迭�� �ڷ� �ű��.
		//���� deque�� �̿��ϸ� ���� ���������
		for (int i = FOLLOWING_CURSOR_LENGTH - 1; i>0; i--) {
			fx[i] = fx[i - 1];
			fy[i] = fy[i - 1];
		}
		fx[0] = X;
		fy[0] = Y;

		return true;
	}
};

// ���� Ŀ��
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
	//���� Ŀ���� �̵�ó��
	virtual bool Move(const CInputState* is) {
		//���� Ŀ���� ���� ��ǥ�� �迭���� �о�
		//���� Ŀ���� ��ǥ�� �����Ѵ�.
		float *fx = FollowingCursorX, *fy = FollowingCursorY;
		X = fx[Delay];
		Y = fy[Delay];
		return true;
	}
};

// ��������
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
