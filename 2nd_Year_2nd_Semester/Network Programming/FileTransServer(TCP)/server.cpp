#pragma comment(lib, "ws2_32.lib")//�Ӽ�->��Ŀ->�Է�->ws2_32.lib; �Է��ص���
#include <WinSock2.h>					 //��������ϻ���ϱ����� �������
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 4096			//���� ������	
#define FILENAMESIZE 256	//�����̸� ������
#define MAXCONCLIENT 100	//Ŭ�� �ִ����Ӽ�

//������ �޼���
#define INTRO_MSG "������ ���ϸ��� �Է��ϼ���"
#define FILE_DUPLICATE_MSG "�����ϰ��� �ϴ� ������ �̹� ������ �����ϴ� �����Դϴ�."

enum PROTOCOL
{
	INTRO = 1,								//��Ʈ��
	FILE_INFO,							// Ŭ�� �������� ������ ��Ŷ ��������
	FILE_TRANS_DENY,				// ���� ���� �ź�
	FILE_TRANS_START_POINT,// ���� ���� ��ġ(ó������, �̾)
	FILE_TRANS_DATA,				// Ŭ�� ���� �о ������ ��������
};

enum STATE
{
	INITE_STATE=1,
	FILE_TRANS_READY_STATE,		// �������� �غ�ܰ� (�����޾ƿͼ� ���۹������ΰ� �ƴѰ� �Ǵ�)
	FILE_TRANS_STATE,					// Ŭ�� ���� ���� �޾ƿ�
	FILE_TRANS_COMPLETE_STATE,	// ���� ���� �Ϸ�
	DISCONNECTED_STATE				// ���� ����
};

enum DENY_CODE   // ���� ���� �źλ���
{
	FILEEXIST = -1   // ���� ����
};

struct _File_info
{
	char filename[FILENAMESIZE];	//�����̸�
	int  filesize;									//���� �� ũ��
	int  nowsize;								// �������� ���� �뷮
};

//ũ��Ƽ�ü���
CRITICAL_SECTION cs;

struct _ClientInfo
{
	SOCKET sock;							//��������
	SOCKADDR_IN addr;					//�ּ�����
	STATE	state;							//������ġ
	_File_info  file_info;					//��������
	char recv_buf[BUFSIZE];			//recv(�ٿ�ε�)�� ����
	char send_buf[BUFSIZE];			//send(���ε�)�� ����
	HANDLE wait_event;				//��ٸ� �̺�Ʈ
};

//Ŭ���̾�Ʈ ���� ����ü �迭 ����
_ClientInfo* ClientInfo[MAXCONCLIENT];
int count;//Ŭ���̾�Ʈ �����ο�

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
_ClientInfo* AddClientInfo(SOCKET sock, SOCKADDR_IN addr)
{
	EnterCriticalSection(&cs);																//�Ӱ迵�� �Ҵ�

	_ClientInfo* ptr = new _ClientInfo;													//�޸� �Ҵ�
	ZeroMemory(ptr, sizeof(_ClientInfo));												//�޸� �ʱ�ȭ
	ptr->sock = sock;																				//�޾ƿ� �������� ����
	memcpy(&ptr->addr, &addr, sizeof(addr));										//Ŭ�� �ּ� ����
	ptr->state = INITE_STATE;																//state(����) ����
	ClientInfo[count++] = ptr;																	//Ŭ���̾�Ʈ���� ����Ʈ�迭�� ���� (ī��Ʈ����)


	LeaveCriticalSection(&cs);																//�Ӱ迵�� ����

	printf("\nFileSender ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",					//���ӵ� Ŭ���̾�Ʈ ���� ���
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

	return ptr;
}

_ClientInfo* SearchNextClient(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);
	_ClientInfo* ptr = nullptr;
	if (count == 1)
	{
		LeaveCriticalSection(&cs);
		return nullptr;
	}

	for (int i = 0; i < count; i++)
	{
		if (ClientInfo[i] == _ptr)
		{
			if (ClientInfo[i + 1] == NULL)
			{
				LeaveCriticalSection(&cs);
				return nullptr;
			}
			else
			{
				ptr = ClientInfo[i + 1];
			}

		}
	}

	if (ptr == _ptr)
	{
		ptr = nullptr;
	}

	LeaveCriticalSection(&cs);
	return ptr;
}

void ReMoveClientInfo(_ClientInfo* ptr)
{

	printf("FileSender ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",						//���������ϴ� Ŭ���̾�Ʈ ���� ���
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

	EnterCriticalSection(&cs);																//�Ӱ迵�� �Ҵ�

	_ClientInfo* next_ptr = SearchNextClient(ptr);
	if (next_ptr != nullptr)
	{
		SetEvent(next_ptr->wait_event);
	}

	for (int i = 0; i < count; i++)
	{
		if (ClientInfo[i] == ptr)
		{
			closesocket(ptr->sock);															//������������
			delete ptr;
			for (int j = i; j < count - 1; j++)													//�����
			{
				ClientInfo[j] = ClientInfo[j + 1];
			}
			break;
		}
	}

	count--;																							//Ŭ���̾� �����ο��� ����
	LeaveCriticalSection(&cs);																//�Ӱ迵�� ����
}

bool SearchFile(const char* filename)
{
	EnterCriticalSection(&cs);																//�Ӱ迵�� �Ҵ�
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = FindFirstFile(filename, &FindFileData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return false;
	else {
		FindClose(hFindFile);
		return true;
	}
	LeaveCriticalSection(&cs);																//�Ӱ迵�� ����
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

int PackPacket(char* _buf, PROTOCOL  _protocol, const char* _str) //��Ʈ�� 
{
	char* ptr = _buf;
	int strsize = strlen(_str);
	int size = 0;

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

int PackPacket(char* _buf, PROTOCOL  _protocol, int _data, const char* _str) //�������� �ź� ����
{
	char* ptr = _buf;
	int strsize = strlen(_str);
	int size = 0;

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_data, sizeof(_data));
	ptr = ptr + sizeof(_data);
	size = size + sizeof(_data);

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


int PackPacket(char* _buf, PROTOCOL _protocol, int _data)//���� ���� ��ġ ����
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

//buf���� �������� ������ �޾ƿ��� �Լ�
PROTOCOL GetProtocol(char* _buf)
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));

	return protocol;
}

void UnPackPacket(const char* _buf, char* _str1, int& _data1)//�����̸� & ���� ũ��
{
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize;

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str1, ptr, strsize);
	ptr = ptr + strsize;

	memcpy(&_data1, ptr, sizeof(_data1));
	ptr = ptr + sizeof(_data1);
}

void UnPackPacket(const char* _buf, int& _size, char* _targetbuf) //���� ���� ������
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_size, ptr, sizeof(_size));
	ptr = ptr + sizeof(_size);

	memcpy(_targetbuf, ptr, _size);
}

//�����ϸ� ����
void InitProcess(_ClientInfo* _ptr)
{	
	EnterCriticalSection(&cs);
	//�����ϸ� Ŭ���̾�Ʈ���� ��Ʈ�� �޼����� ����
	int size = PackPacket(_ptr->send_buf, INTRO, INTRO_MSG);

	if (count == 1)
		SetEvent(_ptr->wait_event);

	int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		_ptr->state = DISCONNECTED_STATE;
		LeaveCriticalSection(&cs);
		return;
	}

	_ptr->state = FILE_TRANS_READY_STATE;
	LeaveCriticalSection(&cs);
}

//���ӿϷ�ó���� ����
void ReadyProcess(_ClientInfo* _ptr)
{
	char filename[FILENAMESIZE];
	int filesize;
	int retval;

	if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
	{
		_ptr->state = DISCONNECTED_STATE;
		LeaveCriticalSection(&cs);
		return;
	}

	PROTOCOL protocol = GetProtocol(_ptr->recv_buf);

	switch (protocol)
	{
	case FILE_INFO:
		memset(filename, 0, sizeof(filename));

		UnPackPacket(_ptr->recv_buf, filename, filesize);

		printf("-> ���� ���� �̸�: %s\n", filename);
		printf("-> ���� ���� ũ��: %d\n", filesize);

		if (SearchFile(filename))
		{
			//�ٸ� ������ ��ٸ�
			WaitForSingleObject(_ptr->wait_event, INFINITE);
		
			FILE* fp = fopen(filename, "rb");
			fseek(fp, 0, SEEK_END);
			int fsize = ftell(fp);
			fclose(fp);
			if (filesize == fsize)
			{
				printf("�����ϴ� ���� ���� �䱸\n");

				int size = PackPacket(_ptr->send_buf, FILE_TRANS_DENY, FILEEXIST, FILE_DUPLICATE_MSG);

				retval = send(_ptr->sock, _ptr->send_buf, size, 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send()");
					_ptr->state = DISCONNECTED_STATE;
					LeaveCriticalSection(&cs);
					return;
				}

				if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
				{
					_ptr->state = DISCONNECTED_STATE;
					LeaveCriticalSection(&cs);
					return;
				}
			}
			else
			{
				strcpy(_ptr->file_info.filename, filename);
				_ptr->file_info.filesize = filesize;
				_ptr->file_info.nowsize = fsize;
			}

		}
		else
		{
			strcpy(_ptr->file_info.filename, filename);
			_ptr->file_info.filesize = filesize;
			_ptr->file_info.nowsize = 0;
		}

		int size = PackPacket(_ptr->send_buf, FILE_TRANS_START_POINT, _ptr->file_info.nowsize);

		retval = send(_ptr->sock, _ptr->send_buf, size, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			_ptr->state = DISCONNECTED_STATE;
			LeaveCriticalSection(&cs);
			return;
		}
		_ptr->state = FILE_TRANS_STATE;
		break;
	}

}

//���� �б�
void FileTransProcess(_ClientInfo* _ptr)
{
	static FILE* fp = nullptr;
	char buf[BUFSIZE];

	if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
	{
		_ptr->state = FILE_TRANS_COMPLETE_STATE;
		fclose(fp);
		fp = nullptr;
		LeaveCriticalSection(&cs);
		return;
	}

	PROTOCOL protocol = GetProtocol(_ptr->recv_buf);

	switch (protocol)
	{
	case FILE_TRANS_DATA:
		if (fp == nullptr)
		{
			fp = fopen(_ptr->file_info.filename, "ab");
		}

		int transsize;
		UnPackPacket(_ptr->recv_buf, transsize, buf);
		fwrite(buf, 1, transsize, fp);
		if (ferror(fp))
		{
			perror("���� ����� ����");
			_ptr->state = FILE_TRANS_COMPLETE_STATE;
			fclose(fp);
			LeaveCriticalSection(&cs);
			return;
		}
		_ptr->file_info.nowsize += transsize;
		break;
	}
}

//�����б� ����
void FileTransCompleteProcess(_ClientInfo* _ptr)
{
	if (_ptr->file_info.filesize != 0 && _ptr->file_info.filesize == _ptr->file_info.nowsize)
	{

		printf("���ۿϷ�!!\n");
	}
	else
	{
		printf("���۽���!!\n");
	}
	_ClientInfo* next_ptr = SearchNextClient(_ptr);
	if (next_ptr != nullptr)
	{
		SetEvent(next_ptr->wait_event);
	}
	_ptr->state = DISCONNECTED_STATE;
}

//Ŭ���̾�Ʈ���� �ٸ��� ����
DWORD CALLBACK ClientThread(LPVOID _ptr)
{
	_ClientInfo* ptr = (_ClientInfo*)_ptr;

	bool endflag = false;

	while (1)
	{

		switch (ptr->state)
		{
		case INITE_STATE:
			InitProcess(ptr);
			break;
		case FILE_TRANS_READY_STATE:
			ReadyProcess(ptr);
			break;
		case FILE_TRANS_STATE:
			FileTransProcess(ptr);
			break;
		case FILE_TRANS_COMPLETE_STATE:
			FileTransCompleteProcess(ptr);
			break;
		case DISCONNECTED_STATE:
			ReMoveClientInfo(ptr);
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

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
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
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	
	
	char buf[BUFSIZE];
	FILE *fp=nullptr;

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			continue;
		}	
		//Ŭ���̾�Ʈ ����
		_ClientInfo* ClientPtr = AddClientInfo(client_sock, clientaddr);		
		//��Ƽ������ ����
		HANDLE hThread = CreateThread(nullptr, 0, ClientThread, ClientPtr, 0, nullptr);
		if (hThread == nullptr)
		{
			ReMoveClientInfo(ClientPtr);
			continue;
		}

		CloseHandle(hThread);
			
	}

	 // closesocket()
	closesocket(listen_sock);
	DeleteCriticalSection(&cs);
	// ���� ����
	WSACleanup();
	return 0;
}
