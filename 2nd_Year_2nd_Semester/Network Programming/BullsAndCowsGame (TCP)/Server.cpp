#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> 
#include <time.h>

#define SERVERPORT 9000
#define BUFSIZE    4096
#define NUMBER_COUNT 3

#define MAX_COUNT 99
#define MIN_COUNT 0

#define INTRO_MSG "숫자야구게임을 시작합니다~ 1~9까지의 수중 중복되지 않는 3개의 숫자를 입력하세요.\n"
#define DATA_DUPLICATE_MSG "입력된 숫자에 중복이 있습니다.다시 입력하세요.\n"
#define DATA_RANGE_ERROR_MSG "입력된 숫자는 숫자야구 게임의 숫자 범위가 아닙니다. 다시 입력하세요.\n"
#define GAME_CONTINUE_MSG "게임을 계속하시겠습니까? "

enum CLIENT_STATE   //접속한 클라마다 상태를 변환
{
    INIT_STATE = 1,                 //접속만 한 상태
    INTRO_STATE,                  //게임의 인트로를 클라에게 보내는 상태
    GAME_PLAY_STATE,        //게임 플레이 상태
    GAME_OVER_STATE,       //추가진행
    DISCONNECTED_STATE  //서버와의 연결 종료
};

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

struct _ClientInfo
{
    SOCKET sock;			                //소켓정보
    SOCKADDR_IN addr;                //주소정보
    CLIENT_STATE state;             //클라이언트의 상태
    int corr_num[NUMBER_COUNT];   //서버가 생성한 숫자 (정답)
    int game_count;                           //게임의 진행 횟수
    int answer_count;                       //정답을 말한 횟수
    int min_answer_count;               //빨리 클리어한 횟수 저장
    int max_answer_count;              //느리게 클리어한 횟수 저장
    char recv_buf[BUFSIZE];           // 버퍼 recv용
    char send_buf[BUFSIZE];          // 버퍼 send용
};

_ClientInfo* ClientInfo[100];           //플레이어의 최대인원수 크기에 맞춰 생성
int Count = 0;                                  //진행횟수


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

_ClientInfo* AddClientInfo(SOCKET sock, SOCKADDR_IN addr)
{
    _ClientInfo* ptr = new _ClientInfo;   // 클라이언트 메모리 할당
    ZeroMemory(ptr, sizeof(_ClientInfo));   // 메모리 초기화
    ptr->sock = sock;
    memcpy(&ptr->addr, &addr, sizeof(addr));
    ptr->state = INIT_STATE;   // 클라이언트 상태 접속중으로 변경
    ptr->max_answer_count = MIN_COUNT;
    ptr->min_answer_count = MAX_COUNT;

    ClientInfo[Count++] = ptr;

    printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
        inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

    return ptr;
}

void ReMoveClientInfo(_ClientInfo* ptr)
{
    printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
        inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

    for (int i = 0; i < Count; i++)
    {
        if (ClientInfo[i] == ptr)
        {
            closesocket(ptr->sock);
            delete ptr;
            for (int j = i; j < Count - 1; j++)
            {
                ClientInfo[j] = ClientInfo[j + 1];
            }
            break;
        }
    }

    Count--;
}

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

bool PacketRecv(SOCKET _sock, char* _buf)   //패킷을 받는 함수
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

//INTRO,GAME_RESULT
int PackPacket(char* _buf, PROTOCOL  _protocol, const char* _str)
{
    int size = 0;
    char* ptr = _buf;
    int strsize = strlen(_str);


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

//CORRECT_ANSWER_RESULT, ANSWER_ERROR,ANSWER_RESULT
int PackPacket(char* _buf, PROTOCOL  _protocol, int _data, const char* _str)
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

//ANSWER_NUM
void UnPackPacket(const char* _buf, int* _data, int& _count)
{
    const char* ptr = _buf + sizeof(PROTOCOL);

    memcpy(&_count, ptr, sizeof(_count));
    ptr = ptr + sizeof(_count);

    for (int i = 0; i < _count; i++)
    {
        memcpy(&_data[i], ptr, sizeof(_data[i]));
        ptr = ptr + sizeof(_data[i]);
    }
}

//GAME_PROCESS
void UnPackPacket(char* _buf, int& _data)
{
    char* ptr = _buf + sizeof(PROTOCOL);

    memcpy(&_data, ptr, sizeof(_data));
    ptr = ptr + sizeof(_data);
}

void RandCorrectNumber(int* _corr_num, int _count)
{
    int count = 0;

    while (1)
    {
        int num = rand() % 9 + 1;
        bool flag = false;

        for (int i = 0; i < count; i++)
        {
            if (_corr_num[i] == num)
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            continue;
        }
        _corr_num[count++] = num;
        if (count == _count)
        {
            break;
        }
    }

    printf("%d  %d  %d\n", _corr_num[0], _corr_num[1], _corr_num[2]);
}

bool ChecKDuplicate(int* _data, int _count)
{
    for (int i = 0; i < _count - 1; i++)
    {
        for (int j = i + 1; j < _count; j++)
        {
            if (_data[i] == _data[j])
            {
                return false;
            }
        }
    }
    return true;
}

bool ChecKDataRange(int* _data, int _count)
{

    for (int i = 0; i < _count; i++)
    {
        if (_data[i] < 1 || _data[i]>9)
        {
            return false;
        }
    }

    return true;
}

void ProcessBaseBallGame(int* _corr_num, int* _answer_num, int* _result, int _count)
{
    memset(_result, 0, sizeof(int) * _count);

    for (int i = 0; i < _count; i++)
    {
        bool flag = false;
        for (int j = 0; j < _count; j++)
        {
            if (_answer_num[i] == _corr_num[j])
            {
                flag = true;
                if (i == j)
                {
                    _result[STRIKE]++;
                }
                else
                {
                    _result[BALL]++;
                }
            }
        }
        if (!flag)
        {
            _result[OUTC]++;
        }
    }
}

void IntroProcess(_ClientInfo* _ptr)
{
    RandCorrectNumber(_ptr->corr_num, NUMBER_COUNT);

    int size = PackPacket(_ptr->send_buf, INTRO, INTRO_MSG);

    int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
    if (retval == SOCKET_ERROR)
    {
        err_display("send()");
        _ptr->state = DISCONNECTED_STATE;
        return;
    }
    _ptr->game_count++;
    _ptr->state = GAME_PLAY_STATE;
}

void GamePlayProcess(_ClientInfo* _ptr)
{
    if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
    {
        _ptr->state = DISCONNECTED_STATE;
        return;
    }

    PROTOCOL protocol = GetProtocol(_ptr->recv_buf);

    int answer[NUMBER_COUNT];
    int result[3];
    int count;

    switch (protocol)
    {
    case ANSWER_NUM:

        memset(answer, 0, sizeof(answer));
        UnPackPacket(_ptr->recv_buf, answer, count);
        _ptr->answer_count++;

        if (!ChecKDuplicate(answer, NUMBER_COUNT))//중복 오류 확인
        {
            int size = PackPacket(_ptr->send_buf, ANSWER_ERROR, DATA_DUPLICATE, DATA_DUPLICATE_MSG);
            int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
            if (retval == SOCKET_ERROR)
            {
                _ptr->state = DISCONNECTED_STATE;
                return;
            }
            break;
        }

        if (!ChecKDataRange(answer, NUMBER_COUNT)) //범위 오류 확인
        {
            int size = PackPacket(_ptr->send_buf, ANSWER_ERROR, DATA_RANGE_ERROR, DATA_RANGE_ERROR_MSG);
            int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
            if (retval == SOCKET_ERROR)
            {
                err_display("send()");
                _ptr->state = DISCONNECTED_STATE;
                return;
            }
            break;
        }

        //게임체크
        ProcessBaseBallGame(_ptr->corr_num, answer, result, NUMBER_COUNT);

        if (result[STRIKE] == NUMBER_COUNT)// 스트라이크 숫자 3개면 게임끝
        {
            _ptr->state = GAME_OVER_STATE;
            return;
        }

        char msg[BUFSIZE];
        sprintf(msg, "Strike:%d Ball:%d Out:%d\n", result[STRIKE], result[BALL], result[OUTC]);
        int size = PackPacket(_ptr->send_buf, ANSWER_RESULT, msg);
        int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
        if (retval == SOCKET_ERROR)
        {
            err_display("send()");
            _ptr->state = DISCONNECTED_STATE;
            return;
        }
    }
}

void GameOverProcess(_ClientInfo* _ptr)
{
    char msg[BUFSIZE];
    sprintf(msg, "%d번 만에 정답을 맞추셨습니다.\n", _ptr->answer_count);
    int size = PackPacket(_ptr->send_buf, CORRECT_ANSWER_RESULT, msg);
    int retval = send(_ptr->sock, _ptr->send_buf, size, 0);
    if (retval == SOCKET_ERROR)
    {
        err_display("send()");
        _ptr->state = DISCONNECTED_STATE;
        return;
    }
    if (_ptr->max_answer_count < _ptr->answer_count)
    {
        _ptr->max_answer_count = _ptr->answer_count;
    }

    if (_ptr->min_answer_count > _ptr->answer_count)
    {
        _ptr->min_answer_count = _ptr->answer_count;
    }

    size = PackPacket(_ptr->send_buf, GAME_PROCESS, GAME_CONTINUE_MSG);
    retval = send(_ptr->sock, _ptr->send_buf, size, 0);
    if (retval == SOCKET_ERROR)
    {
        err_display("send()");
        _ptr->state = DISCONNECTED_STATE;
        return;
    }

    if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
    {
        _ptr->state = DISCONNECTED_STATE;
        return;
    }

    PROTOCOL protocol = GetProtocol(_ptr->recv_buf);

    switch (protocol)
    {
    case GAME_PROCESS:
        int result;
        UnPackPacket(_ptr->recv_buf, result);

        switch (result)
        {
        case GAME_CONTINUE:
            _ptr->answer_count = 0;
            _ptr->state = INTRO_STATE;
            break;
        case GAME_OVER:
            char msg[BUFSIZE];

            sprintf(msg, "전체게임횟수:%d, 최저 성적:%d  최고 성적:%d\n", _ptr->game_count, _ptr->max_answer_count, _ptr->min_answer_count);
            size = PackPacket(_ptr->send_buf, GAME_RESULT, msg);
            retval = send(_ptr->sock, _ptr->send_buf, size, 0);
            if (retval == SOCKET_ERROR)
            {
                err_display("send()");
            }

            if (!PacketRecv(_ptr->sock, _ptr->recv_buf))
            {
                _ptr->state = DISCONNECTED_STATE;
            }

            break;
        }
        break;
    }
}

int main(int argc, char* argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;

    char buf[BUFSIZE + 1];

    srand(time(0));

    while (1)
    {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET)
        {
            err_display("accept()");
            break;
        }

        _ClientInfo* ClientPtr = AddClientInfo(client_sock, clientaddr);

        while (1)
        {
            bool endflag = false;

            switch (ClientPtr->state)
            {
            case INIT_STATE:
                ClientPtr->state = INTRO_STATE;
                break;
            case INTRO_STATE:
                IntroProcess(ClientPtr);
                break;
            case GAME_PLAY_STATE:
                GamePlayProcess(ClientPtr);
                break;
            case GAME_OVER_STATE:
                GameOverProcess(ClientPtr);
                break;
            case DISCONNECTED_STATE:
                ReMoveClientInfo(ClientPtr);
                endflag = true;
                break;
            }

            if (endflag)
            {
                break;
            }
        }

    }
    // closesocket()
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}