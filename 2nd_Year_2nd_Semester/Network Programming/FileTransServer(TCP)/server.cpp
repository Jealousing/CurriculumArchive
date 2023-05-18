#pragma comment(lib, "ws2_32.lib")//속성->링커->입력->ws2_32.lib; 입력해도됨
#include <WinSock2.h>					 //윈도우소켓사용하기위한 헤더파일
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 4096			//버퍼 사이즈	
#define FILENAMESIZE 256	//파일이름 사이즈
#define MAXCONCLIENT 100	//클라 최대접속수

//전달할 메세지
#define INTRO_MSG "전송할 파일명을 입력하세요"
#define FILE_DUPLICATE_MSG "전송하고자 하는 파일은 이미 서버에 존재하는 파일입니다."

enum PROTOCOL
{
	INTRO = 1,								//인트로
	FILE_INFO,							// 클라가 서버한테 보내는 패킷 프로토콜
	FILE_TRANS_DENY,				// 파일 전송 거부
	FILE_TRANS_START_POINT,// 파일 전송 위치(처음부터, 이어서)
	FILE_TRANS_DATA,				// 클라가 파일 읽어서 전송할 프로토콜
};

enum STATE
{
	INITE_STATE=1,
	FILE_TRANS_READY_STATE,		// 파일전송 준비단계 (정보받아와서 전송받을것인가 아닌가 판단)
	FILE_TRANS_STATE,					// 클라가 보낸 파일 받아옴
	FILE_TRANS_COMPLETE_STATE,	// 파일 전송 완료
	DISCONNECTED_STATE				// 연결 종료
};

enum DENY_CODE   // 파일 전송 거부사유
{
	FILEEXIST = -1   // 파일 존재
};

struct _File_info
{
	char filename[FILENAMESIZE];	//파일이름
	int  filesize;									//파일 총 크기
	int  nowsize;								// 현제까지 받은 용량
};

//크리티컬섹션
CRITICAL_SECTION cs;

struct _ClientInfo
{
	SOCKET sock;							//소켓정보
	SOCKADDR_IN addr;					//주소정보
	STATE	state;							//진행위치
	_File_info  file_info;					//파일정보
	char recv_buf[BUFSIZE];			//recv(다운로드)용 버퍼
	char send_buf[BUFSIZE];			//send(업로드)용 버퍼
	HANDLE wait_event;				//기다림 이벤트
};

//클라이언트 정보 구조체 배열 선언
_ClientInfo* ClientInfo[MAXCONCLIENT];
int count;//클라이언트 접속인원

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

//클라이언트 접속 정보처리
_ClientInfo* AddClientInfo(SOCKET sock, SOCKADDR_IN addr)
{
	EnterCriticalSection(&cs);																//임계영역 할당

	_ClientInfo* ptr = new _ClientInfo;													//메모리 할당
	ZeroMemory(ptr, sizeof(_ClientInfo));												//메모리 초기화
	ptr->sock = sock;																				//받아온 소켓정보 저장
	memcpy(&ptr->addr, &addr, sizeof(addr));										//클라 주소 저장
	ptr->state = INITE_STATE;																//state(상태) 설정
	ClientInfo[count++] = ptr;																	//클라이언트정보 포인트배열에 저장 (카운트증가)


	LeaveCriticalSection(&cs);																//임계영역 해제

	printf("\nFileSender 접속: IP 주소=%s, 포트 번호=%d\n",					//접속된 클라이언트 정보 출력
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

	printf("FileSender 종료: IP 주소=%s, 포트 번호=%d\n",						//접속종료하는 클라이언트 정보 출력
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

	EnterCriticalSection(&cs);																//임계영역 할당

	_ClientInfo* next_ptr = SearchNextClient(ptr);
	if (next_ptr != nullptr)
	{
		SetEvent(next_ptr->wait_event);
	}

	for (int i = 0; i < count; i++)
	{
		if (ClientInfo[i] == ptr)
		{
			closesocket(ptr->sock);															//소켓정보삭제
			delete ptr;
			for (int j = i; j < count - 1; j++)													//땡기기
			{
				ClientInfo[j] = ClientInfo[j + 1];
			}
			break;
		}
	}

	count--;																							//클레이어 접속인원수 감소
	LeaveCriticalSection(&cs);																//임계영역 해제
}

bool SearchFile(const char* filename)
{
	EnterCriticalSection(&cs);																//임계영역 할당
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = FindFirstFile(filename, &FindFileData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return false;
	else {
		FindClose(hFindFile);
		return true;
	}
	LeaveCriticalSection(&cs);																//임계영역 해제
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

int PackPacket(char* _buf, PROTOCOL  _protocol, const char* _str) //인트로 
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

int PackPacket(char* _buf, PROTOCOL  _protocol, int _data, const char* _str) //파일전송 거부 정보
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


int PackPacket(char* _buf, PROTOCOL _protocol, int _data)//파일 전송 위치 정보
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

//buf에서 프로토콜 정보만 받아오는 함수
PROTOCOL GetProtocol(char* _buf)
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));

	return protocol;
}

void UnPackPacket(const char* _buf, char* _str1, int& _data1)//파일이름 & 파일 크기
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

void UnPackPacket(const char* _buf, int& _size, char* _targetbuf) //파일 전송 데이터
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_size, ptr, sizeof(_size));
	ptr = ptr + sizeof(_size);

	memcpy(_targetbuf, ptr, _size);
}

//접속하면 실행
void InitProcess(_ClientInfo* _ptr)
{	
	EnterCriticalSection(&cs);
	//접속하면 클라이언트에게 인트로 메세지를 보냄
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

//접속완료처리후 실행
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

		printf("-> 받을 파일 이름: %s\n", filename);
		printf("-> 받을 파일 크기: %d\n", filesize);

		if (SearchFile(filename))
		{
			//다른 실행자 기다림
			WaitForSingleObject(_ptr->wait_event, INFINITE);
		
			FILE* fp = fopen(filename, "rb");
			fseek(fp, 0, SEEK_END);
			int fsize = ftell(fp);
			fclose(fp);
			if (filesize == fsize)
			{
				printf("존재하는 파일 전송 요구\n");

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

//파일 읽기
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
			perror("파일 입출력 오류");
			_ptr->state = FILE_TRANS_COMPLETE_STATE;
			fclose(fp);
			LeaveCriticalSection(&cs);
			return;
		}
		_ptr->file_info.nowsize += transsize;
		break;
	}
}

//파일읽기 종료
void FileTransCompleteProcess(_ClientInfo* _ptr)
{
	if (_ptr->file_info.filesize != 0 && _ptr->file_info.filesize == _ptr->file_info.nowsize)
	{

		printf("전송완료!!\n");
	}
	else
	{
		printf("전송실패!!\n");
	}
	_ClientInfo* next_ptr = SearchNextClient(_ptr);
	if (next_ptr != nullptr)
	{
		SetEvent(next_ptr->wait_event);
	}
	_ptr->state = DISCONNECTED_STATE;
}

//클라이언트마다 다르게 실행
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

	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수

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
		//클라이언트 생성
		_ClientInfo* ClientPtr = AddClientInfo(client_sock, clientaddr);		
		//멀티쓰레드 생성
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
	// 윈속 종료
	WSACleanup();
	return 0;
}
