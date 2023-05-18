#pragma comment(lib, "ws2_32.lib")//�Ӽ�->��Ŀ->�Է�->ws2_32.lib; �Է��ص���
#include <WinSock2.h>					 //��������ϻ���ϱ����� �������
#include <stdio.h>
#include <stdlib.h>

#define SERVERIP "127.0.0.1"			//������ �ּ�
#define SERVERPORT 9000			//������Ʈ ������ �������
#define BUFSIZE 512						//�޸𸮻�����
#define PLAYERNUMCOUNT 3			//�÷��̾ �Է��� ������ ����

enum PROTOCOL //�������� + ������ ���������� �Ŵ��������� �и�
{
	INTRO = 1, // ��Ʈ��

	// ���� �÷���
	ANSWER_NUM,                                //Ŭ���̾�Ʈ�� �Է��� ����
	ANSWER_RESULT,                         //������ �Ǵ��� ���
	ANSWER_ERROR,                          //3�� ���ڰ� ����Ȯ��

	// ���� ���� 
	CORRECT_ANSWER_RESULT,     //������ ���߾��� ��
	GAME_PROCESS,                        //���࿩��
	GAME_RESULT                           //���ݱ��� �÷����� ����
};

enum ERROR_CODE   //�����ڵ� ���ڿ�
{
	DATA_DUPLICATE = 1,   //���� �ߺ�
	DATA_RANGE_ERROR   //���� ����
};

enum GAME_PROCESS   //���� ������ Ŭ��->���� �߰��Ǵ�
{
	GAME_CONTINUE = 1,   //���Ӱ��
	GAME_OVER               //��������
};

enum   //�迭�� INDEX�� ���=���� ���� x
{
	STRIKE,
	BALL,
	OUTC
};

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

bool PacketRecv(SOCKET _sock, char* _buf)   //��Ŷ�� �޴� �Լ� (2��ó�� �ߴ��� �ѹ���)
{
	int size;
	int retval = recvn(_sock, (char*)&size, sizeof(size), 0);   //�޴� �������� ũ��
	if (retval == SOCKET_ERROR)   //�������� (����)
	{
		err_display("gvalue recv error()");
		return false;
	}
	else if (retval == 0)                   //���� ����
	{
		return false;
	}

	retval = recvn(_sock, _buf, size, 0);   // �����ŭ �����͸� �޾ƿ�
	if (retval == SOCKET_ERROR)   //�������� (����)
	{
		err_display("gvalue recv error()");
		return false;

	}
	else if (retval == 0)                   //���� ����
	{
		return false;
	}

	return true;   //��� ����
}

PROTOCOL GetProtocol(char* _buf)   //�������ݸ� �и�
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));

	return protocol;
}

//���� , �������� , ���࿩��
int PackPacket(char* _buf, PROTOCOL  _protocol, int _data)
{
	int size = 0;
	char* ptr = _buf;
	ptr = ptr + sizeof(size);

	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
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

//����, �������� , �÷��̾� ����, ���ڰ���
int PackPacket(char* _buf, PROTOCOL  _protocol, const int* _playernum, int _numcount)
{
	char* ptr = _buf;
	int size = 0;
	ptr = ptr + sizeof(size);

	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_numcount, sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	memcpy(ptr, &_playernum[0], sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	memcpy(ptr, &_playernum[1], sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	memcpy(ptr, &_playernum[2], sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

//����, �޼���
void UnPackPacket(char* _buf, char* _msg)//Packet�� ������ Ǫ�� �Լ�
{
	char* ptr = _buf + sizeof(PROTOCOL);

	int strsize = 0;

	memcpy(&strsize, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(_msg, ptr, strsize);
	ptr = ptr + strsize;

}
//����, ������ , �޼���
void UnPackPacket(char* _buf, int* _data, char* _msg)//Packet�� ������ Ǫ�� �Լ�
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(_data, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	int strsize = 0;

	memcpy(&strsize, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(_msg, ptr, strsize);
	ptr = ptr + strsize;

	printf("����Ȯ�� %s \n", _msg);

}

int main()
{
	//��� ������
	PROTOCOL protocol;			//���� ��ġ(��Ȳ)
	char sendbuf[BUFSIZE];		//send�� ����
	char recvbuf[BUFSIZE];		//recv�� ����
	char msg[BUFSIZE];			//msg����
	bool IsGame = true;				//�������� �Ǵ�
	bool IsPlay = false;				//�߰����� �Ǵ�
	int playernum[PLAYERNUMCOUNT];		//�÷��̾ ���� ���� ����
	int select;												//����
	int size;												//������
	int temp;												//�ӽ�

	WSADATA  wsa;																// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)					//��������� ���� ���� ���� �� �ּҰ� ����, MAKEWORD�� 
		return -1;																		//ù��° ������ �ֹ���, �ι�° ������ �ι��� �� 2.2������ �ǹ�

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);	//IPV4,PCP�������ݻ���ϴ� ���ϻ���
	if (sock == INVALID_SOCKET)	err_quit("socket()");		//����Ȯ��! ->���⼭ �߻��ϴ� ������ �ɰ��ѿ��� = quit

	// connect()
	SOCKADDR_IN serveraddr;											//���� ����ü ���� �ּҿ� ��Ʈ�� �Ҵ��ϱ� ���Ѱ�.
	ZeroMemory(&serveraddr, sizeof(serveraddr));			//�޸𸮸� 0���� �ʱ�ȭ
	serveraddr.sin_family = AF_INET;								//IPv4 �ּ�ü�� ���
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);  //inet_addr�� ���ڿ��� IPv4�ּҷ� ��ȯ�ϴ� �Լ�
	serveraddr.sin_port = htons(SERVERPORT);				//������Ʈ�θ� ����
	int retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));//���� ���� �õ�
	if (retval == SOCKET_ERROR)err_quit("connect()");		//����Ȯ��

	while (1) //�������� ����
	{
		//ó�� ���� �޴� ���
		printf("<<�޴�>>\n");
		printf("1.���� ����\n");
		printf("2.����\n");

		//�޴� ����
		printf("����:");
		scanf("%d", &select);

		if (select == GAME_OVER)					//������ ��� break
		{
			break;
		}
		else if (select == GAME_CONTINUE)	//������ ��� ����
		{
			while (IsGame)								//�������� ����
			{
				IsPlay = true;							//������ �����Ҳ��� true�� �ٲ���
				ZeroMemory(msg, sizeof(msg));//�޸𸮸� 0���� ä����(�ʱ�ȭ)
				//���� �����ϴ� �޽����� �����κ��� ���޹���.
				PacketRecv(sock, recvbuf);			
				UnPackPacket(recvbuf, msg);
				printf("%s", msg);
				
				while (IsPlay)			//��������
				{
					//�÷��̾ �������� ������ ���� �Է¹���
					printf("�Է�: ");
					scanf("%d %d %d", &playernum[0], &playernum[1], &playernum[2]);
					/*
					for (int i = 0; i < PLAYERNUMCOUNT; i++)
					{//�ٸ��Է¹��
						printf("%d��° ���� �Է�: ",i);
						scanf("%d", &playernum[i]);
					}*/			
					protocol = (PROTOCOL)ANSWER_NUM; //���� �����Ѵٴ°� ������ �˷��ֱ� ���� protocol�� ����
					size = PackPacket(sendbuf, protocol, playernum, PLAYERNUMCOUNT); //sndbuf�� ��Ű¡ ó��
					retval = send(sock, sendbuf, size, 0);	//������ ����
					if (retval == SOCKET_ERROR)//����Ȯ��
					{
						err_display("send()");
						break;
					}

					PacketRecv(sock, recvbuf); //�������� ���� ���� �����͸� �ι��� ���� �޾ƿ�
					protocol = GetProtocol(recvbuf);	//recv���ۿ��� �������� ���� ������
					switch (protocol)
					{
					case ANSWER_RESULT: //������ �ƴҰ�� ��Ʈ����ũ �~ �� � ~ �ƿ� �~ �� ���
						ZeroMemory(msg, sizeof(msg));
						UnPackPacket(recvbuf, msg); //��Ű¡�� ���۸� ǰ
						printf("%s", msg);//���
						break;
					case ANSWER_ERROR:							 //������ ������� ������ ���
						ZeroMemory(msg, sizeof(msg));			 //�޸��ʱ�ȭ
						UnPackPacket(recvbuf, &temp, msg);	 //��Ű¡�� ���۸� ǰ
						printf("%s", msg);									 //���
						break;
					case CORRECT_ANSWER_RESULT:			 //������ ��� ����
						ZeroMemory(msg, sizeof(msg));			 //�޸��ʱ�ȭ
						UnPackPacket(recvbuf, msg);				 //��Ű¡�� ���۸� ǰ
						printf("%s", msg);									 //������� ������ ������� �޾ƿ°� ���
						ZeroMemory(msg, sizeof(msg));			 //�޸��ʱ�ȭ
						PacketRecv(sock, recvbuf);					 //�������� ���޹��� �����͸� recv
						UnPackPacket(recvbuf, msg);				 //��Ű¡�� ���۸� ǰ
						printf("%s", msg);									 //�߰��� �����Ұ��� ����� �޼����� �޾ƿͼ� ���

						//�߰� �����Ұ�����
						printf("1.��  2.�ƴϿ�\n");
						scanf("%d", &select);

						if (select == GAME_CONTINUE)												//�߰�����
						{
							protocol = GAME_PROCESS;												//����������ġ�� �߰����� �������κ��ΰ��� �˸��� ���� ����
							size = PackPacket(sendbuf, protocol, GAME_CONTINUE); //�߰� �����ϴ°Ͱ� ���������� ��Ű¡
							retval = send(sock, sendbuf, size, 0);								//��Ű¡�� ������ ����
							if (retval == SOCKET_ERROR)											//����Ȯ��
							{
								err_display("send()");
								break;
							}
							IsPlay = false;																	//�ٽ� ������ ���� while Ż��
						}
						else if (select == GAME_OVER)												//�߰�����x
						{
							protocol = GAME_PROCESS;												//����������ġ�� �߰����� �������κ��ΰ��� �˸��� ���� ����
							size = PackPacket(sendbuf, protocol, GAME_OVER);			//�߰������� ���ϴ� �Ͱ� ���������� ��Ű¡
							retval = send(sock, sendbuf, size, 0);								//��Ű¡�� ������ ����
							if (retval == SOCKET_ERROR)											//����Ȯ��
							{
								err_display("send()");
								break;
							}
							IsPlay = false;																	//while Ż��
							IsGame = false;																//�������� Ż��
						}
						break;
					default:
						break;
					}
				}
			}
			ZeroMemory(msg, sizeof(msg));			//�޸� �ʱ�ȭ
			PacketRecv(sock, recvbuf);					//�����κ��� �����͸� ����
			UnPackPacket(recvbuf, msg);				//���� �������� ��Ű¡�� �����͸� ����
			printf("%s", msg);									//���� �޼��� ���
		}		
		else
		{	//�Է¿���
			printf("���� �ʴ� ���ڸ� �Է��ϼ̽��ϴ� �ٽ��Է����ּ���! \n");
		}
	}
	closesocket(sock);//���� ����
	// ���� ����
	WSACleanup();
	return 0;
}