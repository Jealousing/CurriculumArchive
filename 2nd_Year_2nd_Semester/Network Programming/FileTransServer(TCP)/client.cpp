#pragma comment(lib, "ws2_32.lib")//�Ӽ�->��Ŀ->�Է�->ws2_32.lib; �Է��ص���
#include <WinSock2.h>					 //��������ϻ���ϱ����� �������
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"				//������ �ּ�
#define SERVERPORT 9000				//������Ʈ ������ �������
#define BUFSIZE    4096					//���� ������
#define MAX_FILE_READ_SIZE 2048	//�������� ������ ���ۻ������ 12�̻� �۾ƾߵ�! 
#define FILENAMESIZE 256				//���� �̸� ������

enum PROTOCOL								//�������� + ������ ���������� �Ŵ��������� �и�
{
	INTRO = 1,
	FILE_INFO,									// Ŭ�� �������� ������ ��Ŷ ��������
	FILE_TRANS_DENY,						// ���� ���� �ź�
	FILE_TRANS_START_POINT,			// ���� ���� ��ġ(ó������ & �̾)
	FILE_TRANS_DATA						// Ŭ�� ���� ������ ��������
};

enum DENY_CODE   // ���� ���� �źλ���
{
	FILEEXIST = -1   // ���� ����
};

struct _File_info								//��������
{
	char filename[FILENAMESIZE];	//�����̸�
	int  filesize;									//���ϻ�����
	int  nowsize;								//����������
}Fileinfo;

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

//��Ű¡ ����<- ��������/ �����̸� / ����ũ��
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1, int _data)
{
	char* ptr = _buf;
	int strsize = strlen(_str1);
	int size = 0;

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &strsize, sizeof(strsize));
	ptr = ptr + sizeof(strsize);
	size = size + sizeof(strsize);

	memcpy(ptr, _str1, strsize);
	ptr = ptr + strsize;
	size = size + strsize;

	memcpy(ptr, &_data, sizeof(_data));
	ptr = ptr + sizeof(_data);
	size = size + sizeof(_data);

	ptr = _buf;

	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}
//��Ű¡ ����<- ��������/���ۻ�����/��������
int PackPacket(char* _buf, PROTOCOL _protocol, int _datasize, const char* _filedata)
{
	char* ptr = _buf;
	int size = 0;

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_datasize, sizeof(_datasize));
	ptr = ptr + sizeof(_datasize);
	size = size + sizeof(_datasize);

	memcpy(ptr, _filedata, _datasize);
	ptr = ptr + _datasize;
	size = size + _datasize;

	ptr = _buf;

	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}
//�������� ��������
PROTOCOL GetProtocol(const char* _buf)
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));
	return protocol;
}

//����Ű¡ ����->����/�޼���
void UnPackPacket(const char* _buf, int& _data, char* _str)
{
	const char* ptr = _buf;
	int strsize;

	ptr = ptr + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(_data));
	ptr = ptr + sizeof(_data);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
}
//����Ű¡ ����->�޽���
void UnPackPacket(const char* _buf, char* _str)
{
	const char* ptr = _buf;
	int strsize;

	ptr = ptr + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
}
//����Ű¡ ����->���� �����ͻ�����
void UnPackPacket(const char* _buf, int& _data)
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(_data));
}
//recvn�� �ι�ó���ϴ� �Լ�
bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;
	//����� �޾ƿ���
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
	//�����ŭ �����͸� �޾ƿ�
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

int main(int argc, char* argv[])
{
	int retval;										//����Ȯ��
	char buf[BUFSIZE];						//����
	
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
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));//���� ���� �õ�
	if (retval == SOCKET_ERROR)err_quit("connect()");		//����Ȯ��

	ZeroMemory(&Fileinfo, sizeof(Fileinfo));
		
	FILE* fp = nullptr;			//����������
	bool endflag = false;			

	while (1)
	{
		if (!PacketRecv(sock, buf))
		{
			break;
		}

		PROTOCOL protocol=GetProtocol(buf);

		switch (protocol)
		{
		case INTRO:		
			{				
				char msg[BUFSIZE];	

				memset(msg, 0, sizeof(msg));
				UnPackPacket(buf, msg);
				printf("%s\n", msg);

				strcpy(Fileinfo.filename, argv[1]);

				fp = fopen(Fileinfo.filename, "rb");
				if (!fp)
				{
					err_quit("fopen");
				}

				fseek(fp, 0, SEEK_END);
				Fileinfo.filesize = ftell(fp);
				fclose(fp);

				int size=PackPacket(buf, FILE_INFO, Fileinfo.filename, Fileinfo.filesize);

				retval = send(sock, buf, size, 0);
				if (retval == SOCKET_ERROR)
				{
					err_quit("file info send()");
				}
			}

			break;
		case FILE_TRANS_DENY:
			{
				char msg[BUFSIZE];
				int deny_code;
				memset(msg, 0, sizeof(msg));
				UnPackPacket(buf, deny_code, msg);
				switch (deny_code)
				{
				case FILEEXIST:
					printf("%s\n", msg);
					endflag = true;
					break;
				}
			}
			
			break;
		case FILE_TRANS_START_POINT:
			{
				
				UnPackPacket(buf, Fileinfo.nowsize);

				fp = fopen(Fileinfo.filename, "rb");
				fseek(fp, Fileinfo.nowsize, SEEK_SET);

				while (1)
				{
					char rbuf[BUFSIZE];
					int trans_size = fread(rbuf, 1, MAX_FILE_READ_SIZE, fp);
					if (trans_size > 0)
					{
						int size=PackPacket(buf, FILE_TRANS_DATA, trans_size, rbuf);
						retval = send(sock, buf, size, 0);
						if (retval == SOCKET_ERROR)
						{
							err_display("send()");
							break;
						}
						Fileinfo.nowsize += trans_size;
						printf("..");
						Sleep(200);
					}
					else if (trans_size == 0 && Fileinfo.nowsize == Fileinfo.filesize)
					{
						printf("���� ���� �Ϸ�!: %d ����Ʈ\n", Fileinfo.filesize);
						endflag = true;
						break;
					}
					else
					{
						perror("���� ����� ����");
						break;
					}
				}

				fclose(fp);

			}			
			break;
		}

		if (endflag)
		{
			break;
		}
	}
	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	system("pause");
	return 0;
}
