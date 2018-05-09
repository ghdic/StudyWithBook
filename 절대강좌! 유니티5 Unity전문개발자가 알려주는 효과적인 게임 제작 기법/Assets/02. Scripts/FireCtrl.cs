using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FireCtrl : MonoBehaviour {

    //총알 프리팹
    public GameObject bullet;
    //총알 발사좌표
    public Transform firePos;

    private void Update()
    {
        //마우스 왼쪽 버튼을 클릭했을 때 Fire 함수 호출
        //GetMouseButton 마우스 버튼 클릭하고 있을때
        //GetMouseButton up down 마우스 버튼 누르거나 땔때
        //인자값은 0 왼쪽 1 오른쪽 2 가운데 버튼을 나타낸다.
        if (Input.GetMouseButtonDown(0))
        {
            Fire();
        }
    }
    void Fire()
    {
        //동적으로 총알을 생성하는 함수
        CreateBullet();
    }

    void CreateBullet()
    {
        //Bullet 프리팹을 동적으로 생성
        Instantiate(bullet, firePos.position, firePos.rotation);
    }
}
