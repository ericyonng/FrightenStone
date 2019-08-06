/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : TcpServer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "pch.h"
#include <TcpPort/Server/Impl/TcpServer.h>
#include <TcpPort/Server/Impl/ClientSocket.h>

#pragma region 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
// #include<windows.h>
 #include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif // _WIN32
#pragma endregion

TcpServer::TcpServer()
{

}

TcpServer::~TcpServer()
{
    Close();
}

Int32 TcpServer::InitSocket(bool blockSocket)
{
    if(_socket != MYINVALID_SOCKET)
        return StatusDefs::Repeat;

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_socket == MYINVALID_SOCKET)
    {
        printf("错误，建立Socket失败...\n");
        return StatusDefs::Socket_CreateFailure;
    }

    if(blockSocket)
        fs::SocketUtil::SetBlock(_socket);
    else
        fs::SocketUtil::SetNoBlock(_socket);

    printf("建立Socket=<%llu>成功...\n", _socket);

    return StatusDefs::Success;
}

Int32 TcpServer::Bind(const char* ip, unsigned short port) const
{
    sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);    //host to net unsigned short

    // 2. ip
#ifdef _WIN32
    if(ip) {
        sin.sin_addr.S_un.S_addr = inet_addr(ip);
    }
    else {
        sin.sin_addr.S_un.S_addr = INADDR_ANY;
    }
#else
    if(ip) {
        sin.sin_addr.s_addr = inet_addr(ip);
    }
    else {
        sin.sin_addr.s_addr = INADDR_ANY;
    }
#endif

    auto ret = bind(_socket, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
    if(SOCKET_ERROR == ret)
    {
        printf("port has be binded<%d> fail...\n", port);
    }
    else {
        printf("bind net port<%d>suc...\n", port);
    }

    return ret;
}

Int32 TcpServer::Listen(Int32 backLog) const
{
    auto ret = listen(_socket, backLog);
    if(SOCKET_ERROR == ret)
    {
        printf("socket=<%llu>error,listen fail...\n", _socket);
    }
    else {
        printf("socket=<%llu>listen suc...\n", _socket);
    }

    return ret;
}

MYSOCKET TcpServer::Accept()
{
    sockaddr_in clientAddr = {0};
    int addrLen = sizeof(sockaddr_in);
    MYSOCKET socket = MYINVALID_SOCKET;

#ifdef _WIN32
    socket = accept(_socket, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
#else
    socket = accept(_socket, reinterpret_cast<sockaddr *>(&clientAddr), (socklen_t *)&addrLen);
#endif

    if(MYINVALID_SOCKET == socket)
    {
        printf("socket=<%d>error,invalid SOCKET...\n", static_cast<Int32>(_socket));
    }
    else
    {
        fs::CreatePlayerNty userJoin;
        SendToAll(&userJoin);
        _clients.push_back(new ClientSocket(socket));
        printf("socket=<%llu>new client join：socket = %llu,IP = %s \n", _socket, socket, inet_ntoa(clientAddr.sin_addr));
    }

    return socket;
}

void TcpServer::Close()
{
    if(_socket == MYINVALID_SOCKET)
        return;

#ifdef _WIN32
    for(Int32 n = static_cast<Int32>(_clients.size() - 1); n >= 0; --n)
    {
        ClientSocket *client = _clients[n];
        delete client;
    }
    closesocket(_socket);
    //------------
#else
    for(int n = (int)_clients.size() - 1; n >= 0; n--)
    {
        delete _clients[n];
    }
    close(_socket);
#endif
    _socket = MYINVALID_SOCKET;
    _clients.clear();
}

void TcpServer::CloseClients()
{
#ifdef _WIN32
    for(Int32 n = static_cast<Int32>(_clients.size() - 1); n >= 0; --n)
    {
        ClientSocket *client = _clients[n];
        delete client;
    }
    //------------
#else
    for(int n = (int)_clients.size() - 1; n >= 0; n--)
    {
        delete _clients[n];
    }
#endif
    _clients.clear();
}

Int32 TcpServer::RecvData(ClientSocket *client)
{
    auto nLen = recv(client->GetSocket(), _recv, SOCKET_CACHE_SIZE, 0);
    //printf("nLen=%d\n", nLen);
    if(nLen <= 0)
    {
        printf("client<Socket=%llu>exit，task end。\n", client->GetSocket());
        return -1;
    }

    memcpy(client->GetMsgBuf() + client->GetLastPos(), _recv, nLen);
    client->SetLastPos(client->GetLastPos() + nLen);

    while(client->GetLastPos() >= sizeof(fs::NetMsg_DataHeader))
    {
        auto *header = reinterpret_cast<fs::NetMsg_DataHeader *>(client->GetMsgBuf());
        if(client->GetLastPos() >= header->_packetLength)
        {
            Int32 nSize = client->GetLastPos() - header->_packetLength;
            OnNetMsg(client->GetSocket(), header);
            memcpy(client->GetMsgBuf(), client->GetMsgBuf() + header->_packetLength, nSize);
            client->SetLastPos(nSize);
        }
        else {
            break;
        }
    }
    return 0;
}

Int32 TcpServer::SendData(MYSOCKET socket, fs::NetMsg_DataHeader *header)
{
    if(UNLIKELY(!IsRun() || !header))
        return SOCKET_ERROR;

    return send(socket, reinterpret_cast<const char *>(header), header->_packetLength, 0);
}

void TcpServer::SendToAll(fs::NetMsg_DataHeader* header)
{
    for(Int32 n = static_cast<Int32>(_clients.size() - 1); n >= 0; --n)
        SendData(_clients[n]->GetSocket(), header);
}

bool TcpServer::OnRun()
{
    if(UNLIKELY(!IsRun()))
        return false;

    fd_set fdRead;//
    fd_set fdWrite;
    fd_set fdExp;
    FD_ZERO(&fdRead);
    FD_ZERO(&fdWrite);
    FD_ZERO(&fdExp);

    FD_SET(_socket, &fdRead);
    FD_SET(_socket, &fdWrite);
    FD_SET(_socket, &fdExp);
    MYSOCKET maxSock = _socket;

    for(Int32 n = static_cast<Int32>( _clients.size() - 1); n >= 0; n--)
    {
        FD_SET(_clients[n]->GetSocket(), &fdRead);
        if(maxSock < _clients[n]->GetSocket())
        {
            maxSock = _clients[n]->GetSocket();
        }
    }
    timeval t = {1, 0};
    auto ret = select(static_cast<Int32>(maxSock + 1), &fdRead, &fdWrite, &fdExp, &t); //
    //printf("select ret=%d count=%d\n", ret, _nCount++);
    if(ret < 0)
    {
        printf("select end。\n");
        CloseClients();
        return false;
    }
    if(FD_ISSET(_socket, &fdRead))
    {
        FD_CLR(_socket, &fdRead);
        Accept();
    }
    for(Int32 n = static_cast<Int32>(_clients.size() - 1); n >= 0; n--)
    {
        if(FD_ISSET(_clients[n]->GetSocket(), &fdRead))
        {
            if(-1 == RecvData(_clients[n]))
            {
                auto iter = _clients.begin() + n;//std::vector<SOCKET>::iterator
                if(iter != _clients.end())
                {
                    delete _clients[n];
                    _clients.erase(iter);
                }
            }
        }
    }

    return true;
}

bool TcpServer::IsRun() const
{
    return _socket != MYINVALID_SOCKET;
}

void TcpServer::OnNetMsg(MYSOCKET socket, fs::NetMsg_DataHeader *header)
{
    switch(header->_cmd)
    {
        case fs::ProtocolCmd::LoginReq:
        {

            fs::LoginReq *login = static_cast<fs::LoginReq *>(header);
            printf("recv<Socket=%llu>request：LoginReq, len：%d,userName=%s PassWord=%s\n", socket, login->_packetLength, login->_userName, login->_pwd);

            fs::LoginNty nty;
            memcpy(nty._userName, login->_userName, sizeof(login->_userName));
            memcpy(nty._pwd, login->_pwd, sizeof(login->_pwd));
            SendData(socket, &nty);

            // res
            fs::LoginRes res;
            memcpy(res._userName, login->_userName, sizeof(login->_userName));
            SendData(socket, &res);
        }
        break;
        default:
        {
            printf("<socket=%llu>recv unknown message, len：%d\n", socket, header->_packetLength);
            //DataHeader ret;
            //SendData(cSock, &ret);
        }
        break;
    }
}
