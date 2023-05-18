#pragma comment(lib, "ws2_32.lib")//�Ӽ�->��Ŀ->�Է�->ws2_32.lib; �Է��ص���
#include <WinSock2.h>					 //��������ϻ���ϱ����� �������
#include <stdio.h>
#include <stdlib.h>

#define SERVERIP "127.0.0.1"			//������ �ּ�
#define SERVERPORT 9000			//������Ʈ ������ �������
#define BUFSIZE 512						//�޸𸮻�����

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

int Packing(char* _buf, PROTOCOL _protocol, const char* _id, const char* _pw)	//Packing -> _protocol,���̵�� ���
{	//buf�� ������� ������ �׾� ��Ű¡
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	int strsize1 = strlen(_id);
	memcpy(ptr, &strsize1, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	memcpy(ptr, _id, strsize1);
	size = size + strsize1;
	ptr = ptr + strsize1;

	int strsize2 = strlen(_pw);
	memcpy(ptr, &strsize2, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	memcpy(ptr, _pw, strsize2);
	size = size + strsize2;
	ptr = ptr + strsize2;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}
int Packing(char* _buf, PROTOCOL _protocol, const char* _data)	//Packing ->_protocol, data
{	//buf�� ������� ������ �׾� ��Ű¡
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	int strsize1 = strlen(_data);
	memcpy(ptr, &strsize1, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	memcpy(ptr, _data, strsize1);
	size = size + strsize1;
	ptr = ptr + strsize1;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

int Packing(char* _buf, PROTOCOL _protocol)		//Packing ->_protocol,
{	//buf�� ������� ������ �׾� ��Ű¡
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}


void UnPacking(const char* _buf, int& _result, char* str1)	//Packing�� ������ Ǫ�� �Լ� ->_result,str1
{
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(&_result, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;
}

void UnPacking(const char* _buf, char* str1)	//Packing�� ������ Ǫ�� �Լ�->str1
{
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	//memcpy(a,b,n) b�޸� �������� a �޸� �������� n����Ʈ��ŭ ����
	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;
}

int main()
{
	char roundinfo[3];		//��������
	char buf[BUFSIZE];		//����
	bool gamerun = true;	//���� ���࿩��
	int result;						//���

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
	
	while (1)
	{
		//ó�� ���� �޴� ���
		printf("<<�޴�>>\n");
		printf("1.����\n");
		printf("2.�α���\n");
		printf("3.����\n");
		printf("����:");

		//�޴� ����
		int select;
		scanf("%d", &select);

		if (select == EXIT)			//������ ��� break
		{
			printf("����");
			break;
		}

		_UserInfo info;				//�������� ����ü ����

		printf("ID:");					//ID �Է¹���
		scanf("%s", info.id);

		printf("PW:");				//PW �Է¹���
		scanf("%s", info.pw);

		int size = Packing(buf, (PROTOCOL)select, info.id, info.pw);	//�Է¹��� ID,PW Packing

		retval = send(sock, buf, size, 0);	//Packing�� ���� ������ ����
		if (retval == SOCKET_ERROR)		//���� Ȯ��
		{
			err_display("send()");
			break;
		}

		//packing���� ó���� �����͸� �б� ���� 2�� recvn
		retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1ȸ recvn : ���̱��ϱ�
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
		{
			break;
		}
		retval = recvn(sock, buf, size, 0);							//2ȸ recvn : ���̸�ŭ ������ �ޱ�
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
		{
			break;
		}

		PROTOCOL protocol;											//������ ���� ����
		memcpy(&protocol, buf, sizeof(PROTOCOL));		//�޸� ���� buf�� ������ protocol�� ����

		char msg[BUFSIZE];											//�޼����� ���� �ޱ����� ����
		ZeroMemory(msg, sizeof(msg));							//�ʱ�ȭ
		
		switch (protocol)													//�Է� ���� �������ݷ� ã�Ƽ� ����
		{
		case LOGIN_RESULT:											//�α���
			UnPacking(buf, result, msg);							//Packing�� ���� UnPacking
			printf("%s\n", msg);										//���� ���� �޼��� ���
			switch (result)													//���� ���� ����� ����
			{
			case PW_ERROR:												//�α����� �佺���� ����
			case ID_ERROR:												//�α����� ���̵� ����
				break;
			case LOGIN_SUCCESS:									//�α��� ����
			{
				while (gamerun)											//���� �������� ��� �ݺ�
				{
					//�α������� ��� �޴��� ���
					printf("<<�޴�>>\n");
					printf("1.���ӽ���\n");
					printf("2.����\n");
					printf("����:");
					//�޴�����
					int select;
					scanf("%d", &select);

					size = Packing(buf, (PROTOCOL)(select + 3));	//���õ� �޴���ȣ Packing
					retval = send(sock, buf, size, 0);						//�޴���ȣ �������� ����
					if (retval == SOCKET_ERROR)
					{
						err_display("send()");
						break;
					}

					//packing���� ó���� �����͸� �б� ���� 2�� recvn
					retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1ȸ recvn : ���̱��ϱ�
					if (retval == SOCKET_ERROR)
					{
						err_display("recv()");
						break;
					}
					else if (retval == 0)
					{
						break;
					}
					retval = recvn(sock, buf, size, 0);							//2ȸ recvn : ���̸�ŭ ������ �ޱ�
					if (retval == SOCKET_ERROR)
					{
						err_display("recv()");
						break;
					}
					else if (retval == 0)
					{
						break;
					}

					memcpy(&protocol, buf, sizeof(PROTOCOL));//�޸� ���� buf�� ������ protocol�� protocolũ�⸸ŭ ����


					if (protocol == GAMESTART&&gamerun) //protocol�� ���ӽ����� ����ũ�� ��� && ���� �����ΰ��
					{
						char score[2];					//��Ʈ����ũ�� ���� ������ ����
						char err[1];						//���� ���� ����

						UnPacking(buf, msg);		//packing�� msg�� unpacking
						printf("\n%s", msg);		//����� �޼��� ���

						char pitching[255];			//Ŭ���̾�Ʈ�� �Է��� ���ڸ� ������ ����

						while (1)
						{
							ZeroMemory(&pitching, sizeof(pitching));						//�ʱ�ȭ
							printf("�ߺ� ���� �ʴ� ���ڸ� 3���� �Է��ϼ���. ex) 416 :");
							scanf("%s", &pitching);													//�Է¹ޱ�
							size = Packing(buf, (PROTOCOL)(select + 3), pitching);	//�Էµ� ���ڿ� protocol packing 
							retval = send(sock, buf, size, 0);									//����� ���� ������ ����
							if (retval == SOCKET_ERROR)										//����üũ
							{
								err_display("send()");
								break;
							}

							//packing���� ó���� �����͸� �б� ���� 2�� recvn
							retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1ȸ recvn : ���̱��ϱ�
							if (retval == SOCKET_ERROR)
							{
								err_display("recv()");
								break;
							}
							else if (retval == 0)
							{
								break;
							}
							retval = recvn(sock, buf, size, 0);							//2ȸ recvn : ���̸�ŭ ������ �ޱ�
							if (retval == SOCKET_ERROR)
							{
								err_display("recv()");
								break;
							}
							else if (retval == 0)
							{
								break;
							}

							ZeroMemory(&score, sizeof(score));	//���ھ� ���� �ʱ�ȭ
							ZeroMemory(&err, sizeof(err));			//���� ���� �ʱ�ȭ

							UnPacking(buf, err);								//packing�� err���� unpacking

							if (err[0] !=1)//������ �ƴҰ�� ����
							{
								//packing���� ó���� �����͸� �б� ���� 2�� recvn
								retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1ȸ recvn : ���̱��ϱ�
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}
								retval = recvn(sock, buf, size, 0);							//2ȸ recvn : ���̸�ŭ ������ �ޱ�
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}

								UnPacking(buf, score);						//packing�� score���� unpacking

								if (score[0] == 3)//��Ʈ����ũ�� 3���ϰ��
								{
									printf("\n");
									//packing���� ó���� �����͸� �б� ���� 2�� recvn
									retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1ȸ recvn : ���̱��ϱ�
									if (retval == SOCKET_ERROR)
									{
										err_display("recv()");
										break;
									}
									else if (retval == 0)
									{
										break;
									}
									retval = recvn(sock, buf, size, 0);							//2ȸ recvn : ���̸�ŭ ������ �ޱ�
									if (retval == SOCKET_ERROR)
									{
										err_display("recv()");
										break;
									}
									else if (retval == 0)
									{
										break;
									}

									ZeroMemory(msg, sizeof(msg));	//msg �ʱ�ȭ
									UnPacking(buf, msg);					//packing�� msg���� unpacking
									printf("%s\n", msg);					//�޾ƿ� msg ���
									printf("3Strike ���ӿ��� �¸��ϼ̽��ϴ�.\n\n");//������ ���� �� ����
									break;
								}
								else if (score[0] == 0 && score[1] == 0)
								{	//��Ʈ����ũ�� ���� �ϳ��� ����.
									printf("OUT : �ϳ��� ������ ���߽��ϴ�.\n");
								}
								else
								{	//�޾ƿ� ��Ʈ����ũ�� ���� ���� ���
									printf("Strike: %d, Ball: %d\n", score[0], score[1]);
								}
								printf("%s\n", msg);//����� msg ���
							}
							else//�����ϰ��
							{
								//packing���� ó���� �����͸� �б� ���� 2�� recvn
								retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1ȸ recvn : ���̱��ϱ�
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}
								retval = recvn(sock, buf, size, 0);							//2ȸ recvn : ���̸�ŭ ������ �ޱ�
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}

								UnPacking(buf, msg);					//packing�� msg���� unpacking
								printf("%s\n", msg);					//�޾ƿ� msg ���
							}

							ZeroMemory(buf, sizeof(buf));			//buf �ʱ�ȭ
						}
						
					}
					else if(protocol== GAMESTOP)//��������
					{
						ZeroMemory(msg, sizeof(msg));		//msg�ʱ�ȭ
						gamerun = false;								//�������� off
						UnPacking(buf, msg);					//packing�� msg���� unpacking
						printf("%s\n", msg);					//�޾ƿ� msg ���

						//packing���� ó���� �����͸� �б� ���� 2�� recvn
						retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1ȸ recvn : ���̱��ϱ�
						if (retval == SOCKET_ERROR)
						{
							err_display("recv()");
							break;
						}
						else if (retval == 0)
						{
							break;
						}
						retval = recvn(sock, buf, size, 0);							//2ȸ recvn : ���̸�ŭ ������ �ޱ�
						if (retval == SOCKET_ERROR)
						{
							err_display("recv()");
							break;
						}
						else if (retval == 0)
						{
							break;
						}

						UnPacking(buf, roundinfo);	//packing�� roundinfo���� unpacking
						if(roundinfo[0]>=1)				//������ ����Ƚ���� 1�� ���� ��� ����
						printf("\n���ݱ��� ������ ������ Ƚ��: %d\n���� ������ ���� ������ ���� �õ� Ƚ��: %d\n���� �ʰ� ���� ������ ���� �õ� Ƚ��: %d\n", roundinfo[0], roundinfo[1], roundinfo[2]);
						break;
					}

				}
			}
			break;
			}
			break;

		case JOIN_RESULT:						//ȸ�� ����
			UnPacking(buf, result, msg);	//packing�� msg,result���� unpacking
			switch (result)							//�޾ƿ� result�� ���� ����
			{
			case ID_EXIST:							//�̹� �ִ� ���̵�
			case JOIN_SUCCESS:				//ȸ������ ����
				printf("%s\n", msg);
				break;
			}
			break;
		}
	}
	closesocket(sock);//���� ����

	// ���� ����
	WSACleanup();
	return 0;
}