#include "stdafx.h"
#include <TcpPort/Client/Impl/TcpClient.h>

#pragma region windows网络支持
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
// #include<windows.h>
 #include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif // _WIN32
#pragma endregion

TcpClient::TcpClient()
{
    
}

TcpClient::~TcpClient()
{

}

#pragma region 套接字操作
Int32 TcpClient::InitSocket(bool blockSocket)
{
    // 1.创建套接字
    if(_sock != MYINVALID_SOCKET)
        return StatusDefs::Repeat;

    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sock == MYINVALID_SOCKET)
    {
        printf("错误，建立Socket失败...\n");
        return StatusDefs::Socket_CreateFailure;
    }

    if(blockSocket)
        fs::SocketUtil::SetBlock(_sock);
    else
        fs::SocketUtil::SetNoBlock(_sock);

    printf("建立Socket=<%llu>成功...\n", _sock);
    
    return StatusDefs::Success;
}

Int32 TcpClient::Connect(const char *ip, UInt16 port) const
{
    // 1.判断socket是否初始化
    if(_sock == MYINVALID_SOCKET)
        return StatusDefs::Socket_NotInit;

    // 2.连接服务器
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
#ifdef _WIN32
    sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
    sin.sin_addr.s_addr = inet_addr(ip);
#endif

    printf("<socket=%llu>正在连接服务器<%s:%d>...\n", _sock, ip, port);
    int ret = connect(_sock, reinterpret_cast<sockaddr *>(&sin), sizeof(sockaddr_in));
    if(SOCKET_ERROR == ret)
    {
        printf("<socket=%llu>错误，连接服务器<%s:%d>失败...\n", _sock, ip, port);
    }
    else {
        printf("<socket=%llu>连接服务器<%s:%d>成功...\n", _sock, ip, port);
    }

    return ret;
}

Int32 TcpClient::RecvData()
{
    // 1 接收数据
    auto nLen = recv(_sock, _recv, SOCKET_CACHE_SIZE, 0);
    printf("nLen=%d\n", nLen);

    if(nLen <= 0)
    {
        printf("<socket=%llu>与服务器断开连接，任务结束。\n", _sock);
        return StatusDefs::Error;
    }

    // 2.将收取到的数据拷贝到消息缓冲区
    memcpy(_msgBuf + _lastPos, _recv, nLen);

    // 3.消息缓冲区的数据尾部位置后移
    _lastPos += nLen;

    // 4.判断消息缓冲区的数据长度大于消息头DataHeader长度
    while(_lastPos >= sizeof(PacketHeader))
    {
        // 5.这时就可以知道当前消息的长度
        auto *header = reinterpret_cast<PacketHeader *>(_msgBuf);

        // 7.判断消息缓冲区的数据长度大于消息长度
        if(_lastPos >= header->_packetLength)    // 需要判断消息头是否垃圾包TODO
        {
            // 消息缓冲区剩余未处理数据的长度
            int nSize = _lastPos - header->_packetLength;

            // 处理网络消息
            OnNetMsg(header);

            // 将消息缓冲区剩余未处理数据前移
            memcpy(_msgBuf, _msgBuf + header->_packetLength, nSize);

            // 消息缓冲区的数据尾部位置前移
            _lastPos = nSize;
        }
        else {
            // 消息缓冲区剩余数据不够一条完整消息
            break;
        }
    }

    return StatusDefs::Success;
}

Int32 TcpClient::SendData(PacketHeader* header) const
{
    if(IsRun() && header)
    {
        return send(_sock, reinterpret_cast<char *>(header), header->_packetLength, 0);
    }

    return StatusDefs::Error;
}

void TcpClient::Close()
{
    if(_sock != MYINVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(_sock);
#else
        close(_sock);
#endif
        _sock = MYINVALID_SOCKET;
    }
}
#pragma endregion

#pragma region 消息处理
bool TcpClient::OnRun()
{
    if(IsRun())
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(_sock, &fdReads);
        timeval t = {0, 0};
        auto ret = select(static_cast<Int32>(_sock + 1), &fdReads, NULL, NULL, &t);
        // printf("select ret=%d count=%d\n", ret, _nCount++);
        if(ret < 0)
        {
            printf("<socket=%llu>select任务结束1\n", _sock);
            Close();
            return false;
        }
        if(FD_ISSET(_sock, &fdReads))
        {
            FD_CLR(_sock, &fdReads);

            if(-1 == RecvData())
            {
                printf("<socket=%llu>select任务结束2\n", _sock);
                Close();
                return false;
            }
        }
        return true;
    }
    return false;
}

bool TcpClient::IsRun() const
{
    return _sock != MYINVALID_SOCKET;
}

void TcpClient::OnNetMsg(PacketHeader *header)
{
    switch(header->_cmd)
    {
        case ProtocolCmd::LoginRes:
        {

            auto *res = static_cast<LoginRes *>(header);
            printf("<socket=%llu>收到服务端消息：LoginRes,数据长度：%d\n userName[%s] status[%d]"
                   , _sock, res->_packetLength, res->_userName, res->_status);
        }
        break;
        case ProtocolCmd::LoginNty:
        {
            auto *loginNty = static_cast<LoginNty *>(header);
            printf("<socket=%llu>收到服务端消息：LoginNty,数据长度：%d name[%s] pwd[%s]\n", _sock, loginNty->_packetLength, loginNty->_userName, loginNty->_pwd);
        }
        break;
        default:
        {
            printf("<socket=%llu>收到未定义消息[%d],数据长度：%d\n", _sock, header->_cmd, header->_packetLength);
        }
    }
}
#pragma endregion
