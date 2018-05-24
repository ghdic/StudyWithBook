using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BulletFire : MonoBehaviour {
    //총알의 생명 시간
    private float lifetime = 5.0f;

    private void Start()
    {
        //Rigidbody의 속도를 Forward 방향으로 설정
        GetComponent<Rigidbody>().velocity = transform.forward * 10.0f;
        //일정 시간이 지난 후 제거
        Destroy(gameObject, lifetime);
    }
}
