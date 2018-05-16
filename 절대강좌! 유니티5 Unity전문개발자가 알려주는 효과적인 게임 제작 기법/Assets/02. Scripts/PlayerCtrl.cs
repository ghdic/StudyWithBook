using UnityEngine;
using System.Collections;
using UnityEngine.UI;

//클래스에 System.Serializable이라는 Attribute를 명시해야
//Inspector 뷰에 노출됨
[System.Serializable]
public class Anim
{
    public AnimationClip idle;
    public AnimationClip runForward;
    public AnimationClip runBackward;
    public AnimationClip runRight;
    public AnimationClip runLeft;
}

public class PlayerCtrl : MonoBehaviour
{
    private float h = 0.0f;
    private float v = 0.0f;

    //접근해야 하는 컴포넌트는 반드시 변수에 할당한 후 사용
    private Transform tr;
    //이동 속도 변수(public 선언시 Inspector에 노출되 수정 가능해짐)
    public float moveSpeed = 10.0f;

    //회전 속도 변수
    public float rotSpeed = 100.0f;

    //인스펙터뷰에 표시할 애니메이션 클래스 변수
    public Anim anim;

    //아래에 있는 3D 모델의 Animation 컴포넌트에 접근하기 위한 변수
    public Animation _animation;

    //Player의 생명 변수
    public int hp = 100;
    //Player의 생명 초깃값
    private int initHp;
    //Player의 Health bar 이미지
    public Image imgHpbar;
    //델리게이트 및 이벤트 선언
    public delegate void PlayerDieHanler();
    public static event PlayerDieHanler OnPlayerDie;
    //게임 매니저에 접근하기 위한 변수
    private GameMgr gameMgr;
    private void Start()
    {
        //생명 초깃값 설정
        initHp = hp;
        //스크립트 처음에 Transform 컴포넌트 할당
        //Update 함수에서 접근해야 할 컴포넌트는 Awake, Start 함수에서 미리 변수에 할당한 후 Update 함수에서 사용
        tr = GetComponent<Transform>();
        //다양한 문법으로 같은 의미 표시, 모두 this 지시어가 생략되어 있다. 위는 gameObject까지 생략
        //tr = gameObject.GetComponent<Transform>();
        //tr = (Transform)GetComponent("Transform");
        //tr = (Transform)GetComponent(typeof(Transform));

        //자신의 하위에 있는 Animation 컴포넌트를 찾아와 변수에 할당
        _animation = GetComponentInChildren<Animation>();

        //Animation 컴포넌트의 애니메이션 클립을 지정하고 실행
        _animation.clip = anim.idle;
        _animation.Play();
        //GameMgr 스크립트 할당
        //gameMgr = GameObject.Find("GameManager").GetComponent<GameMgr>();
    }

    private void Update()
    {
        //Editor -> Project Settings -> Input에 Name으로 되있는 값이 인자, 키입력값 받아들임
        //GetAxisRaw사용시 -1, 0, 1 꼴로 반환한다.
        h = Input.GetAxis("Horizontal");//A, D, Left, Right를 눌렀을때 -1 ~+1 값을 반환한다.
        v = Input.GetAxis("Vertical");//W, D, Up, Down 을 눌렀을 때 -1 ~ +1 값을 반환한다.
        //Console View에 텍스트형식으로 output
        //Debug.Log("H=" + h.ToString());
        //Debug.Log("V=" + v.ToString());

        //전후좌우 이동 방향 벡터 계산
        Vector3 moveDir = (Vector3.forward * v) + (Vector3.right * h);

        //Transloate(이동 방향 * 속도 * 변위값 * Time.deltaTime, 기준좌표)
        //Time.deltaTime를 곱하는것은 매 초마다 moveSpeed만큼 움직이게 해준다.(안할경우 매 프레임마다)
        //벡터의 방향 성분만 추출하기 위해 정규화 벡터로 변경하는 normalized 속성 이용
        //대각선으로 갈때 1.4xxx인데 이럴 경우 상하좌우로 가는것보다 속도가 더 빨라지기때문에 속도를 vector의 크기를 1로 정규화 표현한다.
        tr.Translate(moveDir.normalized * moveSpeed * Time.deltaTime, Space.Self);
        //게임 오브젝트의 이동
        //Transform으로 포지션 이동 가능
        //transform.position += new Vector3(0, 0, 1);
        //Translate라는 더 편한게 있음! 위 소스와 동일하게 동작
        //tr.Translate(Vector3.forward);

        //vector3.up 축을 기준으로 rotSpeed만큼의 속도로 회전
        //마우스 좌우로 하면 Rotate 함
        tr.Rotate(Vector3.up * Time.deltaTime * rotSpeed * Input.GetAxis("Mouse X"));

        //키보드 입력값을 기준으로 동작할 애니메이션 수행
        //CrossFade 부드럽게 애니메이션을 바꿔준다. 0.3f는 페이드아웃 시간으로 0.3초내에 애니메이션이 변경된다.
        if (v >= 0.1f)
        {
            //전진 애니메이션
            _animation.CrossFade(anim.runForward.name, 0.3f);
        }
        else if (v <= -0.1f)
        {
            //후진 애니메이션
            _animation.CrossFade(anim.runBackward.name, 0.3f);
        }
        else if (h >= 0.1f)
        {
            //오른쪽 이동 애니메이션
            _animation.CrossFade(anim.runRight.name, 0.3f);
        }
        else if (h <= -0.1f)
        {
            //왼쪽 이동 애니메이션
            _animation.CrossFade(anim.runLeft.name, 0.3f);
        }
        else
        {
            //정지시 idle 애니메이션
            _animation.CrossFade(anim.idle.name, 0.3f);
        }
    }

    //충돌한 Collider의 IsTrigger 옵션이 체크됐을 때 발생
    private void OnTriggerEnter(Collider coll)
    {
        if(coll.gameObject.tag == "WALL")
        {
            tr.position = this.transform.position;
        }
        
        //충돌한 Collider가 몬스터의 PUNCH이면 Player의 HP 차감
        if(coll.gameObject.tag == "PUNCH")
        {
            hp -= 10;
            //Image UI 항목 fillAmount 속성을 조절해 생명 게이지 값 조절
            imgHpbar.fillAmount = (float)hp / (float)initHp;

            Debug.Log("Player HP = " + hp.ToString());

            //Player의 생명이 0이하이면 사망 처리
            if (hp <= 0)
            {
                PlayerDie();
            }
        }
    }

    void PlayerDie()
    {
        Debug.Log("Player Die !!");

        ////MONSTER라는 Tag를 가진 모든 게임오브젝트를 찾아옴
        //GameObject[] monsters = GameObject.FindGameObjectsWithTag("MONSTER");

        ////모든 몬스터의 OnPlayerDie 함수를 순차적으로 호출
        //foreach(GameObject monster in monsters)
        //{
        //    monster.SendMessage("OnPlayerDie", SendMessageOptions.DontRequireReceiver);
        //}

        //이벤트 발생시킴
        OnPlayerDie();
        //GameMgr의 싱글턴 인스턴스를 접근해 isGameOver 변숫값을 변경해 몬스텨 출현을 중지시킴
        GameMgr.instance.isGameOver = true;
    }
}