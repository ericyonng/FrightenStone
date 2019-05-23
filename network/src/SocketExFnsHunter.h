#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <MSWSock.h>

typedef BOOL(WINAPI *AcceptExPtr)(SOCKET, SOCKET, PVOID, DWORD, DWORD, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL(WINAPI *ConnectExPtr)(SOCKET, const struct sockaddr *, int, PVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef void (WINAPI *GetAcceptExSockaddrsPtr)(PVOID, DWORD, DWORD, DWORD, LPSOCKADDR *, LPINT, LPSOCKADDR *, LPINT);

class SocketExFnsHunter
{
public:
    SocketExFnsHunter(){ Hunt(); }
    AcceptExPtr AcceptEx;
    ConnectExPtr ConnectEx;
    GetAcceptExSockaddrsPtr GetAcceptExSockaddrs;

private:

    void Hunt()
    {
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
		CHECK(IS_VALID_SOCKET_IDD(s));

        const GUID acceptex = WSAID_ACCEPTEX;
        AcceptEx = (AcceptExPtr)get_extension_function(s, &acceptex);
        const GUID connectex = WSAID_CONNECTEX;
        ConnectEx = (ConnectExPtr)get_extension_function(s, &connectex);
        const GUID getacceptexsockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
        GetAcceptExSockaddrs = (GetAcceptExSockaddrsPtr)get_extension_function(s, &getacceptexsockaddrs);
        
		RELEASE_SOCKET(s);
    }

    void * get_extension_function(SOCKET s, const GUID *which_fn)
    {
        void *ptr = NULL;
        DWORD bytes = 0;
        WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
            (GUID*)which_fn, sizeof(*which_fn),
            &ptr, sizeof(ptr),
            &bytes, NULL, NULL);

        return ptr;
    }
};