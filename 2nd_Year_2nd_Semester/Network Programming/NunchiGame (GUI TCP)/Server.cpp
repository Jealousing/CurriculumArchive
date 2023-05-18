#pragma comment(lib, "ws2_32.lib")//�Ӽ�->��Ŀ->�Է�->ws2_32.lib; �Է��ص���
#include <WinSock2.h>					 //��������ϻ���ϱ����� �������
#include <stdio.h>
#include <string.h>

/* ���� */
#define SERVERIP "127.0.0.1"		//������ �ּ�
#define SERVERPORT 9000		//������Ʈ


#define BUFSIZE 4096				//���ۻ�����
#define NICKNAMESIZE 255		//�г��� ������
#define MAXUSER   100				//�ִ� ���� ����
#define NODATA   -1					//����!
#define PLAYER_COUNT 5			//�ѹ濡 �÷��̾� ���� ����

//������ �޼��� ����
#define INTRO_MSG "��ġ�����Դϴ�. �г����� �Է��ϼ���"
#define NICKNAME_ERROR_MSG "�̹��ִ� �г����Դϴ�."
#define WAIT_MSG "�ٸ� Player�� ��ٸ��ϴ�....\n"
#define GAME_START_MSG1 "������ �����Ͽ����ϴ�."
#define GAME_START_MSG2 "(���ڸ� 1���� �Է��ؾߵǰ�, 5�� ��ġ�ų� ���ÿ� �Է��ϸ� ���ӿ��� �й��մϴ�)"
#define LOSE_MSG "����� ���ӿ��� �й��ϼ̽��ϴ�."
#define NUMBER_ERROR_MSG "1~5������ ���ڸ� �Է��� �� �ֽ��ϴ�."
#define NUMBER_SAME_MSG "�ٸ� �÷��̾ ���� �Է��Ͽ����ϴ�."
#define NUMBER_PASS_MSG "�̹� �Էµ� �����Դϴ�."
#define NUMBER_END_MSG "����� �̹� ���ڸ� �Է��ϼ̽��ϴ�."


enum STATE							//�������
{
	INITE_STATE,						//����
	INTRO_STATE,						//��Ʈ��
	NAME_SET_STATE,				//�г��Ӽ���
	WAIT_STATE,						//���
	GAMEING_STATE,					//��������
	GAME_END_STATE				//���ӳ�
};

enum PROTOCOL						//�����Ȳ
{
	INTRO,									//��Ʈ��
	WAIT,									//���
	CHATT_NICKNAME,				//�г����Է�
	NICKNAME_EROR, 				//�г��ӿ���
	NICKNAME_LIST,					//�г������
	CHATT_MSG,							//ü�ø޼���
	CHATT_OUT,							//����
};

struct _ClientInfo;
struct _GameInfo													//��������
{	
	int game_number;											//���ݱ��� ����� ����
	HANDLE start_event;										//���ӽ��� �̺�Ʈ
	_ClientInfo* players[PLAYER_COUNT];			//�÷��̾�
	int player_count;												//�÷��̾� �ο���
	bool full;															//��á���� Ȯ��
	char* namelist[5];											//���ӿ� ������ �÷��̾� �г��� ����
};

struct _ClientInfo													//Ŭ���̾�Ʈ ����
{
	SOCKET sock;													//��������
	SOCKADDR_IN clientaddr;								//�ּ�����
	STATE state;													//�������
	_GameInfo* game_info;									//������ ���ӹ�
	bool chatflag;													//ä�ð��ɿ���
	char  nickname[NICKNAMESIZE];						//������ �г���
	char  sendbuf[BUFSIZE];									//send�� ����
	char  recvbuf[BUFSIZE];									//recv�� ����
	int player_number;											//�濡 ���°�� �����ߴ���
	bool turn_end;													//���� ���� �� �߰� ���� x
};

//����ü �迭 ����
_ClientInfo* ClientInfo[MAXUSER];						
_GameInfo* GameInfo[MAXUSER];
//�г��� �迭 ����
char* NickNameList[MAXUSER];

//���� �ο��� ���ӹ� ����
int Client_Count = 0;
int GameCount = 0;
//�г��� ����
int Nick_Count = 0;

//ũ��Ƽ�ü���
CRITICAL_SECTION cs;


DWORD CALLBACK ProcessClient(LPVOID);									//Ŭ���̾�Ʈ ����

void err_quit(const char *msg);													//���� ��� �� ����
void err_display(const char *msg);												//���� ���
int recvn(SOCKET s, char *buf, int len, int flags);						//recv�ݺ��Լ�

void MakeChattMessage(const char* , const char* , char* );		//�г��� : �������� ���ڿ� ���� �Լ�
void MakeEnterMessage(const char* , char* );							//���� �޼��� ���ڿ� ���� �Լ�
void MakeExitMessage(const char* , char* );								//���� �޼��� ���ڿ� ���� �Լ�

_ClientInfo* AddClient(SOCKET sock, SOCKADDR_IN clientaddr);//Ŭ���̾�Ʈ ���� ó��
void RemoveClient(_ClientInfo* ptr);											//Ŭ���̾�Ʈ ���� ����

_GameInfo* _AddGameInfo(_ClientInfo* _ptr);							//���� ����
void RemoveGameinfo(_ClientInfo* _ptr);									//�������� ����

bool NicknameCheck(const char*);												//�г���üũ
void NamelistSet(_ClientInfo* _ptr);											//�г��Ӹ���Ʈ ����
void NickNameSetting(_ClientInfo*);											//�г��� ����
void RemoveNickName(const char*);											//�г��ӻ���

bool PacketRecv(SOCKET, char*);												//recvn�� �ι� ó��

/* ������ ������ Packing ó�� */
int PackPacket(char*, PROTOCOL, const char*);
int PackPacket(char*, PROTOCOL);
int PackPacket(char*, PROTOCOL, char**, int);

/* ���޹��� �����Ϳ��� protocol �и� */
PROTOCOL GetProtocol(const char*);

/* ���޹��� ������ UnPacking */
void UnPackPacket(const char*, char*);

void ChattingMessageProcess(_ClientInfo*);								//�޼��� �Է� �� ����
void InitProcess(_ClientInfo* _ptr);												//���� �� ���
void ChattingOutProcess(_ClientInfo*);										//���� �޼���
void ChattingEnterProcess(_ClientInfo*);									//���� �޼���


/* ���� ���� */
int main(int argc, char **argv)
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	//�Ӱ� ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����		
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
		//Ŭ���̾�Ʈ ����
		_ClientInfo* ptr = AddClient(sock, clientaddr);
		//��Ƽ������
		HANDLE hThread=CreateThread(NULL, 0, ProcessClient, ptr, 0, nullptr);	
		if (hThread != NULL)
		{
			CloseHandle(hThread);
		}

	}
	// closesocket()
	closesocket(listen_sock);
	//�ӰԿ��� ����
	DeleteCriticalSection(&cs);
	WSACleanup();
	return 0;
}

//���ӻ���
_GameInfo* _AddGameInfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);																//�Ӱ迵�� �Ҵ�

	_GameInfo* game_ptr = nullptr;
	int index = NODATA;

	for (int i = 0; i < GameCount; i++)														//�����Ǿ��ִ� ���� Ȯ��
	{
		if (!GameInfo[i]->full)																	//������ ���� ���� �ִ��� Ȯ��
		{
			GameInfo[i]->players[GameInfo[i]->player_count++] = _ptr;	//����ִ� �ڸ��� ��
			_ptr->game_info = GameInfo[i];												//����
			_ptr->player_number = GameInfo[i]->player_count;				//���° �ɹ������� ����
			
			if (GameInfo[i]->player_count == PLAYER_COUNT)					//������
			{
				GameInfo[i]->full = true;														//�濡 �ڸ�����!
				SetEvent(GameInfo[i]->start_event);									//���ӽ���
			}
			game_ptr = GameInfo[i];															
			index = i;
			break;
		}
	}

	if (index == NODATA)																					//�ڸ��� �� ���� ������ ����
	{
		/* ���ο� �� ���� */
		game_ptr = new _GameInfo;
		ZeroMemory(game_ptr, sizeof(_GameInfo));
		game_ptr->full = false;
		game_ptr->start_event = CreateEvent(nullptr, true, false, nullptr);
		game_ptr->players[0] = _ptr;
		game_ptr->game_number = 1;
		_ptr->game_info = game_ptr;
		_ptr->player_number = 1;
		

		game_ptr->player_count++;																	//������ �ο� �� ����
		GameInfo[GameCount++] = game_ptr;													//�迭�� ����
	}

	LeaveCriticalSection(&cs);																//�Ӱ迵�� ����
	return game_ptr;
}

//���� �� ���
void InitProcess(_ClientInfo* _ptr)
{
	_GameInfo* game_info = _ptr->game_info;	
	
	NamelistSet(_ptr);	//���ӹ濡 ������ ������� �г��� ����Ʈ ����

	//���� �Ŀ� ������ �����ߴ������ ���
	int size = PackPacket(_ptr->sendbuf, NICKNAME_LIST, _ptr->game_info->namelist, _ptr->game_info->player_count);
	if (send(_ptr->sock, _ptr->sendbuf, size, 0) == SOCKET_ERROR)
	{
		err_display("nickname list Send()");
		_ptr->state = GAME_END_STATE;
		LeaveCriticalSection(&cs);
		return;
	}

	//���� ���� ���
	WaitForSingleObject(game_info->start_event, INFINITE);

	//���ӽ��� �޼���1
	size = PackPacket(_ptr->sendbuf, CHATT_MSG, GAME_START_MSG1);
	int retval = send(_ptr->sock, _ptr->sendbuf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		_ptr->state = GAME_END_STATE;
		return;
	}
	//���ӽ��� �޼���2
	size = PackPacket(_ptr->sendbuf, CHATT_MSG, GAME_START_MSG2);
	retval = send(_ptr->sock, _ptr->sendbuf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		_ptr->state = GAME_END_STATE;
		return;
	}

}

//Ŭ���̾�Ʈ �������μ���
DWORD CALLBACK ProcessClient(LPVOID  _ptr)
{
	_ClientInfo* Client_ptr = (_ClientInfo*)_ptr;
		
	int size;	
	PROTOCOL protocol;

	bool breakflag =false;

	while (!breakflag)
	{

		switch (Client_ptr->state)	//���� ��Ȳ�� ���� ����
		{
		case INITE_STATE://����
			Client_ptr->state = INTRO_STATE;
			break;
		case INTRO_STATE://��Ʈ��
			//��Ʈ�� �޼��� ����
			size = PackPacket(Client_ptr->sendbuf, INTRO, INTRO_MSG);
			if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("intro Send()");
				Client_ptr->state = GAME_END_STATE;	
				break;
			}

			Client_ptr->state = NAME_SET_STATE;
			break;
		case NAME_SET_STATE://�г��Ӽ���

			if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))	//������ �ޱ�
			{
				Client_ptr->state = GAME_END_STATE;
				break;
			}

			protocol = GetProtocol(Client_ptr->recvbuf);					//�������� 

			switch (protocol)
			{
			case CHATT_NICKNAME:
				NickNameSetting(Client_ptr);										//�г��Ӽ���
				break;
			}

			if (!Client_ptr->chatflag)
			{
				break;
			}
			
			ChattingEnterProcess(Client_ptr);									//���� �޼���

			if (Client_ptr->state != GAME_END_STATE)
			{
				Client_ptr->state = WAIT_STATE;								//����ϴ� ������
			}
			break;
		case WAIT_STATE://���ӽ��� ���
			InitProcess(Client_ptr);													//���ӽ��� ���
			Client_ptr->state = GAMEING_STATE;
			break;
		case GAMEING_STATE://��������

			if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
			{
				Client_ptr->state = GAME_END_STATE;
				break;
			}

			protocol = GetProtocol(Client_ptr->recvbuf);

			switch (protocol)
			{
			case CHATT_MSG:
				ChattingMessageProcess(Client_ptr);							//��������
				break;
			case CHATT_OUT:
				ChattingOutProcess(Client_ptr);									//����
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


//recvn 2�� ������-> ������
bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;
	//���� �������� ������ ����
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
	//������ ��ŭ �����͸� �޾ƿ�
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
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1) //����, ��������, �޼���
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
int PackPacket(char* _buf, PROTOCOL _protocol)//����,��������
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
int PackPacket(char* _buf, PROTOCOL _protocol, char** _strlist, int _count)	//����,�г��Ӹ���Ʈ,�г��� ����
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

//�������� ���
PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}

/* Unpacking */
void UnPackPacket(const char* _buf, char* _str)//����, �޼���
{
	int strsize;
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
	ptr = ptr + strsize;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpmsgbuf;																		//void*
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |								//�����޼��� ������ ���� �Ҵ�
		FORMAT_MESSAGE_FROM_SYSTEM,										//�ü������ �����޼��� ������
		NULL, WSAGetLastError(),														//WSAGetLastError�� �����ڵ带 ������		
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),				//�����ڵ带 �ý��� �⺻���� ����
		(LPSTR)&lpmsgbuf, 0, NULL);													//Ipmsgbuf�� ����
	MessageBox(NULL, (LPCSTR)lpmsgbuf, msg, MB_OK);					//Ipmsgbuf�� ����� ������ �޼����ڽ��� ���
	LocalFree(lpmsgbuf);																	//Ipmsgbuf�� �Ҵ�� �޸� ��ȯ
	exit(-1);																							//����
}
// ���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpmsgbuf;																	//void*
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |							//�����޼��� ������ ���� �Ҵ�
		FORMAT_MESSAGE_FROM_SYSTEM,									//�ü������ �����޼��� ������
		NULL, WSAGetLastError(),													//WSAGetLastError�� �����ڵ带 ������
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),			//�����ڵ带 �ý��� �⺻���� ����
		(LPSTR)&lpmsgbuf, 0, NULL);												//Ipmsgbuf�� ����
	printf("[%s] %s\n", msg, (LPSTR)lpmsgbuf);							//Ipmsgbuf�� ����� ������ Printf�� ���������Ʈâ�� ���
	LocalFree(lpmsgbuf);																//Ipmsgbuf�� �Ҵ�� �޸� ��ȯ
}
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET sock, char* buf, int len, int flags)					// ����� ���� ������ ���� �Լ� recv�� �ݺ�
{
	char* ptr = buf;																		//������ �����ּ� ���������� ptr �� ����
	int left = len;																			//���� ���� �������� ũ��, ������ ���� left �� ����
	int recived;																				//recv�Լ��� ���� ���� �����ϴ� ����

	while (left > 0)																			//���� ���� �����Ͱ� ���������� �ݺ�
	{
		recived = recv(sock, ptr, left, flags);									//�����͸� �����ϴ� �Լ�
		if (recived == SOCKET_ERROR)											//����Ȯ��
		{
			return SOCKET_ERROR;
		}
		if (recived == 0)																	//���� ����� ����
		{
			break;
		}

		left -= recived;																		//�����͸� ���� ������ �����ϸ鼭 
		ptr += recived;																		//��Ȯ�� ����Ʈ ���� �����ϱ� ����ó��
	}

	return (len - left);																	//���� ����Ʈ�� ����
}

//Ŭ���̾�Ʈ ���� ����ó��
_ClientInfo* AddClient(SOCKET sock, SOCKADDR_IN clientaddr)
{
	printf("Client ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr),	//���� �ּ� ���
		ntohs(clientaddr.sin_port));

	EnterCriticalSection(&cs);																					//�Ӱ迵�� �Ҵ�

	_ClientInfo* ptr = new _ClientInfo;																		//�޸� �Ҵ�
	ZeroMemory(ptr, sizeof(_ClientInfo));																	//�޸� �ʱ�ȭ
	ptr->sock = sock;																									//�޾ƿ� �������� ����
	memcpy(&(ptr->clientaddr), &clientaddr, sizeof(clientaddr));								//Ŭ�� �ּ� ����
	ptr->state = INITE_STATE;																					//state(����) ����
	ptr->chatflag = false;																							//�Է°��ɿ��� ����
	ptr->turn_end = false;																							//�߰� �Է� ���� ����
	ClientInfo[Client_Count++] = ptr;																			//Ŭ���̾�Ʈ���� ����Ʈ�迭�� ���� (ī��Ʈ����)
	
	LeaveCriticalSection(&cs);																					//�Ӱ迵�� ����
	return ptr;
}

//�������� ����
void RemoveGameinfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);										//�Ӱ迵�� �Ҵ�

	_GameInfo* game = _ptr->game_info;					//ptr�� �Ҽӵ� ��������

	for (int i = 0; i < game->player_count; i++)				//�Ҽӵ� �÷��̾� ����ŭ �ݺ�
	{
		if (game->players[i] == _ptr)								//�ڽ��� ��ġ�� ã��
		{
			for (int j = i; j < game->player_count -1; j++)		//�����
			{
				game->players[j] = game->players[j + 1];
			}
			game->player_count--;									//���� �ο��� ����
			if (game->player_count == 0)							//�÷��̾��ο��� 0���� ���
			{
				for (int i = 0; i < GameCount; i++)					//���ӹ� ����
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

				GameCount--;												//���Ӽ� ����
			}
			break;
		}
	}


	LeaveCriticalSection(&cs);										//�Ӱ迵�� ����
}

//Ŭ���̾�Ʈ ���� ����
void RemoveClient(_ClientInfo* ptr)
{

	printf("Client ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",			//������ Ŭ���̾�Ʈ ���� ���
		inet_ntoa(ptr->clientaddr.sin_addr),
		ntohs(ptr->clientaddr.sin_port));

	EnterCriticalSection(&cs);											//�Ӱ迵�� �Ҵ�

	
	for (int i = 0; i < Client_Count; i++)									//������ Ŭ���̾�Ʈ ��ȸ
	{
		if (ClientInfo[i] == ptr)												//�ڽ��� ���°�� �ִ��� Ȯ��
		{			
			closesocket(ptr->sock);										//���� ����
			delete ptr;
			for (int j = i; j < Client_Count - 1; j++)						//�����
			{
				ClientInfo[j] = ClientInfo[j + 1];
			}
			break;
		}
	}
	Client_Count--;															//�����ο� ���̱�
	LeaveCriticalSection(&cs);											//�Ӱ迵�� ����
}

//�г��� üũ
bool NicknameCheck(const char* _nick)
{
	EnterCriticalSection(&cs);						//�Ӱ迵�� ����
	for (int i = 0; i < Nick_Count; i++)				//������ �г��� ��ŭ ��ȸ
	{
		if (!strcmp(NickNameList[i], _nick))		//�г��� ����Ʈ�� ���� �г����� �ִ��� Ȯ��
		{
			LeaveCriticalSection(&cs);				//�Ӱ迵�� ����
			return false;
		}
	}
	LeaveCriticalSection(&cs);						//�Ӱ迵�� ����

	return true;
}

//�г��� �߰�
void AddNickName(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);											//�Ӱ迵�� ����

	char* ptr = new char[strlen(_ptr->nickname) + 1];
	strcpy(ptr, _ptr->nickname);
	NickNameList[Nick_Count++] = ptr;								//�г��� ����Ʈ�� �߰�

	LeaveCriticalSection(&cs);
}

//�г��� ����Ʈ ���� (�濡 ���� �г��Ӹ���Ʈ)
void NamelistSet(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);													//�Ӱ迵�� ����
	_GameInfo* _game = _ptr->game_info;								//�������� �ҷ���

	for (int i = 0; i < _game->player_count; i++)							//�����ο� �� ��ŭ
	{
		_game->namelist[i] = _game->players[i]->nickname;		//���ӿ� ������ �÷��̾��� �г��ӵ��� ���� �г��� ����Ʈ�� ����
	}

	LeaveCriticalSection(&cs);													//�Ӱ迵�� ����
}

//�г��� ����
void RemoveNickName(const char* _nick)
{
	EnterCriticalSection(&cs);									//�Ӱ迵�� ����
	for (int i = 0; i < Nick_Count; i++)							//������ �г��� ������ŭ �ݺ�
	{
		if (!strcmp(NickNameList[i], _nick))					//������ �г��� ã��
		{
			delete[] NickNameList[i];								//����

			for (int j = i; j < Nick_Count - 1; j++)				//����
			{
				NickNameList[j] = NickNameList[j + 1];
			}
			NickNameList[Nick_Count--] = nullptr;			
			break;
		}
	}
	LeaveCriticalSection(&cs);									//�Ӱ迵�� ����
}

/* ���ڿ� ���� �Լ� */
void MakeChattMessage(const char* _nick, const char* _msg, char* _chattmsg)
{
	sprintf(_chattmsg, "[ %s ] %s", _nick, _msg);		// [ �г��� ] : �޼���
}
void MakeEnterMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s���� �����ϼ̽��ϴ�.", _nick);		// �г��Ӵ��� ����~
}
void MakeExitMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s���� �����ϼ̽��ϴ�.", _nick);		// �г��Ӵ��� ����~
}

//�г��� ����
void NickNameSetting(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);																										//�Ӱ迵�� ����
	
	char msg[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	UnPackPacket(_clientinfo->recvbuf, msg);												
	if (!NicknameCheck(msg))																										//�г��� üũ
	{
		size = PackPacket(_clientinfo->sendbuf, NICKNAME_EROR, NICKNAME_ERROR_MSG);			//�̹� �ִ� �г���
		if (send(_clientinfo->sock, _clientinfo->sendbuf, size, 0) == SOCKET_ERROR)
		{
			err_display("nickname error Send()");			
			_clientinfo->state = GAME_END_STATE;					
		}
		LeaveCriticalSection(&cs);																									//�Ӱ迵�� ����
		return;
	}
	else
	{
		strcpy(_clientinfo->nickname, msg);
		_AddGameInfo(_clientinfo);																									//�漳��
		AddNickName(_clientinfo);																									//�г����߰�

		_clientinfo->chatflag = true;
	}	

	LeaveCriticalSection(&cs);																										//�Ӱ迵�� ����
}

//��ġ���� ���� �κ�
bool nunci_game(_ClientInfo* _clientinfo,char* msg)
{
	_GameInfo* _game = _clientinfo->game_info;

	int size;
	int retval;

	//�ùٸ� �Է��� �߾��� ��� ����
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
	
	//�ùٸ� �Է¹����� ���ԵǾ����� �ʴ� ���� �Է����� ��� ����
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
	
	//�й� ���� ���� (���� ���ڸ� �Է��ϰų� ������ �����ʴ� ���ڸ� �Է��ϸ� �й�)
	if (atoi(msg) == 5 || _clientinfo->game_info->game_number != atoi(msg))
	{
		char send_msg[BUFSIZE] = { 0 };
		for (int i = 0; i < _game->player_count; i++)	//�ش��÷��̾�� �й�޼����� ������ �÷��̾�� �¸��޼����� �����Ѵ�.
		{
			_ClientInfo* tempptr = _game->players[i];

			_game->players[i]->state = GAME_END_STATE;

			if (_game->players[i] != _clientinfo)
			{
				sprintf(send_msg, "%s���� ���ӿ��� �й��ϼ̽��ϴ�!! \n�¸��� �����մϴ�\n������ ����Ǿ����ϴ�!", _clientinfo->nickname);
				size = PackPacket(tempptr->sendbuf, CHATT_OUT, send_msg);
			}
			else
			{
				printf("[ %s ]���� �й��ϼ̽��ϴ�\n������ ����Ǿ����ϴ�!", tempptr->nickname);
				size = PackPacket(tempptr->sendbuf, CHATT_OUT, LOSE_MSG);
			}
			if (send(tempptr->sock, tempptr->sendbuf, size, 0) == SOCKET_ERROR)
			{
				tempptr->state = GAME_END_STATE;
			}
		}
		return false;
	}

	//����� �Է������� ������ �� �Է��ߴ��� ���
	if (_clientinfo->game_info->game_number == atoi(msg))
	{
		_clientinfo->game_info->game_number++;
		_clientinfo->turn_end = true;
		printf("[ %s ]���� ä��: %s\n", _clientinfo->nickname, msg);
		return true;
	}

	return false;
}

//�޼��� �Է� �� ����
void ChattingMessageProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);																																	//�Ӱ迵�� ����

	_GameInfo* _game = _clientinfo->game_info;

	char msg[BUFSIZE];
	char temp[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	memset(temp, 0, sizeof(temp));

	UnPackPacket(_clientinfo->recvbuf, msg);																											
	MakeChattMessage(_clientinfo->nickname, msg, temp);																						//�޾ƿ� �޼����� ���¸� �ٲ㼭 ����

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


	LeaveCriticalSection(&cs);																																	//�Ӱ迵�� ����
}

//���ӹ濡 ���� �� ��� �����ϴ� �Լ�
void ChattingEnterProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);//�Ӱ迵�� ����
	_GameInfo* _game = _clientinfo->game_info;
	char msg[BUFSIZE];
	int size;

	MakeEnterMessage(_clientinfo->nickname, msg);//����޼��� ����

	for (int i = 0; i < _game->player_count; i++)
	{
		if (_game->players[i]->chatflag)//������ �÷��̾ �� �޼����� ���޹���
		{
			size = PackPacket(_game->players[i]->sendbuf, CHATT_MSG, msg);
			if (send(_game->players[i]->sock, _game->players[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting enter massage Send()");
				ClientInfo[i]->state = GAME_END_STATE;
			}
			if (_game->player_count <5)//������ �÷��̾ 4�����ϸ� ��ٸ���� �޼������� ����
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
	LeaveCriticalSection(&cs);//�Ӱ迵�� ����
}

//���ӹ濡�� ���� �� ��� �����ϴ� �Լ�
void ChattingOutProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);//�Ӱ迵�� ����

	_GameInfo* _game = _clientinfo->game_info;
	char msg[BUFSIZE];
	int size;

	MakeExitMessage(_clientinfo->nickname, msg);		//����޼��� �����Լ�
	
	for (int i = 0; i < _game->player_count; i++)
	{
		if (_game->players[i] == _clientinfo)	//����
		{
			size = PackPacket(ClientInfo[i]->sendbuf, CHATT_OUT);
			if (send(ClientInfo[i]->sock, ClientInfo[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				ClientInfo[i]->state = GAME_END_STATE;
			}

		}
		else if (_game->players[i]->chatflag)//������������ �˷���
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

	RemoveNickName(_clientinfo->nickname);	//�г��ӻ���

	LeaveCriticalSection(&cs);//�Ӱ迵�� ����

}