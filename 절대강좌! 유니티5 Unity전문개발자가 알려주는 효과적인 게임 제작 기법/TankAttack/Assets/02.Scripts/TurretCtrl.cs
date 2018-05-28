using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TurretCtrl : MonoBehaviour {

    private Transform tr;
    //광선이 지면에 맞는 위치를 저장할 변수
    private RaycastHit hit;

    //터렛의 회전 속도
    public float rotSpeed = 5.0f;

    //PhotonView 컴포넌트 변수
    private PhotonView pv = null;
    //원격 네트워크 탱크의 터렛 회전값을 저장할 변수
    private Quaternion currRot = Quaternion.identity;

    private void Awake()
    {
        tr = GetComponent<Transform>();
        pv = GetComponent<PhotonView>();

        //Photon View의 Observed 속성을 이 스크립트로 지정
        pv.ObservedComponents[0] = this;
        //Photon View의 동기화 속성을 설정
        pv.synchronization = ViewSynchronization.UnreliableOnChange;

        //초기 회전값 설정
        currRot = tr.localRotation;
    }

    private void Update()
    {
        if (pv.isMine)//자신의 탱크일 때만 조정
        {
            //메인 카메라에서 마우스 커서의 위치로 캐스팅되는 Ray를 생성
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            //생성된 Ray를 Scene 뷰에서 녹색 광선으로 표현
            Debug.DrawRay(ray.origin, ray.direction * 100.0f, Color.green);

            if (Physics.Raycast(ray, out hit, Mathf.Infinity, 1 << 8))   //8번레이어만 검출
            {
                //Ray에 맞은 위치를 로컬좌표로 변환
                Vector3 relative = tr.InverseTransformPoint(hit.point);
                //역탄젠트 함수인 Atan2로 두 점 간의 각도를 계산
                float angle = Mathf.Atan2(relative.x, relative.z) * Mathf.Rad2Deg;
                //rotSpeed 변수에 지정된 속도로 회전
                tr.Rotate(0, angle * Time.deltaTime * rotSpeed, 0);
            }
        }
        else //원격 네트워크 플레이어의 탱크일 경우
        {
            //현재 회전각도에서 수신받은 실시간 회전각도로 부드럽게 회전
            tr.localRotation = Quaternion.Slerp(tr.localRotation, currRot, Time.deltaTime * 3.0f);
        }
    }

    //송수신 콜백 함수
    void OnPhotonSerializeView(PhotonStream stream, PhotonMessageInfo info)
    {
        if (stream.isWriting)
        {
            stream.SendNext(tr.localRotation);
        }
        else
        {
            currRot = (Quaternion)stream.ReceiveNext();
        }
    }
}
