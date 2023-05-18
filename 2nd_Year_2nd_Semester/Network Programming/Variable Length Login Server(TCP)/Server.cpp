#pragma comment(lib, "ws2_32.lib")//속성->링커->입력->ws2_32.lib; 입력해도됨
#include <WinSock2.h>					 //윈도우소켓사용하기위한 헤더파일
#include <stdio.h>
#include <stdlib.h>
#include <time.h>								

#define SERVERPORT 9000			//서버포트 논리적인 접속장소
#define BUFSIZE 512						//메모리사이즈
#define PLAYER_COUNT 1				//플레이어 인원수
#define JOIN_USER_COUNT 100		//최대 저장가능한 숫자

//메세지 전달을 위한 매크로
#define ID_ERROR_MSG "없는 아이디입니다\n"
#define PW_ERROR_MSG "패스워드가 틀렸습니다\n"
#define LOGIN_SUCCESS_MSG "로그인에 성공했습니다.\n"
#define JOIN_SUCCESS_MSG "가입에 성공했습니다.\n"
#define ID_EXIST_MSG "이미있는 아이디입니다.\n"
#define GAMESTART_MSG "숫자야구 게임이 시작되었습니다.\n"
#define GAMESTOP_MSG "숫자야구 게임이 종료되었습니다.\n"
#define GAME_MSG "추가로 더 진행하시겠습니까?.\n"
#define GAME_ERROR_MSG "\n중복되지 않는 1~9사이의 숫자를 3개 입력해주세요!.\n"
#define GAME_CLEAR_MSG "번만에 클리어 했습니다!."


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

struct _ClientInfo		//클라이언트 정보
{
	SOCKET sock;			//소켓정보
	SOCKADDR_IN addr;//주소정보
	_UserInfo* info;		//유저정보
	bool login;					//로그인 유무

	char buf[BUFSIZE];	//버퍼
};

_ClientInfo* ClientInfo[PLAYER_COUNT];	//플레이어의 최대인원수 크기에 맞춰 생성
int Count = 0;											//진행횟수

_UserInfo* UserInfo[JOIN_USER_COUNT];//회원가입 정보 저장을 위한 선언
int JoinCount = 0;										  //회원가입 진행횟수

_ClientInfo* AddClientInfo(SOCKET sock, SOCKADDR_IN addr)			//클라이언트 접속정보 추가
{
	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));							//접속한 클라의 정보 출력

	_ClientInfo* ptr = new _ClientInfo();												//메모리 추가
	ZeroMemory(ptr, sizeof(_ClientInfo));												//0으로 채움
	ptr->sock = sock;																				//소켓정보 저장
	memcpy(&ptr->addr, &addr, sizeof(addr));										//메모리 내용 복사
	ptr->login = false;																			//로그인 x

	ClientInfo[Count++] = ptr;																	//생성

	return ptr;
}

void ReMoveClientInfo(_ClientInfo* ptr)												//클라이언트 접속정보 삭제
{
	printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));			//접속한 클라의 정보 출력

	for (int i = 0; i < Count; i++)
	{
		if (ClientInfo[i] == ptr)
		{
			closesocket(ptr->sock);															//클라소켓 종료
			delete ptr;																				//정보삭제
			for (int j = i; j < Count - 1; j++)
			{
				ClientInfo[j] = ClientInfo[j + 1];												//메모리 땡김
			}
			break;
		}
	}
	Count--;																							//인원수 감소
}

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

int JoinProcess(_UserInfo info)													//회원가입 프로세스
{
	for (int i = 0; i < JoinCount; i++)
	{
		if (!strcmp(UserInfo[i]->id, info.id))										//같은 아이디가 있는지 확인
		{
			return ID_EXIST;																//있으면 있다고 리턴
		}
	}

	UserInfo[JoinCount] = new _UserInfo(info);								//없으면 생성
	JoinCount++;
	return JOIN_SUCCESS;															//회원가입 성공
}

void DropProcess(_ClientInfo* _ptr)											//회원탈퇴 프로세스
{
	if (_ptr->login)																			//로그인 확인
	{
		for (int i = 0; i < JoinCount; i++)
		{
			if (!strcmp(UserInfo[i]->id, _ptr->info->id))						//같은 아이디를 가진 회원정보를 찾음
			{
				delete UserInfo[i];														//회원정보 삭제
				for (int j = i; j < JoinCount - 1; j++)
				{
					UserInfo[j] = UserInfo[j + 1];										//회원정보 빈공간제거
				}
				break;
			}
		}

		JoinCount--;																		//회원수 감소
	}
}

_UserInfo* SearchUserInfo(const char* _Id)								//유저 정보 찾기
{
	for (int i = 0; i < JoinCount; i++)
	{
		if (!strcmp(UserInfo[i]->id, _Id))											//해당 아이디가 있는지 확인
		{
			return UserInfo[i];															//해당 유저정보데이터 반환
		}
	}

	return nullptr;																			//없으면 nullptr출력
}

int LoginProcess(_UserInfo info)													//로그인 프로세스
{

	for (int i = 0; i < JoinCount; i++)
	{
		if (!strcmp(UserInfo[i]->id, info.id))										//유저정보와 일치하는 아이디찾기
		{
			if (!strcmp(UserInfo[i]->pw, info.pw))								//일치한 아이디의 비밀번호가 입력한 비밀번호랑 같은지 확인
			{
				return LOGIN_SUCCESS;												//로그인 성공
			}
			else
			{
				return PW_ERROR;														//패스워드 에러
			}
		}
	}

	return ID_ERROR;																	//아이디 없음
}

int Packing(char* _buf, PROTOCOL _protocol, int _result, const char* str1) //Packing->_protocol, 결과와 메세지
{	//buf에 순서대로 데이터 쌓아 패키징
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	memcpy(ptr, &_result, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	int strsize1 = strlen(str1);
	memcpy(ptr, &strsize1, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	memcpy(ptr, str1, strsize1);
	size = size + strsize1;
	ptr = ptr + strsize1;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

int Packing(char* _buf, PROTOCOL _protocol, const char* str1)					//Packing->_protocol, 메세지
{	//buf에 순서대로 데이터 쌓아 패키징
	char* ptr = _buf + sizeof(int);
	int size = 0;
	//memcpy(a,b,n) b메모리 영역에서 a 메모리 영역으로 n바이트만큼 복사
	memcpy(ptr, &_protocol, sizeof(PROTOCOL));
	size = size + sizeof(PROTOCOL);
	ptr = ptr + sizeof(PROTOCOL);

	int strsize1 = strlen(str1);
	memcpy(ptr, &strsize1, sizeof(int));
	size = size + sizeof(int);
	ptr = ptr + sizeof(int);

	memcpy(ptr, str1, strsize1);
	size = size + strsize1;
	ptr = ptr + strsize1;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(int));

	return size + sizeof(int);
}

void  UnPacking(const char* _buf, char* str1, char* str2)						//Packing된 정보를 푸는 함수
{	//char* 두개방식
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	int strsize2;

	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;

	memcpy(&strsize2, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str2, ptr, strsize2);
	ptr = ptr + strsize2;
}

void  UnPacking(const char* _buf, char* str1)
{	//char* 한개방식
	const char* ptr = _buf + sizeof(PROTOCOL);
	int strsize1;
	int strsize2;

	memcpy(&strsize1, ptr, sizeof(int));
	ptr = ptr + sizeof(int);

	memcpy(str1, ptr, strsize1);
	ptr = ptr + strsize1;

}

int errcheck(char* num, int _count)	//오류 확인 함수
{															//중복숫자, 숫자갯수 확인
	if (num[0] - '0' + num[1] - '0' + num[2] - '0' < 6 || num[0] - '0' + num[1] - '0' + num[2] - '0' > 24)
		return 1;										//중복되지않는수로 최소로 나올수있는수 123의 미만, 최대 789의 초과를 하면 오류리턴

	for (int i = 0; i < _count - 1; i++)
	{
		int x = num[i] - '0';
		int y = num[i + 1] - '0';
		if (x == y)										//x와y를 비교해서 같으면 오류 
			return 1;
	}
	return 0;											//오류없었음
}


int main()
{
	char err[1];					//에러확인용
	char roundinfo[3];		//라운드정보
	int fastclear = 0;			//빨리클리어한 횟수
	int slowclear = 0;			//느리게클리어한 횟수
	int countclear = 0;		//클리어 횟수
	int runcount = 0;			//시도횟수
	bool gamerun = false; //게임 진행여부

	WSADATA  wsa;																// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)					//윈도우소켓 버전 정보 전달 및 주소값 전달, MAKEWORD의 
		return -1;																		//첫번째 변수는 주버전, 두번째 버전은 부버전 즉 2.2버전을 의미

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);	//IPV4,PCP프로토콜사용하는 소켓생성
	if (listen_sock == INVALID_SOCKET)	err_quit("socket()");		//오류확인! ->여기서 발생하는 오류는 심각한오류 = quit

	// bind()
	SOCKADDR_IN serveraddr;													//소켓 구조체 소켓 주소와 포트를 할당하기 위한것.
	ZeroMemory(&serveraddr, sizeof(serveraddr));					//메모리를 0으로 초기화
	serveraddr.sin_family = AF_INET;										//IPv4 주소체계 사용
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);				//어느주소든지 받겠다는 주소지정
	serveraddr.sin_port = htons(SERVERPORT);						//호스트 하나에 대해서만 SERVERPORT에 설정된 포트 개방
	int retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));//retval=리턴벨류, 클라대기상태로 만듬
	if (retval == SOCKET_ERROR)err_quit("bind()");					//대기 가능한 최대 연결 갯수를 넘으면 요청 거절 ->심각한오류=quit

	// listen()
	retval = listen(listen_sock, SOMAXCONN);								//서버 소켓을 통해 클라이언트의 접속 요청을 확인하도록 설정
	if (retval == SOCKET_ERROR)err_quit("listen()");					//심각한오류! , SOMAXCONN는 socket max connection

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;		//클라이언트 주소
	SOCKET client_sock;				//통신용 소켓 변수
	char buf[BUFSIZE + 1];				//버퍼!

	while (1)
	{
		// accept() 클라의 접속 요청 대기 및 허락
		int addrlen = sizeof(clientaddr);																//소켓 정보를 구하기 위해 클라주소크기를 대입
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);	//접속허락
		if (client_sock == INVALID_SOCKET)															//클라 소켓 받아오기 에러가 없으면 클라 소켓 반환
		{																												//INVALID_SOCKET는 소켓 생성에 실패한 경우 반환하는 값
			err_display("accept()");
			break;
		}

		_ClientInfo* ptr = AddClientInfo(client_sock, clientaddr);							//클라이언트의 접속정보로 구조체 만들어줌

		while (1)
		{
			int size;				//사이즈 정보를 받기위한 변수
			char pitching[3];	//클라에서 넘어온 숫자 3개를 위한 변수
			int baseball[3];		//서버에서 지정한 숫자 3개를 위한 변수
			char score[2];		//스트라이크와 볼의 갯수 체크를 위한 변수
			int result;				//결과

			//packing으로 처리된 데이터를 읽기 위한 2중 recvn
			retval = recvn(ptr->sock, (char*)&size, sizeof(int), 0);  //1회 recvn : 길이구하기
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
			{
				break;
			}
			retval = recvn(ptr->sock, ptr->buf, size, 0);					//2회 recvn : 길이만큼 데이터 받기
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
			{
				break;
			}

			PROTOCOL protocol;													//열거형 변수 정의
			memcpy(&protocol, ptr->buf, sizeof(PROTOCOL));		//메모리 복사 buf의 내용을 protocol로 복사

			_UserInfo info;																//유저정보 구조체 선언

			switch (protocol)												//받아온 protocal 값에 따른 switch문 실행
			{
			case JOIN://회원가입
				ZeroMemory(&info, sizeof(_UserInfo));		//info 초기화
				UnPacking(ptr->buf, info.id, info.pw);			//받아온 packing된 ID와 PW를 unpacking
				result = JoinProcess(info);							//회원가입프로세스 실행
				switch (result)												//결과에 따른 switch 검색
				{
				case ID_EXIST:												//이미 있는 아이디
					size = Packing(ptr->buf, JOIN_RESULT, ID_EXIST, ID_EXIST_MSG); //결과와 ID_EXIST_MSG를 packing
					break;
				case JOIN_SUCCESS:									//가입성공
					size = Packing(ptr->buf, JOIN_RESULT, JOIN_SUCCESS, JOIN_SUCCESS_MSG);	//결과와 JOIN_SUCCESS_MSG를 packing
					break;
				}
				break;
			case LOGIN://로그인
				ZeroMemory(&info, sizeof(_UserInfo));		//info 초기화
				UnPacking(ptr->buf, info.id, info.pw);			//받아온 packing된 ID와 PW를 unpacking
				result = LoginProcess(info);						//로그인 프로세스 실행
				switch (result)												//결과에 따른 switch 검색
				{
				case ID_ERROR:											//없는 아이디
					size = Packing(ptr->buf, LOGIN_RESULT, ID_ERROR, ID_ERROR_MSG);//결과와 MSG를 Packing
					break;
				case PW_ERROR:											//페스워드 에러
					size = Packing(ptr->buf, LOGIN_RESULT, PW_ERROR, PW_ERROR_MSG);//결과와 MSG를 Packing
					break;
				case LOGIN_SUCCESS:								//로그인성공
					size = Packing(ptr->buf, LOGIN_RESULT, LOGIN_SUCCESS, LOGIN_SUCCESS_MSG);//결과와 MSG를 Packing
					ptr->info = SearchUserInfo(info.id);			//로그인 성공한 해당 유저정보 찾기
					ptr->login = true;										//로그인중
					break;
				}
				break;
			case GAMESTART://게임시작
				int strike;		//스트라이크 갯수 체크
				int ball;			//볼 갯수 체크
				if (gamerun == false)	//게임을 진행중이 아니였으면 진행
				{
					size = Packing(ptr->buf, GAMESTART, GAMESTART_MSG);	//게임 시작메세지와 게임시작프로토콜을 Packing
					ZeroMemory(&baseball, sizeof(baseball));							//서버의 숫자 배열 초기화
					srand((unsigned)time(NULL));												//시간에 따른 랜덤값 조정
					for (int i = 0; i < 3; i++)
					{
						baseball[i] = rand() % 9 + 1;												//1~9까지의 숫자 랜덤설정
						for (int j = 0; j < i; j++)
						{
							if (baseball[i] == baseball[j])											//겹치는 숫자가 생성되었으면 실행
								i--;																			//다시 생성
						}
					}
					gamerun = true;																	//게임 진행완료
					printf("서버가 생성한 숫자 : %d, %d, %d\n", baseball[0], baseball[1], baseball[2]);//서버가 생성한 숫자
				}
				else//진행중인 경우
				{
					UnPacking(ptr->buf, pitching);			//클라이언트에게 숫자를 받아옴
					printf("클라이언트가 입력한 숫자 : %c, %c, %c\n", pitching[0], pitching[1], pitching[2]);//받아온 숫자출력
					ZeroMemory(&score, sizeof(score));	//스트라이크와 볼을 전달을 위한 변수 초기화
					ZeroMemory(&strike, sizeof(strike));	//스트라이크 변수 초기화
					ZeroMemory(&ball, sizeof(ball));			//볼 변수 초기화

					err[0] = 0; //0이면 오류x

					for (int i = 0; i < 3; i++)
					{
						for (int j = 0; j < 3; j++)
						{
							if (baseball[i] == pitching[j] - '0')//컴퓨터가 정한 숫자와 클라이언트에게 받은 숫자 비교
							{
								if (i == j)	//같은 위치에 있으면 스트라이크
								{
									strike++;
								}
								else			//같은 숫자가 있지만 위치가 다를경우 볼 증가
								{
									ball++;
								}
							}
						}
					}

					err[0] = errcheck(pitching, 3);	//에러 체크 오류가 있을경우 1이 저장

					size = Packing(ptr->buf, GAMESTART, err);	//에러 정보 전달
					retval = send(ptr->sock, ptr->buf, size, 0);	//데이터 클라이언트로 전달
					if (retval == SOCKET_ERROR)
					{
						err_display("send()");
						break;
					}

					if (err[0] == 0)							//에러가 없을경우 진행
					{
						score[0] = strike;					//체크된 스트라이크 갯수를 스코어 0번배열에 저장
						score[1] = ball;						//체크된 볼 갯수를 스코어 1번 배열에 저장
						printf("Strike : %d , Ball : %d \n", strike, ball);	//스트라이크와 볼의 갯수를 서버에 출력
						runcount++;							//시도 횟수 증가
						if (strike == 3)//스트라이크 3개 = 게임종료
						{
							size = Packing(ptr->buf, GAMESTART, score);//스코어 정보 전달을 위한 packing
							retval = send(ptr->sock, ptr->buf, size, 0);	  //packing된 정보 클라이언트로 전달
							if (retval == SOCKET_ERROR)
							{
								err_display("send()");
								break;
							}
							//서버에 게임 종료 정보 출력
							printf("\n3스트라이크 게임종료\n");
							printf("플레이어가 %d번만에 클리어 했습니다\n", runcount);

							ZeroMemory(buf, sizeof(buf));
							sprintf(buf, "%d", runcount); //buf에 진행횟수 복사
							strcat(buf, GAME_CLEAR_MSG);	//buf 뒤에 GAME_CLEAR_MSG연결

							size = Packing(ptr->buf, GAMESTART, buf);	//몇회 진행했는지 Packing
							countclear++;	//클리어 횟수증가
							if (fastclear == 0 || fastclear > runcount)		//저장된 정보가 없거나 더 짧은 기록이 있으면 변경
							{
								fastclear = runcount;
							}

							if (slowclear == 0 || slowclear < runcount)	//저장된 정보가 없거나 더 긴 기록이 있으면 변경
							{
								slowclear = runcount;
							}

							runcount = 0;	//실행횟수 초기화
							gamerun = false;	//게임진행 off
							break;
						}
						else
						{
							size = Packing(ptr->buf, GAMESTART, score);	//스코어 정보 packing 
						}
					}
					else	//에러일 경우 실행
					{
						printf("\n클라이언트 숫자가 잘못되었습니다.\n");
						err[0] = 0;//에러정보 초기화
						size = Packing(ptr->buf, GAMESTART, GAME_ERROR_MSG); //에러메세지 packing후 전달
					}
				}
				break;
			case GAMESTOP://게임종료
				size = Packing(ptr->buf, GAMESTOP, GAMESTOP_MSG);//게임종료 메세지 packing
				retval = send(ptr->sock, ptr->buf, size, 0);	//게임종료메시지 클라이언트로 전달
				if (retval == SOCKET_ERROR)
				{
					err_display("send()");
					break;
				}
				roundinfo[0] = countclear;	//클리어 횟수저장
				roundinfo[1] = fastclear;		//빠르게 클리어한 정보 저장
				roundinfo[2] = slowclear;		//느리게 클리어한 정보 저장

				size = Packing(ptr->buf, GAMESTOP, roundinfo);	//클리어 정보packing
				//사용된 변수 초기화
				ZeroMemory(roundinfo, sizeof(roundinfo));
				ZeroMemory(&countclear, sizeof(countclear));
				ZeroMemory(&fastclear, sizeof(fastclear));
				ZeroMemory(&slowclear, sizeof(slowclear));
				ptr->login = false;//로그인정보 off
				break;
			}

			retval = send(ptr->sock, ptr->buf, size, 0); //마지막에 Packing한 정보 클라이언트로 전달
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
		}

		ReMoveClientInfo(ptr);//접속종료되는 클라이언트 정보 삭제
	}

	closesocket(listen_sock);//소켓 삭제

	WSACleanup();//윈도우 소켓 해제

	return 0;
}