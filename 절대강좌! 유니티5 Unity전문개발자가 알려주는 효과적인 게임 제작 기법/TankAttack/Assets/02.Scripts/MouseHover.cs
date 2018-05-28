using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class MouseHover : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler {

    public static MouseHover instance = null;

    //마우스 커서의 UI 항목에 대한 Hover 여부
    public bool isUIHover = false;

    void Awake()
    {
        instance = this;
    }

    //스크립트를 컴포넌트한 것 위에 마우스가 올라갔을때
    public void OnPointerEnter(PointerEventData eventData)
    {
        isUIHover = true;
    }

    //스크립트를 컴포넌트한 것 위에 마우스가 빠져나갔을때
    public void OnPointerExit(PointerEventData eventData)
    {
        isUIHover = false;
    }
}
