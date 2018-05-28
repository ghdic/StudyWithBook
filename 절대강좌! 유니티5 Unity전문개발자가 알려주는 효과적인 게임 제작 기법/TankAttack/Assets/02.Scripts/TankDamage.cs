using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class TankDamage : MonoBehaviour {

    //탱크 폭파 후 투명 처리를 위한 MeshRenderer 컴포넌트 배열
    private MeshRenderer[] renderers;

    //탱크 폭발 효과 프리팹을 연결할 변수
    private GameObject expEffect = null;

    //탱크의 초기 생명치
    private int initHp = 100;
    //탱크의 현재 생명치
    private int currHp = 0;

    //탱크 하위의 Canvas 객체를 연결할 변수
    public Canvas hudCanvas;
    //Filled 타입의 Image UI 항목을 연결할 변ㅅ
    public Image hpBar;

    //PhotonViwe 컴포넌트를 할당할 변수
    private PhotonView pv = null;
    //플레이어 id를 저장하는 변수
    public int playerId = -1;
    //적 탱크 파괴 스코어를 저장하는 변수
    public int killCount = 0;
    //탱크 HUD에 표현할 스코어 Text UI 항목
    public Text txtKillCount;

    private void Awake()
    {
        //탱크 모델의 모든 MeshRenderer 컴포넌트를 추출한 후 배열에 할당
        renderers = GetComponentsInChildren<MeshRenderer>();

        //탱크의 초기 생명치
        currHp = initHp;
        //탱크 폭발 시 생성시킬 폭발 효과를 로드
        expEffect = Resources.Load<GameObject>("Large Explosion");

        //Filled 이미지 색상을 녹색으로 설정
        hpBar.color = Color.green;

        //PhotonView 컴포넌트 할당
        pv = GetComponent<PhotonView>();
        //PhotonView의 ownerId를 PlayerId에 저장
        playerId = pv.ownerId;
    }

    void OnTriggerEnter(Collider coll)
    {
        //충돌할 Collider의 태그 비교 #coll.collider.tag 가 안됨..
        if(currHp > 0 && coll.gameObject.tag == "CANNON")
        {

            Debug.Log("Oh No!");
            currHp -= 20;

            //현재 생명치 백분율 = 현재 생명치 / 초기생명치
            hpBar.fillAmount = (float)currHp / (float)initHp;

            //생명 수치에 따라 Filled 이미지의 색상을 변경
            if (hpBar.fillAmount <= 0.4f)
                hpBar.color = Color.red;
            else if (hpBar.fillAmount <= 0.6f)
                hpBar.color = Color.yellow;

            if (currHp <= 0)
            {
                //자시을 파괴시킨 적 탱크의 스코어를 증가시키는 함수를 호출
                SaveKillCount(coll.GetComponent<Cannon>().playerId);
                StartCoroutine(this.ExploisionTank());
            }
        }
    }

    //폭발 효과 생성 및 리스폰 코루틴 함수
    IEnumerator ExploisionTank()
    {
        //폭발효과 생성
        Object effect = GameObject.Instantiate(expEffect, transform.position, Quaternion.identity);
        Destroy(effect, 3.0f);

        //HUD를 비활성화
        hudCanvas.enabled = false;

        //탱크 투명 처리
        SetTankVisible(false);
        //3초 동안 기다렸다가 활성화하는 로직을 수행
        yield return new WaitForSeconds(3.0f);

        //Filled 이미지 초깃값으로 환원
        hpBar.fillAmount = 1.0f;
        //Filled 이미지 색상을 녹색으로 설정
        hpBar.color = Color.green;
        //HUD 활성화
        hudCanvas.enabled = true;

        //리스폰 시 생명 초깃값 설정
        currHp = initHp;
        //탱크를 다시 보이게 처리
        SetTankVisible(true);
    }

    //MeshRenderer를 활성/비활성화 하는 함수
    void SetTankVisible(bool isVisible)
    {
        foreach(MeshRenderer _renderer in renderers)
        {
            _renderer.enabled = isVisible;
        }
    }

    //자신을 파괴시킨 적 탱크를 검색해 스코어를 증가시키는 함수
    void SaveKillCount(int firePlayerId)
    {
        //TANK 태그로 지정된 모든 탱크를 가져와 배열에 저장
        GameObject[] tanks = GameObject.FindGameObjectsWithTag("TANK");

        foreach(GameObject tank in tanks)
        {
            var tankDamage = tank.GetComponent<TankDamage>();
            //탱크의 playerId가 포탄의 playerId와 동일한지 판단
            if(tankDamage != null && tankDamage.playerId == firePlayerId)
            {
                //동일한 탱크일 경우 스코어를 증가시킴
                tankDamage.IncKillCount();
                break;
            }
        }
    }

    void IncKillCount()
    {
        //탱크 파괴 스코어 증가
        ++killCount;
        //HUD Text UI 항목에 스코어 표시
        txtKillCount.text = killCount.ToString();

        //스코어를 증가시킨 탱크가 자신인 경우에만 저장
        if (pv.isMine)
        {
            PhotonNetwork.player.AddScore(1);
            //뒤늦게 들어온 유저들도 볼수있는 스코어
        }
    }
}