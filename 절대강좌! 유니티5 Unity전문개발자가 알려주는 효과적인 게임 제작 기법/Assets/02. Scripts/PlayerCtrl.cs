using UnityEngine;
using System.Collections;

public class PlayerCtrl : MonoBehaviour
{
    private float h = 0.0f;
    private float v = 0.0f;

    //접근해야 하는 컴포넌트는 반드시 변수에 할당한 후 사용
    private Transform tr;
    //이동 속도 변수(public 선언시 Inspector에 노출되 수정 가능해짐)
    public float moveSpeed = 10.0f;

    private void Start()
    {
        //스크립트 처음에 Transform 컴포넌트 할당
        //Update 함수에서 접근해야 할 컴포넌트는 Awake, Start 함수에서 미리 변수에 할당한 후 Update 함수에서 사용
        tr = GetComponent<Transform>();
        //다양한 문법으로 같은 의미 표시, 모두 this 지시어가 생략되어 있다. 위는 gameObject까지 생략
        //tr = gameObject.GetComponent<Transform>();
        //tr = (Transform)GetComponent("Transform");
        //tr = (Transform)GetComponent(typeof(Transform));
    }

    private void Update()
    {
        //Editor -> Project Settings -> Input에 Name으로 되있는 값이 인자, 키입력값 받아들임
        //GetAxisRaw사용시 -1, 0, 1 꼴로 반환한다.
        h = Input.GetAxis("Horizontal");//A, D, Left, Right를 눌렀을때 -1 ~+1 값을 반환한다.
        v = Input.GetAxis("Vertical");//W, D, Up, Down 을 눌렀을 때 -1 ~ +1 값을 반환한다.
        //Console View에 텍스트형식으로 output
        Debug.Log("H=" + h.ToString());
        Debug.Log("V=" + v.ToString());

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
    }
}