using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveCtrl : MonoBehaviour {

    //컴포넌트를 할당할 변수 선언
    private Transform tr;
    private CharacterController controller;

    //키보드 입력값 변수 선언
    private float h = 0.0f;
    private float v = 0.0f;

    //이동 속도, 회전 속도 변수
    public float movSpeed = 5.0f;
    public float rotSpeed = 50.0f;

    //이동할 방향 벡터 변수
    private Vector3 movDir = Vector3.zero;

    private void Start()
    {
        //원격 접속한 네트워크 유저일 경우 이 스크립트를 비활성화
        this.enabled = GetComponent<NetworkView>().isMine;

        //Update 함수에서 접근할 컴포넌트를 변수에 할당
        tr = GetComponent<Transform>();
        controller = GetComponent<CharacterController>();
    }

    private void Update()
    {
        //키보드 입력값을 받아옴
        h = Input.GetAxis("Horizontal");
        v = Input.GetAxis("Vertical");

        //마우스 좌우 이동 값으로 회전
        tr.Rotate(Vector3.up * Input.GetAxis("Mouse X") * rotSpeed * Time.deltaTime);

        //이동 방향을 벡터의 덧셈 연산을 이용해 미리 계산
        movDir = (tr.forward * v) + (tr.right * h);
        //중력의 영향을 받아 밑으로 떨어지도록 y값 변경
        movDir.y -= 20f * Time.deltaTime;

        //플레이어를 이동
        controller.Move(movDir * movSpeed * Time.deltaTime);
    }
}
