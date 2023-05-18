#pragma comment(lib, "ws2_32.lib")//속성->링커->입력->ws2_32.lib; 입력해도됨
#include <WinSock2.h>					 //윈도우소켓사용하기위한 헤더파일
#include <stdio.h>
#include <stdlib.h>

#define SERVERIP "127.0.0.1"			//루프백 주소
#define SERVERPORT 9000			//서버포트 논리적인 접속장소
#define BUFSIZE 512						//메모리사이즈
#define PLAYERNUMCOUNT 3			//플레이어가 입력한 숫자의 갯수

enum PROTOCOL //프로토콜 + 서버가 복잡해지면 매니저단위로 분리
{
	INTRO = 1, // 인트로

	// 게임 플레이
	ANSWER_NUM,                                //클라이언트가 입력한 숫자
	ANSWER_RESULT,                         //서버가 판단한 결과
	ANSWER_ERROR,                          //3개 숫자가 오류확인

	// 게임 오버 
	CORRECT_ANSWER_RESULT,     //정답을 맞추었을 때
	GAME_PROCESS,                        //진행여부
	GAME_RESULT                           //지금까지 플레이한 전적
};

enum ERROR_CODE   //에러코드 문자열
{
	DATA_DUPLICATE = 1,   //숫자 중복
	DATA_RANGE_ERROR   //범위 오류
};

enum GAME_PROCESS   //게임 종료후 클라->서버 추가판단
{
	GAME_CONTINUE = 1,   //게임계속
	GAME_OVER               //게임종료
};

enum   //배열의 INDEX로 사용=숫자 지정 x
{
	STRIKE,
	BALL,
	OUTC
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

bool PacketRecv(SOCKET _sock, char* _buf)   //패킷을 받는 함수 (2번처리 했던거 한번에)
{
	int size;
	int retval = recvn(_sock, (char*)&size, sizeof(size), 0);   //받는 데이터의 크기
	if (retval == SOCKET_ERROR)   //강제종료 (오류)
	{
		err_display("gvalue recv error()");
		return false;
	}
	else if (retval == 0)                   //정상 종료
	{
		return false;
	}

	retval = recvn(_sock, _buf, size, 0);   // 사이즈만큼 데이터를 받아옴
	if (retval == SOCKET_ERROR)   //강제종료 (오류)
	{
		err_display("gvalue recv error()");
		return false;

	}
	else if (retval == 0)                   //정상 종료
	{
		return false;
	}

	return true;   //모두 받음
}

PROTOCOL GetProtocol(char* _buf)   //프로토콜만 분리
{
	PROTOCOL protocol;
	memcpy(&protocol, _buf, sizeof(PROTOCOL));

	return protocol;
}

//버퍼 , 프로토콜 , 진행여부
int PackPacket(char* _buf, PROTOCOL  _protocol, int _data)
{
	int size = 0;
	char* ptr = _buf;
	ptr = ptr + sizeof(size);

	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
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

//버퍼, 프로토콜 , 플레이어 숫자, 숫자갯수
int PackPacket(char* _buf, PROTOCOL  _protocol, const int* _playernum, int _numcount)
{
	char* ptr = _buf;
	int size = 0;
	ptr = ptr + sizeof(size);

	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_numcount, sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	memcpy(ptr, &_playernum[0], sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	memcpy(ptr, &_playernum[1], sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	memcpy(ptr, &_playernum[2], sizeof(int));
	ptr = ptr + sizeof(int);
	size = size + sizeof(int);

	ptr = _buf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

//버퍼, 메세지
void UnPackPacket(char* _buf, char* _msg)//Packet된 정보를 푸는 함수
{
	char* ptr = _buf + sizeof(PROTOCOL);

	int strsize = 0;

	memcpy(&strsize, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(_msg, ptr, strsize);
	ptr = ptr + strsize;

}
//버퍼, 데이터 , 메세지
void UnPackPacket(char* _buf, int* _data, char* _msg)//Packet된 정보를 푸는 함수
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(_data, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	int strsize = 0;

	memcpy(&strsize, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(_msg, ptr, strsize);
	ptr = ptr + strsize;

	printf("오류확인 %s \n", _msg);

}

int main()
{
	//사용 변수들
	PROTOCOL protocol;			//진행 위치(상황)
	char sendbuf[BUFSIZE];		//send용 버퍼
	char recvbuf[BUFSIZE];		//recv용 버퍼
	char msg[BUFSIZE];			//msg버퍼
	bool IsGame = true;				//게임진행 판단
	bool IsPlay = false;				//추가진행 판단
	int playernum[PLAYERNUMCOUNT];		//플레이어가 던질 공의 숫자
	int select;												//선택
	int size;												//사이즈
	int temp;												//임시

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

	while (1) //접속이후 진행
	{
		//처음 나올 메뉴 출력
		printf("<<메뉴>>\n");
		printf("1.게임 시작\n");
		printf("2.종료\n");

		//메뉴 선택
		printf("선택:");
		scanf("%d", &select);

		if (select == GAME_OVER)					//종료일 경우 break
		{
			break;
		}
		else if (select == GAME_CONTINUE)	//시작일 경우 진행
		{
			while (IsGame)								//게임진행 여부
			{
				IsPlay = true;							//게임을 진행할꺼면 true로 바꿔줌
				ZeroMemory(msg, sizeof(msg));//메모리를 0으로 채워줌(초기화)
				//게임 시작하는 메시지를 서버로부터 전달받음.
				PacketRecv(sock, recvbuf);			
				UnPackPacket(recvbuf, msg);
				printf("%s", msg);
				
				while (IsPlay)			//게임진행
				{
					//플레이어가 서버에게 전달할 숫자 입력받음
					printf("입력: ");
					scanf("%d %d %d", &playernum[0], &playernum[1], &playernum[2]);
					/*
					for (int i = 0; i < PLAYERNUMCOUNT; i++)
					{//다른입력방식
						printf("%d번째 숫자 입력: ",i);
						scanf("%d", &playernum[i]);
					}*/			
					protocol = (PROTOCOL)ANSWER_NUM; //숫자 전달한다는걸 서버로 알려주기 위한 protocol값 설정
					size = PackPacket(sendbuf, protocol, playernum, PLAYERNUMCOUNT); //sndbuf에 패키징 처리
					retval = send(sock, sendbuf, size, 0);	//서버에 전송
					if (retval == SOCKET_ERROR)//오류확인
					{
						err_display("send()");
						break;
					}

					PacketRecv(sock, recvbuf); //서버에서 전달 받은 데이터를 두번에 걸쳐 받아옴
					protocol = GetProtocol(recvbuf);	//recv버퍼에서 프로토콜 값만 가져옴
					switch (protocol)
					{
					case ANSWER_RESULT: //정답이 아닐경우 스트라이크 몇개~ 볼 몇개 ~ 아웃 몇개~ 로 출력
						ZeroMemory(msg, sizeof(msg));
						UnPackPacket(recvbuf, msg); //패키징된 버퍼를 품
						printf("%s", msg);//출력
						break;
					case ANSWER_ERROR:							 //오류가 있을경우 오류를 출력
						ZeroMemory(msg, sizeof(msg));			 //메모리초기화
						UnPackPacket(recvbuf, &temp, msg);	 //패키징된 버퍼를 품
						printf("%s", msg);									 //출력
						break;
					case CORRECT_ANSWER_RESULT:			 //정답일 경우 진행
						ZeroMemory(msg, sizeof(msg));			 //메모리초기화
						UnPackPacket(recvbuf, msg);				 //패키징된 버퍼를 품
						printf("%s", msg);									 //몇번만에 정답을 맞췄는지 받아온걸 출력
						ZeroMemory(msg, sizeof(msg));			 //메모리초기화
						PacketRecv(sock, recvbuf);					 //서버에서 전달받은 데이터를 recv
						UnPackPacket(recvbuf, msg);				 //패키징된 버퍼를 품
						printf("%s", msg);									 //추가로 진행할건지 물어보는 메세지를 받아와서 출력

						//추가 진행할것인지
						printf("1.예  2.아니요\n");
						scanf("%d", &select);

						if (select == GAME_CONTINUE)												//추가진행
						{
							protocol = GAME_PROCESS;												//진행중인위치가 추가진행 선택지부분인것을 알리기 위해 설정
							size = PackPacket(sendbuf, protocol, GAME_CONTINUE); //추가 진행하는것과 프로토콜을 패키징
							retval = send(sock, sendbuf, size, 0);								//패키징된 정보를 전달
							if (retval == SOCKET_ERROR)											//오류확인
							{
								err_display("send()");
								break;
							}
							IsPlay = false;																	//다시 진행을 위한 while 탈출
						}
						else if (select == GAME_OVER)												//추가진행x
						{
							protocol = GAME_PROCESS;												//진행중인위치가 추가진행 선택지부분인것을 알리기 위해 설정
							size = PackPacket(sendbuf, protocol, GAME_OVER);			//추가진행을 안하는 것과 프로토콜을 패키징
							retval = send(sock, sendbuf, size, 0);								//패키징된 정보를 전달
							if (retval == SOCKET_ERROR)											//오류확인
							{
								err_display("send()");
								break;
							}
							IsPlay = false;																	//while 탈출
							IsGame = false;																//게임진행 탈출
						}
						break;
					default:
						break;
					}
				}
			}
			ZeroMemory(msg, sizeof(msg));			//메모리 초기화
			PacketRecv(sock, recvbuf);					//서버로부터 데이터를 받음
			UnPackPacket(recvbuf, msg);				//받은 데이터의 패키징된 데이터를 해제
			printf("%s", msg);									//받은 메세지 출력
		}		
		else
		{	//입력오류
			printf("옳지 않는 숫자를 입력하셨습니다 다시입력해주세요! \n");
		}
	}
	closesocket(sock);//소켓 삭제
	// 윈속 종료
	WSACleanup();
	return 0;
}