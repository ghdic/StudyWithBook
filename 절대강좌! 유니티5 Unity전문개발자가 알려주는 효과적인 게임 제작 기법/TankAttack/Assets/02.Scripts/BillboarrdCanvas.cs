using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BillboarrdCanvas : MonoBehaviour {

    private Transform tr;
    private Transform mainCameraTr;

    private void Start()
    {
        tr = GetComponent<Transform>();
        //스테이지에 있는 메인 카메라의 Transform 컴포넌트를 추출
        mainCameraTr = Camera.main.transform;
    }

    private void Update()
    {
        tr.LookAt(mainCameraTr);
    }
}
