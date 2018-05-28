using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameMgr : MonoBehaviour {

    //접속된 플레이어 수를 표시할 Text UI 항목 변수
    public Text txtConnect;
    //접속 로그를 표시할 Text UI 항목 변수
    public Text txtLogMsg;
    //RPC 호출을 위한 PhotonView
    private PhotonView pv;

    private void Awake()
    {
        //PhotonView 컴포넌트 할당
        pv = GetComponent<PhotonView>();
        //탱크를 생성하는 함수 호출
        CreateTank();
        //포톤 클라우드의 네트워크 메세지 수신을 다시 연결
        PhotonNetwork.isMessageQueueRunning = true;
        //룸에 입장 후 기존 접속자 정보를 출력
        GetConnectPlayerCount();
    }

    IEnumerator Start()
    {
        //로그 메세지에 출력할 문자열 생성
        string msg = "\n<color=#00ff00>["
            + PhotonNetwork.player.NickName
            + "] Connected</color>";
        //RPC 함수 호출 AllBuffed는 RPC를 모두에게 전송, 버퍼로기록(새로운플레이어도 이 RPC를 받음), 자신은 RPC수행하지않음
        pv.RPC("LogMsg", PhotonTargets.AllBuffered, msg);
        //룸에 있는 네트워크 객체 간의 통신이 완료 될때까지 잠시 대기
        yield return new WaitForSeconds(1.0f);
        //모든 탱크의 스코어 UI에 점수를 표시하는 함수를 호출
        SetConnectPlayerScore();
    }

    //모든 탱크의 스코어 UI에 점수를 표시하는 함수를 호출
    void SetConnectPlayerScore()
    {
        //현재 입장한 룸에 접속한 모든 네트워크 플레이어 정보를 저장
        PhotonPlayer[] players = PhotonNetwork.playerList;
        foreach(PhotonPlayer _player in players)
        {
            Debug.Log("[" + _player.ID + "]" + _player.NickName + " " + _player.GetScore() + " kill");
        }

        //모든 Tank 프리팹을 배열에 저장
        GameObject[] tanks = GameObject.FindGameObjectsWithTag("TANK");

        foreach(GameObject tank in tanks)
        {
            //각 Tank별 스코어를 조회
            int currKillCount = tank.GetComponent<PhotonView>().owner.GetScore();
            //해당 Tank의 UI에 스코어 표시
            tank.GetComponent<TankDamage>().txtKillCount.text = currKillCount.ToString();
        }
    }
    //탱크를 생성하는 함수
    void CreateTank()
    {
        float pos = Random.Range(-100.0f, 100.0f);
        PhotonNetwork.Instantiate("Tank", new Vector3(pos, 20.0f, pos), Quaternion.identity, 0);
    }

    //룸 접속자 정보를 조회하는 함수
    void GetConnectPlayerCount()
    {
        //현재 입장한 룸 정보를 받아옴
        Room currRoom = PhotonNetwork.room;

        //현재 룸의 접속자 수와 최대 접속 가능한 수를 문자열로 구성한 후 TextUI 항목에 출력
        txtConnect.text = currRoom.PlayerCount.ToString() + "/" + currRoom.MaxPlayers.ToString();
    }

    //네트워크 플레이어가 접속했을 때 호출되는 함수
    void OnPhotonPlayerConnected(PhotonPlayer newPlayer)
    {
        GetConnectPlayerCount();
    }

    //네트워크 플레이어가 룸을 나가거나 접속이 끊어졌을 때 호출되는 함수
    void OnPhotonPlayerDisconnected(PhotonPlayer outPlayer)
    {
        GetConnectPlayerCount();
    }

    //룸 나가기 버튼 클릭 이벤트에 연결될 함수
    public void OnClickExitRoom()
    {
        //로그 메세지에 출력할 문자열 생성
        string msg = "\n<color=#00ff00>["
            + PhotonNetwork.player.NickName
            + "] Disconnected</color>";
        //RPC 함수 호출
        pv.RPC("LogMsg", PhotonTargets.AllBuffered, msg);
        //현재 룸을 빠져나가며 생성한 모든 네트워크 객체를 삭제
        PhotonNetwork.LeaveRoom();
    }

    //룸에서 접속 종료됐을 때 호출되는 콜백 함수
    void OnLeftRoom()
    {
        //로빈 씬을 호출
        SceneManager.LoadScene("scLobby");
    }

    [PunRPC]
    void LogMsg(string msg)
    {
        //로그 메세지 Text UI에 텍스트를 누적시켜 표시
        txtLogMsg.text = txtLogMsg.text + msg;
    }
}
