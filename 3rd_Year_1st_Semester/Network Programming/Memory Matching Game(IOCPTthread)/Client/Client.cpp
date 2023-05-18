#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include "resource.h"

#define random(n) (rand()%n)
#define BUFSIZE 4096
#define EB_NUM 0
#define EB_STATUS 4

//�����Ȳ
enum PROTOCOL
{
	INTRO = 1,
	GAME_VALUE,
	GAME_RESULT
};

//���Ӱ��
enum RESULT
{
	NODATA = -1,
	WIN = 1,
	LOSE,
	NO_WIN
};

//���� �����Ȳ
enum _GameStatus 
{ 
	NOSTATUS, 
	RUN, 
	HINT, 
	VIEW 
} GameStatus;

//�ڽ�����
struct _Mystate
{
	SOCKET tcp_sock;
	char sendbuf[BUFSIZE];
	char recvbuf[BUFSIZE];
	bool isstart;
	int GameEnd;
}Mystate;

//ī�����
enum Status 
{ 
	HIDDEN,
	FLIP, 
	TEMPFLIP 
};


//��뺯��
LPCTSTR lpszClass=TEXT("Couple3_Child"); //Ŭ������
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ
HANDLE hClientThread;	//��ž�����
HINSTANCE g_hInst;		//â����
HBITMAP hShape[9];		//ī������
HWND hWndMain;		//�ڵ�
HWND hChild[16];			//ī���ڵ�
 char buf[BUFSIZE];		//�ӽù���
bool start;					//���ӽ��ۿ���
int count;						//����Ƚ��


//����Լ�
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ClientMain(LPVOID arg);
void InitGame();
HWND GetTempFlip();
int GetRemain();
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit);

//������ ����
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	//�̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hReadEvent == NULL) return -1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return -1;

	//â����
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_COUPLE));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=0;
	RegisterClass(&WndClass);

	// ���ϵ� ������ Ŭ����. ���� 8����Ʈ, 0=����, 4=����
	WndClass.cbWndExtra=8;
	WndClass.hbrBackground=NULL;
	WndClass.lpfnWndProc=ChildProc;
	WndClass.lpszClassName="CoupleChild";
	RegisterClass(&WndClass);

	//������ ����
	memset(&Mystate, 0, sizeof(Mystate));
	DWORD ThreadId;
	hClientThread = CreateThread(NULL, 0, ClientMain, NULL, 0, &ThreadId);

	//������ ����
	hWnd=CreateWindow(lpszClass,lpszClass,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | 
		WS_BORDER | WS_CLIPCHILDREN,
		CW_USEDEFAULT,CW_USEDEFAULT,0,0,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
	
	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

//�޼��� ó���Լ�
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int i;
	RECT crt;
	TCHAR Mes[128];

	switch (iMessage) 
	{
		//������ �����Ǿ����� ����
	case WM_CREATE:
		srand(time(NULL)); //������ �ʱ�ȭ
		SetRect(&crt,0,0,64*4+250,64*4);	//ũ������
		AdjustWindowRect(&crt,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | 
			WS_MINIMIZEBOX | WS_BORDER | WS_CLIPCHILDREN,FALSE); //âũ�� ����
		SetWindowPos(hWnd,NULL,0,0,crt.right-crt.left,crt.bottom-crt.top,
			SWP_NOMOVE | SWP_NOZORDER); //â��ġ����
		hWndMain=hWnd; //�ڵ�����
		return 0;
	case WM_TIMER://Ÿ�̸�
		switch (wParam) {
		case 0:
			KillTimer(hWnd,0);
			GameStatus=RUN;
			for (i=0;i<16;i++) {
				//â ���ΰ�ħ
				InvalidateRect(hChild[i],NULL,FALSE);
			}
			break;
		case 1:
			KillTimer(hWnd,1);
			GameStatus=RUN;
			for (i=0;i<16;i++) {
				//������ ���ΰ�ħ
				if (GetWindowLong(hChild[i],EB_STATUS) == TEMPFLIP) {
					SetWindowLong(hChild[i],EB_STATUS,HIDDEN);
					InvalidateRect(hChild[i],NULL,FALSE);
				}
			}
			break;
		}
		return 0;
	case WM_PAINT://�׸���
		//�׸������
		hdc=BeginPaint(hWnd, &ps);

		//������ �����߰� �ڱ������� ������ �ȵǾ������
		if (start && !Mystate.isstart)
		{
			//���ӽ���
			Mystate.isstart = true;
			//�⺻����(ī��)
			for (i = 0; i < sizeof(hShape) / sizeof(hShape[0]); i++) {
				hShape[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_SHAPE1 + i));
			}
			// ���ϵ� ����
			for (i = 0; i < 16; i++) {
				hChild[i] = CreateWindow("CoupleChild", NULL, WS_CHILD | WS_VISIBLE,
					(i / 4) * 64, (i % 4) * 64, 64, 64, hWnd, (HMENU)i, g_hInst, NULL);
			}
			//���ӽ���
			InitGame();
		}

		//���ӽ��۽� ����� text
		if (Mystate.isstart)
		{
			lstrcpy(Mes, "¦ ���߱� ���� Ver 1.2");
			TextOut(hdc, 300, 10, Mes, lstrlen(Mes));
			wsprintf(Mes, "�� �õ� ȸ�� : %d", count);
			TextOut(hdc, 300, 50, Mes, lstrlen(Mes));
			wsprintf(Mes, "���� �� ã�� �� : %d   ", GetRemain());
			TextOut(hdc, 300, 70, Mes, lstrlen(Mes));
		}
		else//���ӽ����� ���text
		{
			wsprintf(Mes, "�ٸ��÷��̾ �����ϸ�");
			TextOut(hdc, 300, 90, Mes, lstrlen(Mes));
			wsprintf(Mes, "������ �����մϴ�");
			TextOut(hdc, 300, 110, Mes, lstrlen(Mes));
		}
		//�׸�������
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY://����
		for (i=0;i<sizeof(hShape)/sizeof(hShape[0]);i++) {
			DeleteObject(hShape[i]);
		}
		PostQuitMessage(0);
		return 0;
	}
	
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));

}

//ī�� ���μ���
LRESULT CALLBACK ChildProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HWND hBrother;
	int image;

	switch (iMessage) {
	case WM_CREATE:
		//ī����������
		SetWindowLong(hWnd,EB_NUM,0);
		SetWindowLong(hWnd,EB_STATUS,(LONG)HIDDEN);
		return 0;
	case WM_LBUTTONDOWN:
		//���콺Ŭ����
		if (GameStatus != RUN || GetWindowLong(hWnd,EB_STATUS) != HIDDEN) {
			return 0;
		}

		// �ϴ� Ŭ���ϸ� ���ǿ� ������� �ٽ� �׷��� ��. �θ� ��������
		InvalidateRect(hWnd,NULL,FALSE);
		InvalidateRect(hWndMain,NULL,FALSE);

		hBrother=GetTempFlip();
		// ������ ������ ������ �ڽŸ� ����.
		if (hBrother == NULL) {
			SetWindowLong(hWnd,EB_STATUS,TEMPFLIP);
		} else {
			count++;
			// ������ �� �� ����
			if (GetWindowLong(hWnd,EB_NUM) == GetWindowLong(hBrother,EB_NUM)) {
				SetWindowLong(hWnd,EB_STATUS,FLIP);
				SetWindowLong(hBrother,EB_STATUS,FLIP);
				//���������� 0����
				if (GetRemain() == 0) 
				{
					//��������
					Mystate.GameEnd = 1;
					//�̺�ƮȰ��ȭ
					SetEvent(hWriteEvent);
				}
			// Ʋ���� �� �� ��� ���� �ְ� �ٽ� ����
			} else {
				SetWindowLong(hWnd,EB_STATUS,TEMPFLIP);
				GameStatus=VIEW;
				SetTimer(hWndMain,1,1000,NULL);
			}
		}
		return 0;
	case WM_PAINT:
		//ī��׸���
		hdc=BeginPaint(hWnd, &ps);
		if (GameStatus==HINT || GetWindowLong(hWnd,EB_STATUS)!=HIDDEN) {
			image=GetWindowLong(hWnd,EB_NUM)-1;
		} else {
			image=8;
		}
		DrawBitmap(hdc,0,0,hShape[image]);
		EndPaint(hWnd, &ps);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//���ӽ���
void InitGame()
{
	
	int i,j;
	int c;

	count=0;
	// ���ϵ� �ʱ�ȭ
	for (i=0;i<16;i++) {
		SetWindowLong(hChild[i],EB_STATUS,HIDDEN);
		SetWindowLong(hChild[i],EB_NUM,0);
	}

	// ���ϵ忡 ���ڸ� ������ ��ġ�Ѵ�.
	for (i=1;i<=8;i++) {
		for (j=0;j<2;j++) {
			do {
				c=random(16);
			} while (GetWindowLong(hChild[c],EB_NUM) != 0);
			SetWindowLong(hChild[c],EB_NUM,i);
		}
	}

	// 2�ʰ� ��Ʈ�� �����ش�.
	GameStatus=HINT;
	for (i=0;i<16;i++) {
		InvalidateRect(hChild[i],NULL,FALSE);
	}

	InvalidateRect(hWndMain,NULL,TRUE);
	SetTimer(hWndMain,0,2000,NULL);
}

//�������� ī�� �����޾ƿ��� �Լ�
HWND GetTempFlip()
{

	int i;
	for (i=0;i<16;i++) {
		if (GetWindowLong(hChild[i],EB_STATUS) == TEMPFLIP) {
			return hChild[i];
		}
	}
	return NULL;
}

//ã������ī�� ���� ã�� �Լ�
int GetRemain()
{
	int i;
	int remain=16;

	for (i=0;i<16;i++) {
		if (GetWindowLong(hChild[i],EB_STATUS) == FLIP) {
			remain--;
		}
	}
	return remain;
}

//ī�� �׸��� �Լ�
void DrawBitmap(HDC hdc,int x,int y,HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx,by;
	BITMAP bit;

	MemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;

	BitBlt(hdc,x,y,bx,by,MemDC,0,0,SRCCOPY);

	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}
// ���� �Լ� ���� ���
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
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
//recvn 2ȸ �����ͻ�����-> �����͹ޱ�
bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;

	int retval = recvn(_sock, (char*)&size, sizeof(size), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv error()");
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
//�������� ������
PROTOCOL GetProtocol(char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}
//������������+������ �����¿뵵
int PackPacket(char* _buf, PROTOCOL _protocol, int _data)
{
	int size = 0;

	char* ptr = _buf;

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
//packing�� ���� ��ü
void UnPackPacket(char* _buf, RESULT& _result, char* _str1)
{
	int strsize1;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_result, ptr, sizeof(_result));
	ptr = ptr + sizeof(_result);

	memcpy(&strsize1, ptr, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);

	memcpy(_str1, ptr, strsize1);
	ptr = ptr + strsize1;
}

//��� ������
DWORD WINAPI ClientResult(LPVOID arg)
{
	char msgbuf[BUFSIZE + 1];
	bool endflag = false;

	while (1)
	{
		//���� ����� ���������� ���
		if (!PacketRecv(Mystate.tcp_sock, Mystate.recvbuf))
		{
			break;
		}

		PROTOCOL protocol = GetProtocol(Mystate.recvbuf);

		RESULT result = NODATA;
		//����� �������� ��������
		switch (protocol)
		{
		case GAME_RESULT:
			ZeroMemory(msgbuf, sizeof(msgbuf));
			UnPackPacket(Mystate.recvbuf, result, msgbuf);
			if (MessageBox(hWndMain, msgbuf, TEXT("������ �������ϴ�."), MB_OK))
			{
				for (int i = 0; i < sizeof(hShape) / sizeof(hShape[0]); i++) {
					DeleteObject(hShape[i]);
				}
				PostQuitMessage(0);
				endflag = true;
				SendMessage(hWndMain, WM_CLOSE, 0, 0);

				return 0;
			}
		}

		if (endflag)
		{
			break;
		}

	}

	return 0;
}


//���ӽ��۰� ���ӳ����� �˸��� �뵵�� ������
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	char temp_buf[BUFSIZE];

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;


	// ���� ��ſ� TCP socket()
	Mystate.tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (Mystate.tcp_sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	retval = connect(Mystate.tcp_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect() error");

	HANDLE hClientresult = nullptr;

	int size;
	bool endflg = false;

	while (1)
	{
		//recv�� �ɶ����� ��� (���ӽ��۴��)
		if (!PacketRecv(Mystate.tcp_sock, Mystate.recvbuf))
		{

			break;
		}
		PROTOCOL protocol = GetProtocol(Mystate.recvbuf);
		char msg[BUFSIZE];
		RESULT result = NODATA;

		//���ӽ��۽� ��Ʈ�� ������ ������ ������ �Բ� ȭ�� ���ΰ�ħ
		switch (protocol)
		{
		case INTRO:
			start = true;
			InvalidateRect(NULL, NULL, TRUE);
			DWORD ThreadId;
			hClientresult = CreateThread(NULL, 0, ClientResult, NULL, 0, &ThreadId);
			break;
		}

		//���Ӱ���� �Ѱ��ִ� �κ�
		switch (result)
		{
		case NODATA:
		{
			//������ ������ hWriteEvent�� Ȱ��ȭ�Ǽ� ����
			WaitForSingleObject(hWriteEvent, INFINITE);

			//���Ӱ�� ������ ����
			int size = PackPacket(Mystate.sendbuf, GAME_VALUE, Mystate.GameEnd);
			retval = send(Mystate.tcp_sock, Mystate.sendbuf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("value send()");
			}
			//������ ��������� ���
			WaitForSingleObject(hReadEvent,INFINITE);
		}
		break;
		}
		

		if (endflg)
		{
			break;
		}
	}

	return 0;
}

