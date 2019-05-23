/**
* @file ClientSocket.h
* @author Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

#include <base/exportbase.h>

class ClientSocket
{
public:
    explicit ClientSocket(MYSOCKET socket = MYINVALID_SOCKET, bool blockSocket = false);
    virtual ~ClientSocket();

    MYSOCKET GetSocket() const;
    char *GetMsgBuf();
    Int32 GetLastPos() const;
    void SetLastPos(Int32 pos);

private:
    #pragma region 
    // socket fd_set  file desc set
    MYSOCKET _socket = MYINVALID_SOCKET;
    char _msgBuf[SOCKET_CACHE_SIZE * 10] = {0};
    Int32 _lastPos = 0;
    #pragma region
};

#pragma region Inline
inline MYSOCKET ClientSocket::GetSocket() const
{
    return _socket;
}

inline char *ClientSocket::GetMsgBuf()
{
    return _msgBuf;
}

inline Int32 ClientSocket::GetLastPos() const
{
    return _lastPos;
}

inline void ClientSocket::SetLastPos(Int32 pos)
{
    _lastPos = pos;
}

#pragma endregion
