#pragma comment(lib, "ws2_32.lib")//�Ӽ�->��Ŀ->�Է�->ws2_32.lib; �Է��ص���
#include <WinSock2.h>					 //��������ϻ���ϱ����� �������
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"

#define SERVERIP "127.0.0.1"		//������ �ּ�
#define SERVERPORT 9000		//������Ʈ
#define BUFSIZE 4096				//���ۻ�����
#define NICKNAMESIZE 255		//�г��� ������


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

enum STATE							//�������
{
	INITE_STATE,						//����
	CHATT_INITE_STATE,			//�г��Ӽ���
	GAMEING_STATE,					//��������
	GAME_END_STATE				//����
};

struct _MyInfo							//���� ����
{
	SOCKET sock;						//��������	
	STATE state;						//��������
	char sendbuf[BUFSIZE];		//send�� ����
	char recvbuf[BUFSIZE];		//recv�� ����
}*MyInfo;

bool PacketRecv(SOCKET, char*);//recvn�� �ι� ó��

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...);
// ���� ��� �Լ�
void err_quit(const char*);														//���� ��� �� ����
void err_display(const char*);													//���� ���
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET , char*, int, int);											//recv�ݺ��Լ�
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID);
DWORD CALLBACK RecvThread(LPVOID);

/* ���޹��� �����Ϳ��� protocol �и� */
PROTOCOL GetProtocol(const char*);

/* ������ ������ Packing ó�� */
int PackPacket(char*, PROTOCOL, const char*);
int PackPacket(char*, PROTOCOL);

/* ���޹��� ������ UnPacking */
void UnPackPacket(const char*, char*, int&);
void UnPackPacket(const char*, char*);


char buf[BUFSIZE+1];							//������ �ۼ��� ����
HANDLE hReadEvent, hWriteEvent;	//�̺�Ʈ
HWND hSendButton;							//������ ��ư
HWND hEdit1, hEdit2;							//���� ��Ʈ��
HANDLE hThread[2];							//������

//����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return 1;
		
	// ���� ��� ������ ����	
	// ��ȭ���� ����

	MyInfo = new _MyInfo;
	memset(MyInfo, 0, sizeof(_MyInfo));

	//������
	hThread[0] = CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	//��ȭ����
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	WaitForMultipleObjects(2, hThread, true, INFINITE);

	// �̺�Ʈ ����
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	// closesocket()
	closesocket(MyInfo->sock);

	delete MyInfo;
	// ���� ����
	WSACleanup();
	return 0;
}

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		hSendButton = GetDlgItem(hDlg, IDOK);		
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		hThread[1] = CreateThread(NULL, 0, RecvThread, NULL, 0, NULL);
		MyInfo->state = INITE_STATE; 
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:																	// �������ư
			EnableWindow(hSendButton, FALSE);						// ������ ��ư ��Ȱ��ȭ
			WaitForSingleObject(hReadEvent, INFINITE);			// �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE+1);
			SetEvent(hWriteEvent);											// ���� �Ϸ� �˸���
			SetWindowText(hEdit1, "");
			SetFocus(hEdit1);			
			return TRUE;
		case IDCANCEL:															// ������ ��ư
			WaitForSingleObject(hReadEvent, INFINITE);
			MyInfo->state = GAME_END_STATE;
			SetEvent(hWriteEvent);		
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// ���� ��Ʈ�� ��� �Լ� ( �������� �Լ� )
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE+256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
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

// TCP Ŭ���̾�Ʈ ���� �κ�
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	MyInfo->sock = socket(AF_INET, SOCK_STREAM, 0);
	if(MyInfo->sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(MyInfo->sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ������ ���
	int size;
	bool endflag = false;

	while(1)
	{		
		WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���

		// ���ڿ� ���̰� 0�̸� ������ ����
		if(MyInfo->state!= GAME_END_STATE && strlen(buf) == 0)
		{
			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent); // �б� �Ϸ� �˸���
			continue;
		}

		switch (MyInfo->state)//������¿� ���� ����
		{			
		case CHATT_INITE_STATE:			
			size=PackPacket(MyInfo->sendbuf, CHATT_NICKNAME, buf);
			if (send(MyInfo->sock, MyInfo->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("nickname Send()");
			}
			
			break;
		case GAMEING_STATE:			
			size = PackPacket(MyInfo->sendbuf, CHATT_MSG, buf);
			if (send(MyInfo->sock, MyInfo->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("nickname Send()");
			}

			break;

		case GAME_END_STATE:			
			size = PackPacket(MyInfo->sendbuf, CHATT_OUT, buf);
			if (send(MyInfo->sock, MyInfo->sendbuf, size, 0) == SOCKET_ERROR)
			{
				err_display("chatting out Send()");
			}
			endflag = true;
			break;

		}
		
		EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
		SetEvent(hReadEvent); // �б� �Ϸ� �˸���

		if (endflag)
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

//recv������
DWORD CALLBACK RecvThread(LPVOID _ptr)
{
	PROTOCOL protocol;
	int size;	
	char nickname[NICKNAMESIZE];
	char msg[BUFSIZE];
	int count;	
	bool flag = true;
	while (flag)
	{
		if (!PacketRecv(MyInfo->sock, MyInfo->recvbuf))	//�����͹���
		{
			err_display("recv error()");
			return -1;
		}

		protocol=GetProtocol(MyInfo->recvbuf);				//�������� �и�
		
		switch (protocol)
		{
		case INTRO:
		case WAIT:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(MyInfo->recvbuf, msg);
			MyInfo->state = CHATT_INITE_STATE;
			break;

		case NICKNAME_EROR:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(MyInfo->recvbuf, msg);
			MyInfo->state = CHATT_INITE_STATE;
			break;
		case NICKNAME_LIST:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(MyInfo->recvbuf, msg, count);
			MyInfo->state = GAMEING_STATE;
			break;
		case CHATT_MSG:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(MyInfo->recvbuf, msg);
			break;
		case CHATT_OUT:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(MyInfo->recvbuf, msg);
			flag = false;
			break;
		}

		DisplayText("%s\r\n", msg);									//�޾ƿ� msg ���

	}

	return 0;
}

//�������� ���
PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}


/* Packing */
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1) //����, ��������, msg
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
int PackPacket(char* _buf, PROTOCOL _protocol)//���� ��������
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

/* UnPacking */
void UnPackPacket(const char* _buf, char* _str) //����, msg
{
	int strsize;
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
	ptr = ptr + strsize;
}
void UnPackPacket(const char* _buf, char* _str, int& _count) //����, namelist, listnum
{	
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_count, ptr, sizeof(_count));
	ptr = ptr + sizeof(_count);

	for (int i = 0; i < _count; i++)
	{
		int strsize;
		memcpy(&strsize, ptr, sizeof(strsize));
		ptr = ptr + sizeof(strsize);

		memcpy(_str, ptr, strsize);
		ptr = ptr + strsize;
		_str = _str + strsize;
		strcat(_str, ",");
		_str++;
	}

}