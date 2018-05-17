using System.Collections;
using UnityEngine;

[RequireComponent(typeof(AudioSource))]
public class FireCtrl : MonoBehaviour {

    //총알 프리팹
    public GameObject bullet;
    //총알 발사좌표
    public Transform firePos;
    //총알 발사 사운드
    public AudioClip fireSfx;
    //AudioSource 컴포넌트를 저장할 변수
    private AudioSource source = null;
    //MuzzleFlash의 MeshRenderer 컴포넌트 연결 변수
    public MeshRenderer muzzleFlash;

    private void Start()
    {
        //AudioSource 컴포넌트를 추출한 후 변수에 할당
        source = GetComponent<AudioSource>();
        //최초에 MuzzleFlash MeshRenderer를 비활성화
        muzzleFlash.enabled = false;
    }
    private void Update()
    {
        //Ray를 시각적으로 표시하기 위해 사용
        Debug.DrawRay(firePos.position, firePos.forward * 10.0f, Color.green);
        //마우스 왼쪽 버튼을 클릭했을 때 Fire 함수 호출
        //GetMouseButton 마우스 버튼 클릭하고 있을때
        //GetMouseButton up down 마우스 버튼 누르거나 땔때
        //인자값은 0 왼쪽 1 오른쪽 2 가운데 버튼을 나타낸다.
        if (Input.GetMouseButtonDown(0))
        {
            Fire();

            //Ray에 맞은 게임오브젝트의 정보를 받아올 변수
            RaycastHit hit;

            //Raycast 함수로 Ray를 발사해 맞은 게임오브젝트가 있을때 true를 반환
            if (Physics.Raycast(firePos.position, firePos.forward, out hit, 10.0f))
            {
                //Ray에 맞은 게임오브젝트의 Tag값을 비교해 몬스터 여부 체크
                if(hit.collider.tag == "MONSTER")
                {
                    //SenMessage를 이용해 전달한 인자를 배열에 담음
                    object[] _params = new object[2];
                    _params[0] = hit.point;//Ray에 맞은 정확한 위치값(Vector3)
                    _params[1] = 20;    //몬스터에 입힐 데미지값;
                    //몬스터에 데미지 입히는 함수 호출
                    hit.collider.gameObject.SendMessage("OnDamage", _params, SendMessageOptions.DontRequireReceiver);
                }

                //Ray에 맞은 게임오브젝트의 Tag값을 비교해 Barrel인지 확인
                if (hit.collider.tag == "BARREL")
                {
                    //SenMessage를 이용해 전달한 인자를 배열에 담음 입사각 계산!
                    object[] _params = new object[2];
                    _params[0] = firePos.position;
                    _params[1] = hit.point;
                    
                    hit.collider.gameObject.SendMessage("OnDamage", _params, SendMessageOptions.DontRequireReceiver);
                }
            }
        }
    }
    void Fire()
    {
        //동적으로 총알을 생성하는 함수
        CreateBullet();
        //사운드 발생 함수 (오디오클립, 볼륨)
        //source.PlayOneShot(fireSfx, 0.9f);
        GameMgr.instance.PlaySfx(firePos.position, fireSfx);
        //잠시 기다리는 루틴을 위해 코루틴 함수로 호출
        StartCoroutine(this.ShowMuzzleFlash());
    }

    void CreateBullet()
    {
        //Bullet 프리팹을 동적으로 생성
        Instantiate(bullet, firePos.position, firePos.rotation);
    }

    //MuzzleFlash 활성/비활성화를 짧은 시간 동안 반복
    IEnumerator ShowMuzzleFlash()
    {
        //MuzzlerFlash 스케일을 불규칙하게 변경
        float scale = Random.Range(1.0f, 2.0f);
        muzzleFlash.transform.localScale = Vector3.one * scale;

        //MuzzleFlash를 Z축 기준으로 불규칙하게 회전 시킴
        Quaternion rot = Quaternion.Euler(0, 0, Random.Range(0, 360));
        muzzleFlash.transform.localRotation = rot;

        //활성화해서 보이게 함
        muzzleFlash.enabled = true;

        //불규칙적인 시간 동안 Delay한 다음 MeshRenderer를 비활성화
        yield return new WaitForSeconds(Random.Range(0.05f, 0.3f));

        //비활성화해서 보이지 않게 함
        muzzleFlash.enabled = false;
    }
}
