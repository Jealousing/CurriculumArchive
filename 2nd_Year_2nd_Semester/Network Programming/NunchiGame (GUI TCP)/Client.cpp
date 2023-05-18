#pragma comment(lib, "ws2_32.lib")//속성->링커->입력->ws2_32.lib; 입력해도됨
#include <WinSock2.h>					 //윈도우소켓사용하기위한 헤더파일
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"

#define SERVERIP "127.0.0.1"		//루프백 주소
#define SERVERPORT 9000		//서버포트
#define BUFSIZE 4096				//버퍼사이즈
#define NICKNAMESIZE 255		//닉네임 사이즈


enum PROTOCOL						//진행상황
{
	INTRO,									//인트로
	WAIT,									//대기
	CHATT_NICKNAME,				//닉네임입력
	NICKNAME_EROR, 				//닉네임에러
	NICKNAME_LIST,					//닉네임출력
	CHATT_MSG,							//체팅메세지
	CHATT_OUT,							//종료
};

enum STATE							//진행상태
{
	INITE_STATE,						//접속
	CHATT_INITE_STATE,			//닉네임설정
	GAMEING_STATE,					//게임진행
	GAME_END_STATE				//종료
};

struct _MyInfo							//나의 정보
{
	SOCKET sock;						//소켓정보	
	STATE state;						//진행정보
	char sendbuf[BUFSIZE];		//send용 버퍼
	char recvbuf[BUFSIZE];		//recv용 버퍼
}*MyInfo;

bool PacketRecv(SOCKET, char*);//recvn을 두번 처리

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
void err_quit(const char*);														//에러 출력 및 종료
void err_display(const char*);													//에러 출력
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET , char*, int, int);											//recv반복함수
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID);
DWORD CALLBACK RecvThread(LPVOID);

/* 전달받은 데이터에서 protocol 분리 */
PROTOCOL GetProtocol(const char*);

/* 전달할 데이터 Packing 처리 */
int PackPacket(char*, PROTOCOL, const char*);
int PackPacket(char*, PROTOCOL);

/* 전달받은 데이터 UnPacking */
void UnPackPacket(const char*, char*, int&);
void UnPackPacket(const char*, char*);


char buf[BUFSIZE+1];							//데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent;	//이벤트
HWND hSendButton;							//보내기 버튼
HWND hEdit1, hEdit2;							//편집 컨트롤
HANDLE hThread[2];							//쓰레드

//메인
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return 1;
		
	// 소켓 통신 스레드 생성	
	// 대화상자 생성

	MyInfo = new _MyInfo;
	memset(MyInfo, 0, sizeof(_MyInfo));

	//쓰레드
	hThread[0] = CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	//대화상자
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	WaitForMultipleObjects(2, hThread, true, INFINITE);

	// 이벤트 제거
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	// closesocket()
	closesocket(MyInfo->sock);

	delete MyInfo;
	// 윈속 종료
	WSACleanup();
	return 0;
}

// 대화상자 프로시저
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
		case IDOK:																	// 보내기버튼
			EnableWindow(hSendButton, FALSE);						// 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE);			// 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE+1);
			SetEvent(hWriteEvent);											// 쓰기 완료 알리기
			SetWindowText(hEdit1, "");
			SetFocus(hEdit1);			
			return TRUE;
		case IDCANCEL:															// 끝내기 버튼
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

// 편집 컨트롤 출력 함수 ( 가변인자 함수 )
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

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpmsgbuf;																		//void*
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |								//오류메세지 저장할 공간 할당
		FORMAT_MESSAGE_FROM_SYSTEM,										//운영체제에서 오류메세지 가져옴
		NULL, WSAGetLastError(),														//WSAGetLastError가 에러코드를 가져옴		
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),				//오류코드를 시스템 기본언어로 번역
		(LPSTR)&lpmsgbuf, 0, NULL);													//Ipmsgbuf에 저장
	MessageBox(NULL, (LPCSTR)lpmsgbuf, msg, MB_OK);					//Ipmsgbuf에 저장된 내용을 메세지박스로 출력
	LocalFree(lpmsgbuf);																	//Ipmsgbuf에 할당된 메모리 반환
	exit(-1);																							//종료
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpmsgbuf;																	//void*
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |							//오류메세지 저장할 공간 할당
		FORMAT_MESSAGE_FROM_SYSTEM,									//운영체제에서 오류메세지 가져옴
		NULL, WSAGetLastError(),													//WSAGetLastError가 에러코드를 가져옴
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),			//오류코드를 시스템 기본언어로 번역
		(LPSTR)&lpmsgbuf, 0, NULL);												//Ipmsgbuf에 저장
	printf("[%s] %s\n", msg, (LPSTR)lpmsgbuf);							//Ipmsgbuf에 저장된 내용을 Printf로 명령프롬프트창에 출력
	LocalFree(lpmsgbuf);																//Ipmsgbuf에 할당된 메모리 반환
}
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET sock, char* buf, int len, int flags)					// 사용자 정의 데이터 수신 함수 recv를 반복
{
	char* ptr = buf;																		//버퍼의 시작주소 읽을때마다 ptr 값 증가
	int left = len;																			//읽지 않은 데이터의 크기, 읽을때 마다 left 값 감소
	int recived;																				//recv함수의 리턴 값을 저장하는 변수

	while (left > 0)																			//읽지 않은 데이터가 없을때까지 반복
	{
		recived = recv(sock, ptr, left, flags);									//데이터를 수신하는 함수
		if (recived == SOCKET_ERROR)											//에러확인
		{
			return SOCKET_ERROR;
		}
		if (recived == 0)																	//정상 종료시 나감
		{
			break;
		}

		left -= recived;																		//데이터를 읽을 때마다 증감하면서 
		ptr += recived;																		//정확한 바이트 수를 리턴하기 위한처리
	}

	return (len - left);																	//읽은 바이트수 리턴
}

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
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

	// 서버와 데이터 통신
	int size;
	bool endflag = false;

	while(1)
	{		
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

		// 문자열 길이가 0이면 보내지 않음
		if(MyInfo->state!= GAME_END_STATE && strlen(buf) == 0)
		{
			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent); // 읽기 완료 알리기
			continue;
		}

		switch (MyInfo->state)//진행상태에 따른 진행
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
		
		EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
		SetEvent(hReadEvent); // 읽기 완료 알리기

		if (endflag)
		{
			break;
		}
	}

	return 0;
}

//recvn 2번 사이즈-> 데이터
bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;
	//받을 데이터의 사이즈 저장
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
	//사이즈 만큼 데이터를 받아옴
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

//recv쓰레드
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
		if (!PacketRecv(MyInfo->sock, MyInfo->recvbuf))	//데이터받음
		{
			err_display("recv error()");
			return -1;
		}

		protocol=GetProtocol(MyInfo->recvbuf);				//프로토콜 분리
		
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

		DisplayText("%s\r\n", msg);									//받아온 msg 출력

	}

	return 0;
}

//프로토콜 출력
PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}


/* Packing */
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1) //버퍼, 프로토콜, msg
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
int PackPacket(char* _buf, PROTOCOL _protocol)//버퍼 프로토콜
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
void UnPackPacket(const char* _buf, char* _str) //버퍼, msg
{
	int strsize;
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
	ptr = ptr + strsize;
}
void UnPackPacket(const char* _buf, char* _str, int& _count) //버퍼, namelist, listnum
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