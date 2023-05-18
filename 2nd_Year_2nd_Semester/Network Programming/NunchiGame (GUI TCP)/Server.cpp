#pragma comment(lib, "ws2_32.lib")//속성->링커->입력->ws2_32.lib; 입력해도됨
#include <WinSock2.h>					 //윈도우소켓사용하기위한 헤더파일
#include <stdio.h>
#include <string.h>

/* 서버 */
#define SERVERIP "127.0.0.1"		//루프백 주소
#define SERVERPORT 9000		//서버포트


#define BUFSIZE 4096				//버퍼사이즈
#define NICKNAMESIZE 255		//닉네임 사이즈
#define MAXUSER   100				//최대 유저 숫자
#define NODATA   -1					//없음!
#define PLAYER_COUNT 5			//한방에 플레이어 제한 숫자

//전달할 메세지 설정
#define INTRO_MSG "눈치게임입니다. 닉네임을 입력하세요"
#define NICKNAME_ERROR_MSG "이미있는 닉네임입니다."
#define WAIT_MSG "다른 Player를 기다립니다....\n"
#define GAME_START_MSG1 "게임이 시작하였습니다."
#define GAME_START_MSG2 "(숫자를 1부터 입력해야되고, 5를 외치거나 동시에 입력하면 게임에서 패배합니다)"
#define LOSE_MSG "당신이 게임에서 패배하셨습니다."
#define NUMBER_ERROR_MSG "1~5사이의 숫자만 입력할 수 있습니다."
#define NUMBER_SAME_MSG "다른 플레이어가 먼저 입력하였습니다."
#define NUMBER_PASS_MSG "이미 입력된 숫자입니다."
#define NUMBER_END_MSG "당신은 이미 숫자를 입력하셨습니다."


enum STATE							//진행상태
{
	INITE_STATE,						//접속
	INTRO_STATE,						//인트로
	NAME_SET_STATE,				//닉네임설정
	WAIT_STATE,						//대기
	GAMEING_STATE,					//게임진행
	GAME_END_STATE				//게임끝
};

enum PROTOCOL						//진행상황
{
	INTRO,									//인트로
	WAIT,									//대기
	CHATT_NICKNAME,				//닉네임입력
	NICKNAME_EROR, 				//닉네임에러
	NICKNAME_LIST,					//닉네임출력
	CHATT_MSG,							//체팅메세지
	CHATT_OUT,							//종료
};

struct _ClientInfo;
struct _GameInfo													//게임정보
{	
	int game_number;											//지금까지 진행된 숫자
	HANDLE start_event;										//게임시작 이벤트
	_ClientInfo* players[PLAYER_COUNT];			//플레이어
	int player_count;												//플레이어 인원수
	bool full;															//꽉찼는지 확인
	char* namelist[5];											//게임에 참가한 플레이어 닉네임 모음
};

struct _ClientInfo													//클라이언트 정렬
{
	SOCKET sock;													//소켓정보
	SOCKADDR_IN clientaddr;								//주소정보
	STATE state;													//진행상태
	_GameInfo* game_info;									//참여한 게임방
	bool chatflag;													//채팅가능여부
	char  nickname[NICKNAMESIZE];						//설정한 닉네임
	char  sendbuf[BUFSIZE];									//send용 버퍼
	char  recvbuf[BUFSIZE];									//recv용 버퍼
	int player_number;											//방에 몇번째로 입장했는지
	bool turn_end;													//게임 참여 후 추가 참여 x
};

//구조체 배열 선언
_ClientInfo* ClientInfo[MAXUSER];						
_GameInfo* GameInfo[MAXUSER];
//닉네임 배열 선언
char* NickNameList[MAXUSER];

//접속 인원과 게임방 갯수
int Client_Count = 0;
int GameCount = 0;
//닉네임 갯수
int Nick_Count = 0;

//크리티컬섹션
CRITICAL_SECTION cs;


DWORD CALLBACK ProcessClient(LPVOID);									//클라이언트 진행

void err_quit(const char *msg);													//에러 출력 및 종료
void err_display(const char *msg);												//에러 출력
int recvn(SOCKET s, char *buf, int len, int flags);						//recv반복함수

void MakeChattMessage(const char* , const char* , char* );		//닉네임 : 내용으로 문자열 생성 함수
void MakeEnterMessage(const char* , char* );							//입장 메세지 문자열 생성 함수
void MakeExitMessage(const char* , char* );								//퇴장 메세지 문자열 생성 함수

_ClientInfo* AddClient(SOCKET sock, SOCKADDR_IN clientaddr);//클라이언트 접속 처리
void RemoveClient(_ClientInfo* ptr);											//클라이언트 정보 삭제

_GameInfo* _AddGameInfo(_ClientInfo* _ptr);							//게임 생성
void RemoveGameinfo(_ClientInfo* _ptr);									//게임정보 삭제

bool NicknameCheck(const char*);												//닉네임체크
void NamelistSet(_ClientInfo* _ptr);											//닉네임리스트 설정
void NickNameSetting(_ClientInfo*);											//닉네임 설정
void RemoveNickName(const char*);											//닉네임삭제

bool PacketRecv(SOCKET, char*);												//recvn을 두번 처리

/* 전달할 데이터 Packing 처리 */
int PackPacket(char*, PROTOCOL, const char*);
int PackPacket(char*, PROTOCOL);
int PackPacket(char*, PROTOCOL, char**, int);

/* 전달받은 데이터에서 protocol 분리 */
PROTOCOL GetProtocol(const char*);

/* 전달받은 데이터 UnPacking */
void UnPackPacket(const char*, char*);

void ChattingMessageProcess(_ClientInfo*);								//메세지 입력 및 전달
void InitProcess(_ClientInfo* _ptr);												//접속 및 대기
void ChattingOutProcess(_ClientInfo*);										//퇴장 메세지
void ChattingEnterProcess(_ClientInfo*);									//입장 메세지


/* 메인 실행 */
int main(int argc, char **argv)
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	//임계 영역 초기화
	InitializeCriticalSection(&cs);
	
	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr =inet_addr(SERVERIP);
	int retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수		
	int addrlen;
	SOCKET sock;
	SOCKADDR_IN clientaddr;	
	
	while (1)
	{
		addrlen = sizeof(clientaddr);

		sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (sock == INVALID_SOCKET)
		{
			err_display("accept()");
			continue;
		}
		//클라이언트 생성
		_ClientInfo* ptr = AddClient(sock, clientaddr);
		//멀티쓰레드
		HANDLE hThread=CreateThread(NULL, 0, ProcessClient, ptr, 0, nullptr);	
		if (hThread != NULL)
		{
			CloseHandle(hThread);
		}

	}
	// closesocket()
	closesocket(listen_sock);
	//임게영역 정리
	DeleteCriticalSection(&cs);
	WSACleanup();
	return 0;
}

//게임생성
_GameInfo* _AddGameInfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);																//임계영역 할당

	_GameInfo* game_ptr = nullptr;
	int index = NODATA;

	for (int i = 0; i < GameCount; i++)														//생성되어있는 방을 확인
	{
		if (!GameInfo[i]->full)																	//꽉차지 않은 방이 있는지 확인
		{
			GameInfo[i]->players[GameInfo[i]->player_count++] = _ptr;	//비어있는 자리에 들어감
			_ptr->game_info = GameInfo[i];												//연결
			_ptr->player_number = GameInfo[i]->player_count;				//몇번째 맴버인지를 설정
			
			if (GameInfo[i]->player_count == PLAYER_COUNT)					//꽉차면
			{
				GameInfo[i]->full = true;														//방에 자리없음!
				SetEvent(GameInfo[i]->start_event);									//게임시작
			}
			game_ptr = GameInfo[i];															
			index = i;
			break;
		}
	}

	if (index == NODATA)																					//자리가 빈 방이 없으면 실행
	{
		/* 새로운 방 설정 */
		game_ptr = new _GameInfo;
		ZeroMemory(game_ptr, sizeof(_GameInfo));
		game_ptr->full = false;
		game_ptr->start_event = CreateEvent(nullptr, true, false, nullptr);
		game_ptr->players[0] = _ptr;
		game_ptr->game_number = 1;
		_ptr->game_info = game_ptr;
		_ptr->player_number = 1;
		

		game_ptr->player_count++;																	//참가한 인원 수 증가
		GameInfo[GameCount++] = game_ptr;													//배열에 저장
	}

	LeaveCriticalSection(&cs);																//임계영역 해제
	return game_ptr;
}

//접속 및 대기
void InitProcess(_ClientInfo* _ptr)
{
	_GameInfo* game_info = _ptr->game_info;	
	
	NamelistSet(_ptr);	//게임방에 접속한 사람들의 닉네임 리스트 설정

	//접속 후에 그전에 접속했던사람들 출력
	int size = PackPacket(_ptr->sendbuf, NICKNAME_LIST, _ptr->game_info->namelist, _ptr->game_info->player_count);
	if (send(_ptr->sock, _ptr->sendbuf, size, 0) == SOCKET_ERROR)
	{
		err_display("nickname list Send()");
		_ptr->state = GAME_END_STATE;
		LeaveCriticalSection(&cs);
		return;
	}

	//게임 시작 대기
	WaitForSingleObject(game_info->start_event, INFINITE);

	//게임시작 메세지1
	size = PackPacket(_ptr->sendbuf, CHATT_MSG, GAME_START_MSG1);
	int retval = send(_ptr->sock, _ptr->sendbuf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		_ptr->state = GAME_END_STATE;
		return;
	}
	//게임시작 메세지2
	size = PackPacket(_ptr->sendbuf, CHATT_MSG, GAME_START_MSG2);
	retval = send(_ptr->sock, _ptr->sendbuf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		_ptr->state = GAME_END_STATE;
		return;
	}

}

//클라이언트 진행프로세스
DWORD CALLBACK ProcessClient(LPVOID  _ptr)
{
	_ClientInfo* Client_ptr = (_ClientInfo*)_ptr;
		
	int size;	
	PROTOCOL protocol;

	bool breakflag =false;

	while (!breakflag)
	{

		switch (Client_ptr->state)	//진행 상황에 따른 진행
		{
		case INITE_STATE://접속
			Client_ptr->state = INTRO_STATE;
			break;
		case INTRO_STATE://인트로
			//인트로 메세지 전달
			size = PackPacket(Client_ptr->sendbuf, INTRO, INTRO_MSG);
			if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("intro Send()");
				Client_ptr->state = GAME_END_STATE;	
				break;
			}

			Client_ptr->state = NAME_SET_STATE;
			break;
		case NAME_SET_STATE://닉네임설정

			if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))	//데이터 받기
			{
				Client_ptr->state = GAME_END_STATE;
				break;
			}

			protocol = GetProtocol(Client_ptr->recvbuf);					//프로토콜 

			switch (protocol)
			{
			case CHATT_NICKNAME:
				NickNameSetting(Client_ptr);										//닉네임설정
				break;
			}

			if (!Client_ptr->chatflag)
			{
				break;
			}
			
			ChattingEnterProcess(Client_ptr);									//입장 메세지

			if (Client_ptr->state != GAME_END_STATE)
			{
				Client_ptr->state = WAIT_STATE;								//대기하는 곳으로
			}
			break;
		case WAIT_STATE://게임시작 대기
			InitProcess(Client_ptr);													//게임시작 대기
			Client_ptr->state = GAMEING_STATE;
			break;
		case GAMEING_STATE://게임진행

			if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
			{
				Client_ptr->state = GAME_END_STATE;
				break;
			}

			protocol = GetProtocol(Client_ptr->recvbuf);

			switch (protocol)
			{
			case CHATT_MSG:
				ChattingMessageProcess(Client_ptr);							//게임진행
				break;
			case CHATT_OUT:
				ChattingOutProcess(Client_ptr);									//퇴장
				break;
			}
			break;
		case GAME_END_STATE:
			RemoveGameinfo(Client_ptr);
			RemoveClient(Client_ptr);
			breakflag = true;
			break;
		}

		if (breakflag)
		{
			break;
		}

	}


	return 0;
}


//recvn 2번 사이즈-> 데이터
bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;
	//받을 데이터의 사이즈 저장
	int retval = recvn(_sock, (char*)&size, sizeof(size), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("packetsize recv error()");
		return false;
	}
	else if (retval == 0)
	{
		return false;
	}
	//사이즈 만큼 데이터를 받아옴
	retval = recvn(_sock, _buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv error()");
		return false;

	}
	else if (retval == 0)
	{
		return false;
	}

	return true;
}

/* Packing */
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1) //버퍼, 프로토콜, 메세지
{
	char* ptr = _buf;	
	int size = 0;
	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(size);

	int strsize1 = strlen(_str1);
	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	size = size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	size = size + strsize1;

	ptr = _buf;

	memcpy(ptr, &size, sizeof(size));
	size = size + sizeof(size);
	return size;
}
int PackPacket(char* _buf, PROTOCOL _protocol)//버퍼,프로토콜
{
	char* ptr = _buf;
	int size = 0;
	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(size);
		
	ptr = _buf;

	memcpy(ptr, &size, sizeof(size));
	size = size + sizeof(size);
	return size;
}
int PackPacket(char* _buf, PROTOCOL _protocol, char** _strlist, int _count)	//버퍼,닉네임리스트,닉네임 갯수
{
	char* ptr = _buf;
	int strsize;
	int size = 0;

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_count, sizeof(_count));
	ptr = ptr + sizeof(_count);
	size = size + sizeof(_count);

	for (int i = 0; i < _count; i++)
	{
		strsize = strlen(_strlist[i]);

		memcpy(ptr, &strsize, sizeof(strsize));
		ptr = ptr + sizeof(strsize);
		size = size + sizeof(strsize);

		memcpy(ptr, _strlist[i], strsize);
		ptr = ptr + strsize;
		size = size + strsize;
	}

	ptr = _buf;

	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

//프로토콜 출력
PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}

/* Unpacking */
void UnPackPacket(const char* _buf, char* _str)//버퍼, 메세지
{
	int strsize;
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
	ptr = ptr + strsize;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpmsgbuf;																		//void*
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |								//오류메세지 저장할 공간 할당
		FORMAT_MESSAGE_FROM_SYSTEM,										//운영체제에서 오류메세지 가져옴
		NULL, WSAGetLastError(),														//WSAGetLastError가 에러코드를 가져옴		
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),				//오류코드를 시스템 기본언어로 번역
		(LPSTR)&lpmsgbuf, 0, NULL);													//Ipmsgbuf에 저장
	MessageBox(NULL, (LPCSTR)lpmsgbuf, msg, MB_OK);					//Ipmsgbuf에 저장된 내용을 메세지박스로 출력
	LocalFree(lpmsgbuf);																	//Ipmsgbuf에 할당된 메모리 반환
	exit(-1);																							//종료
}
// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpmsgbuf;																	//void*
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |							//오류메세지 저장할 공간 할당
		FORMAT_MESSAGE_FROM_SYSTEM,									//운영체제에서 오류메세지 가져옴
		NULL, WSAGetLastError(),													//WSAGetLastError가 에러코드를 가져옴
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),			//오류코드를 시스템 기본언어로 번역
		(LPSTR)&lpmsgbuf, 0, NULL);												//Ipmsgbuf에 저장
	printf("[%s] %s\n", msg, (LPSTR)lpmsgbuf);							//Ipmsgbuf에 저장된 내용을 Printf로 명령프롬프트창에 출력
	LocalFree(lpmsgbuf);																//Ipmsgbuf에 할당된 메모리 반환
}
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET sock, char* buf, int len, int flags)					// 사용자 정의 데이터 수신 함수 recv를 반복
{
	char* ptr = buf;																		//버퍼의 시작주소 읽을때마다 ptr 값 증가
	int left = len;																			//읽지 않은 데이터의 크기, 읽을때 마다 left 값 감소
	int recived;																				//recv함수의 리턴 값을 저장하는 변수

	while (left > 0)																			//읽지 않은 데이터가 없을때까지 반복
	{
		recived = recv(sock, ptr, left, flags);									//데이터를 수신하는 함수
		if (recived == SOCKET_ERROR)											//에러확인
		{
			return SOCKET_ERROR;
		}
		if (recived == 0)																	//정상 종료시 나감
		{
			break;
		}

		left -= recived;																		//데이터를 읽을 때마다 증감하면서 
		ptr += recived;																		//정확한 바이트 수를 리턴하기 위한처리
	}

	return (len - left);																	//읽은 바이트수 리턴
}

//클라이언트 접속 정보처리
_ClientInfo* AddClient(SOCKET sock, SOCKADDR_IN clientaddr)
{
	printf("Client 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr),	//접속 주소 출력
		ntohs(clientaddr.sin_port));

	EnterCriticalSection(&cs);																					//임계영역 할당

	_ClientInfo* ptr = new _ClientInfo;																		//메모리 할당
	ZeroMemory(ptr, sizeof(_ClientInfo));																	//메모리 초기화
	ptr->sock = sock;																									//받아온 소켓정보 저장
	memcpy(&(ptr->clientaddr), &clientaddr, sizeof(clientaddr));								//클라 주소 저장
	ptr->state = INITE_STATE;																					//state(상태) 설정
	ptr->chatflag = false;																							//입력가능여부 설정
	ptr->turn_end = false;																							//추가 입력 가능 여부
	ClientInfo[Client_Count++] = ptr;																			//클라이언트정보 포인트배열에 저장 (카운트증가)
	
	LeaveCriticalSection(&cs);																					//임계영역 해제
	return ptr;
}

//게임정보 삭제
void RemoveGameinfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);										//임계영역 할당

	_GameInfo* game = _ptr->game_info;					//ptr이 소속된 게임정보

	for (int i = 0; i < game->player_count; i++)				//소속된 플레이어 수만큼 반복
	{
		if (game->players[i] == _ptr)								//자신의 위치를 찾음
		{
			for (int j = i; j < game->player_count -1; j++)		//땡기기
			{
				game->players[j] = game->players[j + 1];
			}
			game->player_count--;									//참가 인원수 줄임
			if (game->player_count == 0)							//플레이어인원이 0명일 경우
			{
				for (int i = 0; i < GameCount; i++)					//게임방 땡김
				{
					if (GameInfo[i] == game)
					{
						CloseHandle(game->start_event);
						delete _ptr->game_info;
						for (int j = i; j < GameCount - 1; j++)
						{
							GameInfo[j] = GameInfo[j + 1];
						}
						break;
					}
				}

				GameCount--;												//게임수 감소
			}
			break;
		}
	}


	LeaveCriticalSection(&cs);										//임계영역 해제
}

//클라이언트 정보 삭제
void RemoveClient(_ClientInfo* ptr)
{

	printf("Client 종료: IP 주소=%s, 포트 번호=%d\n",			//종료한 클라이언트 정보 출력
		inet_ntoa(ptr->clientaddr.sin_addr),
		ntohs(ptr->clientaddr.sin_port));

	EnterCriticalSection(&cs);											//임계영역 할당

	
	for (int i = 0; i < Client_Count; i++)									//접속한 클라이언트 순회
	{
		if (ClientInfo[i] == ptr)												//자신이 몇번째에 있는지 확인
		{			
			closesocket(ptr->sock);										//소켓 종료
			delete ptr;
			for (int j = i; j < Client_Count - 1; j++)						//땡기기
			{
				ClientInfo[j] = ClientInfo[j + 1];
			}
			break;
		}
	}
	Client_Count--;															//접속인원 줄이기
	LeaveCriticalSection(&cs);											//임계영역 해제
}

//닉네임 체크
bool NicknameCheck(const char* _nick)
{
	EnterCriticalSection(&cs);						//임계영역 설정
	for (int i = 0; i < Nick_Count; i++)				//설정된 닉네임 만큼 순회
	{
		if (!strcmp(NickNameList[i], _nick))		//닉네임 리스트에 같은 닉네임이 있는지 확인
		{
			LeaveCriticalSection(&cs);				//임계영역 해제
			return false;
		}
	}
	LeaveCriticalSection(&cs);						//임계영역 해제

	return true;
}

//닉네임 추가
void AddNickName(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);											//임계영역 설정

	char* ptr = new char[strlen(_ptr->nickname) + 1];
	strcpy(ptr, _ptr->nickname);
	NickNameList[Nick_Count++] = ptr;								//닉네임 리스트에 추가

	LeaveCriticalSection(&cs);
}

//닉네임 리스트 설정 (방에 속한 닉네임리스트)
void NamelistSet(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);													//임계영역 설정
	_GameInfo* _game = _ptr->game_info;								//게임정보 불러옴

	for (int i = 0; i < _game->player_count; i++)							//게임인원 수 만큼
	{
		_game->namelist[i] = _game->players[i]->nickname;		//게임에 참가한 플레이어의 닉네임들을 게임 닉네임 리스트에 저장
	}

	LeaveCriticalSection(&cs);													//임계영역 해제
}

//닉네임 삭제
void RemoveNickName(const char* _nick)
{
	EnterCriticalSection(&cs);									//임계영역 설정
	for (int i = 0; i < Nick_Count; i++)							//설정된 닉네임 갯수만큼 반복
	{
		if (!strcmp(NickNameList[i], _nick))					//설정된 닉네임 찾기
		{
			delete[] NickNameList[i];								//삭제

			for (int j = i; j < Nick_Count - 1; j++)				//땡김
			{
				NickNameList[j] = NickNameList[j + 1];
			}
			NickNameList[Nick_Count--] = nullptr;			
			break;
		}
	}
	LeaveCriticalSection(&cs);									//임계영역 해제
}

/* 문자열 생성 함수 */
void MakeChattMessage(const char* _nick, const char* _msg, char* _chattmsg)
{
	sprintf(_chattmsg, "[ %s ] %s", _nick, _msg);		// [ 닉네임 ] : 메세지
}
void MakeEnterMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s님이 입장하셨습니다.", _nick);		// 닉네임님이 입장~
}
void MakeExitMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s님이 퇴장하셨습니다.", _nick);		// 닉네임님이 퇴장~
}

//닉네임 설정
void NickNameSetting(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);																										//임계영역 설정
	
	char msg[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	UnPackPacket(_clientinfo->recvbuf, msg);												
	if (!NicknameCheck(msg))																										//닉네임 체크
	{
		size = PackPacket(_clientinfo->sendbuf, NICKNAME_EROR, NICKNAME_ERROR_MSG);			//이미 있는 닉네임
		if (send(_clientinfo->sock, _clientinfo->sendbuf, size, 0) == SOCKET_ERROR)
		{
			err_display("nickname error Send()");			
			_clientinfo->state = GAME_END_STATE;					
		}
		LeaveCriticalSection(&cs);																									//임계영역 해제
		return;
	}
	else
	{
		strcpy(_clientinfo->nickname, msg);
		_AddGameInfo(_clientinfo);																									//방설정
		AddNickName(_clientinfo);																									//닉네임추가

		_clientinfo->chatflag = true;
	}	

	LeaveCriticalSection(&cs);																										//임계영역 해제
}

//눈치게임 실행 부분
bool nunci_game(_ClientInfo* _clientinfo,char* msg)
{
	_GameInfo* _game = _clientinfo->game_info;

	int size;
	int retval;

	//올바른 입력을 했었을 경우 실행
	if (_clientinfo->turn_end)																							
	{
		size = PackPacket(_clientinfo->sendbuf, CHATT_MSG, NUMBER_END_MSG);
		if (send(_clientinfo->sock, _clientinfo->sendbuf, size, 0) == SOCKET_ERROR)
		{
			err_display("chatting message Send()");
			_clientinfo->state = GAME_END_STATE;
		}
		return false;
	}
	
	//올바른 입력범위에 포함되어있지 않는 것을 입력했을 경우 실행
	if (atoi(msg) <= 0 || atoi(msg) > 5)																				
	{
		size = PackPacket(_clientinfo->sendbuf, CHATT_MSG, NUMBER_ERROR_MSG);
		if (send(_clientinfo->sock, _clientinfo->sendbuf, size, 0) == SOCKET_ERROR)
		{
			err_display("chatting message Send()");
			_clientinfo->state = GAME_END_STATE;
		}
		return false;
	}
	
	//패배 조건 설정 (같은 숫자를 입력하거나 순서에 맞지않는 숫자를 입력하면 패배)
	if (atoi(msg) == 5 || _clientinfo->game_info->game_number != atoi(msg))
	{
		char send_msg[BUFSIZE] = { 0 };
		for (int i = 0; i < _game->player_count; i++)	//해당플레이어는 패배메세지를 나머지 플레이어는 승리메세지를 전달한다.
		{
			_ClientInfo* tempptr = _game->players[i];

			_game->players[i]->state = GAME_END_STATE;

			if (_game->players[i] != _clientinfo)
			{
				sprintf(send_msg, "%s님이 게임에서 패배하셨습니다!! \n승리를 축하합니다\n게임이 종료되었습니다!", _clientinfo->nickname);
				size = PackPacket(tempptr->sendbuf, CHATT_OUT, send_msg);
			}
			else
			{
				printf("[ %s ]님이 패배하셨습니다\n게임이 종료되었습니다!", tempptr->nickname);
				size = PackPacket(tempptr->sendbuf, CHATT_OUT, LOSE_MSG);
			}
			if (send(tempptr->sock, tempptr->sendbuf, size, 0) == SOCKET_ERROR)
			{
				tempptr->state = GAME_END_STATE;
			}
		}
		return false;
	}

	//제대로 입력했으면 서버에 뭐 입력했는지 출력
	if (_clientinfo->game_info->game_number == atoi(msg))
	{
		_clientinfo->game_info->game_number++;
		_clientinfo->turn_end = true;
		printf("[ %s ]님의 채팅: %s\n", _clientinfo->nickname, msg);
		return true;
	}

	return false;
}

//메세지 입력 및 전달
void ChattingMessageProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);																																	//임계영역 설정

	_GameInfo* _game = _clientinfo->game_info;

	char msg[BUFSIZE];
	char temp[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	memset(temp, 0, sizeof(temp));

	UnPackPacket(_clientinfo->recvbuf, msg);																											
	MakeChattMessage(_clientinfo->nickname, msg, temp);																						//받아온 메세지를 형태를 바꿔서 보냄

	if (nunci_game(_clientinfo, msg))
	{
		for (int i = 0; i < _game->player_count; i++)
		{
			if (_game->players[i]->chatflag)
			{
				size = PackPacket(_game->players[i]->sendbuf, CHATT_MSG, temp);
				if (send(_game->players[i]->sock, _game->players[i]->sendbuf, size, 0) == SOCKET_ERROR)
				{
					err_display("chatting message Send()");
					ClientInfo[i]->state = GAME_END_STATE;
				}
			}
		}
	}


	LeaveCriticalSection(&cs);																																	//임계영역 해제
}

//게임방에 입장 할 경우 실행하는 함수
void ChattingEnterProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);//임계영역 설정
	_GameInfo* _game = _clientinfo->game_info;
	char msg[BUFSIZE];
	int size;

	MakeEnterMessage(_clientinfo->nickname, msg);//입장메세지 생성

	for (int i = 0; i < _game->player_count; i++)
	{
		if (_game->players[i]->chatflag)//입장한 플레이어만 이 메세지를 전달받음
		{
			size = PackPacket(_game->players[i]->sendbuf, CHATT_MSG, msg);
			if (send(_game->players[i]->sock, _game->players[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting enter massage Send()");
				ClientInfo[i]->state = GAME_END_STATE;
			}
			if (_game->player_count <5)//입장한 플레이어가 4명이하면 기다리라는 메세지까지 전달
			{
				size = PackPacket(_game->players[i]->sendbuf, CHATT_MSG, WAIT_MSG);
				if (send(_game->players[i]->sock, _game->players[i]->sendbuf, size, 0) == SOCKET_ERROR)
				{
					err_display("chatting enter massage Send()");
					ClientInfo[i]->state = GAME_END_STATE;
				}
			}
		}
	}	
	LeaveCriticalSection(&cs);//임계영역 해제
}

//게임방에서 퇴장 할 경우 실행하는 함수
void ChattingOutProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);//임계영역 설정

	_GameInfo* _game = _clientinfo->game_info;
	char msg[BUFSIZE];
	int size;

	MakeExitMessage(_clientinfo->nickname, msg);		//퇴장메세지 생성함수
	
	for (int i = 0; i < _game->player_count; i++)
	{
		if (_game->players[i] == _clientinfo)	//종료
		{
			size = PackPacket(ClientInfo[i]->sendbuf, CHATT_OUT);
			if (send(ClientInfo[i]->sock, ClientInfo[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				ClientInfo[i]->state = GAME_END_STATE;
			}

		}
		else if (_game->players[i]->chatflag)//누가나갔는지 알려줌
		{
			size = PackPacket(ClientInfo[i]->sendbuf, CHATT_MSG, msg);
			if (send(ClientInfo[i]->sock, ClientInfo[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				ClientInfo[i]->state = GAME_END_STATE;
			}
		}
	}	

	_clientinfo->chatflag = false;

	RemoveNickName(_clientinfo->nickname);	//닉네임삭제

	LeaveCriticalSection(&cs);//임계영역 해제

}