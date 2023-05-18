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

//진행상황
enum PROTOCOL
{
	INTRO = 1,
	GAME_VALUE,
	GAME_RESULT
};

//게임결과
enum RESULT
{
	NODATA = -1,
	WIN = 1,
	LOSE,
	NO_WIN
};

//게임 진행상황
enum _GameStatus 
{ 
	NOSTATUS, 
	RUN, 
	HINT, 
	VIEW 
} GameStatus;

//자신정보
struct _Mystate
{
	SOCKET tcp_sock;
	char sendbuf[BUFSIZE];
	char recvbuf[BUFSIZE];
	bool isstart;
	int GameEnd;
}Mystate;

//카드상태
enum Status 
{ 
	HIDDEN,
	FLIP, 
	TEMPFLIP 
};


//사용변수
LPCTSTR lpszClass=TEXT("Couple3_Child"); //클라제목
HANDLE hReadEvent, hWriteEvent; // 이벤트
HANDLE hClientThread;	//통신쓰레드
HINSTANCE g_hInst;		//창정보
HBITMAP hShape[9];		//카드정보
HWND hWndMain;		//핸들
HWND hChild[16];			//카드핸들
 char buf[BUFSIZE];		//임시버퍼
bool start;					//게임시작여부
int count;						//진행횟수


//사용함수
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ClientMain(LPVOID arg);
void InitGame();
HWND GetTempFlip();
int GetRemain();
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit);

//윈도우 설정
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	//이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hReadEvent == NULL) return -1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return -1;

	//창설정
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

	// 차일드 윈도우 클래스. 여분 8바이트, 0=숫자, 4=상태
	WndClass.cbWndExtra=8;
	WndClass.hbrBackground=NULL;
	WndClass.lpfnWndProc=ChildProc;
	WndClass.lpszClassName="CoupleChild";
	RegisterClass(&WndClass);

	//쓰레드 생성
	memset(&Mystate, 0, sizeof(Mystate));
	DWORD ThreadId;
	hClientThread = CreateThread(NULL, 0, ClientMain, NULL, 0, &ThreadId);

	//윈도우 생성
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

//메세지 처리함수
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int i;
	RECT crt;
	TCHAR Mes[128];

	switch (iMessage) 
	{
		//윈도우 생성되었을때 실행
	case WM_CREATE:
		srand(time(NULL)); //랜덤값 초기화
		SetRect(&crt,0,0,64*4+250,64*4);	//크기조정
		AdjustWindowRect(&crt,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | 
			WS_MINIMIZEBOX | WS_BORDER | WS_CLIPCHILDREN,FALSE); //창크기 세팅
		SetWindowPos(hWnd,NULL,0,0,crt.right-crt.left,crt.bottom-crt.top,
			SWP_NOMOVE | SWP_NOZORDER); //창위치설정
		hWndMain=hWnd; //핸들저장
		return 0;
	case WM_TIMER://타이머
		switch (wParam) {
		case 0:
			KillTimer(hWnd,0);
			GameStatus=RUN;
			for (i=0;i<16;i++) {
				//창 새로고침
				InvalidateRect(hChild[i],NULL,FALSE);
			}
			break;
		case 1:
			KillTimer(hWnd,1);
			GameStatus=RUN;
			for (i=0;i<16;i++) {
				//변경후 새로고침
				if (GetWindowLong(hChild[i],EB_STATUS) == TEMPFLIP) {
					SetWindowLong(hChild[i],EB_STATUS,HIDDEN);
					InvalidateRect(hChild[i],NULL,FALSE);
				}
			}
			break;
		}
		return 0;
	case WM_PAINT://그리기
		//그리기시작
		hdc=BeginPaint(hWnd, &ps);

		//게임이 시작했고 자기정보에 시작이 안되었을경우
		if (start && !Mystate.isstart)
		{
			//게임시작
			Mystate.isstart = true;
			//기본설정(카드)
			for (i = 0; i < sizeof(hShape) / sizeof(hShape[0]); i++) {
				hShape[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_SHAPE1 + i));
			}
			// 차일드 생성
			for (i = 0; i < 16; i++) {
				hChild[i] = CreateWindow("CoupleChild", NULL, WS_CHILD | WS_VISIBLE,
					(i / 4) * 64, (i % 4) * 64, 64, 64, hWnd, (HMENU)i, g_hInst, NULL);
			}
			//게임시작
			InitGame();
		}

		//게임시작시 띄워줄 text
		if (Mystate.isstart)
		{
			lstrcpy(Mes, "짝 맞추기 게임 Ver 1.2");
			TextOut(hdc, 300, 10, Mes, lstrlen(Mes));
			wsprintf(Mes, "총 시도 회수 : %d", count);
			TextOut(hdc, 300, 50, Mes, lstrlen(Mes));
			wsprintf(Mes, "아직 못 찾은 것 : %d   ", GetRemain());
			TextOut(hdc, 300, 70, Mes, lstrlen(Mes));
		}
		else//게임시작전 대기text
		{
			wsprintf(Mes, "다른플레이어가 입장하면");
			TextOut(hdc, 300, 90, Mes, lstrlen(Mes));
			wsprintf(Mes, "게임을 시작합니다");
			TextOut(hdc, 300, 110, Mes, lstrlen(Mes));
		}
		//그리기종료
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY://종료
		for (i=0;i<sizeof(hShape)/sizeof(hShape[0]);i++) {
			DeleteObject(hShape[i]);
		}
		PostQuitMessage(0);
		return 0;
	}
	
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));

}

//카드 프로세스
LRESULT CALLBACK ChildProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HWND hBrother;
	int image;

	switch (iMessage) {
	case WM_CREATE:
		//카드뒤집어서생성
		SetWindowLong(hWnd,EB_NUM,0);
		SetWindowLong(hWnd,EB_STATUS,(LONG)HIDDEN);
		return 0;
	case WM_LBUTTONDOWN:
		//마우스클릭시
		if (GameStatus != RUN || GetWindowLong(hWnd,EB_STATUS) != HIDDEN) {
			return 0;
		}

		// 일단 클릭하면 조건에 상관없이 다시 그려야 함. 부모도 마찬가지
		InvalidateRect(hWnd,NULL,FALSE);
		InvalidateRect(hWndMain,NULL,FALSE);

		hBrother=GetTempFlip();
		// 열려진 형제가 없으면 자신만 연다.
		if (hBrother == NULL) {
			SetWindowLong(hWnd,EB_STATUS,TEMPFLIP);
		} else {
			count++;
			// 맞으면 둘 다 오픈
			if (GetWindowLong(hWnd,EB_NUM) == GetWindowLong(hBrother,EB_NUM)) {
				SetWindowLong(hWnd,EB_STATUS,FLIP);
				SetWindowLong(hBrother,EB_STATUS,FLIP);
				//남은갯수가 0개면
				if (GetRemain() == 0) 
				{
					//게임종료
					Mystate.GameEnd = 1;
					//이벤트활성화
					SetEvent(hWriteEvent);
				}
			// 틀리면 둘 다 잠시 보여 주고 다시 숨김
			} else {
				SetWindowLong(hWnd,EB_STATUS,TEMPFLIP);
				GameStatus=VIEW;
				SetTimer(hWndMain,1,1000,NULL);
			}
		}
		return 0;
	case WM_PAINT:
		//카드그리기
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

//게임시작
void InitGame()
{
	
	int i,j;
	int c;

	count=0;
	// 차일드 초기화
	for (i=0;i<16;i++) {
		SetWindowLong(hChild[i],EB_STATUS,HIDDEN);
		SetWindowLong(hChild[i],EB_NUM,0);
	}

	// 차일드에 숫자를 난수로 배치한다.
	for (i=1;i<=8;i++) {
		for (j=0;j<2;j++) {
			do {
				c=random(16);
			} while (GetWindowLong(hChild[c],EB_NUM) != 0);
			SetWindowLong(hChild[c],EB_NUM,i);
		}
	}

	// 2초간 힌트를 보여준다.
	GameStatus=HINT;
	for (i=0;i<16;i++) {
		InvalidateRect(hChild[i],NULL,FALSE);
	}

	InvalidateRect(hWndMain,NULL,TRUE);
	SetTimer(hWndMain,0,2000,NULL);
}

//열기위해 카드 정보받아오는 함수
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

//찾지못한카드 갯수 찾는 함수
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

//카드 그리는 함수
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

// 소켓 함수 오류 출력 후 종료
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
// 소켓 함수 오류 출력
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
// 사용자 정의 데이터 수신 함수
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
//recvn 2회 데이터사이즈-> 데이터받기
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
//프로토콜 빼오기
PROTOCOL GetProtocol(char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}
//프로토콜정보+데이터 보내는용도
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
//packing된 정보 해체
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

//결과 쓰레드
DWORD WINAPI ClientResult(LPVOID arg)
{
	char msgbuf[BUFSIZE + 1];
	bool endflag = false;

	while (1)
	{
		//게임 결과가 오기전까지 대기
		if (!PacketRecv(Mystate.tcp_sock, Mystate.recvbuf))
		{
			break;
		}

		PROTOCOL protocol = GetProtocol(Mystate.recvbuf);

		RESULT result = NODATA;
		//결과가 결정나면 게임종료
		switch (protocol)
		{
		case GAME_RESULT:
			ZeroMemory(msgbuf, sizeof(msgbuf));
			UnPackPacket(Mystate.recvbuf, result, msgbuf);
			if (MessageBox(hWndMain, msgbuf, TEXT("게임이 끝났습니다."), MB_OK))
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


//게임시작과 게임끝남을 알리는 용도의 쓰레드
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	char temp_buf[BUFSIZE];

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;


	// 서버 통신용 TCP socket()
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
		//recv가 될때까지 대기 (게임시작대기)
		if (!PacketRecv(Mystate.tcp_sock, Mystate.recvbuf))
		{

			break;
		}
		PROTOCOL protocol = GetProtocol(Mystate.recvbuf);
		char msg[BUFSIZE];
		RESULT result = NODATA;

		//게임시작시 인트로 진입후 쓰레드 생성과 함께 화면 새로고침
		switch (protocol)
		{
		case INTRO:
			start = true;
			InvalidateRect(NULL, NULL, TRUE);
			DWORD ThreadId;
			hClientresult = CreateThread(NULL, 0, ClientResult, NULL, 0, &ThreadId);
			break;
		}

		//게임결과를 넘겨주는 부분
		switch (result)
		{
		case NODATA:
		{
			//게임이 끝나면 hWriteEvent가 활성화되서 진행
			WaitForSingleObject(hWriteEvent, INFINITE);

			//게임결과 서버에 전달
			int size = PackPacket(Mystate.sendbuf, GAME_VALUE, Mystate.GameEnd);
			retval = send(Mystate.tcp_sock, Mystate.sendbuf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("value send()");
			}
			//게임이 끝나기까지 대기
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

