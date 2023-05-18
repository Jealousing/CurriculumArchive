#pragma comment(lib, "ws2_32.lib")//속성->링커->입력->ws2_32.lib; 입력해도됨
#include <WinSock2.h>					 //윈도우소켓사용하기위한 헤더파일
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"				//루프백 주소
#define SERVERPORT 9000				//서버포트 논리적인 접속장소
#define BUFSIZE    4096					//버퍼 사이즈
#define MAX_FILE_READ_SIZE 2048	//파일전송 사이즈 버퍼사이즈보다 12이상 작아야됨! 
#define FILENAMESIZE 256				//파일 이름 사이즈

enum PROTOCOL								//프로토콜 + 서버가 복잡해지면 매니저단위로 분리
{
	INTRO = 1,
	FILE_INFO,									// 클라가 서버한테 보내는 패킷 프로토콜
	FILE_TRANS_DENY,						// 파일 전송 거부
	FILE_TRANS_START_POINT,			// 파일 전송 위치(처음부터 & 이어서)
	FILE_TRANS_DATA						// 클라가 파일 전송할 프로토콜
};

enum DENY_CODE   // 파일 전송 거부사유
{
	FILEEXIST = -1   // 파일 존재
};

struct _File_info								//파일정보
{
	char filename[FILENAMESIZE];	//파일이름
	int  filesize;									//파일사이즈
	int  nowsize;								//읽은사이즈
}Fileinfo;

void err_quit(const char* msg)														// 소켓 함수 오류 출력 후 종료
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

void err_display(const char* msg)												// 소켓 함수 오류 출력
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

//패키징 버퍼<- 프로토콜/ 파일이름 / 파일크기
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
//패키징 버퍼<- 프로토콜/전송사이즈/파일정보
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
//프로토콜 가져오기
PROTOCOL GetProtocol(const char* _buf)
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));
	return protocol;
}

//언패키징 버퍼->오류/메세지
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
//언패키징 버퍼->메시지
void UnPackPacket(const char* _buf, char* _str)
{
	const char* ptr = _buf;
	int strsize;

	ptr = ptr + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_str, ptr, strsize);
}
//언패키징 버퍼->읽은 데이터사이즈
void UnPackPacket(const char* _buf, int& _data)
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(_data));
}
//recvn을 두번처리하는 함수
bool PacketRecv(SOCKET _sock, char* _buf)
{
	int size;
	//사이즈를 받아오고
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
	//사이즈만큼 데이터를 받아옴
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
	int retval;										//오류확인
	char buf[BUFSIZE];						//버퍼
	
	WSADATA  wsa;																// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)					//윈도우소켓 버전 정보 전달 및 주소값 전달, MAKEWORD의 
		return -1;																		//첫번째 변수는 주버전, 두번째 버전은 부버전 즉 2.2버전을 의미

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);	//IPV4,PCP프로토콜사용하는 소켓생성
	if (sock == INVALID_SOCKET)	err_quit("socket()");		//오류확인! ->여기서 발생하는 오류는 심각한오류 = quit

	// connect()
	SOCKADDR_IN serveraddr;											//소켓 구조체 소켓 주소와 포트를 할당하기 위한것.
	ZeroMemory(&serveraddr, sizeof(serveraddr));			//메모리를 0으로 초기화
	serveraddr.sin_family = AF_INET;								//IPv4 주소체계 사용
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);  //inet_addr는 문자열을 IPv4주소로 변환하는 함수
	serveraddr.sin_port = htons(SERVERPORT);				//서버포트로만 접속
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));//서버 접속 시도
	if (retval == SOCKET_ERROR)err_quit("connect()");		//오류확인

	ZeroMemory(&Fileinfo, sizeof(Fileinfo));
		
	FILE* fp = nullptr;			//파일포인터
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
						printf("파일 전송 완료!: %d 바이트\n", Fileinfo.filesize);
						endflag = true;
						break;
					}
					else
					{
						perror("파일 입출력 오류");
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

	// 윈속 종료
	WSACleanup();
	system("pause");
	return 0;
}
