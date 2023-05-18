#include "Global.h"

//쓰레드
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;

	while (1)
	{
		// 비동기 입출력 완료 기다리기
		DWORD cbTransferred;
		SOCKET client_sock;
		WSAOVERLAPPED_EX* overlapped;

		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			(LPDWORD)&client_sock, (LPOVERLAPPED*)&overlapped, INFINITE);

		_ClientInfo* ptr = overlapped->ptr;

		// 클라이언트 정보 얻기
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);

		// 비동기 입출력 결과 확인
		if (retval == 0 || cbTransferred == 0)
		{
			if (retval == 0)
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &overlapped->overlapped,
					&temp1, FALSE, &temp2);
				err_display("WSAGetOverlappedResult()");
			}

			ptr->state = DISCONNECTED_STATE;
		}

		if (ptr->state == DISCONNECTED_STATE)
		{
			RemoveClientInfo(ptr);
			continue;
		}


		int result;
		//타입에따라 recv send 나눔
		switch (overlapped->type)
		{
		case IO_TYPE::IO_RECV:
			result = CompleteRecv(ptr, cbTransferred);
			switch (result)
			{
			case SOC_ERROR:
				continue;
			case SOC_FALSE:
				continue;
			case SOC_TRUE:
				break;
			}

			CompleteRecvProcess(ptr);

			if (!Recv(ptr))
			{
				ptr->state = DISCONNECTED_STATE;
			}

			break;
		case IO_TYPE::IO_SEND:
			result = CompleteSend(ptr, cbTransferred);
			switch (result)
			{
			case SOC_ERROR:
				continue;
			case SOC_FALSE:
				continue;
			case SOC_TRUE:
				break;
			}

			CompleteSendProcess(ptr);
			break;
		}

	}
	return 0;
}
