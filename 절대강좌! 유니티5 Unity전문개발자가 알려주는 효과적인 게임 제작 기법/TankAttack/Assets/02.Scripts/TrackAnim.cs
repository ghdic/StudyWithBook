using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TrackAnim : MonoBehaviour {

    //텍스처의 회전 속도
    private float scrollSpeed = 1.0f;
    private Renderer _renderer;

    private void Start()
    {
        _renderer = GetComponent<Renderer>();
    }

    private void Update()
    {
        var offset = Time.time * scrollSpeed * Input.GetAxisRaw("Vertical");

        //기본 텍스처의 Y 오프셋 값 변경
        _renderer.material.SetTextureOffset("_MainTex", new Vector2(0, offset));
        //노말 텍스처의 Y 오프셋 값 변경
        _renderer.material.SetTextureOffset("_BumpMap", new Vector2(0, offset));
    }
}
