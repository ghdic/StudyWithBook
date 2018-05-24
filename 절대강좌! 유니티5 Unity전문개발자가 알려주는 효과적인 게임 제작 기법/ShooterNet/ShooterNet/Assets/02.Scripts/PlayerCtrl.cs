using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//SmoothFollow 스크립트를 사용하기 위해 네임스페이스를 선언
using UnityStandardAssets.Utility;

public class PlayerCtrl : MonoBehaviour {
    //애니메이션 상태
    public enum AnimState
    {
        idle=0 //애니메이션 배열의 Index에 맞게 0 부터 시작
        ,runForward
            ,runBackward
            ,runRight
            ,runLeft
    }
    //애니메이션 상태를 저장하는 변수
    public AnimState animState = AnimState.idle;
    //사용할 애니메이션 클립 배열
    public AnimationClip[] animClips;

    //CharacterController 컴포넌트를 할당할 변수
    private CharacterController controller;
    //하위의 Animation 컴포넌트를 할당할 변수
    private Animation anim;

    //Transform 컴포넌트를 할당할 변수
    private Transform tr;
    //NetworkView 컴포넌트를 할당할 변수
    private NetworkView _networkView;

    //위치 정보를 송수신할 때 사용할 변수 선언 및 초깃값 설정
    private Vector3 currPos = Vector3.zero;
    private Quaternion currRot = Quaternion.identity;

    //Bullet 프리팹 할당
    public GameObject bullet;
    //총알 발사 위치
    public Transform firePos;

    //사망 여부를 나타내는 변수
    private bool isDie = false;
    //플레이어 생명치
    private int hp = 100;
    //부활 시간
    private float respawnTime = 3.0f;

    private void Awake()
    {
        //접근할 컴포넌트를 할당한다.
        tr = GetComponent<Transform>();
        _networkView = GetComponent<NetworkView>();
        _networkView.observed = this; //자신의 스크립트를 Observed 속성에 연결
        controller = GetComponent<CharacterController>();
        anim = GetComponentInChildren<Animation>();

        //NetworkView가 자신의 것인지 확인한다.
        if (_networkView.isMine)
        {
            //Main Camera가 추적해야 할 대상을 설정한다.
            Camera.main.GetComponent<SmoothFollow>().target = tr;
        }
    }

    private void Update()
    {
        if (_networkView.isMine)
        {
            if (Input.GetMouseButtonDown(0))
            {
                //사망 했을 때 발사 로직 및 이동 로직을 수행하지 않고 빠져나감
                if (isDie) return;
                
                //자신은 로컬 함수를 호출해 발사
                Fire();
                //자신을 제외한 나머지 원격 사요자에게 Fire 함수를 원격 호출
                _networkView.RPC("Fire", RPCMode.Others);
            }

            //CharacterController의 속도벡터를 로컬벡터로 변환
            Vector3 localVelocity = tr.InverseTransformDirection(controller.velocity);
            //전진후진 방향의 가속도
            Vector3 forwardDir = new Vector3(0f, 0f, localVelocity.z);
            //좌우방향의 가속도
            Vector3 rightDir = new Vector3(localVelocity.x, 0f, 0f);

            //전후진, 좌우 방향에 따라 애니메이션 상태 설정
            if(forwardDir.z >= 0.1f)
            {
                animState = AnimState.runForward;
            }else if (forwardDir.z <= -1.0f)
            {
                animState = AnimState.runBackward;
            }else if(rightDir.x >= 0.1f)
            {
                animState = AnimState.runRight;
            }else if(rightDir.x <= -0.1f)
            {
                animState = AnimState.runLeft;
            }
            else
            {
                animState = AnimState.idle;
            }
            //애니메이션 실행
            anim.CrossFade(animClips[(int)animState].name, 0.2f);
        }
        else//원격 플레이어 일때 수행
        {
            //현재 좌표와 전송받은 새로운 좌표 간의 거리차가 크다면 바로 이동
            if (Vector3.Distance(tr.position, currPos) >= 2.0f)
            {
                tr.position = currPos;
                tr.rotation = currRot;
            }
            else
            {
                //전송 받아온 변경된 위치로 부드럽게 이동
                tr.position = Vector3.Lerp(tr.position, currPos, Time.deltaTime * 10.0f);
                //전송 받아온 변경된 각도로 부드럽게 회전
                tr.rotation = Quaternion.Slerp(tr.rotation, currRot, Time.deltaTime * 10.0f);
            }
        }
    }

    //RPC 함수 지정을 위해 반드시 [RPC] 어트리뷰트를 명시
    [RPC]
    void Fire()
    {
        GameObject.Instantiate(bullet, firePos.position, firePos.rotation);
    }

    //NetworkVeiw 컴포넌트에서 호출해 주는 콜백 함수
    private void OnSerializeNetworkView(BitStream stream, NetworkMessageInfo info)
    {
        //로컬 플레이어의 위치 및 회전 정보 송신
        if (stream.isWriting)
        {
            Vector3 pos = tr.position;
            Quaternion rot = tr.rotation;
            int _animState = (int)animState;

            //데이터 전송
            stream.Serialize(ref pos);
            stream.Serialize(ref rot);
            stream.Serialize(ref _animState);
        }
        else //원격 플레이어의 위치 및 회전 정보 수신
        {
            Vector3 revPos = Vector3.zero;
            Quaternion revRot = Quaternion.identity;
            int _animState = 0;

            //데이터 수신
            stream.Serialize(ref revPos);
            stream.Serialize(ref revRot);
            stream.Serialize(ref _animState);

            currPos = revPos;
            currRot = revRot;
            animState = (AnimState)_animState;
        }
    }

    //총알의 충돌 체크
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.tag == "BULLET")
        {
            Destroy(other.gameObject);
            //플레이어의 생명치를 차감
            hp -= 20;

            //생명치가 0 이하일 때 사망 및 Respawn 코루틴 함수 호출
            if (hp <= 0)
            {
                StartCoroutine(this.RespawnPlayer(respawnTime));
            }
        }
    }

    //사망 처리 및 Respawn 처리
    IEnumerator RespawnPlayer(float waitTime)
    {
        isDie = true;
        //플레이어의 Mesh Renderer를 비활성화하는 코루틴 함수 호출
        StartCoroutine(this.PlayerVisible(false, 0.0f));

        //Respawn 시간까지 기다림
        yield return new WaitForSeconds(waitTime);

        //Respawn 시간이 지난 후 플레이어의 위치를 무작위로 산출
        tr.position = new Vector3(Random.Range(-20.0f, 20.0f), 0.0f, Random.Range(-20.0f, 20.0f));
        //생명치를 초깃값으로 재설정
        hp = 100;

        //플레이어를 컨트롤 할 수 있게 변수 설정
        isDie = false;
        //플레이어의 Mesh Renderer 활성화
        StartCoroutine(this.PlayerVisible(true, 0.5f));
    }

    //플레이어의 Mesh Renderer와 Character Controller의 활성/비활성 처리
    IEnumerator PlayerVisible(bool visibled, float delayTime)
    {
        yield return new WaitForSeconds(delayTime);
        //플레이어의 바디의 Skinned Mesh Renderer 활성/비활성화
        GetComponentInChildren<SkinnedMeshRenderer>().enabled = visibled;
        //플레이어의 Weapon의 Mesh Renderer 활성/비활성화
        GetComponentInChildren<MeshRenderer>().enabled = visibled;

        //키보드 움직임에 반응하지 않게 MoveCtrl과 Charactor Controller를 활성/비활성화
        if (_networkView.isMine)
        {
            GetComponent<MoveCtrl>().enabled = visibled;
            controller.enabled = visibled;
        }
    }
}
