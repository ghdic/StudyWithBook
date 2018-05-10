using System.Collections;
using UnityEngine;

public class WallCtrl : MonoBehaviour {
    //스파크 파티클 프리팹 연결할 변수
    public GameObject sparkEffect;

    //충돌이 시작할 때 발생하는 이벤트
    private void OnCollisionEnter(Collision collision)
    {
        //충돌한 게임오브젝트의 태그값 비교
        if(collision.collider.tag == "BULLET")
        {
            //스파크 파티클을 동적으로 생성
            //Instantiate(sparkEffect, collision.transform.position, Quaternion.identity);
            //스파크 파티클을 동적으로 생성하고 변수에 할당
            GameObject spark = (GameObject)Instantiate(sparkEffect, collision.transform.position, Quaternion.identity);
            //ParticleSystem 컴포넌트 수행시간이 지난 후 삭제 처리
            Destroy(spark, spark.GetComponent<ParticleSystem>().duration + 0.2f);

            //충돌한 게임오브젝트 삭제
            Destroy(collision.gameObject, 3.0f);
        }
    }
}
