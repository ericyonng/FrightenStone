/**
* @file TcpServer.cpp
* @author Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/
#include "pch.h"
#include <TcpPort/Server/Impl/TcpServer.h>
#include <TcpPort/Server/Impl/ClientSocket.h>

#pragma region windows网络支持
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
    // 1.创建套接字
    if(_socket != MYINVALID_SOCKET)
        return StatusDefs::Repeat;

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_socket == MYINVALID_SOCKET)
    {
        printf("错误，建立Socket失败...\n");
        return StatusDefs::Socket_CreateFailure;
    }

    // 设置阻塞模式
    if(blockSocket)
        fs::SocketUtil::SetBlock(_socket);
    else
        fs::SocketUtil::SetNoBlock(_socket);

    printf("建立Socket=<%llu>成功...\n", _socket);

    return StatusDefs::Success;
}

Int32 TcpServer::Bind(const char* ip, unsigned short port) const
{
    // 1. bind 绑定用于接受客户端连接的网络端口
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

    // 3.绑定
    auto ret = bind(_socket, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
    if(SOCKET_ERROR == ret)
    {
        printf("错误,绑定网络端口<%d>失败...\n", port);
    }
    else {
        printf("绑定网络端口<%d>成功...\n", port);
    }

    return ret;
}

Int32 TcpServer::Listen(Int32 backLog) const
{
    // 3 listen 监听网络端口
    auto ret = listen(_socket, backLog);
    if(SOCKET_ERROR == ret)
    {
        printf("socket=<%llu>错误,监听网络端口失败...\n", _socket);
    }
    else {
        printf("socket=<%llu>监听网络端口成功...\n", _socket);
    }

    return ret;
}

MYSOCKET TcpServer::Accept()
{
    // 1. accept 等待接受客户端连接
    sockaddr_in clientAddr = {0};
    int addrLen = sizeof(sockaddr_in);
    MYSOCKET socket = MYINVALID_SOCKET;

    // 2.accept监听链接
#ifdef _WIN32
    socket = accept(_socket, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
#else
    socket = accept(_socket, reinterpret_cast<sockaddr *>(&clientAddr), (socklen_t *)&addrLen);
#endif

    if(MYINVALID_SOCKET == socket)
    {
        printf("socket=<%d>错误,接受到无效客户端SOCKET...\n", static_cast<Int32>(_socket));
    }
    else
    {
        CreatePlayerNty userJoin;
        SendToAll(&userJoin);
        _clients.push_back(new ClientSocket(socket));
        printf("socket=<%llu>新客户端加入：socket = %llu,IP = %s \n", _socket, socket, inet_ntoa(clientAddr.sin_addr));
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
    // 8 关闭套节字closesocket
    closesocket(_socket);
    //------------
#else
    for(int n = (int)_clients.size() - 1; n >= 0; n--)
    {
        delete _clients[n];
    }
    // 8 关闭套节字closesocket
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
    // 1. 接收客户端数据
    auto nLen = recv(client->GetSocket(), _recv, SOCKET_CACHE_SIZE, 0);
    //printf("nLen=%d\n", nLen);
    if(nLen <= 0)
    {
        printf("客户端<Socket=%llu>已退出，任务结束。\n", client->GetSocket());
        return -1;
    }

    // 2.将收取到的数据拷贝到消息缓冲区
    memcpy(client->GetMsgBuf() + client->GetLastPos(), _recv, nLen);
    // 3.消息缓冲区的数据尾部位置后移
    client->SetLastPos(client->GetLastPos() + nLen);

    // 4.判断消息缓冲区的数据长度大于消息头DataHeader长度
    while(client->GetLastPos() >= sizeof(PacketHeader))
    {
        // 这时就可以知道当前消息的长度
        auto *header = reinterpret_cast<PacketHeader *>(client->GetMsgBuf());
        // 判断消息缓冲区的数据长度大于消息长度
        if(client->GetLastPos() >= header->_packetLength)
        {
            // 消息缓冲区剩余未处理数据的长度
            Int32 nSize = client->GetLastPos() - header->_packetLength;
            // 处理网络消息
            OnNetMsg(client->GetSocket(), header);
            // 将消息缓冲区剩余未处理数据前移
            memcpy(client->GetMsgBuf(), client->GetMsgBuf() + header->_packetLength, nSize);
            // 消息缓冲区的数据尾部位置前移
            client->SetLastPos(nSize);
        }
        else {
            // 消息缓冲区剩余数据不够一条完整消息
            break;
        }
    }
    return 0;
}

Int32 TcpServer::SendData(MYSOCKET socket, PacketHeader *header)
{
    if(UNLIKELY(!IsRun() || !header))
        return SOCKET_ERROR;

    return send(socket, reinterpret_cast<const char *>(header), header->_packetLength, 0);
}

void TcpServer::SendToAll(PacketHeader* header)
{
    for(Int32 n = static_cast<Int32>(_clients.size() - 1); n >= 0; --n)
        SendData(_clients[n]->GetSocket(), header);
}

bool TcpServer::OnRun()
{
    if(UNLIKELY(!IsRun()))
        return false;

    //伯克利套接字 BSD socket
    fd_set fdRead;//描述符（socket） 集合
    fd_set fdWrite;
    fd_set fdExp;
    //清理集合
    FD_ZERO(&fdRead);
    FD_ZERO(&fdWrite);
    FD_ZERO(&fdExp);

    //将描述符（socket）加入集合
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
    ///nfds 是一个整数值 是指fd_set集合中所有描述符(socket)的范围，而不是数量
    ///既是所有文件描述符最大值+1 在Windows中这个参数可以写0
    timeval t = {1, 0};
    auto ret = select(static_cast<Int32>(maxSock + 1), &fdRead, &fdWrite, &fdExp, &t); //
    //printf("select ret=%d count=%d\n", ret, _nCount++);
    if(ret < 0)
    {
        printf("select任务结束。\n");
        CloseClients();
        return false;
    }
    //判断描述符（socket）是否在集合中
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

void TcpServer::OnNetMsg(MYSOCKET socket, PacketHeader *header)
{
    switch(header->_cmd)
    {
        case ProtocolCmd::LoginReq:
        {

            LoginReq *login = static_cast<LoginReq *>(header);
            printf("收到客户端<Socket=%llu>请求：LoginReq,数据长度：%d,userName=%s PassWord=%s\n", socket, login->_packetLength, login->_userName, login->_pwd);

            // 数据
            LoginNty nty;
            memcpy(nty._userName, login->_userName, sizeof(login->_userName));
            memcpy(nty._pwd, login->_pwd, sizeof(login->_pwd));
            SendData(socket, &nty);

            // res
            LoginRes res;
            memcpy(res._userName, login->_userName, sizeof(login->_userName));
            SendData(socket, &res);
        }
        break;
        default:
        {
            printf("<socket=%llu>收到未定义消息,数据长度：%d\n", socket, header->_packetLength);
            //DataHeader ret;
            //SendData(cSock, &ret);
        }
        break;
    }
}
