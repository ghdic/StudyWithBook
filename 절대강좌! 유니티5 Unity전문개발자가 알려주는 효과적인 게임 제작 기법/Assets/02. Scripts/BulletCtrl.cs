using System.Collections;
using UnityEngine;

//OnCollisionEnter 두 물체간의 충돌이 일어나기 시작했을때
//OnCollisionStay 두 물체 간의 충돌이 지속 될때
//OnCollisionExit 두 물체가 서로 떨어졌을때
//OnTriggerEnter 두 물체간의 충돌이 일어나기 시작했을때
//OnTriggerStay 두 물체 간의 충돌이 지속될 때
//OnTriggerExit 두 물체가 서로 떨어졌을 때
//trigger 체크할 경우 collision 무시하고 벽 관통하며 이벤트 발생
public class BulletCtrl : MonoBehaviour {
    //총알의 파괴력
    public int damage = 20;
    //총알의 발사 속도
    public float speed = 1000.0f;

	void Start () {
        GetComponent<Rigidbody>().AddForce(transform.forward * speed);
	}

    //충돌이 시작할 때 발생하는 이벤트
    private void OnCollisionEnter(Collision coll)
    {
        //충돌한 게임오브젝트의 태그값 비교
        if (coll.collider.tag == "BULLET")
        {
            //충돌한 게임오브젝트 삭제
            Destroy(coll.gameObject, 0.5f);
        }
    }
}
