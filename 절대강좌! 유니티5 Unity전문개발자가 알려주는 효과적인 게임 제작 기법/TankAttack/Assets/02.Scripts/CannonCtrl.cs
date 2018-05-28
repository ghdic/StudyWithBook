using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CannonCtrl : MonoBehaviour {

    private Transform tr;
    //PhotonView 컴포넌트 변수
    private PhotonView pv = null;
    //원격 네트워크 탱크의 포신 회전 각도를 저장할 변수
    private Quaternion currRot = Quaternion.identity;

    public float rotSpeed = 100.0f;

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
        if (pv.isMine)
        {
            float angle = Input.GetAxis("Mouse ScrollWheel") * Time.deltaTime * rotSpeed * 5.0f;
            tr.Rotate(angle, 0, 0);
        }
        else
        {
            //현재 회전 각도에서 수신받은 실시간 회전 각도로 부드럽게 회전
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
