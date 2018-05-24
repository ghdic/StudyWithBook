using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class UIMgr : MonoBehaviour {

	public void OnClickStartBtn()
    {
        Debug.Log("Click Button");
        SceneManager.LoadScene("scLevel01");
        SceneManager.LoadScene("scPlay", LoadSceneMode.Additive);
        //Additive 기존씬 삭제하지 않고 추가해서 새로운씬은 로드
        //Single 기존에 로드된 씬을 모두 삭제한 후 새로운 씬을 로드한다.
    }
}
