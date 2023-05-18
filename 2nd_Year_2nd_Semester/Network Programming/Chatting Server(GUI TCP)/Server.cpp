#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

#define BUFSIZE 4096
#define NICKNAMESIZE 255
#define MAXUSER   10
#define NODATA   -1
#define MAX_CHATT_LIST 3
#define MAX_CHATT_GROUP 3

#define SELECT_CHATT_ROOM_MSG "채팅방을 선택하세요\r\n1. 박용택방\r\n2. 오지환방\r\n3. 이형종방"
#define SELECT_CHATT_ROOM_FULL_MSG "선택한 채팅방의 인원이 모두 찼습니다."
#define SELECT_CHATT_ROOM_ERROR_MSG "없는 채팅방입니다."
#define INTRO_MSG "채팅프로그램입니다. 닉네임을 입력하세요"
#define NICKNAME_ERROR_MSG "이미있는 닉네임입니다."

const char* RoomList[] = { nullptr,"1. 박용택방","2. 오지환방", "3. 이형종방" };

enum STATE
{
	INITE_STATE = 1,
	SET_NICNAME_STATE,
	SELECT_CHATT_ROOM_STATE,
	CHATTING_STATE,
	CONNECT_END_STATE
};

enum PROTOCOL
{
	INTRO = 1,
	CHATT_NICKNAME,
	NICKNAME_ERROR, 
	SELECT_CHATT_ROOM,
	SELECT_CHATT_ROOM_ERROR,
	SELECT_CHATT_ROOM_NUMBER,
	NICKNAME_LIST,	
	CHATT_MSG,
	CHATT_OUT,

};

enum SELECT_ERROR_CODE
{
	SELECT_ROOM_FULL=1,
	SELECT_ROOM_NUMBER_ERROR
};

struct _ClientInfo
{
	SOCKET sock;
	SOCKADDR_IN clientaddr;
	STATE state;	
	int chatt_room_number;
	char  nickname[NICKNAMESIZE];
	char  sendbuf[BUFSIZE];
	char  recvbuf[BUFSIZE];
	
};

struct _ChattGroupInfo
{
	_ClientInfo* chattList[MAX_CHATT_LIST];
	const char* chatt_name;
	const char* nickname_list[MAX_CHATT_LIST];
	bool full;
	int chat_count;
};

DWORD CALLBACK ProcessClient(LPVOID);

void err_quit(const char *msg);
void err_display(const char *msg);
int recvn(SOCKET s, char *buf, int len, int flags);

_ClientInfo* SearchClient(const char*);
bool CheckNickname(const char*);
bool CheckRoomNumber(int);
void MaKeChattMessage(const char* , const char* , char* );
void MakeEnterMessage(const char* , char* );
void MakeExitMessage(const char* , char* );


_ClientInfo* AddClient(SOCKET, SOCKADDR_IN);
void RemoveClient(_ClientInfo*);

bool EnterChattGroup(_ClientInfo*, int);
void ExitChattInfo(_ClientInfo*);


void AddNickName(const char*);
void RemoveNickName(const char*);

void InitializeChattRoom();

bool PacketRecv(SOCKET, char*);

int PackPacket(char*, PROTOCOL, const char*);
int PackPacket(char*, PROTOCOL, int, const char*);
int PackPacket(char*, PROTOCOL);

int PackPacket(char*, PROTOCOL, const char**, int);

PROTOCOL GetProtocol(const char*);

void UnPackPacket(const char*, char*);
void UnPackPacket(const char*, int&);

bool NickNameSetting(_ClientInfo*);
void ChattingMessageProcess(_ClientInfo*);
void ChattingOutProcess(_ClientInfo*);
void ChattingEnterProcess(_ClientInfo*);

_ClientInfo* ClientInfo[MAXUSER];
char* NickNameList[MAXUSER];
_ChattGroupInfo* ChattGroupList[MAX_CHATT_GROUP+1];

int Client_Count = 0;
int Nick_Count = 0;

CRITICAL_SECTION cs;

int main(int argc, char **argv)
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;
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

	InitializeChattRoom();
	
	while (1)
	{
		addrlen = sizeof(clientaddr);

		sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (sock == INVALID_SOCKET)
		{
			err_display("accept()");
			continue;
		}

		_ClientInfo* ptr = AddClient(sock, clientaddr);
		
		HANDLE hThread=CreateThread(NULL, 0, ProcessClient, ptr, 0, nullptr);	
		if (hThread != NULL)
		{
			CloseHandle(hThread);
		}

	}

	closesocket(listen_sock);
	DeleteCriticalSection(&cs);
	WSACleanup();
	return 0;
}


DWORD CALLBACK ProcessClient(LPVOID  _ptr)
{
	_ClientInfo* Client_ptr = (_ClientInfo*)_ptr;
		
	int size;	
	PROTOCOL protocol;

	bool breakflag = false;

	while (1)
	{

		switch (Client_ptr->state)
		{
		case INITE_STATE:
			size = PackPacket(Client_ptr->sendbuf, INTRO, INTRO_MSG);
			if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("intro Send()");
				Client_ptr->state = CONNECT_END_STATE;
				break;
			}
			Client_ptr->state = SET_NICNAME_STATE;
			break;
		case SET_NICNAME_STATE:
			if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
			{
				Client_ptr->state = CONNECT_END_STATE;
				break;
			}

			protocol = GetProtocol(Client_ptr->recvbuf);

			switch (protocol)
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
			
		case SELECT_CHATT_ROOM_STATE:
			{
				size = PackPacket(Client_ptr->sendbuf, SELECT_CHATT_ROOM, SELECT_CHATT_ROOM_MSG);
				if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
				{
					err_display("intro Send()");
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}

				if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
				{
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}

				protocol = GetProtocol(Client_ptr->recvbuf);
				int room_number = NODATA;

				switch (protocol)
				{
				case SELECT_CHATT_ROOM_NUMBER:
					UnPackPacket(Client_ptr->recvbuf, room_number);
					if (!CheckRoomNumber(room_number))
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

					if (!EnterChattGroup(Client_ptr, room_number))
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

					const char** nickname_list = ChattGroupList[Client_ptr->chatt_room_number]->nickname_list;
					int chatt_count = ChattGroupList[Client_ptr->chatt_room_number]->chat_count;

					size = PackPacket(Client_ptr->sendbuf, NICKNAME_LIST, nickname_list, chatt_count);
					if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
					{
						err_display("nickname list Send()");
						Client_ptr->state = CONNECT_END_STATE;
						break;
					}

					ChattingEnterProcess(Client_ptr);

					Client_ptr->state = CHATTING_STATE;
					break;
				}

			}
			
			break;
			
		case CHATTING_STATE:
			if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
			{
				Client_ptr->state = CONNECT_END_STATE;
				break;
			}

			protocol = GetProtocol(Client_ptr->recvbuf);

			switch (protocol)
			{
			case CHATT_MSG:
				ChattingMessageProcess(Client_ptr);
				break;
			case CHATT_OUT:
				ChattingOutProcess(Client_ptr);	
				break;
			}
			break;
		case CONNECT_END_STATE:
			ExitChattInfo(Client_ptr);
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

PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}


void UnPackPacket(const char* _buf, char* _str)
{
	int strsize;
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
	ptr = ptr + strsize;
}

void UnPackPacket(const char* _buf, int& _data)
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

}
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

// 소켓 함수 오류 출력
void err_display(const char *msg)
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


_ClientInfo* AddClient(SOCKET sock, SOCKADDR_IN clientaddr)
{
	printf("\nClient 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port));

	EnterCriticalSection(&cs);
	_ClientInfo* ptr = new _ClientInfo;
	ZeroMemory(ptr, sizeof(_ClientInfo));
	ptr->sock = sock;
	memcpy(&(ptr->clientaddr), &clientaddr, sizeof(clientaddr));
	ptr->state = INITE_STATE;		
	ptr->chatt_room_number = NODATA;
	ClientInfo[Client_Count++] = ptr;
	
	LeaveCriticalSection(&cs);
	return ptr;
}

void RemoveClient(_ClientInfo* ptr)
{
	closesocket(ptr->sock);

	printf("\nClient 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(ptr->clientaddr.sin_addr),
		ntohs(ptr->clientaddr.sin_port));

	EnterCriticalSection(&cs);

	
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

	Client_Count--;
	LeaveCriticalSection(&cs);
}

bool EnterChattGroup(_ClientInfo* _ptr, int _roomnumber)
{
	EnterCriticalSection(&cs);
	
	bool enterchattroom = false;

	_ChattGroupInfo* chattgroup = ChattGroupList[_roomnumber];

	if (!chattgroup->full)
	{
		chattgroup->chattList[chattgroup->chat_count] = _ptr;
		_ptr->chatt_room_number = _roomnumber;
		chattgroup->nickname_list[chattgroup->chat_count] = _ptr->nickname;
		chattgroup->chat_count++;

		if (chattgroup->chat_count == MAX_CHATT_LIST)
		{
			chattgroup->full = true;
		}

		enterchattroom = true;
	}

	LeaveCriticalSection(&cs);
	return enterchattroom;
}

void ExitChattInfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);
	_ChattGroupInfo* chattgroup = ChattGroupList[_ptr->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)
	{
		if (chattgroup->chattList[i] == _ptr)
		{
			for (int j = i; j < chattgroup->chat_count - 1; j++)
			{
				chattgroup->chattList[j] = chattgroup->chattList[j + 1];
				chattgroup->nickname_list[j] = chattgroup->nickname_list[j + 1];
			}

			chattgroup->chat_count--;
			chattgroup->full=false;
			break;
		}
	}

	_ptr->chatt_room_number = NODATA;

	LeaveCriticalSection(&cs);
}

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

bool CheckRoomNumber(int _number)
{
	if (_number< 1 || _number > MAX_CHATT_GROUP)
	{
		return false;
	}
	return true;
}


void AddNickName(const char* _nick)
{
	EnterCriticalSection(&cs);
	char* ptr = new char[strlen(_nick) + 1];
	strcpy(ptr, _nick);
	NickNameList[Nick_Count++] = ptr;
	LeaveCriticalSection(&cs);
}

void RemoveNickName(const char* _nick)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < Nick_Count; i++)
	{
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

void MaKeChattMessage(const char* _nick, const char* _msg, char* _chattmsg)
{
	sprintf(_chattmsg, "[ %s ] %s", _nick, _msg);
}

void MakeEnterMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s님이 입장하셨습니다.", _nick);
}
void MakeExitMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s님이 퇴장하셨습니다.", _nick);
}


bool NickNameSetting(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	UnPackPacket(_clientinfo->recvbuf, msg);
	if (!CheckNickname(msg))
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
		AddNickName(_clientinfo->nickname);			
	}	
	LeaveCriticalSection(&cs);	
	return true;
}

void ChattingMessageProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	char temp[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	memset(temp, 0, sizeof(temp));

	UnPackPacket(_clientinfo->recvbuf, msg);
	MaKeChattMessage(_clientinfo->nickname, msg, temp);

	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)
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

void ChattingEnterProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	MakeEnterMessage(_clientinfo->nickname, msg);

	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)
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

void ChattingOutProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	MakeExitMessage(_clientinfo->nickname, msg);

	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)
	{
		if (chattgroup->chattList[i] == _clientinfo)
		{
			size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_OUT);
			if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				chattgroup->chattList[i]->state = CONNECT_END_STATE;
			}

		}
		else
		{
			size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_MSG, msg);
			if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				chattgroup->chattList[i]->state = CONNECT_END_STATE;
			}
		}
	}	

	RemoveNickName(_clientinfo->nickname);

	LeaveCriticalSection(&cs);

}