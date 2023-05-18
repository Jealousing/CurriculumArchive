#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

//서버ip,port
#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

#define BUFSIZE 4096				//버퍼사이즈
#define NICKNAMESIZE 255		//닉네임사이즈
#define MAXUSER   10					//최대 유저수
#define NODATA   -1					//데이터x
#define MAX_CHATT_LIST 3		//최대 채팅에 참여 할수 있는 유저
#define MAX_CHATT_GROUP 3		//최대 채팅방 갯수

//msg 설정
#define SELECT_CHATT_ROOM_MSG "채팅방을 선택하세요\r\n1. 박용택방\r\n2. 오지환방\r\n3. 이형종방"		
#define SELECT_CHATT_ROOM_FULL_MSG "선택한 채팅방의 인원이 모두 찼습니다."
#define SELECT_CHATT_ROOM_ERROR_MSG "없는 채팅방입니다."
#define INTRO_MSG "채팅프로그램입니다. 닉네임을 입력하세요"
#define NICKNAME_ERROR_MSG "이미있는 닉네임입니다."

//채팅방 설정
const char* RoomList[] = { nullptr,"1. 박용택방","2. 오지환방", "3. 이형종방" };

//스테이트(진행위치) 정의
enum STATE
{
	INITE_STATE = 1,							//접속처리 부분
	SET_NICNAME_STATE,					//닉네임설정
	SELECT_CHATT_ROOM_STATE,	//채팅방 선택
	CHATTING_STATE,						//채팅진행
	CONNECT_END_STATE,				//채팅연결 종료
	CHATT_RESELECT_STATE			//채팅방 재선택
};

// protocol 정의
enum PROTOCOL
{
	INTRO = 1,											//인트로
	CHATT_NICKNAME,							//닉네임 설정
	NICKNAME_ERROR,							//닉네임 에러
	SELECT_CHATT_ROOM,					//채팅방 선택
	SELECT_CHATT_ROOM_ERROR,		//채팅방 에러
	SELECT_CHATT_ROOM_NUMBER,		//채팅방 숫자
	NICKNAME_LIST,								//채팅에 참가한 닉네임들
	CHATT_MSG,										//채팅 메세지
	CHATT_OUT,										//채팅프로그렘 종료
	CHATT_RESELECT,							//채팅방 다른방으로 가기위한 퇴장
};

//채팅방 선택 에러 정의
enum SELECT_ERROR_CODE
{
	SELECT_ROOM_FULL = 1,					//꽉참
	SELECT_ROOM_NUMBER_ERROR		//있는 방의 숫자가 아님
};

//클라이언트 정보 구조체
struct _ClientInfo
{
	SOCKET sock;									//소켓정보
	SOCKADDR_IN clientaddr;				//클라이언트 주소
	STATE state;									//스테이트(진행상황)
	bool ReSelect = false;						//채팅방 재설정용
	int chatt_room_number;					//참가한 채팅방
	char  nickname[NICKNAMESIZE];		//닉네임
	char  sendbuf[BUFSIZE];					//보내기용 버퍼
	char  recvbuf[BUFSIZE];					//받기용 버퍼
	
};

//채팅방 그룹
struct _ChattGroupInfo
{
	_ClientInfo* chattList[MAX_CHATT_LIST];			//채팅에 참가한 클라이언트 정보
	const char* chatt_name;										//채팅방 이름
	const char* nickname_list[MAX_CHATT_LIST];	//채팅방에 참가한 닉네임 리스트
	bool full;																//꽉찼는지 확인
	int chat_count;														//참가인원
};

_ClientInfo* ClientInfo[MAXUSER];											//클라정보
char* NickNameList[MAXUSER];												//닉네임정보
_ChattGroupInfo* ChattGroupList[MAX_CHATT_GROUP + 1];	//채팅 그룹

int Client_Count = 0;				//클라 접속 수
int Nick_Count = 0;				//닉네임 갯수

CRITICAL_SECTION cs;			//임계영역
FD_SET Rset, Wset;				//select


_ClientInfo* SearchClient(const char*);									//클라이언트 찾기
bool CheckNickname(const char*);											//있는 닉네임인지 확인
bool CheckRoomNumber(int);													//채팅방 있는지 확인
void MaKeChattMessage(const char* , const char* , char* );	//채팅메세지 생성
void MakeEnterMessage(const char* , char* );						//입장용 메세지 생성
void MakeExitMessage(const char* , char* );							//퇴장용 메세지 생성

DWORD CALLBACK ProcessClient(LPVOID);			//쓰레드용
_ClientInfo* AddClient(SOCKET, SOCKADDR_IN);	//클라이언트 정보 추가
void RemoveClient(_ClientInfo*);							//클라이언트 정보삭제

bool EnterChattGroup(_ClientInfo*, int);				//채팅방에 입장
void ExitChattInfo(_ClientInfo*);							//채팅방에서 나감


void AddNickName(const char*);							//닉네임 추가
void RemoveNickName(const char*);					//닉네임 삭제

void InitializeChattRoom();									//채팅방 초기화

bool NickNameSetting(_ClientInfo*);					//닉네임 세팅
void ChattingMessageProcess(_ClientInfo*);		//채팅뿌리기
void ChattingOutProcess(_ClientInfo*);				//채팅방나감
void ChattingEnterProcess(_ClientInfo*);			//채팅방들어옴

// 오류 출력 함수
void err_quit(const char* msg);
void err_display(const char* msg);
//사용자 정의함수 데이터 수신용
int recvn(SOCKET s, char* buf, int len, int flags);
//사용자 정의함수 recvn을 2번 사이즈->데이터 
bool PacketRecv(SOCKET, char*);
//데이터를 packing 
int PackPacket(char*, PROTOCOL, const char*);
int PackPacket(char*, PROTOCOL, int, const char*);
int PackPacket(char*, PROTOCOL);
int PackPacket(char*, PROTOCOL, const char**, int);
//데이터에서 protocol얻어옴
PROTOCOL GetProtocol(const char*);
//packing된 데이터 unpacking
void UnPackPacket(const char*, char*);
void UnPackPacket(const char*, int&);



int main(int argc, char **argv)
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSAStartup() error!");

	//크리티컬섹션 초기화
	InitializeCriticalSection(&cs);

	// socket()
	SOCKET listen_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket() error");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr =inet_addr(SERVERIP);
	if (bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
		err_quit("bind() error");
	// listen()
	if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR)
		err_quit("listen() error");
	
	// 데이터 통신에 사용할 변수		
	int addrlen;
	int retval;
	SOCKET sock;
	SOCKADDR_IN clientaddr;	

	InitializeChattRoom();//채팅방 초기화
	
	while (1)
	{
		FD_SET(listen_sock, &Rset); //검사 세팅

		if (select(0, &Rset, &Wset, 0, NULL) == SOCKET_ERROR) //오류잡기
		{
			err_quit("select() error");
		}

		if (FD_ISSET(listen_sock, &Rset)) //세팅 확인
		{
			//주소길이 
			addrlen = sizeof(clientaddr);
			//소켓정보
			sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (sock == INVALID_SOCKET)
			{
				err_display("accept()");
				continue;
			}
			//클라이언트 추가
			_ClientInfo* ptr = AddClient(sock, clientaddr);
			//쓰레드만들기
			HANDLE hThread = CreateThread(NULL, 0, ProcessClient, ptr, 0, nullptr);
			if (hThread != NULL)
			{
				CloseHandle(hThread);
			}
			continue;
		}

	}
	//소켓종료
	closesocket(listen_sock);
	//삭제임계영역
	DeleteCriticalSection(&cs);
	WSACleanup();
	return 0;
}

//쓰레드용 클라이언트통신
DWORD CALLBACK ProcessClient(LPVOID  _ptr)
{
	//연결된 클라
	_ClientInfo* Client_ptr = (_ClientInfo*)_ptr;
		
	int size;	//데이터 사이즈
	PROTOCOL protocol;	//프로토콜

	bool breakflag = false;	//종료플레그

	while (1)
	{
		//초기화
		FD_ZERO(&Rset);
		FD_ZERO(&Wset);

		//rest wset 세팅
		FD_SET(Client_ptr->sock, &Rset);
		switch (Client_ptr->state)
		{
		case INITE_STATE:
		case SELECT_CHATT_ROOM_STATE:
		case CHATTING_STATE:
		case CONNECT_END_STATE:
		case CHATT_RESELECT_STATE:
			FD_SET(Client_ptr->sock, &Wset);
			break;
		}

		//오류검사
		if (select(0, &Rset, &Wset, 0, NULL) == SOCKET_ERROR)
		{
			err_quit("select() error");
		}


		if (FD_ISSET(Client_ptr->sock, &Rset))				//Rset가 세팅되어있는지 확인 
		{
			switch (Client_ptr->state)
			{
			case SET_NICNAME_STATE://닉네임 세팅
				if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))//데이터받기
				{
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}

				protocol = GetProtocol(Client_ptr->recvbuf);//프로토콜받아옴

				switch (protocol) //세팅및 state 설정
				{
				case CHATT_NICKNAME:
					if (Client_ptr->state == CONNECT_END_STATE || !NickNameSetting(Client_ptr))
					{
						break;
					}
					else
					{
						Client_ptr->state = SELECT_CHATT_ROOM_STATE;
					}
					break;
				}
				break;


			}
		}

		if (FD_ISSET(Client_ptr->sock, &Wset)) //wset이 세팅되어있는지 확인
		{
			switch (Client_ptr->state)
			{
			case INITE_STATE://접속시작
				//인트로 메세지 보냄
				size = PackPacket(Client_ptr->sendbuf, INTRO, INTRO_MSG);
				if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
				{
					err_display("intro Send()");
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}
				//스테이트 설정
				Client_ptr->state = SET_NICNAME_STATE;
				break;
			case SELECT_CHATT_ROOM_STATE://채팅방 선택
			{
				//채팅방 선택 메세지보냄
				size = PackPacket(Client_ptr->sendbuf, SELECT_CHATT_ROOM, SELECT_CHATT_ROOM_MSG);
				if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
				{
					err_display("intro Send()");
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}
				//데이터 받기
				if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
				{
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}
				//프로토콜 받기
				protocol = GetProtocol(Client_ptr->recvbuf);
				int room_number = NODATA; //선택방

				switch (protocol)
				{
				case SELECT_CHATT_ROOM_NUMBER: //선택한 채팅방연결
					UnPackPacket(Client_ptr->recvbuf, room_number); //unpacking
					if (!CheckRoomNumber(room_number))//채팅방 존재 확인
					{
						size = PackPacket(Client_ptr->sendbuf, SELECT_CHATT_ROOM_ERROR, SELECT_ROOM_NUMBER_ERROR, SELECT_CHATT_ROOM_ERROR_MSG);
						if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
						{
							err_display("intro Send()");
							Client_ptr->state = CONNECT_END_STATE;
							break;
						}
						break;
					}

					if (!EnterChattGroup(Client_ptr, room_number))//채팅방이 꽉차있는지 확인
					{
						size = PackPacket(Client_ptr->sendbuf, SELECT_CHATT_ROOM_ERROR, SELECT_ROOM_FULL, SELECT_CHATT_ROOM_FULL_MSG);
						if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
						{
							err_display("intro Send()");
							Client_ptr->state = CONNECT_END_STATE;
							break;
						}
						break;
					}

					//닉네임리스트에 자기 닉네임 추가
					const char** nickname_list = ChattGroupList[Client_ptr->chatt_room_number]->nickname_list;
					int chatt_count = ChattGroupList[Client_ptr->chatt_room_number]->chat_count;

					//참가한 방의 클라이언트의 닉네임 리스트 보내기
					size = PackPacket(Client_ptr->sendbuf, NICKNAME_LIST, nickname_list, chatt_count);
					if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
					{
						err_display("nickname list Send()");
						Client_ptr->state = CONNECT_END_STATE;
						break;
					}

					//참가
					ChattingEnterProcess(Client_ptr);
					//스테이트 설정
					Client_ptr->state = CHATTING_STATE;
					break;
				}
			}
			break;

			case CHATTING_STATE: // 채팅 진행
				//데이터 받기
				if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
				{
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}

				//프로토콜 받기
				protocol = GetProtocol(Client_ptr->recvbuf);

				switch (protocol)
				{
				case CHATT_MSG://일반 메세지 보냄
					ChattingMessageProcess(Client_ptr);//메세지통신
					break;
				case CHATT_OUT://채팅방 나감
					ChattingOutProcess(Client_ptr);//채팅방나감
					break;
				case CHATT_RESELECT://채팅방 재설정
					Client_ptr->ReSelect = true;//재시작 알림
					ChattingOutProcess(Client_ptr);//채팅방나감
					Client_ptr->state = CHATT_RESELECT_STATE;//재설정 스테이트
					break;
				}
				break;
			case CONNECT_END_STATE: //연결종료
				ExitChattInfo(Client_ptr);//채팅나감
				RemoveClient(Client_ptr);//클라이언트 정보삭제
				breakflag = true;
				break;
			case CHATT_RESELECT_STATE:
				ExitChattInfo(Client_ptr);//채팅나감
				Client_ptr->state = INITE_STATE;//처음스테이트로 돌아감
				break;
			}
		}
		//쓰레드종료
		if (breakflag)
		{
			break;
		}
	}
	return 0;
}

// recvn(사용자 정의 데이터 수신함수)를 2번 반복 사이즈->데이터
bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;

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
//데이터 Packing
//버퍼에 프로토콜+string
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1)
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
//버퍼에 프로토콜+data+string
int PackPacket(char* _buf, PROTOCOL _protocol, int _data, const char* _str1)
{
	char* ptr = _buf;
	int size = 0;
	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(size);

	memcpy(ptr, &_data, sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

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
//버퍼에 프로토콜
int PackPacket(char* _buf, PROTOCOL _protocol)
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
//버퍼에 프로토콜+stringlist,count
int PackPacket(char* _buf, PROTOCOL _protocol, const char** _strlist, int _count)
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

//프로토콜 받음
PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}

//데이터 unpacking
//버퍼에서 string 분리
void UnPackPacket(const char* _buf, char* _str)
{
	int strsize;
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
	ptr = ptr + strsize;
}
//버퍼에서 data 분리
void UnPackPacket(const char* _buf, int& _data)
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

}

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg)
{
	LPVOID lpMsgbuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgbuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgbuf, msg, MB_ICONERROR);
	LocalFree(lpMsgbuf);
	exit(-1);
}
// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgbuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgbuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgbuf);
	LocalFree(lpMsgbuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0){
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

//클라이언트 정보 추가
_ClientInfo* AddClient(SOCKET sock, SOCKADDR_IN clientaddr)
{
	//접속알림(서버에)
	printf("\nClient 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port));

	EnterCriticalSection(&cs); //임계영역 진입

	//클라이언트 정보 생성 및 입력
	_ClientInfo* ptr = new _ClientInfo;
	ZeroMemory(ptr, sizeof(_ClientInfo));
	ptr->sock = sock;
	memcpy(&(ptr->clientaddr), &clientaddr, sizeof(clientaddr));
	ptr->state = INITE_STATE;		
	ptr->chatt_room_number = NODATA;
	ClientInfo[Client_Count++] = ptr;
	
	LeaveCriticalSection(&cs); //임계영역 퇴장
	return ptr;
}

//클라이언트 정보 삭제
void RemoveClient(_ClientInfo* ptr)
{
	//소켓 닫기
	closesocket(ptr->sock);

	//종료 클라이언트 정보 서버에 출력
	printf("\nClient 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(ptr->clientaddr.sin_addr),
		ntohs(ptr->clientaddr.sin_port));

	EnterCriticalSection(&cs); //임계영역 진입

	//클라이언트 정렬
	for (int i = 0; i < Client_Count; i++)
	{
		if (ClientInfo[i] == ptr)
		{			
			delete ptr;
			int j;
			for (j = i; j < Client_Count - 1; j++)
			{
				ClientInfo[j] = ClientInfo[j + 1];
			}
			ClientInfo[j] = nullptr;
			break;
		}
	}

	Client_Count--; //접속한 클라이언트 수 감소
	LeaveCriticalSection(&cs);//임계영역 탈출
}

//채팅그룹 참가
bool EnterChattGroup(_ClientInfo* _ptr, int _roomnumber)
{
	EnterCriticalSection(&cs);//임계영역 진입
	
	bool enterchattroom = false;

	_ChattGroupInfo* chattgroup = ChattGroupList[_roomnumber];

	if (!chattgroup->full) //꽉차있지 않으면
	{
		//채팅그룹에 설정
		chattgroup->chattList[chattgroup->chat_count] = _ptr;//리스트에 정보추가
		_ptr->chatt_room_number = _roomnumber;//방번호
		chattgroup->nickname_list[chattgroup->chat_count] = _ptr->nickname;//닉네임설정
		chattgroup->chat_count++;//인원증가

		if (chattgroup->chat_count == MAX_CHATT_LIST)
		{//꽉찼으면 알림
			chattgroup->full = true;
		}

		enterchattroom = true;
	}

	LeaveCriticalSection(&cs);//임계영역 종료
	return enterchattroom;
}

//채팅그룹에서 나감
void ExitChattInfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);//임계영역 설정
	_ChattGroupInfo* chattgroup = ChattGroupList[_ptr->chatt_room_number];

	//정렬
	for (int i = 0; i < chattgroup->chat_count; i++)
	{
		if (chattgroup->chattList[i] == _ptr)
		{
			for (int j = i; j < chattgroup->chat_count - 1; j++)
			{
				chattgroup->chattList[j] = chattgroup->chattList[j + 1];
				chattgroup->nickname_list[j] = chattgroup->nickname_list[j + 1];
			}

			chattgroup->chat_count--;//수감소
			chattgroup->full=false;//풀아니라고 알림
			break;
		}
	}

	_ptr->chatt_room_number = NODATA;//연결된 채팅방 해제

	LeaveCriticalSection(&cs);//임계영역 탈출
}

//채팅방 초기화
void InitializeChattRoom()
{
	for (int i = 1; i <= MAX_CHATT_GROUP; i++)
	{
		ChattGroupList[i] = new _ChattGroupInfo;
		ZeroMemory(ChattGroupList[i], sizeof(_ChattGroupInfo));
		ChattGroupList[i]->chatt_name = RoomList[i];
		ChattGroupList[i]->full = false;		
	}
}
//클라 찾기
_ClientInfo* SearchClient(const char* _nick)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < Client_Count; i++)
	{
		if (!strcmp(ClientInfo[i]->nickname, _nick))
		{
			LeaveCriticalSection(&cs);
			return ClientInfo[i];
		}
	}
	LeaveCriticalSection(&cs);
	return nullptr;
}

//닉네임 체크(중복검사)
bool CheckNickname(const char* _nick)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < Nick_Count; i++)
	{
		if (!strcmp(NickNameList[i], _nick))
		{
			LeaveCriticalSection(&cs);
			return false;
		}
	}
	LeaveCriticalSection(&cs);

	return true;
}

//채팅방이 존재하는 번호인지 확인
bool CheckRoomNumber(int _number)
{
	if (_number< 1 || _number > MAX_CHATT_GROUP)//최대 번호까지
	{
		return false;
	}
	return true;
}

//닉네임 추가
void AddNickName(const char* _nick)
{
	EnterCriticalSection(&cs);
	char* ptr = new char[strlen(_nick) + 1];
	strcpy(ptr, _nick);
	NickNameList[Nick_Count++] = ptr;//닉네임 리스트에 저장
	LeaveCriticalSection(&cs);
}

//닉네임 삭제
void RemoveNickName(const char* _nick)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < Nick_Count; i++)
	{
		//정렬
		if (!strcmp(NickNameList[i], _nick))
		{
			delete[] NickNameList[i];

			for (int j = i; j < Nick_Count - 1; j++)
			{
				NickNameList[j] = NickNameList[j + 1];
			}
			NickNameList[Nick_Count--] = nullptr;			
			break;
		}
	}
	LeaveCriticalSection(&cs);
}

//메세지 생성 함수들
//닉: 채팅
void MaKeChattMessage(const char* _nick, const char* _msg, char* _chattmsg)
{
	sprintf(_chattmsg, "[ %s ] %s", _nick, _msg);
}
//~~입장
void MakeEnterMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s님이 입장하셨습니다.", _nick);
}
//~~퇴장
void MakeExitMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s님이 퇴장하셨습니다.", _nick);
}

//닉네임 세팅
bool NickNameSetting(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	UnPackPacket(_clientinfo->recvbuf, msg);
	if (!CheckNickname(msg))//닉네임 체크
	{
		size = PackPacket(_clientinfo->sendbuf, NICKNAME_ERROR, NICKNAME_ERROR_MSG);
		if (send(_clientinfo->sock, _clientinfo->sendbuf, size, 0) == SOCKET_ERROR)
		{
			err_display("nickname error Send()");			
			_clientinfo->state = CONNECT_END_STATE;					
		}
		LeaveCriticalSection(&cs);
		return false;
	}
	else
	{
		strcpy(_clientinfo->nickname, msg);
		AddNickName(_clientinfo->nickname);			//닉네임추가
	}	
	LeaveCriticalSection(&cs);	
	return true;
}

//채팅 뿌리기(메세지 전달)
void ChattingMessageProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	char temp[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	memset(temp, 0, sizeof(temp));

	UnPackPacket(_clientinfo->recvbuf, msg);//들어있는 메세지를
	MaKeChattMessage(_clientinfo->nickname, msg, temp);//채팅형 메세지로 바꿈

	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)//같은 채팅그룹인 클라에게 다 전달
	{
		size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_MSG, temp);
		if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
		{
			err_display("chatting message Send()");
			chattgroup->chattList[i]->state = CONNECT_END_STATE;
		}
	}

	LeaveCriticalSection(&cs);
}

// 채팅방 입장 알림
void ChattingEnterProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	MakeEnterMessage(_clientinfo->nickname, msg);//입장메세지
	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];
	for (int i = 0; i < chattgroup->chat_count; i++)//같은방 참가한 사람들에게 입장알림
	{
		size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_MSG, msg);
		if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
		{
			err_display("chatting message Send()");
			chattgroup->chattList[i]->state = CONNECT_END_STATE;
		}
	}	

	LeaveCriticalSection(&cs);
}

//채팅방 나감 알림
void ChattingOutProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	MakeExitMessage(_clientinfo->nickname, msg); //채팅방 퇴장알림

	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)
	{
		if (chattgroup->chattList[i] == _clientinfo)
		{
			
			if (_clientinfo->ReSelect)//재설정용
			{
				_clientinfo->ReSelect = false;
				size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_RESELECT);
			}
			else//퇴장용
			{
				size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_OUT);
			}
			
			if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				chattgroup->chattList[i]->state = CONNECT_END_STATE;
			}

		}
		else //당사자 제외 
		{
			size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_MSG, msg);
			if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				chattgroup->chattList[i]->state = CONNECT_END_STATE;
			}
		}
	}	

	RemoveNickName(_clientinfo->nickname); // 닉네임 제거

	LeaveCriticalSection(&cs);

}
