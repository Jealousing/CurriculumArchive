#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    4096

#define PLAYER_COUNT 100

#define INTRO_MSG "가위(0), 바위(1), 보(2)중 하나를 입력하세요:"
#define WIN_MSG "이겼습니다.\n"
#define LOSE_MSG "졌습니다.\n"
#define NO_WIN_MSG "비겼습니다\n"

enum RESULT
{
	NODATA = -1,
	WIN = 1,
	LOSE,
	NO_WIN
};

enum PROTOCOL
{
	INTRO = 1,
	GAME_VALUE,
	GAME_RESULT
};

enum
{
	SOC_ERROR = 1,
	SOC_TRUE,
	SOC_FALSE
};

enum STATE
{
	INTRO_STATE,
	GAME_STATE,
	DISCONNECTED_STATE
};

enum IO_TYPE
{
	IO_RECV = 1,
	IO_SEND
};

struct _ClientInfo;

struct WSAOVERLAPPED_EX
{
	WSAOVERLAPPED overlapped;
	_ClientInfo* ptr;
	IO_TYPE       type;
};

struct _ClientInfo
{
	WSAOVERLAPPED_EX	r_overlapped;
	WSAOVERLAPPED_EX	s_overlapped;

	SOCKET			sock;
	SOCKADDR_IN		addr;
	STATE			state;
	bool			r_sizeflag;

	int				game_value;
	_ClientInfo*	part;

	int				recvbytes;
	int				comp_recvbytes;
	int				sendbytes;
	int				comp_sendbytes;

	char			recvbuf[BUFSIZE];
	char			sendbuf[BUFSIZE];

	WSABUF			r_wsabuf;
	WSABUF			s_wsabuf;
};


void err_quit(const char* msg);
void err_display(const char* msg);

int who_win(int first, int second);

PROTOCOL GetProtocol(const char* _ptr);
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1);
int PackPacket(char* _buf, PROTOCOL _protocol, RESULT _result, const char* _str1);

void UnPackPacket(const char* _buf, int& _data);

_ClientInfo* AddClientInfo(SOCKET _sock, SOCKADDR_IN _addr);
void RemoveClientInfo(_ClientInfo* _ptr);

bool MatchPartner(_ClientInfo*);

void GameProcess(_ClientInfo*);

DWORD WINAPI ProcessClient(LPVOID);

bool Recv(_ClientInfo* _ptr);

int CompleteRecv(_ClientInfo* _ptr, int _completebyte);

bool Send(_ClientInfo* _ptr, int _size);

int CompleteSend(_ClientInfo* _ptr, int _completebyte);

void CompleteRecvProcess(_ClientInfo*);

void CompleteSendProcess(_ClientInfo*);

DWORD WINAPI WorkerThread(LPVOID arg);


#ifdef MAIN

_ClientInfo* ClientInfo[PLAYER_COUNT];
int Count = 0;

CRITICAL_SECTION cs;
#else

extern _ClientInfo* ClientInfo[PLAYER_COUNT];
extern int Count;

extern CRITICAL_SECTION cs;

#endif