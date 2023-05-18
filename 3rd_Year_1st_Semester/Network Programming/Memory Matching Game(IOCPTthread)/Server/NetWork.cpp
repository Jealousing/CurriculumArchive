#include "Global.h"

//소켓 함수 오류 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//프로토콜 출력
PROTOCOL GetProtocol(const char* _ptr)
{
	PROTOCOL protocol;
	memcpy(&protocol, _ptr, sizeof(PROTOCOL));

	return protocol;
}


//send용 데이터 packing
int PackPacket(char* _buf, PROTOCOL _protocol, const char* _str1)
{
	int size = 0;
	char* ptr = _buf;
	int strsize1 = strlen(_str1);

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

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
//send용 데이터 packing
int PackPacket(char* _buf, PROTOCOL _protocol, RESULT _result, const char* _str1)
{
	int size = 0;
	char* ptr = _buf;
	int strsize1 = strlen(_str1);

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_result, sizeof(_result));
	ptr = ptr + sizeof(_result);
	size = size + sizeof(_result);

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

//packing된 정보 unpacking
void UnPackPacket(const char* _buf, int& _data)
{
	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data, ptr, sizeof(_data));
	ptr = ptr + sizeof(_data);

}

//recv
bool Recv(_ClientInfo* _ptr)
{
	int retval;
	DWORD recvbytes;
	DWORD flags = 0;

	ZeroMemory(&_ptr->r_overlapped.overlapped, sizeof(_ptr->r_overlapped.overlapped));

	_ptr->r_wsabuf.buf = _ptr->recvbuf + _ptr->comp_recvbytes;

	if (_ptr->r_sizeflag)
	{
		_ptr->r_wsabuf.len = _ptr->recvbytes - _ptr->comp_recvbytes;
	}
	else
	{
		_ptr->r_wsabuf.len = sizeof(int) - _ptr->comp_recvbytes;
	}

	retval = WSARecv(_ptr->sock, &_ptr->r_wsabuf, 1, &recvbytes,
		&flags, &_ptr->r_overlapped.overlapped, nullptr);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSARecv()");
			//RemoveClientInfo(_ptr);
			return false;
		}
	}

	return true;
}

//recv 종료 여부
int CompleteRecv(_ClientInfo* _ptr, int _completebyte)
{

	if (!_ptr->r_sizeflag)
	{
		_ptr->comp_recvbytes += _completebyte;

		if (_ptr->comp_recvbytes == sizeof(int))
		{
			memcpy(&_ptr->recvbytes, _ptr->recvbuf, sizeof(int));
			_ptr->comp_recvbytes = 0;
			_ptr->r_sizeflag = true;
		}

		if (!Recv(_ptr))
		{
			return SOC_ERROR;
		}

		return SOC_FALSE;
	}

	_ptr->comp_recvbytes += _completebyte;

	if (_ptr->comp_recvbytes == _ptr->recvbytes)
	{
		_ptr->comp_recvbytes = 0;
		_ptr->recvbytes = 0;
		_ptr->r_sizeflag = false;

		return SOC_TRUE;
	}
	else
	{
		if (!Recv(_ptr))
		{
			return SOC_ERROR;
		}

		return SOC_FALSE;
	}
}

//send
bool Send(_ClientInfo* _ptr, int _size)
{
	int retval;
	DWORD sendbytes;
	DWORD flags;

	ZeroMemory(&_ptr->s_overlapped.overlapped, sizeof(_ptr->s_overlapped.overlapped));
	if (_ptr->sendbytes == 0)
	{
		_ptr->sendbytes = _size;
	}

	_ptr->s_wsabuf.buf = _ptr->sendbuf + _ptr->comp_sendbytes;
	_ptr->s_wsabuf.len = _ptr->sendbytes - _ptr->comp_sendbytes;

	retval = WSASend(_ptr->sock, &_ptr->s_wsabuf, 1, &sendbytes,
		0, &_ptr->s_overlapped.overlapped, nullptr);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSASend()");
			//RemoveClientInfo(_ptr);
			return false;
		}
	}

	return true;
}

//send종료여부
int CompleteSend(_ClientInfo* _ptr, int _completebyte)
{
	_ptr->comp_sendbytes += _completebyte;
	if (_ptr->comp_sendbytes == _ptr->sendbytes)
	{
		_ptr->comp_sendbytes = 0;
		_ptr->sendbytes = 0;

		return SOC_TRUE;
	}
	if (!Send(_ptr, _ptr->sendbytes))
	{
		return SOC_ERROR;
	}

	return SOC_FALSE;
}
