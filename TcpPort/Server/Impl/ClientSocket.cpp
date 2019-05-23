/**
* @file ClientSocket.cpp
* @author Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/
#include "pch.h"
#include "TcpPort/Server/Impl/ClientSocket.h"

ClientSocket::ClientSocket(MYSOCKET socket /*= MYINVALID_SOCKET*/, bool blockSocket)
    :_socket(socket)
{
    if(blockSocket)
        fs::SocketUtil::SetBlock(_socket);
    else
        fs::SocketUtil::SetNoBlock(_socket);
}

ClientSocket::~ClientSocket()
{
    if(_socket != MYINVALID_SOCKET)
    {
        #ifdef _WIN32
        closesocket(_socket);
        #else
        close(_socket);
        #endif
        _socket = MYINVALID_SOCKET;
    }
}
