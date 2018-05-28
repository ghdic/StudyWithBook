using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class PhotonInit : MonoBehaviour {

    //App의 버전 정보
    public string version = "v1.0";

    //플레이어의 이름을 이력하는 UI 항목 연결
    public InputField userID;
    //룸 이름을 입력받을 UI 항목 연결 변수
    public InputField roomName;
    //RootItem이 차일드로 생성될 Parent 객체
    public GameObject scrollContents;
    //룸 목록만큼 생성될 RoomItem 프리팹
    public GameObject roomItem;

    //랜덤방 입장시 누르는 버튼
    public Button joinButton;
    //방 만들때 누르는 버튼
    public Button makeButton;

    private void Awake()
    {
        //버튼 입장전에는 비활성화(누르면 에러남)
        joinButton.enabled = false;
        makeButton.enabled = false;
        
        //필드에서 로비로 다시 돌아올때는 다시 접속할필요 없음
        if (!PhotonNetwork.connected)
        {
            //포톤 클라우드에 접속
            PhotonNetwork.ConnectUsingSettings(version);
        }
        //룸 이름을 무작위로 설정
        roomName.text = "ROOM_" + Random.Range(0, 999).ToString("000");
    }

    //포톤 클라우드에 정상적으로 접속한 후 로비에 입장하면 호출되는 콜백 함수
    void OnJoinedLobby()
    {
        Debug.Log("Entered Lobby !");
        joinButton.enabled = true;
        makeButton.enabled = true;
        userID.text = GetUserId();
        //PhotonNetwork.JoinRandomRoom();
    }

    //로컬에 저장된 플레이어 이름을 반환하거나 생성하는 함수
    string GetUserId()
    {
        string userId = PlayerPrefs.GetString("USER_ID");

        if (string.IsNullOrEmpty(userId))
        {
            userId = "USER_" + Random.Range(0, 999).ToString("000");
        }
        return userId;
    }

    //무작위 룸 접속에 실패한 경우 호출되는 콜백 함수
    void OnPhotonRandomJoinFailed()
    {
        Debug.Log("No rooms !");
        //룸 생성
        OnClickCreateRoom();
    }

    //룸에 입장하면 호출되는 콜백 함수
    void OnJoinedRoom()
    {
        Debug.Log("Enter Room");
        ////탱크를 생성하는 함수 호출
        //CreateTank();

        //룸 씬으로 이동하는 코루틴 실행
        StartCoroutine(this.LoadBattleField());
    }

    //룸 씬으로 이동하는 코루틴 함수
    IEnumerator LoadBattleField()
    {
        //씬을 이동하는 동안 포톤 클라우드 서버로부터 네트워크 메세지 수신 중단
        PhotonNetwork.isMessageQueueRunning = false;
        //백그라운드 씬 로딩
        AsyncOperation ao = SceneManager.LoadSceneAsync("scBattleField");
        yield return ao;
    }

    //Join Random Room 버튼 클릭시 호출되는 함수
    public void OnClickJoinRandomRoom()
    {
        //로컬 플레이어의 이름을 설정
        PhotonNetwork.player.NickName = userID.text;
        //플레이어의 이름을 저장
        PlayerPrefs.SetString("USER_ID", userID.text);

        //무작위로 추출된 룸으로 입장
        PhotonNetwork.JoinRandomRoom();
    }

    //Make Room 버튼 클릭 시 호출될 함수
    public void OnClickCreateRoom()
    {
        string _roomName = roomName.text;
        //룸 이름이 없거나 Null일 경우 룸 이름 지정
        if (string.IsNullOrEmpty(roomName.text))
        {
            _roomName = "ROOM_" + Random.Range(0, 999).ToString("000");
        }

        //로컬 플레이어의 이름을 설정
        PhotonNetwork.player.NickName = userID.text;
        //플레이어 이름을 저장
        PlayerPrefs.SetString("USER_ID", userID.text);

        //생성할 룸의 조건 설정
        RoomOptions roomOptions = new RoomOptions();
        roomOptions.IsOpen = true;
        roomOptions.IsVisible = true;
        roomOptions.MaxPlayers = 20;

        //지정한 조건에 맞는 룸 생성 함수
        PhotonNetwork.CreateRoom(_roomName, roomOptions, TypedLobby.Default);
    }

    //룸 생성 실패할 때 호출되는 콜백 함수
    void OnPhotonCreateRoomFailed(object[] codeAndMsg)
    {
        Debug.Log("Create Room Failed = " + codeAndMsg[1]);
    }

    //생성된 룸 목록이 변경됐을 때 호출되는 콜백 함수
    void OnReceivedRoomListUpdate()
    {
        //룸 목록을 다시 받았을 때 갱신하기 위해 기존에 생성된 RoomItem을 삭제
        foreach(GameObject obj in GameObject.FindGameObjectsWithTag("ROOM_ITEM"))
        {
            Destroy(obj);
        }

        //Grid Layout Group 컴포넌트의 Constraint Count 값을 증가시킬 변수
        int rowCount = 0;
        //스크롤 영역 초기화
        scrollContents.GetComponent<RectTransform>().sizeDelta = Vector2.zero;

        //수신받은 룸 목록의 정보로 RoomItem을 생성
        foreach(RoomInfo _room in PhotonNetwork.GetRoomList())
        {
            Debug.Log(_room.Name);
            //RoomItem 프리팹을 동적으로 생성
            GameObject room = (GameObject)Instantiate(roomItem);
            //생성한 RoomItem 프리팹의 Parent를 지정
            room.transform.SetParent(scrollContents.transform, false);

            //생성한 RoomItem에 표시하기 위한 텍스트 정보 전달
            RoomData roomData = room.GetComponent<RoomData>();
            roomData.roomName = _room.Name;
            roomData.connectPlayer = _room.PlayerCount;
            roomData.maxPlayers = _room.MaxPlayers;
            //텍스트 정보를 표시
            roomData.DispRoomData();
            //RoomItem의 Button 컴포넌트에 클릭 이벤트를 동적으로 연결
            //이벤트를 연결할때 AddLister, 버튼을 클릭하면 onclick, 이벤트에 특정 메서드를 지정하는 delegate
            roomData.GetComponent<UnityEngine.UI.Button>().onClick.AddListener(delegate { OnClickRoomItem(roomData.roomName); });
            ++rowCount;
        }

        //Grid Layout Group 컴포넌트의 Constrain Count 값을 증가시킴
        scrollContents.GetComponent<GridLayoutGroup>().constraintCount = rowCount;
        //스크롤 영역의 높이를 증가시킴
        scrollContents.GetComponent<RectTransform>().sizeDelta += new Vector2(100, rowCount * 100);
    }

    void OnClickRoomItem(string roomName)
    {
        //로컬 플레이어의 이름 지정
        PhotonNetwork.player.NickName = userID.text;
        //플레이어 이름을 저장
        PlayerPrefs.SetString("USER_ID", userID.text);

        //인자로 전달된 이름에 해당하는 룸으로 입장
        PhotonNetwork.JoinRoom(roomName);
    }

    ////탱크를 생성하는 함수 호출
    //void CreateTank()
    //{
    //    float pos = Random.Range(-100.0f, 100.0f);
    //    PhotonNetwork.Instantiate("Tank", new Vector3(pos, 20.0f, pos), Quaternion.identity, 0);
    //}

    private void OnGUI()
    {
        //화면 좌측 상단에 접속 과정에 대한 로그를 출력
        GUILayout.Label(PhotonNetwork.connectionStateDetailed.ToString());
    }
}
