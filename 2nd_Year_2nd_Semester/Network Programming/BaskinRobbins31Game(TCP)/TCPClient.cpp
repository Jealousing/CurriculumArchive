#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

enum PROTOCOL
{
	WAIT = 1,
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

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
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

void err_quit(char *msg)
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
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;

	int retval = recvn(_sock, (char*)&size, sizeof(size), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("gvalue recv error()");
		return false;
	}
	else if (retval == 0)
	{
		return false;
	}

	retval = recvn(_sock, _buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("gvalue recv error()");
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

int PackPacket(char* _buf, PROTOCOL _protocol, int _data)
{
	char* ptr = _buf;
	int size = 0;

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_data, sizeof(_data));
	ptr = ptr + sizeof(_data);
	size = size + sizeof(_data);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

void UnPackPacket(char* _buf, int* _data, int& _count)
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_count, ptr, sizeof(_count));
	ptr = ptr + sizeof(_count);

	for (int i = 0; i < _count; i++)
	{
		memcpy(&_data[i], ptr, sizeof(_data[i]));
		ptr = ptr + sizeof(_data[i]);
	}
}

void UnPackPacket(char* _buf, int& _data, char* _str1)
{
	int strsize1;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(_data));
	ptr = ptr + sizeof(_data);

	memcpy(&strsize1, ptr, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);

	memcpy(_str1, ptr, strsize1);
	ptr = ptr + strsize1;
}

void UnPackPacket(char* _buf, char* _str1)
{
	int strsize1;

	char* ptr = _buf + sizeof(PROTOCOL);
	
	memcpy(&strsize1, ptr, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);

	memcpy(_str1, ptr, strsize1);
	ptr = ptr + strsize1;
}


int main(int argc, char *argv[])
{	
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	int retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	char msg[BUFSIZE];

	// 서버와 데이터 통신
	while(1)
	{
		bool endflag = false;

		if (!PacketRecv(sock, buf))
		{
			break;
		}

		PROTOCOL protocol=GetProtocol(buf);

		switch (protocol)
		{
		case WAIT:		
		case INTRO:
		case PLAYER_INFO:
		case EXIT_PLAYER_INFO:
		case COUNT_VALUE:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(buf, msg);
			printf("%s", msg);			
			break;
		case CLIENT_TURN:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(buf, msg);
			printf("%s", msg);

			//while (getchar() != '\n');
			int count;
			scanf_s("%d", &count);
			int size;
			size = PackPacket(buf, SELECT_COUNT, count);
			int retval;
			retval = send(sock, buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				endflag = true;				
			}			
			break;
		case GAME_ERROR:
			int err_code;
			memset(msg, 0, sizeof(msg));
			UnPackPacket(buf, err_code, msg);
			printf("%s", msg);
			break;
		case GAME_RESULT:
			memset(msg, 0, sizeof(msg));
			int result;
			UnPackPacket(buf, result, msg);
			printf("%s", msg);		
			endflag = true;
			break;
		case INVALID_GAME:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(buf, msg);
			printf("%s", msg);
			endflag = true;
			break;
			
		}

		if (endflag)
		{
			break;
		}
		
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	system("pause");
	return 0;
}