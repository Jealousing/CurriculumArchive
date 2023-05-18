#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> 
#include <time.h>
#include <tchar.h>

#define SERVERPORT 9000
#define BUFSIZE    4096
#define GAME_OVER_NUMBER 31
#define PLAYER_COUNT 3

#define WAIT_MSG "다른 Player를 기다립니다. 잠시기다려주세요........\n"
#define INTRO_MSG "31숫자게임입니다.1부터 시작합니다.\n당신차례를 기다립니다....\n"
#define CLIENT_TURN_MSG "당신차례입니다.한번에 3개까지 선택할 수 있습니다 선택하세요:"
#define COUNT_ERROR_MSG "잘못 선택했습니다. 1~3개까지 선택가능합니다.\n"
#define WIN_MSG "번 Player가 졌습니다!!짝!짝!짝\n"
#define LOSE_MSG "당신이 졌습니다.ㅠ.ㅠ\n"

enum CLIENT_STATE
{
	C_INIT_STATE = 1,
	C_CLIENT_TURN_STATE,
	C_OTHER_TURN_STATE,
	C_GAME_OVER_STATE,
	C_DISCONNECTED_STATE
};

enum PROTOCOL
{
	WAIT=1,
	INTRO,
	PLAYER_INFO,
	SELECT_COUNT,	
	COUNT_VALUE,	
	CLIENT_TURN,
	EXIT_PLAYER_INFO,
	INVALID_GAME,
	GAME_ERROR,
	GAME_RESULT
};

enum RESULT
{
	NODATA = -1,
	WIN = 1,
	LOSE	
};


enum ERROR_CODE
{
	SELECT_COUNT_ERROR = 1
};

struct _ClientInfo;

enum GAME_STATE
{	
	G_WAIT_STATE=1,	
	G_PLAYING_STATE,
	G_GAME_OVER_STATE,	
	G_INVALID_GAME_STATE
};

struct _GameInfo
{
	int game_number;
	HANDLE start_event;
	HANDLE wait_event;
	GAME_STATE state;
	_ClientInfo* players[PLAYER_COUNT];
	_ClientInfo* cur_player;
	_ClientInfo* lose_player;
	int player_count;	
	bool full;
};

struct _ClientInfo
{
	SOCKET sock;
	SOCKADDR_IN addr;
	CLIENT_STATE state;
	int player_number;	
	HANDLE turn_event;		
	_GameInfo* game_info;
	RESULT game_result;	
	char recv_buf[BUFSIZE];
	char send_buf[BUFSIZE];
};

_ClientInfo* ClientInfo[100];
int Count = 0;

_GameInfo* GameInfo[100];
int GameCount = 0;

CRITICAL_SECTION cs;

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg);
// 소켓 함수 오류 출력
void err_display(char* msg);

_ClientInfo* AddClientInfo(SOCKET, SOCKADDR_IN);

void ReMoveClientInfo(_ClientInfo*);

_GameInfo* _AddGameInfo(_ClientInfo*);

void ReMoveGameInfo(_GameInfo*);

int recvn(SOCKET, char*, int, int);

bool PacketRecv(SOCKET, char*);

PROTOCOL GetProtocol(char*);

int PackPacket(char*, PROTOCOL, const char*);

int PackPacket(char*, PROTOCOL, int, const char*);

int PackPacket(char*, PROTOCOL, int, int*);

void UnPackPacket(char*, int&);

DWORD CALLBACK ClientThread(LPVOID);

void InitProcess(_ClientInfo*);

void ClientTurnProcess(_ClientInfo*);

void OtherTurnProcess(_ClientInfo*);

void GameOverProcess(_ClientInfo*);

void DisconnectedProcess(_ClientInfo*);

bool GameContinueCheck(_ClientInfo*, int);

_ClientInfo* SearchNextClient(_ClientInfo*);

void ExitClientCheck(_ClientInfo*);

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	InitializeCriticalSection(&cs);

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;		

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		_ClientInfo* ClientPtr = AddClientInfo(client_sock, clientaddr);
		
		HANDLE hThread = CreateThread(nullptr, 0, ClientThread, ClientPtr, 0, nullptr);
		if (hThread == nullptr)
		{
			ReMoveClientInfo(ClientPtr);
			continue;
		}

		CloseHandle(hThread);		
	}

	closesocket(listen_sock);
	DeleteCriticalSection(&cs);
	// 윈속 종료
	WSACleanup();
	return 0;
}

void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

_ClientInfo* AddClientInfo(SOCKET sock, SOCKADDR_IN addr)
{
	EnterCriticalSection(&cs);
	_ClientInfo* ptr = new _ClientInfo;
	ZeroMemory(ptr, sizeof(_ClientInfo));
	ptr->sock = sock;
	memcpy(&ptr->addr, &addr, sizeof(addr));
	ptr->state = C_INIT_STATE;
	ptr->player_number = NODATA;
	ptr->game_result = NODATA;	

	ptr->turn_event = CreateEvent(nullptr, false, false, nullptr);

	ClientInfo[Count++] = ptr;

	LeaveCriticalSection(&cs);

	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));
	
	return ptr;
}

void ReMoveClientInfo(_ClientInfo* ptr)
{
	printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

	EnterCriticalSection(&cs);

	for (int i = 0; i < Count; i++)
	{
		if (ClientInfo[i] == ptr)
		{
			closesocket(ptr->sock);
			CloseHandle(ptr->turn_event);			
			delete ptr;
			for (int j = i; j < Count - 1; j++)
			{
				ClientInfo[j] = ClientInfo[j + 1];
			}
			break;
		}
	}

	Count--;
	LeaveCriticalSection(&cs);
}

_GameInfo* _AddGameInfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);

	_GameInfo* game_ptr = nullptr;
	int index = NODATA;

	for (int i = 0; i < GameCount; i++)
	{
		if (!GameInfo[i]->full)
		{
			GameInfo[i]->players[GameInfo[i]->player_count++] = _ptr;
			_ptr->game_info = GameInfo[i];
			_ptr->player_number = GameInfo[i]->player_count;			

			if (GameInfo[i]->player_count == PLAYER_COUNT)
			{
				GameInfo[i]->full = true;
				GameInfo[i]->state = G_PLAYING_STATE;
				GameInfo[i]->cur_player = GameInfo[i]->players[0];
				SetEvent(GameInfo[i]->start_event);
			}
			game_ptr = GameInfo[i];
			index = i;
			break;
		}
	}

	if (index == NODATA)
	{
		game_ptr = new _GameInfo;
		ZeroMemory(game_ptr, sizeof(_GameInfo));
		game_ptr->full = false;
		game_ptr->start_event = CreateEvent(nullptr, true, false, nullptr);
		game_ptr->wait_event = CreateEvent(nullptr, false, false, nullptr);
		game_ptr->players[0] = _ptr;
		game_ptr->lose_player= nullptr;
		game_ptr->cur_player=nullptr;

		_ptr->game_info = game_ptr;
		_ptr->player_number = 1;
		
		game_ptr->player_count++;
		game_ptr->state = G_WAIT_STATE;
		GameInfo[GameCount++] = game_ptr;		
	}
	
	LeaveCriticalSection(&cs);
	return game_ptr;
}

void ReMoveGameInfo(_GameInfo* _ptr)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < GameCount; i++)
	{
		if (GameInfo[i] == _ptr)
		{
			CloseHandle(_ptr->start_event);
			CloseHandle(_ptr->wait_event);
			delete _ptr;
			for (int j = i; j < GameCount - 1; j++)
			{
				GameInfo[j] = GameInfo[j + 1];
			}
			break;
		}
	}

	GameCount--;
	LeaveCriticalSection(&cs);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
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

bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;

	int retval = recvn(_sock, (char*)&size, sizeof(size), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv error()");
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

PROTOCOL GetProtocol(char* _buf)
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));

	return protocol;
}


int PackPacket(char* _buf, PROTOCOL  _protocol, const char* _str)
{
	int size = 0;
	char* ptr = _buf;
	int strsize = strlen(_str);


	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);


	memcpy(ptr, &strsize, sizeof(strsize));
	ptr = ptr + sizeof(strsize);
	size = size + sizeof(strsize);

	memcpy(ptr, _str, strsize);
	ptr = ptr + strsize;
	size = size + strsize;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

int PackPacket(char* _buf, PROTOCOL  _protocol, int _result, const char* _str)
{
	int size = 0;
	char* ptr = _buf;
	int strsize = strlen(_str);


	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_result, sizeof(_result));
	ptr = ptr + sizeof(_result);
	size = size + sizeof(_result);


	memcpy(ptr, &strsize, sizeof(strsize));
	ptr = ptr + sizeof(strsize);
	size = size + sizeof(strsize);

	memcpy(ptr, _str, strsize);
	ptr = ptr + strsize;
	size = size + strsize;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

int PackPacket(char* _buf, PROTOCOL _protocol, int _count, int* _data)
{
	char* ptr = _buf;
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
		memcpy(ptr, &_data[i], sizeof(_data[i]));
		ptr = ptr + sizeof(_data[i]);
		size = size + sizeof(_data[i]);
	}

	ptr = _buf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

void UnPackPacket(char* _buf, int& _data)
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(_data));
	ptr = ptr + sizeof(_data);
}

void MakeGameMessage(_ClientInfo* _ptr, int _count, int* _data, char* _msg)
{	
	char temp[3] = { 0 };

	_itoa(_ptr->player_number, temp, 10);
	strcpy(_msg, temp);
	strcat(_msg, "번 Player가 선택한 숫자는");

	for (int i = 0; i < _count; i++)
	{
		_itoa(_data[i], temp, 10);
		strcat(_msg, temp);
		strcat(_msg, "  ");
	}

	strcat(_msg, "입니다.\n");
}

bool GameContinueCheck(_ClientInfo* _ptr, int _game_number)
{
	EnterCriticalSection(&cs);
	_GameInfo* game = _ptr->game_info;

	if (_game_number == GAME_OVER_NUMBER)
	{		
		_ptr->game_result = LOSE;
		game->lose_player = _ptr;
		game->state = G_GAME_OVER_STATE;
		_ptr->state = C_GAME_OVER_STATE;
		for (int i = 0; i < game->player_count; i++)
		{
			if (game->players[i] != _ptr)
			{
				game->players[i]->game_result = WIN;
				SetEvent(game->players[i]->turn_event);
			}
		}
		LeaveCriticalSection(&cs);
		return false;
	}	
	LeaveCriticalSection(&cs);

	return true;
}

void ClientTurnProcess(_ClientInfo* _ptr)
{	
	_GameInfo* game = _ptr->game_info;
		
	if (!GameContinueCheck(_ptr, game->game_number+1))
	{		
		return;
	}	

	int size = PackPacket(_ptr->send_buf, CLIENT_TURN, CLIENT_TURN_MSG);
	int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("CLIENT_TURN_MSG send()");
		_ptr->state = C_DISCONNECTED_STATE;
		return;
	}

	if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
	{
		_ptr->state = C_DISCONNECTED_STATE;
		return;
	}	

	PROTOCOL protocol = GetProtocol(_ptr->recv_buf);
	switch (protocol)
	{
	case SELECT_COUNT:
		int count;
		UnPackPacket(_ptr->recv_buf, count);
		if (count < 1 || count >3)
		{
			int size = PackPacket(_ptr->send_buf, GAME_ERROR, SELECT_COUNT_ERROR, COUNT_ERROR_MSG);
			retval = send(_ptr->sock, _ptr->send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("COUNT_ERROR_MSG send()");
				_ptr->state = C_DISCONNECTED_STATE;
				return;
			}			
			return;
		}

		int  count_value[3] = { 0 };

		for (int i = 0; i < count; i++)
		{
			game->game_number++;

			if (!GameContinueCheck(_ptr, game->game_number))
			{				
				return;
			}

			count_value[i] = game->game_number;
		}


		char msg[BUFSIZE] = { 0 };

		MakeGameMessage(_ptr, count, count_value, msg);
		int size = PackPacket(_ptr->send_buf, COUNT_VALUE, msg);
		
		for (int i = 0; i < game->player_count; i++)
		{
			_ClientInfo* temp_ptr = game->players[i];

			retval = send(temp_ptr->sock, _ptr->send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("COUNT_VALUE send()");
				temp_ptr->state = C_DISCONNECTED_STATE;
				SetEvent(temp_ptr->turn_event);
			}
		}
	}

	_ClientInfo* next_ptr = SearchNextClient(_ptr);
	if (next_ptr == nullptr)
	{		
		WaitForSingleObject(game->wait_event, INFINITE);
		game->state = G_INVALID_GAME_STATE;
		_ptr->state = C_DISCONNECTED_STATE;
		return;
	}

	SetEvent(next_ptr->turn_event);
	_ptr->state = C_OTHER_TURN_STATE;
	
}

void OtherTurnProcess(_ClientInfo* _ptr)
 {
	WaitForSingleObject(_ptr->turn_event, INFINITE);

	if (_ptr->state == C_DISCONNECTED_STATE)
	{
		return;
	}

	_GameInfo* game = _ptr->game_info;

	switch (game->state)
	{
	case G_PLAYING_STATE:
		game->cur_player = _ptr;
		_ptr->state = C_CLIENT_TURN_STATE;
		break;
	case G_GAME_OVER_STATE:
		_ptr->state = C_GAME_OVER_STATE;
		break;
	}
	
}

void GameOverProcess(_ClientInfo* _ptr)
{
	int size;
	int retval;
	_GameInfo* game = _ptr->game_info;	

	switch (_ptr->game_result)
	{
	case WIN:		
		
		char msg[BUFSIZE];	

		sprintf_s(msg, "%d%s", game->lose_player->player_number, WIN_MSG);
	
		size = PackPacket(_ptr->send_buf, GAME_RESULT, WIN, msg);
		retval = send(_ptr->sock, _ptr->send_buf, size, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			_ptr->state = C_DISCONNECTED_STATE;
			return;
		}

		break;
	case LOSE:
		size = PackPacket(_ptr->send_buf, GAME_RESULT, LOSE, LOSE_MSG);
		retval = send(_ptr->sock, _ptr->send_buf, size, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			_ptr->state = C_DISCONNECTED_STATE;
			return;
		}		

		break;
	}

	if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
	{
		_ptr->state = C_DISCONNECTED_STATE;
		return;
	}

}

void InitProcess(_ClientInfo* _ptr)
{
	_GameInfo* game_info = _AddGameInfo(_ptr);

	if (game_info->state == G_WAIT_STATE)
	{
		int size = PackPacket(_ptr->send_buf, WAIT, WAIT_MSG);
		int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			_ptr->state = C_DISCONNECTED_STATE;
			return;
		}
	}

	WaitForSingleObject(game_info->start_event, INFINITE);

	int size = PackPacket(_ptr->send_buf, INTRO, INTRO_MSG);
	int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		_ptr->state = C_DISCONNECTED_STATE;
		return;
	}

	char msg[BUFSIZE];

	sprintf(msg, "당신은 %d번 Player 입니다\n", _ptr->player_number);

	size = PackPacket(_ptr->send_buf, PLAYER_INFO, msg);
	retval = send(_ptr->sock, _ptr->send_buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		_ptr->state = C_DISCONNECTED_STATE;
		return;
	}

	if (_ptr->player_number == 1)
	{
		_ptr->state =  C_CLIENT_TURN_STATE;
	}
	else
	{
		_ptr->state = C_OTHER_TURN_STATE;
	}
	
}

DWORD CALLBACK ClientThread(LPVOID _ptr)
{
	_ClientInfo* ptr = (_ClientInfo*)_ptr;

	bool endflag = false;

	while (1)
	{
		switch (ptr->state)
		{
		case C_INIT_STATE:
			InitProcess(ptr);
			break;
		case C_CLIENT_TURN_STATE:
			ClientTurnProcess(ptr);
			break;
		case C_OTHER_TURN_STATE:
			OtherTurnProcess(ptr);
			break;
		case C_GAME_OVER_STATE:
			GameOverProcess(ptr);
			break;
		case C_DISCONNECTED_STATE:
			DisconnectedProcess(ptr);
			endflag = true;
			break;
		}

		if (endflag)
		{
			break;
		}
	}
	return 0;
}

void UpdateGameInfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);
	_GameInfo* game = _ptr->game_info;

	for (int i=0; i < game->player_count ; i++)
	{
		if (game->players[i] == _ptr)
		{
			for (int j = i; j < game->player_count - 1; j++)
			{
				game->players[j] = game->players[j + 1];
			}		
			game->player_count--;
			if (game->player_count == 1)
			{
				SetEvent(game->wait_event);
			}
			if (game->player_count == 0)
			{
				ReMoveGameInfo(game);
			}
			break;
		}

	}
	LeaveCriticalSection(&cs);
}

_ClientInfo* SearchNextClient(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);
		
	_GameInfo* game = _ptr->game_info;
	
	if (game->player_count == 1)
	{
		LeaveCriticalSection(&cs);
		return nullptr;
	}

	int i = -1;
	for (i = 0; i < game->player_count; i++)
	{
		if (game->players[i] == _ptr)
		{
			break;
		}			
	}

	if (i == -1)
	{
		LeaveCriticalSection(&cs);
		return nullptr;
	}

	_ClientInfo* ptr = nullptr;

	while (1)
	{
		int index = (i + 1) % game->player_count;
		if (game->players[index]->state == C_DISCONNECTED_STATE)
		{
			i++;
			continue;
		}

		ptr = game->players[index];
		break;
	}	

	if (ptr == _ptr)
	{
		ptr = nullptr;
	}
	LeaveCriticalSection(&cs);
	return ptr;	
}

void ExitClientCheck(_ClientInfo* _ptr)
{
	_GameInfo* game = _ptr->game_info;

	switch (game->state)
	{
	case G_WAIT_STATE:	
		break;
	case G_PLAYING_STATE:
		{
			char msg[BUFSIZE] = { 0 };
			sprintf(msg, "%d번 Player가 나갔습니다. \n", _ptr->player_number);
			int size = PackPacket(_ptr->send_buf, EXIT_PLAYER_INFO, msg);

			for (int i = 0; i < game->player_count; i++)
			{
				if (game->players[i] != _ptr)
				{
					_ClientInfo* tempptr = game->players[i];

					int retval = send(tempptr->sock, _ptr->send_buf, size, 0);
					if (retval == SOCKET_ERROR)
					{
						tempptr->state = C_DISCONNECTED_STATE;
						SetEvent(tempptr->turn_event);
					}	
				
				}
			}

			if (game->cur_player == _ptr)
			{
				_ClientInfo* next_ptr = SearchNextClient(_ptr);
				if (next_ptr != nullptr)
				{
					SetEvent(next_ptr->turn_event);
				}			
			}
			break;

		}
		break;
	case G_GAME_OVER_STATE:
		break;
	case G_INVALID_GAME_STATE:			
		{
			char msg[BUFSIZE] = { 0 };
			strcpy(msg, "함께할 Player가 하나도 없어서 게임을 종료합니다.\n");

			int size = PackPacket(_ptr->send_buf, INVALID_GAME, msg);
			int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
			}

			if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
			{
				_ptr->state = C_DISCONNECTED_STATE;
			}			
		}
		break;
	}

}



void DisconnectedProcess(_ClientInfo* _ptr)
{
	ExitClientCheck(_ptr);

	UpdateGameInfo(_ptr);	

	ReMoveClientInfo(_ptr);	
}
