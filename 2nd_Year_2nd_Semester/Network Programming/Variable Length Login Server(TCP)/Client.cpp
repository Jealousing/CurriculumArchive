#pragma comment(lib, "ws2_32.lib")//속성->링커->입력->ws2_32.lib; 입력해도됨
#include <WinSock2.h>					 //윈도우소켓사용하기위한 헤더파일
#include <stdio.h>
#include <stdlib.h>

#define SERVERIP "127.0.0.1"			//루프백 주소
#define SERVERPORT 9000			//서버포트 논리적인 접속장소
#define BUFSIZE 512						//메모리사이즈

enum PROTOCOL		//열거형을 통한 상수관리
{
	JOIN = 1,					//회원가입
	LOGIN,						//로그인
	EXIT,							//종료
	GAMESTART,			//게임시작
	GAMESTOP,				//종료

	LOGIN_RESULT,		//로그인 방향
	JOIN_RESULT			//회원가입 방향
};

enum RES_LOGIN	//열거형을 통한 상수관리
{
	ID_ERROR = 1,			//없는아이디
	PW_ERROR,				//패스워드 오류
	LOGIN_SUCCESS		//로그인 성공
};

enum RES_JOIN		//열거형을 통한 상수관리
{
	JOIN_SUCCESS = 1,	//가입성공
	ID_EXIST					//이미있는 아이디
};

struct _UserInfo		//유저 정보 관리
{
	char id[255];			//아이디
	char pw[255];			//페스워드

	_UserInfo(const _UserInfo& info)//복사생성자
	{
		strcpy(id, info.id);
		strcpy(pw, info.pw);
	}
	_UserInfo()//소멸자
	{

	}
};

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

int Packing(char* _buf, PROTOCOL _protocol, const char* _id, const char* _pw)	//Packing -> _protocol,아이디와 비번
{	//buf에 순서대로 데이터 쌓아 패키징
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
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
{	//buf에 순서대로 데이터 쌓아 패키징
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
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
{	//buf에 순서대로 데이터 쌓아 패키징
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}


void UnPacking(const char* _buf, int& _result, char* str1)	//Packing된 정보를 푸는 함수 ->_result,str1
{
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
	memcpy(&_result, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;
}

void UnPacking(const char* _buf, char* str1)	//Packing된 정보를 푸는 함수->str1
{
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;
}

int main()
{
	char roundinfo[3];		//라운드정보
	char buf[BUFSIZE];		//버퍼
	bool gamerun = true;	//게임 진행여부
	int result;						//결과

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
	int retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));//서버 접속 시도
	if (retval == SOCKET_ERROR)err_quit("connect()");		//오류확인
	
	while (1)
	{
		//처음 나올 메뉴 출력
		printf("<<메뉴>>\n");
		printf("1.가입\n");
		printf("2.로그인\n");
		printf("3.종료\n");
		printf("선택:");

		//메뉴 선택
		int select;
		scanf("%d", &select);

		if (select == EXIT)			//종료일 경우 break
		{
			printf("종료");
			break;
		}

		_UserInfo info;				//유저정보 구조체 선언

		printf("ID:");					//ID 입력받음
		scanf("%s", info.id);

		printf("PW:");				//PW 입력받음
		scanf("%s", info.pw);

		int size = Packing(buf, (PROTOCOL)select, info.id, info.pw);	//입력받은 ID,PW Packing

		retval = send(sock, buf, size, 0);	//Packing된 정보 서버로 전달
		if (retval == SOCKET_ERROR)		//오류 확인
		{
			err_display("send()");
			break;
		}

		//packing으로 처리된 데이터를 읽기 위한 2중 recvn
		retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1회 recvn : 길이구하기
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
		{
			break;
		}
		retval = recvn(sock, buf, size, 0);							//2회 recvn : 길이만큼 데이터 받기
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
		{
			break;
		}

		PROTOCOL protocol;											//열거형 변수 정의
		memcpy(&protocol, buf, sizeof(PROTOCOL));		//메모리 복사 buf의 내용을 protocol로 복사

		char msg[BUFSIZE];											//메세지를 전달 받기위한 변수
		ZeroMemory(msg, sizeof(msg));							//초기화
		
		switch (protocol)													//입력 받은 프로토콜로 찾아서 진행
		{
		case LOGIN_RESULT:											//로그인
			UnPacking(buf, result, msg);							//Packing된 정보 UnPacking
			printf("%s\n", msg);										//전달 받은 메세지 출력
			switch (result)													//전달 받은 결과로 진행
			{
			case PW_ERROR:												//로그인중 페스워드 오류
			case ID_ERROR:												//로그인중 아이디 오류
				break;
			case LOGIN_SUCCESS:									//로그인 성공
			{
				while (gamerun)											//게임 진행중일 경우 반복
				{
					//로그인중일 경우 메뉴의 출력
					printf("<<메뉴>>\n");
					printf("1.게임시작\n");
					printf("2.종료\n");
					printf("선택:");
					//메뉴선택
					int select;
					scanf("%d", &select);

					size = Packing(buf, (PROTOCOL)(select + 3));	//선택된 메뉴번호 Packing
					retval = send(sock, buf, size, 0);						//메뉴번호 서버에게 전달
					if (retval == SOCKET_ERROR)
					{
						err_display("send()");
						break;
					}

					//packing으로 처리된 데이터를 읽기 위한 2중 recvn
					retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1회 recvn : 길이구하기
					if (retval == SOCKET_ERROR)
					{
						err_display("recv()");
						break;
					}
					else if (retval == 0)
					{
						break;
					}
					retval = recvn(sock, buf, size, 0);							//2회 recvn : 길이만큼 데이터 받기
					if (retval == SOCKET_ERROR)
					{
						err_display("recv()");
						break;
					}
					else if (retval == 0)
					{
						break;
					}

					memcpy(&protocol, buf, sizeof(PROTOCOL));//메모리 복사 buf의 내용을 protocol로 protocol크기만큼 복사


					if (protocol == GAMESTART&&gamerun) //protocol이 게임시작을 가리크는 경우 && 게임 진행인경우
					{
						char score[2];					//스트라이크와 볼을 저장할 변수
						char err[1];						//에러 저장 변수

						UnPacking(buf, msg);		//packing된 msg를 unpacking
						printf("\n%s", msg);		//저장된 메세지 출력

						char pitching[255];			//클라이언트가 입력한 숫자를 저장할 변수

						while (1)
						{
							ZeroMemory(&pitching, sizeof(pitching));						//초기화
							printf("중복 되지 않는 숫자를 3개를 입력하세요. ex) 416 :");
							scanf("%s", &pitching);													//입력받기
							size = Packing(buf, (PROTOCOL)(select + 3), pitching);	//입력된 숫자와 protocol packing 
							retval = send(sock, buf, size, 0);									//저장된 정보 서버에 전달
							if (retval == SOCKET_ERROR)										//에러체크
							{
								err_display("send()");
								break;
							}

							//packing으로 처리된 데이터를 읽기 위한 2중 recvn
							retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1회 recvn : 길이구하기
							if (retval == SOCKET_ERROR)
							{
								err_display("recv()");
								break;
							}
							else if (retval == 0)
							{
								break;
							}
							retval = recvn(sock, buf, size, 0);							//2회 recvn : 길이만큼 데이터 받기
							if (retval == SOCKET_ERROR)
							{
								err_display("recv()");
								break;
							}
							else if (retval == 0)
							{
								break;
							}

							ZeroMemory(&score, sizeof(score));	//스코어 정보 초기화
							ZeroMemory(&err, sizeof(err));			//에러 정보 초기화

							UnPacking(buf, err);								//packing된 err정보 unpacking

							if (err[0] !=1)//오류가 아닐경우 진행
							{
								//packing으로 처리된 데이터를 읽기 위한 2중 recvn
								retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1회 recvn : 길이구하기
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}
								retval = recvn(sock, buf, size, 0);							//2회 recvn : 길이만큼 데이터 받기
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}

								UnPacking(buf, score);						//packing된 score정보 unpacking

								if (score[0] == 3)//스트라이크가 3개일경우
								{
									printf("\n");
									//packing으로 처리된 데이터를 읽기 위한 2중 recvn
									retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1회 recvn : 길이구하기
									if (retval == SOCKET_ERROR)
									{
										err_display("recv()");
										break;
									}
									else if (retval == 0)
									{
										break;
									}
									retval = recvn(sock, buf, size, 0);							//2회 recvn : 길이만큼 데이터 받기
									if (retval == SOCKET_ERROR)
									{
										err_display("recv()");
										break;
									}
									else if (retval == 0)
									{
										break;
									}

									ZeroMemory(msg, sizeof(msg));	//msg 초기화
									UnPacking(buf, msg);					//packing된 msg정보 unpacking
									printf("%s\n", msg);					//받아온 msg 출력
									printf("3Strike 게임에서 승리하셨습니다.\n\n");//정해진 숫자 다 맞춤
									break;
								}
								else if (score[0] == 0 && score[1] == 0)
								{	//스트라이크와 볼이 하나도 없음.
									printf("OUT : 하나도 맞추질 못했습니다.\n");
								}
								else
								{	//받아온 스트라이크와 볼의 갯수 출력
									printf("Strike: %d, Ball: %d\n", score[0], score[1]);
								}
								printf("%s\n", msg);//저장된 msg 출력
							}
							else//오류일경우
							{
								//packing으로 처리된 데이터를 읽기 위한 2중 recvn
								retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1회 recvn : 길이구하기
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}
								retval = recvn(sock, buf, size, 0);							//2회 recvn : 길이만큼 데이터 받기
								if (retval == SOCKET_ERROR)
								{
									err_display("recv()");
									break;
								}
								else if (retval == 0)
								{
									break;
								}

								UnPacking(buf, msg);					//packing된 msg정보 unpacking
								printf("%s\n", msg);					//받아온 msg 출력
							}

							ZeroMemory(buf, sizeof(buf));			//buf 초기화
						}
						
					}
					else if(protocol== GAMESTOP)//게임종료
					{
						ZeroMemory(msg, sizeof(msg));		//msg초기화
						gamerun = false;								//게임진행 off
						UnPacking(buf, msg);					//packing된 msg정보 unpacking
						printf("%s\n", msg);					//받아온 msg 출력

						//packing으로 처리된 데이터를 읽기 위한 2중 recvn
						retval = recvn(sock, (char*)&size, sizeof(int), 0);	 //1회 recvn : 길이구하기
						if (retval == SOCKET_ERROR)
						{
							err_display("recv()");
							break;
						}
						else if (retval == 0)
						{
							break;
						}
						retval = recvn(sock, buf, size, 0);							//2회 recvn : 길이만큼 데이터 받기
						if (retval == SOCKET_ERROR)
						{
							err_display("recv()");
							break;
						}
						else if (retval == 0)
						{
							break;
						}

						UnPacking(buf, roundinfo);	//packing된 roundinfo정보 unpacking
						if(roundinfo[0]>=1)				//진행한 게임횟수가 1을 넘을 경우 진행
						printf("\n지금까지 진행한 게임의 횟수: %d\n가장 빠르게 맞춘 게임의 정답 시도 횟수: %d\n가장 늦게 맞춘 게임의 정답 시도 횟수: %d\n", roundinfo[0], roundinfo[1], roundinfo[2]);
						break;
					}

				}
			}
			break;
			}
			break;

		case JOIN_RESULT:						//회원 가입
			UnPacking(buf, result, msg);	//packing된 msg,result정보 unpacking
			switch (result)							//받아온 result에 따라 진행
			{
			case ID_EXIST:							//이미 있는 아이디
			case JOIN_SUCCESS:				//회원가입 성공
				printf("%s\n", msg);
				break;
			}
			break;
		}
	}
	closesocket(sock);//소켓 삭제

	// 윈속 종료
	WSACleanup();
	return 0;
}