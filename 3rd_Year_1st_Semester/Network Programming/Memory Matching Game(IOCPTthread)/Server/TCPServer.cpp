#define MAIN
#include "Global.h"

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	InitializeCriticalSection(&cs);

	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;
		
	// CPU ���� Ȯ��
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU ���� * 2)���� �۾��� ������ ����
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++) {
		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hThread == NULL) return 1;
		CloseHandle(hThread);
	}

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr,
		sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;	
	int size;

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr,
			&addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		_ClientInfo* ptr = AddClientInfo(client_sock, clientaddr);

		if (MatchPartner(ptr))
		{		
			printf("��Ī����\n");
			int size = PackPacket(ptr->sendbuf, INTRO, INTRO_MSG);

			if (!Send(ptr, size))
			{
				ptr->state = DISCONNECTED_STATE;
			}

			if (ptr->part->state == INTRO_STATE)
			{			

				int size = PackPacket(ptr->part->sendbuf, INTRO, INTRO_MSG);

				if (!Send(ptr->part, size))
				{
					ptr->part->state = DISCONNECTED_STATE;
				}
			}			
		}	

		if (!Recv(ptr))
		{
			ptr->state = DISCONNECTED_STATE;
		}
	}

	// closesocket()
	closesocket(listen_sock);

	DeleteCriticalSection(&cs);
	// ���� ����
	WSACleanup();
	return 0;
}
