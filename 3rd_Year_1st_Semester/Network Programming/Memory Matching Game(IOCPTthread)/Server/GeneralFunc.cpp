#include "Global.h"

//클라이언트 정보추가
_ClientInfo* AddClientInfo(SOCKET _sock, SOCKADDR_IN _addr)
{
	EnterCriticalSection(&cs);

	_ClientInfo* ptr = new _ClientInfo;
	memset(ptr, 0, sizeof(_ClientInfo));
	ptr->sock = _sock;
	memcpy(&ptr->addr, &_addr, sizeof(SOCKADDR_IN));
	ptr->r_sizeflag = false;
	ptr->game_value = NODATA;
	ptr->part = nullptr;
	ptr->state = INTRO_STATE;

	ptr->r_overlapped.ptr = ptr;
	ptr->r_overlapped.type = IO_TYPE::IO_RECV;

	ptr->s_overlapped.ptr = ptr;
	ptr->s_overlapped.type = IO_TYPE::IO_SEND;

	ClientInfo[Count] = ptr;
	Count++;
	LeaveCriticalSection(&cs);

	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

	return ptr;
}

//클라이언트정보삭제
void RemoveClientInfo(_ClientInfo* _ptr)
{
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(_ptr->addr.sin_addr), ntohs(_ptr->addr.sin_port));

	EnterCriticalSection(&cs);
	for (int i = 0; i < Count; i++)
	{
		if (ClientInfo[i] == _ptr)
		{
			closesocket(ClientInfo[i]->sock);

			if (ClientInfo[i]->part != nullptr)
			{
				ClientInfo[i]->part->part = nullptr;
			}

			delete ClientInfo[i];

			for (int j = i; j < Count - 1; j++)
			{
				ClientInfo[j] = ClientInfo[j + 1];
			}

			break;
		}
	}

	Count--;
	LeaveCriticalSection(&cs);
}

//클라이언트 매칭 함수
bool MatchPartner(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);

	for (int i = 0; i < Count; i++)
	{
		if (_ptr != ClientInfo[i] && ClientInfo[i]->part == nullptr)
		{
			ClientInfo[i]->part = _ptr;
			_ptr->part = ClientInfo[i];
			LeaveCriticalSection(&cs);
			return true;
		}
	}

	LeaveCriticalSection(&cs);
	return false;
}


// 승자결정 함수
int who_win(int first, int second)
{
	//데이터가 들어온쪽이 승리(먼저 끝)
	if (first == second)
		return NO_WIN;
	else if (first ==1 &&second==NODATA)
		return WIN;
	else
		return LOSE;
}

//recv된 정보로 실행하는 프로세스
void CompleteRecvProcess(_ClientInfo* _ptr)
{
	PROTOCOL protocol = GetProtocol(_ptr->recvbuf);

	switch (_ptr->state)
	{
	case GAME_STATE:
		switch (protocol)
		{
		case GAME_VALUE:
			UnPackPacket(_ptr->recvbuf, _ptr->game_value);
			GameProcess(_ptr);
			break;
		}	

		break;
	}

}
//send에 따른 state 설정
void CompleteSendProcess(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);
	switch (_ptr->state)
	{
	case INTRO_STATE:
		_ptr->state = GAME_STATE;
		break;
	case GAME_STATE:

		break;
	}

	LeaveCriticalSection(&cs);

}

//게임승패 프로세스
void GameProcess(_ClientInfo* _ptr)
{
	int size1, size2;
	int retval;

	EnterCriticalSection(&cs);
	if (_ptr->part != nullptr )
	{
		//승자판단
		int result = who_win(_ptr->game_value, _ptr->part->game_value);
		switch (result)
		{
			//승자에 따른 sendbuf 설정
		case NO_WIN:			
			size1 = PackPacket(_ptr->sendbuf, GAME_RESULT, NO_WIN, NO_WIN_MSG);		
			size2 = PackPacket(_ptr->part->sendbuf, GAME_RESULT, NO_WIN, NO_WIN_MSG);
			break;
		case WIN:
			size1 = PackPacket(_ptr->sendbuf, GAME_RESULT, WIN, WIN_MSG);
			size2 = PackPacket(_ptr->part->sendbuf, GAME_RESULT, LOSE, LOSE_MSG);			
			break;
		case LOSE:
			size1 = PackPacket(_ptr->sendbuf, GAME_RESULT, LOSE, LOSE_MSG);
			size2 = PackPacket(_ptr->part->sendbuf, GAME_RESULT, WIN, WIN_MSG);		
			break;
		}
		//send
		if (!Send(_ptr, size1))
		{
			_ptr->state = DISCONNECTED_STATE;			
		}

		if (!Send(_ptr->part, size2))
		{
			
			_ptr->part->state = DISCONNECTED_STATE;
		}

	}

	LeaveCriticalSection(&cs);
}