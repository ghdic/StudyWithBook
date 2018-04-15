실행환경 세팅
*DirectX SDK 설치
https://www.microsoft.com/en-us/download/details.aspx?id=6812

![vsSetting](https://github.com/ghdic/StudyWithBook/blob/master/image/gameVsSetting1.png)

위 사진처럼 visual studio 속성에서
포함 디렉터리 -> DirectX경로/Include
라이브러리 디렉터리 -> DirectX경로/Lib/x86
를 추가한다.

![vsSetting2](https://github.com/ghdic/StudyWithBook/blob/master/image/gameVsSetting2.png)
추가 종속성으로 d3d9.lib, d3dx9.lib, winmm.lib 를 추가해준다.

그럼 세팅 완료!!

DirectX SDK를 설치하는 중에 오류가 난다면 이미 DirectX가 사용자버젼으로 설치되어 있는지 확인해보자.
