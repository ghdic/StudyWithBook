using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameMgr : MonoBehaviour {
    //몬스터가 출현할 위치를 담을 배열
    public Transform[] points;
    //몬스터 프리팹을 할당할 변수
    public GameObject monsterPrefab;

    //몬스터를 발생시킬 주기
    public float createTime = 2.0f;
    //몬스터의 최대 발생 개수
    public int maxMonster = 10;
    //게임 종료 여부 변수
    public bool isGameOver = false;
    //싱글턴 패턴을 위한 인스턴스 변수 선언
    public static GameMgr instance = null;

    private void Awake()
    {
        //GameMgr 클래스를 인스터스에 대입
        instance = this;
    }

    private void Start()
    {
        //Hierarchy 뷰의 SpawnPoint를 찾아 하위에 있는 모든 Transform 컴포넌트를 찾아옴
        points = GameObject.Find("SpawnPoint").GetComponentsInChildren<Transform>();

        if (points.Length > 0)
        {
            //몬스터 생성 코루틴 함수 호출
            StartCoroutine(this.CreateMonster());
        }
    }

    //몬스터 생성 코루틴 함수
    IEnumerator CreateMonster()
    {
        //게임 종료 시까지 무한 루프
        while (!isGameOver)
        {
            //현재 생성된 몬스터 개수 산출
            int monsterCount = (int)GameObject.FindGameObjectsWithTag("MONSTER").Length;
            //몬스터의 최대 생성 개수보다 작을 때만 몬스터 생성
            if (monsterCount < maxMonster)
            {
                //몬스터 생성 주기 시간만큼 대기
                yield return new WaitForSeconds(createTime);

                //불규칙적인 위치 산출
                int idx = Random.Range(1, points.Length);
                //몬스터의 동적 생성
                Instantiate(monsterPrefab, points[idx].position, points[idx].rotation);
            }
            else
            {
                yield return null;
            }
        }
    }
}
