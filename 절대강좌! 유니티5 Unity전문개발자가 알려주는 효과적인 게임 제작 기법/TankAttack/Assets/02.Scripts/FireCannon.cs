using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FireCannon : MonoBehaviour {

    //cannon 프리팹을 연결할 변수
    public GameObject cannon = null;
    //폭타 발사 사운드 파일
    private AudioClip fireSfx = null;
    //AudioSource 컴포넌트를 할당할 변수
    private AudioSource sfx = null;
    //cannon 발사 지점
    public Transform firePos;
    //PhotonView 컴포너트를 할당할 변수
    private PhotonView pv = null;

    private void Awake()
    {
        //cannon 프리팹을 Resources 폴더에서 불러와 변수에 할당
        cannon = (GameObject)Resources.Load("Cannon");
        //폭탄 발사 사운드 파일을 Resources 폴더에서 불러와 변수에 할당
        fireSfx = Resources.Load<AudioClip>("CannonFire");
        //AudioSource 컴포넌트를 할당
        sfx = GetComponent<AudioSource>();
        //PhotonView 컴포넌트를 pv 변수에 할당
        pv = GetComponent<PhotonView>();
    }

    private void Update()
    {
        //현재 마우스 커서가 Canvas UI 항목 위에 있으면 Updatae 함수를 빠져나감
        if (MouseHover.instance.isUIHover) return;

        //마우스 왼쪽 버튼 클릭 시 발사 로직 수행
        if (pv.isMine && Input.GetMouseButtonDown(0))
        {
            //원격 네트워크 플레이어의 탱크에 RPC로 원격으로 Fire함수를 호출
            pv.RPC("Fire", PhotonTargets.All, null);
        }
    }

    [PunRPC] //RPC로 호출할 함수는 반드시 어트리뷰트를 함수 앞에 명기해야 한다.
    void Fire()
    {
        //발사 사운드 발생
        sfx.PlayOneShot(fireSfx, 1.0f);
        GameObject _cannon = (GameObject)Instantiate(cannon, firePos.position, firePos.rotation);
        _cannon.GetComponent<Cannon>().playerId = pv.ownerId;   //누가 쏜걸지 id값으로 식별한다.
    }
}
