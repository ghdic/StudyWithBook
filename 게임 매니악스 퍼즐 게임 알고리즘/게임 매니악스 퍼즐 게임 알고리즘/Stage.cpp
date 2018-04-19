#include "Main.h"

//셀을 관리하는 클래스
class CCell {
	//셀의 종류를 나타내는 배열
	char * Cell;
	//X 방향(가로 방향)의 셀의 수, Y방향(세로 방향)의 셀의 수
	int XSize, YSize;
public:
	//생성자(셀의 개수를 지정한다)
	//셀의 배열을 생성한다.
	CCell(int xsize, int ysize)
		: XSize(xsize), YSize(ysize)
	{
		Cell = new char[XSize*YSize];
	}
	//생성자(셀의 개수를 기본값으로 사용)
	//셀의 배열을 생성한다.
	CCell()
		: XSize(MAX_X), YSize(MAX_Y)
	{
		Cell = new char[XSize*YSize];
	}
	//소멸자
	//셀의 배열을 삭제한다.
	~CCell() {
		delete[] Cell;
	}

	//셀의 초기화
	//매개변수에 지정된 내용으로 셀의 배열을 초기화한다.
	void Init(char* cell) {
		for (int i = 0, n = XSize * YSize; i < n; i++)Cell[i] = cell[i];
	}

	//셀의 정보 얻기
	//지정된 좌표에 있는 셀의 종류를 얻는다.
	char Get(int x, int y) {
		if (0 <= x && x < XSize && 0 <= y && y < YSize) {
			return Cell[x + y * XSize];
		}
		else {
			return ' ';
		}
	}

	//X size 구하기
	int GetXSize() {
		return XSize;
	}

	//Y size 구하기
	int GetYSize() {
		return YSize;
	}

	//셀의 설정
	//지정된 좌표에 있는 셀의 종류를 설정한다.
	void Set(int x, int y, char value) {
		if (0 <= x && x < XSize && 0 <= y && y < YSize) {
			Cell[x + y * XSize] = value;
		}
	}

	//셀 바꿔 넣기
	//지정된 2개의 좌표에 있는 셀을 바꿔 넣는다.
	void Swap(int xa, int ya, int xb, int yb) {
		char c = Get(xa, ya);
		char d = Get(xb, yb);
		Set(xa, ya, d);
		Set(xb, yb, c);
	}

	//배열 값들과 AND연산
	void And(char value) {
		int xs = GetXSize(), ys = GetYSize();

		for (int y = 0; y < ys; y++) {
			for (int x = 0; x < xs; x++) {
				Set(x, y, Get(x, y)&value);
			}
		}
	}

	//배열 값들과 OR연산
	void Or(char value) {
		int xs = GetXSize(), ys = GetYSize();
		for (int y = 0; y < ys; y++) {
			for (int x = 0; x < xs; x++) {
				Set(x, y, Get(x, y) | value);
			}
		}
	}

	//vx, vy 만큼 이동하기
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

		for (int y = fy; y != ty; y += dy) {	//예제 오탈자? x,y구분이..?
			for (int x = fx; x != tx; x += dx) {
				Set(x, y, Get(x - vx, y - vy));
			}
		}
	}

	//	문자 c0 -> c1 바꾸기
	void Replace(char c0, char c1) {
		int xs = GetXSize(), ys = GetYSize();
		
		for (int y = 0; y < ys; y++) {
			for (int x = 0; x < xs; x++) {
				if (Get(x, y) == c0)Set(x, y, c1);
			}
		}
	}
	
	//셀의 접촉판정 처리
	//셀의 모임을 좌표에 배치했을 때
	//공백이 아닌 셀끼리 겹쳐지는지를 검사한다.
	//겹쳐지는 경우에는 true, 겹쳐지지 않는 경우에는 false를 반환한다.
	bool Hit(int x, int y, CCell* cell) {
		for (int i = 0, in = cell->GetYSize(); i < in; i++) {
			for (int j = 0, jn = cell->GetXSize(); j < jn; j++) {
				//공백이 아닌 셀끼리 한 개라도 겹치는 경우에는
				//true를 반환한다.
				if (cell->Get(j, i) != ' '&&Get(x + j, y + i) != ' ') {
					return true;
				}
			}
		}
		//공백이 아닌 셀끼리 한 개도 겹치지 않는 경우에는
		//false를 반환한다.
		return false;
	}

	//셀의 합성 처리
	//셀의 묶음을 좌표에 합성한다.
	//공백인 셀은 합성하지 않는다.
	void Merge(int x, int y, CCell* cell) {
		for (int i = 0, in = cell->GetYSize(); i < in; i++) {
			for (int j = 0, jn = cell->GetXSize(); j < jn; j++) {
				//공백이 아닌 셀만 합성한다.
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

//셀 (3D)
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

//난수
CRand Rand;

#include "Stage.h"
#include "Stage1.h"
#include "Stage2.h"
//#include "Stage3.h"
//#include "Stage4.h"
//#include "Stage5.h"
//#include "Stage6.h"

//============================================
//모든 스테이지의 작성

void MakeStage() {

	//Stage1 움직이기
	Game->Stage.push_back(new CMazeWalkerStage());
	Game->Stage.push_back(new CLoadPusherStage());
	Game->Stage.push_back(new CSlidingLoadPusherStage());
	Game->Stage.push_back(new CLoadPusherInGravityStage());
	Game->Stage.push_back(new CSelfDirectiveCharacterStage());
	Game->Stage.push_back(new CFollowingCursorStage());

	// Stage2 떨어뜨리기
	Game->Stage.push_back(new CDroppingBlockStage());
	Game->Stage.push_back(new CDroppingJewelStage());
	Game->Stage.push_back(new CDroppingBallStage());
	Game->Stage.push_back(new CDroppingBall2Stage());
	Game->Stage.push_back(new CDroppingBlock3DStage());

	//// Stage3 연결하기
	//Game->Stage.push_back(new CConnectedRailStage());
	//Game->Stage.push_back(new CConnectedPipeStage());
	//Game->Stage.push_back(new CLinkedShapeStage());
	//Game->Stage.push_back(new CEnclosedAreaStage());
	//Game->Stage.push_back(new CTraversableRouteStage());
	//Game->Stage.push_back(new CCrosswordStage());

	//// Stage4 블록
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

	//// Stage5 볼
	//Game->Stage.push_back(new CBallOnRailStage());
	//Game->Stage.push_back(new CHangingBallStage());
	//Game->Stage.push_back(new CCollectedBallStage());
	//Game->Stage.push_back(new CSwappedBallStage());
	//Game->Stage.push_back(new CSnakeBallStage());
	//Game->Stage.push_back(new CFlippedBallStage());
	//Game->Stage.push_back(new CRollingBallStage());

	//// Stage6 기타
	//Game->Stage.push_back(new CMemorizingItemPositionStage());
	//Game->Stage.push_back(new CTreasureMapStage());
	//Game->Stage.push_back(new CDeliveringLoadStage());
	//Game->Stage.push_back(new CMakingFloorStage());
	//Game->Stage.push_back(new CDiceStage());
	//Game->Stage.push_back(new CBuildingStage());
}