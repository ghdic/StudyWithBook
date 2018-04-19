#include "Main.h"

//���� �����ϴ� Ŭ����
class CCell {
	//���� ������ ��Ÿ���� �迭
	char * Cell;
	//X ����(���� ����)�� ���� ��, Y����(���� ����)�� ���� ��
	int XSize, YSize;
public:
	//������(���� ������ �����Ѵ�)
	//���� �迭�� �����Ѵ�.
	CCell(int xsize, int ysize)
		: XSize(xsize), YSize(ysize)
	{
		Cell = new char[XSize*YSize];
	}
	//������(���� ������ �⺻������ ���)
	//���� �迭�� �����Ѵ�.
	CCell()
		: XSize(MAX_X), YSize(MAX_Y)
	{
		Cell = new char[XSize*YSize];
	}
	//�Ҹ���
	//���� �迭�� �����Ѵ�.
	~CCell() {
		delete[] Cell;
	}

	//���� �ʱ�ȭ
	//�Ű������� ������ �������� ���� �迭�� �ʱ�ȭ�Ѵ�.
	void Init(char* cell) {
		for (int i = 0, n = XSize * YSize; i < n; i++)Cell[i] = cell[i];
	}

	//���� ���� ���
	//������ ��ǥ�� �ִ� ���� ������ ��´�.
	char Get(int x, int y) {
		if (0 <= x && x < XSize && 0 <= y && y < YSize) {
			return Cell[x + y * XSize];
		}
		else {
			return ' ';
		}
	}

	//X size ���ϱ�
	int GetXSize() {
		return XSize;
	}

	//Y size ���ϱ�
	int GetYSize() {
		return YSize;
	}

	//���� ����
	//������ ��ǥ�� �ִ� ���� ������ �����Ѵ�.
	void Set(int x, int y, char value) {
		if (0 <= x && x < XSize && 0 <= y && y < YSize) {
			Cell[x + y * XSize] = value;
		}
	}

	//�� �ٲ� �ֱ�
	//������ 2���� ��ǥ�� �ִ� ���� �ٲ� �ִ´�.
	void Swap(int xa, int ya, int xb, int yb) {
		char c = Get(xa, ya);
		char d = Get(xb, yb);
		Set(xa, ya, d);
		Set(xb, yb, c);
	}

	//�迭 ����� AND����
	void And(char value) {
		int xs = GetXSize(), ys = GetYSize();

		for (int y = 0; y < ys; y++) {
			for (int x = 0; x < xs; x++) {
				Set(x, y, Get(x, y)&value);
			}
		}
	}

	//�迭 ����� OR����
	void Or(char value) {
		int xs = GetXSize(), ys = GetYSize();
		for (int y = 0; y < ys; y++) {
			for (int x = 0; x < xs; x++) {
				Set(x, y, Get(x, y) | value);
			}
		}
	}

	//vx, vy ��ŭ �̵��ϱ�
	void Shift(int vx, int vy) {
		int xs = GetXSize(), ys = GetYSize();
		int fx, tx, dx, fy, ty, dy;
		if (vx < 0) {
			fx = 0;
			tx = xs - 1;
			dx = -1;
		}
		else {
			fx = xs - 1;
			tx = 0;
			dx = 1;
		}
		if (vy < 0) {
			fy = 0;
			ty = ys - 1;
			dy = -1;
		}
		else {
			fy = ys - 1;
			ty = 0;
			dy = 1;
		}

		for (int y = fy; y != ty; y += dy) {	//���� ��Ż��? x,y������..?
			for (int x = fx; x != tx; x += dx) {
				Set(x, y, Get(x - vx, y - vy));
			}
		}
	}

	//	���� c0 -> c1 �ٲٱ�
	void Replace(char c0, char c1) {
		int xs = GetXSize(), ys = GetYSize();
		
		for (int y = 0; y < ys; y++) {
			for (int x = 0; x < xs; x++) {
				if (Get(x, y) == c0)Set(x, y, c1);
			}
		}
	}
	
	//���� �������� ó��
	//���� ������ ��ǥ�� ��ġ���� ��
	//������ �ƴ� ������ ������������ �˻��Ѵ�.
	//�������� ��쿡�� true, �������� �ʴ� ��쿡�� false�� ��ȯ�Ѵ�.
	bool Hit(int x, int y, CCell* cell) {
		for (int i = 0, in = cell->GetYSize(); i < in; i++) {
			for (int j = 0, jn = cell->GetXSize(); j < jn; j++) {
				//������ �ƴ� ������ �� ���� ��ġ�� ��쿡��
				//true�� ��ȯ�Ѵ�.
				if (cell->Get(j, i) != ' '&&Get(x + j, y + i) != ' ') {
					return true;
				}
			}
		}
		//������ �ƴ� ������ �� ���� ��ġ�� �ʴ� ��쿡��
		//false�� ��ȯ�Ѵ�.
		return false;
	}

	//���� �ռ� ó��
	//���� ������ ��ǥ�� �ռ��Ѵ�.
	//������ ���� �ռ����� �ʴ´�.
	void Merge(int x, int y, CCell* cell) {
		for (int i = 0, in = cell->GetYSize(); i < in; i++) {
			for (int j = 0, jn = cell->GetXSize(); j < jn; j++) {
				//������ �ƴ� ���� �ռ��Ѵ�.
				if (cell->Get(j, i) != ' ') {
					Set(x + j, y + i, cell->Get(j, i));
				}
			}
		}
	}

	
	void Set(int x, int y, CCell* cell) {
		for (int i = 0, in = cell->GetYSize(); i < in; i++) {
			for (int j = 0, jn = cell->GetXSize(); j < jn; j++) {
				Set(x + j, y + i, cell->Get(j, i));
			}
		}
	}

	void Clear(char value = ' ') {
		for (int i = 0, in = GetYSize(); i < in; i++) {
			for (int j = 0, jn = GetXSize(); j < jn; j++) {
				Set(j, i, value);
			}
		}
	}
};

//�� (3D)
class CCell3D {
	char* Cell;
	int XSize, YSize, ZSize;
public:
	CCell3D(int xsize, int ysize, int zsize)
		: XSize(xsize), YSize(ysize), ZSize(zsize)
	{
		Cell = new char[XSize*YSize*ZSize];
	}
	~CCell3D() {
		delete[] Cell;
	}
	char Get(int x, int y, int z) {
		return Cell[x + (y + z * YSize)*XSize];
	}
	void Set(int x, int y, int z, char value) {
		Cell[x + (y + z * YSize)*XSize] = value;
	}
	void Swap(int xa, int ya, int za, int xb, int yb, int zb) {
		char c = Get(xa, ya, za);
		char d = Get(xb, yb, zb);
		Set(xa, ya, za, d);
		Set(xb, yb, zb, c);
	}
	int GetXSize() {
		return XSize;
	}
	int GetYSize() {
		return YSize;
	}
	int GetZSize() {
		return ZSize;
	}
	bool Hit(int x, int y, int z, CCell3D* cell) {
		for (int i = 0, in = cell->GetZSize(); i<in; i++) {
			for (int j = 0, jn = cell->GetYSize(); j<jn; j++) {
				for (int k = 0, kn = cell->GetXSize(); k<kn; k++) {
					if (cell->Get(k, j, i) != ' ' && Get(x + k, y + j, z + i) != ' ') {
						return true;
					}
				}
			}
		}
		return false;
	}
	void Merge(int x, int y, int z, CCell3D* cell) {
		for (int i = 0, in = cell->GetZSize(); i<in; i++) {
			for (int j = 0, jn = cell->GetYSize(); j<jn; j++) {
				for (int k = 0, kn = cell->GetXSize(); k<kn; k++) {
					if (cell->Get(k, j, i) != ' ') {
						Set(x + k, y + j, z + i, cell->Get(k, j, i));
					}
				}
			}
		}
	}
	void Set(int x, int y, int z, CCell3D* cell) {
		for (int i = 0, in = cell->GetZSize(); i<in; i++) {
			for (int j = 0, jn = cell->GetYSize(); j<jn; j++) {
				for (int k = 0, kn = cell->GetXSize(); k<kn; k++) {
					Set(x + k, y + j, z + i, cell->Get(k, j, i));
				}
			}
		}
	}
	void Clear() {
		for (int i = 0, in = GetZSize(); i<in; i++) {
			for (int j = 0, jn = GetYSize(); j<jn; j++) {
				for (int k = 0, kn = GetXSize(); k<kn; k++) {
					Set(k, j, i, ' ');
				}
			}
		}
	}
};

//����
CRand Rand;

#include "Stage.h"
#include "Stage1.h"
#include "Stage2.h"
//#include "Stage3.h"
//#include "Stage4.h"
//#include "Stage5.h"
//#include "Stage6.h"

//============================================
//��� ���������� �ۼ�

void MakeStage() {

	//Stage1 �����̱�
	Game->Stage.push_back(new CMazeWalkerStage());
	Game->Stage.push_back(new CLoadPusherStage());
	Game->Stage.push_back(new CSlidingLoadPusherStage());
	Game->Stage.push_back(new CLoadPusherInGravityStage());
	Game->Stage.push_back(new CSelfDirectiveCharacterStage());
	Game->Stage.push_back(new CFollowingCursorStage());

	// Stage2 ����߸���
	Game->Stage.push_back(new CDroppingBlockStage());
	Game->Stage.push_back(new CDroppingJewelStage());
	Game->Stage.push_back(new CDroppingBallStage());
	Game->Stage.push_back(new CDroppingBall2Stage());
	Game->Stage.push_back(new CDroppingBlock3DStage());

	//// Stage3 �����ϱ�
	//Game->Stage.push_back(new CConnectedRailStage());
	//Game->Stage.push_back(new CConnectedPipeStage());
	//Game->Stage.push_back(new CLinkedShapeStage());
	//Game->Stage.push_back(new CEnclosedAreaStage());
	//Game->Stage.push_back(new CTraversableRouteStage());
	//Game->Stage.push_back(new CCrosswordStage());

	//// Stage4 ���
	//Game->Stage.push_back(new CRectangleShapedBlockStage());
	//Game->Stage.push_back(new CTransformedBlockStage());
	//Game->Stage.push_back(new CStruckBlockStage());
	//Game->Stage.push_back(new CRotatedStageStage());
	//Game->Stage.push_back(new CFoodBlockStage());
	//Game->Stage.push_back(new CSurroundingBlockStage());
	//Game->Stage.push_back(new CConnectedBlockStage());
	//Game->Stage.push_back(new CDrawnAndShotBlockStage());
	//Game->Stage.push_back(new CPushedAndCollectedBlockStage());
	//Game->Stage.push_back(new CCaughtAndPiledBlockStage());
	//Game->Stage.push_back(new CMarkedAndSunkBlockStage());

	//// Stage5 ��
	//Game->Stage.push_back(new CBallOnRailStage());
	//Game->Stage.push_back(new CHangingBallStage());
	//Game->Stage.push_back(new CCollectedBallStage());
	//Game->Stage.push_back(new CSwappedBallStage());
	//Game->Stage.push_back(new CSnakeBallStage());
	//Game->Stage.push_back(new CFlippedBallStage());
	//Game->Stage.push_back(new CRollingBallStage());

	//// Stage6 ��Ÿ
	//Game->Stage.push_back(new CMemorizingItemPositionStage());
	//Game->Stage.push_back(new CTreasureMapStage());
	//Game->Stage.push_back(new CDeliveringLoadStage());
	//Game->Stage.push_back(new CMakingFloorStage());
	//Game->Stage.push_back(new CDiceStage());
	//Game->Stage.push_back(new CBuildingStage());
}