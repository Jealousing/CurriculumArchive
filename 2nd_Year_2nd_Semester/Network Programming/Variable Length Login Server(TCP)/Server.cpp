#pragma comment(lib, "ws2_32.lib")//�Ӽ�->��Ŀ->�Է�->ws2_32.lib; �Է��ص���
#include <WinSock2.h>					 //��������ϻ���ϱ����� �������
#include <stdio.h>
#include <stdlib.h>
#include <time.h>								

#define SERVERPORT 9000			//������Ʈ ������ �������
#define BUFSIZE 512						//�޸𸮻�����
#define PLAYER_COUNT 1				//�÷��̾� �ο���
#define JOIN_USER_COUNT 100		//�ִ� ���尡���� ����

//�޼��� ������ ���� ��ũ��
#define ID_ERROR_MSG "���� ���̵��Դϴ�\n"
#define PW_ERROR_MSG "�н����尡 Ʋ�Ƚ��ϴ�\n"
#define LOGIN_SUCCESS_MSG "�α��ο� �����߽��ϴ�.\n"
#define JOIN_SUCCESS_MSG "���Կ� �����߽��ϴ�.\n"
#define ID_EXIST_MSG "�̹��ִ� ���̵��Դϴ�.\n"
#define GAMESTART_MSG "���ھ߱� ������ ���۵Ǿ����ϴ�.\n"
#define GAMESTOP_MSG "���ھ߱� ������ ����Ǿ����ϴ�.\n"
#define GAME_MSG "�߰��� �� �����Ͻðڽ��ϱ�?.\n"
#define GAME_ERROR_MSG "\n�ߺ����� �ʴ� 1~9������ ���ڸ� 3�� �Է����ּ���!.\n"
#define GAME_CLEAR_MSG "������ Ŭ���� �߽��ϴ�!."


enum PROTOCOL		//�������� ���� �������
{
	JOIN = 1,					//ȸ������
	LOGIN,						//�α���
	EXIT,							//����
	GAMESTART,			//���ӽ���
	GAMESTOP,				//����

	LOGIN_RESULT,		//�α��� ����
	JOIN_RESULT			//ȸ������ ����
};

enum RES_LOGIN	//�������� ���� �������
{
	ID_ERROR = 1,			//���¾��̵�
	PW_ERROR,				//�н����� ����
	LOGIN_SUCCESS		//�α��� ����
};

enum RES_JOIN		//�������� ���� �������
{
	JOIN_SUCCESS = 1,	//���Լ���
	ID_EXIST					//�̹��ִ� ���̵�
};

struct _UserInfo		//���� ���� ����
{
	char id[255];			//���̵�
	char pw[255];			//�佺����

	_UserInfo(const _UserInfo& info)//���������
	{
		strcpy(id, info.id);
		strcpy(pw, info.pw);
	}
	_UserInfo()//�Ҹ���
	{

	}
};

struct _ClientInfo		//Ŭ���̾�Ʈ ����
{
	SOCKET sock;			//��������
	SOCKADDR_IN addr;//�ּ�����
	_UserInfo* info;		//��������
	bool login;					//�α��� ����

	char buf[BUFSIZE];	//����
};

_ClientInfo* ClientInfo[PLAYER_COUNT];	//�÷��̾��� �ִ��ο��� ũ�⿡ ���� ����
int Count = 0;											//����Ƚ��

_UserInfo* UserInfo[JOIN_USER_COUNT];//ȸ������ ���� ������ ���� ����
int JoinCount = 0;										  //ȸ������ ����Ƚ��

_ClientInfo* AddClientInfo(SOCKET sock, SOCKADDR_IN addr)			//Ŭ���̾�Ʈ �������� �߰�
{
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));							//������ Ŭ���� ���� ���

	_ClientInfo* ptr = new _ClientInfo();												//�޸� �߰�
	ZeroMemory(ptr, sizeof(_ClientInfo));												//0���� ä��
	ptr->sock = sock;																				//�������� ����
	memcpy(&ptr->addr, &addr, sizeof(addr));										//�޸� ���� ����
	ptr->login = false;																			//�α��� x

	ClientInfo[Count++] = ptr;																	//����

	return ptr;
}

void ReMoveClientInfo(_ClientInfo* ptr)												//Ŭ���̾�Ʈ �������� ����
{
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));			//������ Ŭ���� ���� ���

	for (int i = 0; i < Count; i++)
	{
		if (ClientInfo[i] == ptr)
		{
			closesocket(ptr->sock);															//Ŭ����� ����
			delete ptr;																				//��������
			for (int j = i; j < Count - 1; j++)
			{
				ClientInfo[j] = ClientInfo[j + 1];												//�޸� ����
			}
			break;
		}
	}
	Count--;																							//�ο��� ����
}

void err_quit(const char* msg)														// ���� �Լ� ���� ��� �� ����
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

void err_display(const char* msg)												// ���� �Լ� ���� ���
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

int JoinProcess(_UserInfo info)													//ȸ������ ���μ���
{
	for (int i = 0; i < JoinCount; i++)
	{
		if (!strcmp(UserInfo[i]->id, info.id))										//���� ���̵� �ִ��� Ȯ��
		{
			return ID_EXIST;																//������ �ִٰ� ����
		}
	}

	UserInfo[JoinCount] = new _UserInfo(info);								//������ ����
	JoinCount++;
	return JOIN_SUCCESS;															//ȸ������ ����
}

void DropProcess(_ClientInfo* _ptr)											//ȸ��Ż�� ���μ���
{
	if (_ptr->login)																			//�α��� Ȯ��
	{
		for (int i = 0; i < JoinCount; i++)
		{
			if (!strcmp(UserInfo[i]->id, _ptr->info->id))						//���� ���̵� ���� ȸ�������� ã��
			{
				delete UserInfo[i];														//ȸ������ ����
				for (int j = i; j < JoinCount - 1; j++)
				{
					UserInfo[j] = UserInfo[j + 1];										//ȸ������ ���������
				}
				break;
			}
		}

		JoinCount--;																		//ȸ���� ����
	}
}

_UserInfo* SearchUserInfo(const char* _Id)								//���� ���� ã��
{
	for (int i = 0; i < JoinCount; i++)
	{
		if (!strcmp(UserInfo[i]->id, _Id))											//�ش� ���̵� �ִ��� Ȯ��
		{
			return UserInfo[i];															//�ش� �������������� ��ȯ
		}
	}

	return nullptr;																			//������ nullptr���
}

int LoginProcess(_UserInfo info)													//�α��� ���μ���
{

	for (int i = 0; i < JoinCount; i++)
	{
		if (!strcmp(UserInfo[i]->id, info.id))										//���������� ��ġ�ϴ� ���̵�ã��
		{
			if (!strcmp(UserInfo[i]->pw, info.pw))								//��ġ�� ���̵��� ��й�ȣ�� �Է��� ��й�ȣ�� ������ Ȯ��
			{
				return LOGIN_SUCCESS;												//�α��� ����
			}
			else
			{
				return PW_ERROR;														//�н����� ����
			}
		}
	}

	return ID_ERROR;																	//���̵� ����
}

int Packing(char* _buf, PROTOCOL _protocol, int _result, const char* str1) //Packing->_protocol, ����� �޼���
{	//buf�� ������� ������ �׾� ��Ű¡
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	memcpy(ptr, &_result, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	int strsize1 = strlen(str1);
	memcpy(ptr, &strsize1, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	memcpy(ptr, str1, strsize1);
	size = size + strsize1;
	ptr = ptr + strsize1;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

int Packing(char* _buf, PROTOCOL _protocol, const char* str1)					//Packing->_protocol, �޼���
{	//buf�� ������� ������ �׾� ��Ű¡
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	int strsize1 = strlen(str1);
	memcpy(ptr, &strsize1, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	memcpy(ptr, str1, strsize1);
	size = size + strsize1;
	ptr = ptr + strsize1;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

void  UnPacking(const char* _buf, char* str1, char* str2)						//Packing�� ������ Ǫ�� �Լ�
{	//char* �ΰ����
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	int strsize2;

	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;

	memcpy(&strsize2, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str2, ptr, strsize2);
	ptr = ptr + strsize2;
}

void  UnPacking(const char* _buf, char* str1)
{	//char* �Ѱ����
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	int strsize2;

	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;

}

int errcheck(char* num, int _count)	//���� Ȯ�� �Լ�
{															//�ߺ�����, ���ڰ��� Ȯ��
	if (num[0] - '0' + num[1] - '0' + num[2] - '0' < 6 || num[0] - '0' + num[1] - '0' + num[2] - '0' > 24)
		return 1;										//�ߺ������ʴ¼��� �ּҷ� ���ü��ִ¼� 123�� �̸�, �ִ� 789�� �ʰ��� �ϸ� ��������

	for (int i = 0; i < _count - 1; i++)
	{
		int x = num[i] - '0';
		int y = num[i + 1] - '0';
		if (x == y)										//x��y�� ���ؼ� ������ ���� 
			return 1;
	}
	return 0;											//����������
}


int main()
{
	char err[1];					//����Ȯ�ο�
	char roundinfo[3];		//��������
	int fastclear = 0;			//����Ŭ������ Ƚ��
	int slowclear = 0;			//������Ŭ������ Ƚ��
	int countclear = 0;		//Ŭ���� Ƚ��
	int runcount = 0;			//�õ�Ƚ��
	bool gamerun = false; //���� ���࿩��

	WSADATA  wsa;																// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)					//��������� ���� ���� ���� �� �ּҰ� ����, MAKEWORD�� 
		return -1;																		//ù��° ������ �ֹ���, �ι�° ������ �ι��� �� 2.2������ �ǹ�

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);	//IPV4,PCP�������ݻ���ϴ� ���ϻ���
	if (listen_sock == INVALID_SOCKET)	err_quit("socket()");		//����Ȯ��! ->���⼭ �߻��ϴ� ������ �ɰ��ѿ��� = quit

	// bind()
	SOCKADDR_IN serveraddr;													//���� ����ü ���� �ּҿ� ��Ʈ�� �Ҵ��ϱ� ���Ѱ�.
	ZeroMemory(&serveraddr, sizeof(serveraddr));					//�޸𸮸� 0���� �ʱ�ȭ
	serveraddr.sin_family = AF_INET;										//IPv4 �ּ�ü�� ���
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);				//����ּҵ��� �ްڴٴ� �ּ�����
	serveraddr.sin_port = htons(SERVERPORT);						//ȣ��Ʈ �ϳ��� ���ؼ��� SERVERPORT�� ������ ��Ʈ ����
	int retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));//retval=���Ϻ���, Ŭ������·� ����
	if (retval == SOCKET_ERROR)err_quit("bind()");					//��� ������ �ִ� ���� ������ ������ ��û ���� ->�ɰ��ѿ���=quit

	// listen()
	retval = listen(listen_sock, SOMAXCONN);								//���� ������ ���� Ŭ���̾�Ʈ�� ���� ��û�� Ȯ���ϵ��� ����
	if (retval == SOCKET_ERROR)err_quit("listen()");					//�ɰ��ѿ���! , SOMAXCONN�� socket max connection

	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;		//Ŭ���̾�Ʈ �ּ�
	SOCKET client_sock;				//��ſ� ���� ����
	char buf[BUFSIZE + 1];				//����!

	while (1)
	{
		// accept() Ŭ���� ���� ��û ��� �� ���
		int addrlen = sizeof(clientaddr);																//���� ������ ���ϱ� ���� Ŭ���ּ�ũ�⸦ ����
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);	//�������
		if (client_sock == INVALID_SOCKET)															//Ŭ�� ���� �޾ƿ��� ������ ������ Ŭ�� ���� ��ȯ
		{																												//INVALID_SOCKET�� ���� ������ ������ ��� ��ȯ�ϴ� ��
			err_display("accept()");
			break;
		}

		_ClientInfo* ptr = AddClientInfo(client_sock, clientaddr);							//Ŭ���̾�Ʈ�� ���������� ����ü �������

		while (1)
		{
			int size;				//������ ������ �ޱ����� ����
			char pitching[3];	//Ŭ�󿡼� �Ѿ�� ���� 3���� ���� ����
			int baseball[3];		//�������� ������ ���� 3���� ���� ����
			char score[2];		//��Ʈ����ũ�� ���� ���� üũ�� ���� ����
			int result;				//���

			//packing���� ó���� �����͸� �б� ���� 2�� recvn
			retval = recvn(ptr->sock, (char*)&size, sizeof(int), 0);  //1ȸ recvn : ���̱��ϱ�
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
			{
				break;
			}
			retval = recvn(ptr->sock, ptr->buf, size, 0);					//2ȸ recvn : ���̸�ŭ ������ �ޱ�
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
			{
				break;
			}

			PROTOCOL protocol;													//������ ���� ����
			memcpy(&protocol, ptr->buf, sizeof(PROTOCOL));		//�޸� ���� buf�� ������ protocol�� ����

			_UserInfo info;																//�������� ����ü ����

			switch (protocol)												//�޾ƿ� protocal ���� ���� switch�� ����
			{
			case JOIN://ȸ������
				ZeroMemory(&info, sizeof(_UserInfo));		//info �ʱ�ȭ
				UnPacking(ptr->buf, info.id, info.pw);			//�޾ƿ� packing�� ID�� PW�� unpacking
				result = JoinProcess(info);							//ȸ���������μ��� ����
				switch (result)												//����� ���� switch �˻�
				{
				case ID_EXIST:												//�̹� �ִ� ���̵�
					size = Packing(ptr->buf, JOIN_RESULT, ID_EXIST, ID_EXIST_MSG); //����� ID_EXIST_MSG�� packing
					break;
				case JOIN_SUCCESS:									//���Լ���
					size = Packing(ptr->buf, JOIN_RESULT, JOIN_SUCCESS, JOIN_SUCCESS_MSG);	//����� JOIN_SUCCESS_MSG�� packing
					break;
				}
				break;
			case LOGIN://�α���
				ZeroMemory(&info, sizeof(_UserInfo));		//info �ʱ�ȭ
				UnPacking(ptr->buf, info.id, info.pw);			//�޾ƿ� packing�� ID�� PW�� unpacking
				result = LoginProcess(info);						//�α��� ���μ��� ����
				switch (result)												//����� ���� switch �˻�
				{
				case ID_ERROR:											//���� ���̵�
					size = Packing(ptr->buf, LOGIN_RESULT, ID_ERROR, ID_ERROR_MSG);//����� MSG�� Packing
					break;
				case PW_ERROR:											//�佺���� ����
					size = Packing(ptr->buf, LOGIN_RESULT, PW_ERROR, PW_ERROR_MSG);//����� MSG�� Packing
					break;
				case LOGIN_SUCCESS:								//�α��μ���
					size = Packing(ptr->buf, LOGIN_RESULT, LOGIN_SUCCESS, LOGIN_SUCCESS_MSG);//����� MSG�� Packing
					ptr->info = SearchUserInfo(info.id);			//�α��� ������ �ش� �������� ã��
					ptr->login = true;										//�α�����
					break;
				}
				break;
			case GAMESTART://���ӽ���
				int strike;		//��Ʈ����ũ ���� üũ
				int ball;			//�� ���� üũ
				if (gamerun == false)	//������ �������� �ƴϿ����� ����
				{
					size = Packing(ptr->buf, GAMESTART, GAMESTART_MSG);	//���� ���۸޼����� ���ӽ������������� Packing
					ZeroMemory(&baseball, sizeof(baseball));							//������ ���� �迭 �ʱ�ȭ
					srand((unsigned)time(NULL));												//�ð��� ���� ������ ����
					for (int i = 0; i < 3; i++)
					{
						baseball[i] = rand() % 9 + 1;												//1~9������ ���� ��������
						for (int j = 0; j < i; j++)
						{
							if (baseball[i] == baseball[j])											//��ġ�� ���ڰ� �����Ǿ����� ����
								i--;																			//�ٽ� ����
						}
					}
					gamerun = true;																	//���� ����Ϸ�
					printf("������ ������ ���� : %d, %d, %d\n", baseball[0], baseball[1], baseball[2]);//������ ������ ����
				}
				else//�������� ���
				{
					UnPacking(ptr->buf, pitching);			//Ŭ���̾�Ʈ���� ���ڸ� �޾ƿ�
					printf("Ŭ���̾�Ʈ�� �Է��� ���� : %c, %c, %c\n", pitching[0], pitching[1], pitching[2]);//�޾ƿ� �������
					ZeroMemory(&score, sizeof(score));	//��Ʈ����ũ�� ���� ������ ���� ���� �ʱ�ȭ
					ZeroMemory(&strike, sizeof(strike));	//��Ʈ����ũ ���� �ʱ�ȭ
					ZeroMemory(&ball, sizeof(ball));			//�� ���� �ʱ�ȭ

					err[0] = 0; //0�̸� ����x

					for (int i = 0; i < 3; i++)
					{
						for (int j = 0; j < 3; j++)
						{
							if (baseball[i] == pitching[j] - '0')//��ǻ�Ͱ� ���� ���ڿ� Ŭ���̾�Ʈ���� ���� ���� ��
							{
								if (i == j)	//���� ��ġ�� ������ ��Ʈ����ũ
								{
									strike++;
								}
								else			//���� ���ڰ� ������ ��ġ�� �ٸ���� �� ����
								{
									ball++;
								}
							}
						}
					}

					err[0] = errcheck(pitching, 3);	//���� üũ ������ ������� 1�� ����

					size = Packing(ptr->buf, GAMESTART, err);	//���� ���� ����
					retval = send(ptr->sock, ptr->buf, size, 0);	//������ Ŭ���̾�Ʈ�� ����
					if (retval == SOCKET_ERROR)
					{
						err_display("send()");
						break;
					}

					if (err[0] == 0)							//������ ������� ����
					{
						score[0] = strike;					//üũ�� ��Ʈ����ũ ������ ���ھ� 0���迭�� ����
						score[1] = ball;						//üũ�� �� ������ ���ھ� 1�� �迭�� ����
						printf("Strike : %d , Ball : %d \n", strike, ball);	//��Ʈ����ũ�� ���� ������ ������ ���
						runcount++;							//�õ� Ƚ�� ����
						if (strike == 3)//��Ʈ����ũ 3�� = ��������
						{
							size = Packing(ptr->buf, GAMESTART, score);//���ھ� ���� ������ ���� packing
							retval = send(ptr->sock, ptr->buf, size, 0);	  //packing�� ���� Ŭ���̾�Ʈ�� ����
							if (retval == SOCKET_ERROR)
							{
								err_display("send()");
								break;
							}
							//������ ���� ���� ���� ���
							printf("\n3��Ʈ����ũ ��������\n");
							printf("�÷��̾ %d������ Ŭ���� �߽��ϴ�\n", runcount);

							ZeroMemory(buf, sizeof(buf));
							sprintf(buf, "%d", runcount); //buf�� ����Ƚ�� ����
							strcat(buf, GAME_CLEAR_MSG);	//buf �ڿ� GAME_CLEAR_MSG����

							size = Packing(ptr->buf, GAMESTART, buf);	//��ȸ �����ߴ��� Packing
							countclear++;	//Ŭ���� Ƚ������
							if (fastclear == 0 || fastclear > runcount)		//����� ������ ���ų� �� ª�� ����� ������ ����
							{
								fastclear = runcount;
							}

							if (slowclear == 0 || slowclear < runcount)	//����� ������ ���ų� �� �� ����� ������ ����
							{
								slowclear = runcount;
							}

							runcount = 0;	//����Ƚ�� �ʱ�ȭ
							gamerun = false;	//�������� off
							break;
						}
						else
						{
							size = Packing(ptr->buf, GAMESTART, score);	//���ھ� ���� packing 
						}
					}
					else	//������ ��� ����
					{
						printf("\nŬ���̾�Ʈ ���ڰ� �߸��Ǿ����ϴ�.\n");
						err[0] = 0;//�������� �ʱ�ȭ
						size = Packing(ptr->buf, GAMESTART, GAME_ERROR_MSG); //�����޼��� packing�� ����
					}
				}
				break;
			case GAMESTOP://��������
				size = Packing(ptr->buf, GAMESTOP, GAMESTOP_MSG);//�������� �޼��� packing
				retval = send(ptr->sock, ptr->buf, size, 0);	//��������޽��� Ŭ���̾�Ʈ�� ����
				if (retval == SOCKET_ERROR)
				{
					err_display("send()");
					break;
				}
				roundinfo[0] = countclear;	//Ŭ���� Ƚ������
				roundinfo[1] = fastclear;		//������ Ŭ������ ���� ����
				roundinfo[2] = slowclear;		//������ Ŭ������ ���� ����

				size = Packing(ptr->buf, GAMESTOP, roundinfo);	//Ŭ���� ����packing
				//���� ���� �ʱ�ȭ
				ZeroMemory(roundinfo, sizeof(roundinfo));
				ZeroMemory(&countclear, sizeof(countclear));
				ZeroMemory(&fastclear, sizeof(fastclear));
				ZeroMemory(&slowclear, sizeof(slowclear));
				ptr->login = false;//�α������� off
				break;
			}

			retval = send(ptr->sock, ptr->buf, size, 0); //�������� Packing�� ���� Ŭ���̾�Ʈ�� ����
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		}

		ReMoveClientInfo(ptr);//��������Ǵ� Ŭ���̾�Ʈ ���� ����
	}

	closesocket(listen_sock);//���� ����

	WSACleanup();//������ ���� ����

	return 0;
}