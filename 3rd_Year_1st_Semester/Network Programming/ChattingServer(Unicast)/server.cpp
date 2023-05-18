#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

//����ip,port
#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

#define BUFSIZE 4096				//���ۻ�����
#define NICKNAMESIZE 255		//�г��ӻ�����
#define MAXUSER   10					//�ִ� ������
#define NODATA   -1					//������x
#define MAX_CHATT_LIST 3		//�ִ� ä�ÿ� ���� �Ҽ� �ִ� ����
#define MAX_CHATT_GROUP 3		//�ִ� ä�ù� ����

//msg ����
#define SELECT_CHATT_ROOM_MSG "ä�ù��� �����ϼ���\r\n1. �ڿ��ù�\r\n2. ����ȯ��\r\n3. ��������"		
#define SELECT_CHATT_ROOM_FULL_MSG "������ ä�ù��� �ο��� ��� á���ϴ�."
#define SELECT_CHATT_ROOM_ERROR_MSG "���� ä�ù��Դϴ�."
#define INTRO_MSG "ä�����α׷��Դϴ�. �г����� �Է��ϼ���"
#define NICKNAME_ERROR_MSG "�̹��ִ� �г����Դϴ�."

//ä�ù� ����
const char* RoomList[] = { nullptr,"1. �ڿ��ù�","2. ����ȯ��", "3. ��������" };

//������Ʈ(������ġ) ����
enum STATE
{
	INITE_STATE = 1,							//����ó�� �κ�
	SET_NICNAME_STATE,					//�г��Ӽ���
	SELECT_CHATT_ROOM_STATE,	//ä�ù� ����
	CHATTING_STATE,						//ä������
	CONNECT_END_STATE,				//ä�ÿ��� ����
	CHATT_RESELECT_STATE			//ä�ù� �缱��
};

// protocol ����
enum PROTOCOL
{
	INTRO = 1,											//��Ʈ��
	CHATT_NICKNAME,							//�г��� ����
	NICKNAME_ERROR,							//�г��� ����
	SELECT_CHATT_ROOM,					//ä�ù� ����
	SELECT_CHATT_ROOM_ERROR,		//ä�ù� ����
	SELECT_CHATT_ROOM_NUMBER,		//ä�ù� ����
	NICKNAME_LIST,								//ä�ÿ� ������ �г��ӵ�
	CHATT_MSG,										//ä�� �޼���
	CHATT_OUT,										//ä�����α׷� ����
	CHATT_RESELECT,							//ä�ù� �ٸ������� �������� ����
};

//ä�ù� ���� ���� ����
enum SELECT_ERROR_CODE
{
	SELECT_ROOM_FULL = 1,					//����
	SELECT_ROOM_NUMBER_ERROR		//�ִ� ���� ���ڰ� �ƴ�
};

//Ŭ���̾�Ʈ ���� ����ü
struct _ClientInfo
{
	SOCKET sock;									//��������
	SOCKADDR_IN clientaddr;				//Ŭ���̾�Ʈ �ּ�
	STATE state;									//������Ʈ(�����Ȳ)
	bool ReSelect = false;						//ä�ù� �缳����
	int chatt_room_number;					//������ ä�ù�
	char  nickname[NICKNAMESIZE];		//�г���
	char  sendbuf[BUFSIZE];					//������� ����
	char  recvbuf[BUFSIZE];					//�ޱ�� ����
	
};

//ä�ù� �׷�
struct _ChattGroupInfo
{
	_ClientInfo* chattList[MAX_CHATT_LIST];			//ä�ÿ� ������ Ŭ���̾�Ʈ ����
	const char* chatt_name;										//ä�ù� �̸�
	const char* nickname_list[MAX_CHATT_LIST];	//ä�ù濡 ������ �г��� ����Ʈ
	bool full;																//��á���� Ȯ��
	int chat_count;														//�����ο�
};

_ClientInfo* ClientInfo[MAXUSER];											//Ŭ������
char* NickNameList[MAXUSER];												//�г�������
_ChattGroupInfo* ChattGroupList[MAX_CHATT_GROUP + 1];	//ä�� �׷�

int Client_Count = 0;				//Ŭ�� ���� ��
int Nick_Count = 0;				//�г��� ����

CRITICAL_SECTION cs;			//�Ӱ迵��
FD_SET Rset, Wset;				//select


_ClientInfo* SearchClient(const char*);									//Ŭ���̾�Ʈ ã��
bool CheckNickname(const char*);											//�ִ� �г������� Ȯ��
bool CheckRoomNumber(int);													//ä�ù� �ִ��� Ȯ��
void MaKeChattMessage(const char* , const char* , char* );	//ä�ø޼��� ����
void MakeEnterMessage(const char* , char* );						//����� �޼��� ����
void MakeExitMessage(const char* , char* );							//����� �޼��� ����

DWORD CALLBACK ProcessClient(LPVOID);			//�������
_ClientInfo* AddClient(SOCKET, SOCKADDR_IN);	//Ŭ���̾�Ʈ ���� �߰�
void RemoveClient(_ClientInfo*);							//Ŭ���̾�Ʈ ��������

bool EnterChattGroup(_ClientInfo*, int);				//ä�ù濡 ����
void ExitChattInfo(_ClientInfo*);							//ä�ù濡�� ����


void AddNickName(const char*);							//�г��� �߰�
void RemoveNickName(const char*);					//�г��� ����

void InitializeChattRoom();									//ä�ù� �ʱ�ȭ

bool NickNameSetting(_ClientInfo*);					//�г��� ����
void ChattingMessageProcess(_ClientInfo*);		//ä�ûѸ���
void ChattingOutProcess(_ClientInfo*);				//ä�ù泪��
void ChattingEnterProcess(_ClientInfo*);			//ä�ù����

// ���� ��� �Լ�
void err_quit(const char* msg);
void err_display(const char* msg);
//����� �����Լ� ������ ���ſ�
int recvn(SOCKET s, char* buf, int len, int flags);
//����� �����Լ� recvn�� 2�� ������->������ 
bool PacketRecv(SOCKET, char*);
//�����͸� packing 
int PackPacket(char*, PROTOCOL, const char*);
int PackPacket(char*, PROTOCOL, int, const char*);
int PackPacket(char*, PROTOCOL);
int PackPacket(char*, PROTOCOL, const char**, int);
//�����Ϳ��� protocol����
PROTOCOL GetProtocol(const char*);
//packing�� ������ unpacking
void UnPackPacket(const char*, char*);
void UnPackPacket(const char*, int&);



int main(int argc, char **argv)
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSAStartup() error!");

	//ũ��Ƽ�ü��� �ʱ�ȭ
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
	
	// ������ ��ſ� ����� ����		
	int addrlen;
	int retval;
	SOCKET sock;
	SOCKADDR_IN clientaddr;	

	InitializeChattRoom();//ä�ù� �ʱ�ȭ
	
	while (1)
	{
		FD_SET(listen_sock, &Rset); //�˻� ����

		if (select(0, &Rset, &Wset, 0, NULL) == SOCKET_ERROR) //�������
		{
			err_quit("select() error");
		}

		if (FD_ISSET(listen_sock, &Rset)) //���� Ȯ��
		{
			//�ּұ��� 
			addrlen = sizeof(clientaddr);
			//��������
			sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (sock == INVALID_SOCKET)
			{
				err_display("accept()");
				continue;
			}
			//Ŭ���̾�Ʈ �߰�
			_ClientInfo* ptr = AddClient(sock, clientaddr);
			//�����常���
			HANDLE hThread = CreateThread(NULL, 0, ProcessClient, ptr, 0, nullptr);
			if (hThread != NULL)
			{
				CloseHandle(hThread);
			}
			continue;
		}

	}
	//��������
	closesocket(listen_sock);
	//�����Ӱ迵��
	DeleteCriticalSection(&cs);
	WSACleanup();
	return 0;
}

//������� Ŭ���̾�Ʈ���
DWORD CALLBACK ProcessClient(LPVOID  _ptr)
{
	//����� Ŭ��
	_ClientInfo* Client_ptr = (_ClientInfo*)_ptr;
		
	int size;	//������ ������
	PROTOCOL protocol;	//��������

	bool breakflag = false;	//�����÷���

	while (1)
	{
		//�ʱ�ȭ
		FD_ZERO(&Rset);
		FD_ZERO(&Wset);

		//rest wset ����
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

		//�����˻�
		if (select(0, &Rset, &Wset, 0, NULL) == SOCKET_ERROR)
		{
			err_quit("select() error");
		}


		if (FD_ISSET(Client_ptr->sock, &Rset))				//Rset�� ���õǾ��ִ��� Ȯ�� 
		{
			switch (Client_ptr->state)
			{
			case SET_NICNAME_STATE://�г��� ����
				if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))//�����͹ޱ�
				{
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}

				protocol = GetProtocol(Client_ptr->recvbuf);//�������ݹ޾ƿ�

				switch (protocol) //���ù� state ����
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

		if (FD_ISSET(Client_ptr->sock, &Wset)) //wset�� ���õǾ��ִ��� Ȯ��
		{
			switch (Client_ptr->state)
			{
			case INITE_STATE://���ӽ���
				//��Ʈ�� �޼��� ����
				size = PackPacket(Client_ptr->sendbuf, INTRO, INTRO_MSG);
				if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
				{
					err_display("intro Send()");
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}
				//������Ʈ ����
				Client_ptr->state = SET_NICNAME_STATE;
				break;
			case SELECT_CHATT_ROOM_STATE://ä�ù� ����
			{
				//ä�ù� ���� �޼�������
				size = PackPacket(Client_ptr->sendbuf, SELECT_CHATT_ROOM, SELECT_CHATT_ROOM_MSG);
				if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
				{
					err_display("intro Send()");
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}
				//������ �ޱ�
				if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
				{
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}
				//�������� �ޱ�
				protocol = GetProtocol(Client_ptr->recvbuf);
				int room_number = NODATA; //���ù�

				switch (protocol)
				{
				case SELECT_CHATT_ROOM_NUMBER: //������ ä�ù濬��
					UnPackPacket(Client_ptr->recvbuf, room_number); //unpacking
					if (!CheckRoomNumber(room_number))//ä�ù� ���� Ȯ��
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

					if (!EnterChattGroup(Client_ptr, room_number))//ä�ù��� �����ִ��� Ȯ��
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

					//�г��Ӹ���Ʈ�� �ڱ� �г��� �߰�
					const char** nickname_list = ChattGroupList[Client_ptr->chatt_room_number]->nickname_list;
					int chatt_count = ChattGroupList[Client_ptr->chatt_room_number]->chat_count;

					//������ ���� Ŭ���̾�Ʈ�� �г��� ����Ʈ ������
					size = PackPacket(Client_ptr->sendbuf, NICKNAME_LIST, nickname_list, chatt_count);
					if (send(Client_ptr->sock, Client_ptr->sendbuf, size, 0) == SOCKET_ERROR)
					{
						err_display("nickname list Send()");
						Client_ptr->state = CONNECT_END_STATE;
						break;
					}

					//����
					ChattingEnterProcess(Client_ptr);
					//������Ʈ ����
					Client_ptr->state = CHATTING_STATE;
					break;
				}
			}
			break;

			case CHATTING_STATE: // ä�� ����
				//������ �ޱ�
				if (!PacketRecv(Client_ptr->sock, Client_ptr->recvbuf))
				{
					Client_ptr->state = CONNECT_END_STATE;
					break;
				}

				//�������� �ޱ�
				protocol = GetProtocol(Client_ptr->recvbuf);

				switch (protocol)
				{
				case CHATT_MSG://�Ϲ� �޼��� ����
					ChattingMessageProcess(Client_ptr);//�޼������
					break;
				case CHATT_OUT://ä�ù� ����
					ChattingOutProcess(Client_ptr);//ä�ù泪��
					break;
				case CHATT_RESELECT://ä�ù� �缳��
					Client_ptr->ReSelect = true;//����� �˸�
					ChattingOutProcess(Client_ptr);//ä�ù泪��
					Client_ptr->state = CHATT_RESELECT_STATE;//�缳�� ������Ʈ
					break;
				}
				break;
			case CONNECT_END_STATE: //��������
				ExitChattInfo(Client_ptr);//ä�ó���
				RemoveClient(Client_ptr);//Ŭ���̾�Ʈ ��������
				breakflag = true;
				break;
			case CHATT_RESELECT_STATE:
				ExitChattInfo(Client_ptr);//ä�ó���
				Client_ptr->state = INITE_STATE;//ó��������Ʈ�� ���ư�
				break;
			}
		}
		//����������
		if (breakflag)
		{
			break;
		}
	}
	return 0;
}

// recvn(����� ���� ������ �����Լ�)�� 2�� �ݺ� ������->������
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
//������ Packing
//���ۿ� ��������+string
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
//���ۿ� ��������+data+string
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
//���ۿ� ��������
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
//���ۿ� ��������+stringlist,count
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

//�������� ����
PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}

//������ unpacking
//���ۿ��� string �и�
void UnPackPacket(const char* _buf, char* _str)
{
	int strsize;
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
	ptr = ptr + strsize;
}
//���ۿ��� data �и�
void UnPackPacket(const char* _buf, int& _data)
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

}

// ���� �Լ� ���� ��� �� ����
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
// ���� �Լ� ���� ���
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

// ����� ���� ������ ���� �Լ�
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

//Ŭ���̾�Ʈ ���� �߰�
_ClientInfo* AddClient(SOCKET sock, SOCKADDR_IN clientaddr)
{
	//���Ӿ˸�(������)
	printf("\nClient ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port));

	EnterCriticalSection(&cs); //�Ӱ迵�� ����

	//Ŭ���̾�Ʈ ���� ���� �� �Է�
	_ClientInfo* ptr = new _ClientInfo;
	ZeroMemory(ptr, sizeof(_ClientInfo));
	ptr->sock = sock;
	memcpy(&(ptr->clientaddr), &clientaddr, sizeof(clientaddr));
	ptr->state = INITE_STATE;		
	ptr->chatt_room_number = NODATA;
	ClientInfo[Client_Count++] = ptr;
	
	LeaveCriticalSection(&cs); //�Ӱ迵�� ����
	return ptr;
}

//Ŭ���̾�Ʈ ���� ����
void RemoveClient(_ClientInfo* ptr)
{
	//���� �ݱ�
	closesocket(ptr->sock);

	//���� Ŭ���̾�Ʈ ���� ������ ���
	printf("\nClient ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(ptr->clientaddr.sin_addr),
		ntohs(ptr->clientaddr.sin_port));

	EnterCriticalSection(&cs); //�Ӱ迵�� ����

	//Ŭ���̾�Ʈ ����
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

	Client_Count--; //������ Ŭ���̾�Ʈ �� ����
	LeaveCriticalSection(&cs);//�Ӱ迵�� Ż��
}

//ä�ñ׷� ����
bool EnterChattGroup(_ClientInfo* _ptr, int _roomnumber)
{
	EnterCriticalSection(&cs);//�Ӱ迵�� ����
	
	bool enterchattroom = false;

	_ChattGroupInfo* chattgroup = ChattGroupList[_roomnumber];

	if (!chattgroup->full) //�������� ������
	{
		//ä�ñ׷쿡 ����
		chattgroup->chattList[chattgroup->chat_count] = _ptr;//����Ʈ�� �����߰�
		_ptr->chatt_room_number = _roomnumber;//���ȣ
		chattgroup->nickname_list[chattgroup->chat_count] = _ptr->nickname;//�г��Ӽ���
		chattgroup->chat_count++;//�ο�����

		if (chattgroup->chat_count == MAX_CHATT_LIST)
		{//��á���� �˸�
			chattgroup->full = true;
		}

		enterchattroom = true;
	}

	LeaveCriticalSection(&cs);//�Ӱ迵�� ����
	return enterchattroom;
}

//ä�ñ׷쿡�� ����
void ExitChattInfo(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);//�Ӱ迵�� ����
	_ChattGroupInfo* chattgroup = ChattGroupList[_ptr->chatt_room_number];

	//����
	for (int i = 0; i < chattgroup->chat_count; i++)
	{
		if (chattgroup->chattList[i] == _ptr)
		{
			for (int j = i; j < chattgroup->chat_count - 1; j++)
			{
				chattgroup->chattList[j] = chattgroup->chattList[j + 1];
				chattgroup->nickname_list[j] = chattgroup->nickname_list[j + 1];
			}

			chattgroup->chat_count--;//������
			chattgroup->full=false;//Ǯ�ƴ϶�� �˸�
			break;
		}
	}

	_ptr->chatt_room_number = NODATA;//����� ä�ù� ����

	LeaveCriticalSection(&cs);//�Ӱ迵�� Ż��
}

//ä�ù� �ʱ�ȭ
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
//Ŭ�� ã��
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

//�г��� üũ(�ߺ��˻�)
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

//ä�ù��� �����ϴ� ��ȣ���� Ȯ��
bool CheckRoomNumber(int _number)
{
	if (_number< 1 || _number > MAX_CHATT_GROUP)//�ִ� ��ȣ����
	{
		return false;
	}
	return true;
}

//�г��� �߰�
void AddNickName(const char* _nick)
{
	EnterCriticalSection(&cs);
	char* ptr = new char[strlen(_nick) + 1];
	strcpy(ptr, _nick);
	NickNameList[Nick_Count++] = ptr;//�г��� ����Ʈ�� ����
	LeaveCriticalSection(&cs);
}

//�г��� ����
void RemoveNickName(const char* _nick)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < Nick_Count; i++)
	{
		//����
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

//�޼��� ���� �Լ���
//��: ä��
void MaKeChattMessage(const char* _nick, const char* _msg, char* _chattmsg)
{
	sprintf(_chattmsg, "[ %s ] %s", _nick, _msg);
}
//~~����
void MakeEnterMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s���� �����ϼ̽��ϴ�.", _nick);
}
//~~����
void MakeExitMessage(const char* _nick, char* _msg)
{
	sprintf(_msg, "%s���� �����ϼ̽��ϴ�.", _nick);
}

//�г��� ����
bool NickNameSetting(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	UnPackPacket(_clientinfo->recvbuf, msg);
	if (!CheckNickname(msg))//�г��� üũ
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
		AddNickName(_clientinfo->nickname);			//�г����߰�
	}	
	LeaveCriticalSection(&cs);	
	return true;
}

//ä�� �Ѹ���(�޼��� ����)
void ChattingMessageProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	char temp[BUFSIZE];
	int size;

	memset(msg, 0, sizeof(msg));
	memset(temp, 0, sizeof(temp));

	UnPackPacket(_clientinfo->recvbuf, msg);//����ִ� �޼�����
	MaKeChattMessage(_clientinfo->nickname, msg, temp);//ä���� �޼����� �ٲ�

	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)//���� ä�ñ׷��� Ŭ�󿡰� �� ����
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

// ä�ù� ���� �˸�
void ChattingEnterProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	MakeEnterMessage(_clientinfo->nickname, msg);//����޼���
	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];
	for (int i = 0; i < chattgroup->chat_count; i++)//������ ������ ����鿡�� ����˸�
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

//ä�ù� ���� �˸�
void ChattingOutProcess(_ClientInfo* _clientinfo)
{
	EnterCriticalSection(&cs);

	char msg[BUFSIZE];
	int size;

	MakeExitMessage(_clientinfo->nickname, msg); //ä�ù� ����˸�

	_ChattGroupInfo* chattgroup = ChattGroupList[_clientinfo->chatt_room_number];

	for (int i = 0; i < chattgroup->chat_count; i++)
	{
		if (chattgroup->chattList[i] == _clientinfo)
		{
			
			if (_clientinfo->ReSelect)//�缳����
			{
				_clientinfo->ReSelect = false;
				size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_RESELECT);
			}
			else//�����
			{
				size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_OUT);
			}
			
			if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				chattgroup->chattList[i]->state = CONNECT_END_STATE;
			}

		}
		else //����� ���� 
		{
			size = PackPacket(chattgroup->chattList[i]->sendbuf, CHATT_MSG, msg);
			if (send(chattgroup->chattList[i]->sock, chattgroup->chattList[i]->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting exit message Send()");
				chattgroup->chattList[i]->state = CONNECT_END_STATE;
			}
		}
	}	

	RemoveNickName(_clientinfo->nickname); // �г��� ����

	LeaveCriticalSection(&cs);

}
