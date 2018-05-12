using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class MonsterCtrl : MonoBehaviour {

    //몬스터의 상태 정보가 있는 Enumerable 변수 선언
    public enum MonsterState { idle, trace, attack, die };
    //몬스터의 현재 상태 정보를 저장할 Enum 변수
    public MonsterState monsterState = MonsterState.idle;

    //속도 향상을 위해 각종 컴포넌트를 변수에 할당
    private Transform monsterTr;
    private Transform playerTr;
    private NavMeshAgent nvAgent;

    //추적 사정거리
    public float traceDist = 10.0f;
    //공격 사정거리
    public float attackDist = 2.0f;

    //몬스터 사망 여부
    private bool isDie = false;

    private void Start()
    {
        //몬스터의 Transform 할당
        monsterTr = this.gameObject.GetComponent<Transform>();
        //추적 대상인 Player의 Transform 할당
        playerTr = GameObject.FindWithTag("Player").GetComponent<Transform>();
        //NavMeshAgent 컴포넌트 할당
        nvAgent = this.gameObject.GetComponent<NavMeshAgent>();

        //추적 대상의 위치를 설정하면 바로 추적 시작
        //nvAgent.destination = playerTr.position;

        //일정한 간격으로 몬스터의 행동 상태를 체크하는 코루틴 함수 실행
        StartCoroutine(this.CheckMonsterState());

        //몬스터의 상태에 따라 동작하는 루틴을 실행하는 코루틴 함수 실행
        StartCoroutine(this.MonsterAction());
    }

    //일정한 간격으로 몬스터의 행동 상태를 체크하고 monsterState 값 변경
    IEnumerator CheckMonsterState()
    {
        while (!isDie)
        {
            //0.2초 동안 기다렸ㄷ가 다음으로 넘어감
            yield return new WaitForSeconds(0.2f);
        }

        //몬스터와 플레이어 사이의 거리 측정
        float dist = Vector3.Distance(playerTr.position, monsterTr.position);

        if(dist<=attackDist) // 공격거리 범위 이내로 들어왔는지 확인
        {
            monsterState = MonsterState.attack;
        }
        else if (dist <= traceDist) //추적거리 범위 이내로 들어왔는지 확인
        {
            monsterState = MonsterState.trace;
        }
        else
        {
            monsterState = MonsterState.idle; //몬스터의 상태를 idle 모드로 설정
        }
    }

    //몬스터의 상태값에 따라 적절한 동작을 수행하는 함수
    IEnumerator MonsterAction()
    {
        while (!isDie)
        {
            switch (monsterState)
            {
                case MonsterState.idle:
                    //추적 중지
                    nvAgent.isStopped = true;
                    break;

                case MonsterState.trace:
                    //추적 대상의 위치를 넘겨줌
                    nvAgent.destination = playerTr.position;
                    //추적 재시작
                    nvAgent.isStopped = false;
                    break;
                case MonsterState.attack:
                    break;
            }
            yield return null;
        }
    }
}
