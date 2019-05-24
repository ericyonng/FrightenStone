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
 * @file  : TcpClient.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include <TcpPort/Client/Impl/TcpClient.h>

#pragma region 
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

#pragma region 
Int32 TcpClient::InitSocket(bool blockSocket)
{
    if(_sock != MYINVALID_SOCKET)
        return StatusDefs::Repeat;

    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sock == MYINVALID_SOCKET)
    {
        printf("¥ÌŒÛ£¨Ω®¡¢Socket ß∞‹...\n");
        return StatusDefs::Socket_CreateFailure;
    }

    if(blockSocket)
        fs::SocketUtil::SetBlock(_sock);
    else
        fs::SocketUtil::SetNoBlock(_sock);

    printf("create Socket=<%llu>suc...\n", _sock);
    
    return StatusDefs::Success;
}

Int32 TcpClient::Connect(const char *ip, UInt16 port) const
{
    if(_sock == MYINVALID_SOCKET)
        return StatusDefs::Socket_NotInit;

    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
#ifdef _WIN32
    sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
    sin.sin_addr.s_addr = inet_addr(ip);
#endif

    printf("<socket=%llu>connecting<%s:%d>...\n", _sock, ip, port);
    int ret = connect(_sock, reinterpret_cast<sockaddr *>(&sin), sizeof(sockaddr_in));
    if(SOCKET_ERROR == ret)
    {
        printf("<socket=%llu>error£¨linking<%s:%d>fail...\n", _sock, ip, port);
    }
    else {
        printf("<socket=%llu>link server<%s:%d>suc...\n", _sock, ip, port);
    }

    return ret;
}

Int32 TcpClient::RecvData()
{
    auto nLen = recv(_sock, _recv, SOCKET_CACHE_SIZE, 0);
    printf("nLen=%d\n", nLen);

    if(nLen <= 0)
    {
        printf("<socket=%llu>end°£\n", _sock);
        return StatusDefs::Error;
    }

    memcpy(_msgBuf + _lastPos, _recv, nLen);

    _lastPos += nLen;

    while(_lastPos >= sizeof(PacketHeader))
    {
        auto *header = reinterpret_cast<PacketHeader *>(_msgBuf);

        if(_lastPos >= header->_packetLength)    // TODO
        {
            int nSize = _lastPos - header->_packetLength;

            OnNetMsg(header);

            memcpy(_msgBuf, _msgBuf + header->_packetLength, nSize);

            _lastPos = nSize;
        }
        else {
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

#pragma region 
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
            printf("<socket=%llu>select end1\n", _sock);
            Close();
            return false;
        }
        if(FD_ISSET(_sock, &fdReads))
        {
            FD_CLR(_sock, &fdReads);

            if(-1 == RecvData())
            {
                printf("<socket=%llu>select end2\n", _sock);
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
            printf("<socket=%llu>recv£∫LoginRes,len£∫%d\n userName[%s] status[%d]"
                   , _sock, res->_packetLength, res->_userName, res->_status);
        }
        break;
        case ProtocolCmd::LoginNty:
        {
            auto *loginNty = static_cast<LoginNty *>(header);
            printf("<socket=%llu>recv£∫LoginNty,len£∫%d name[%s] pwd[%s]\n", _sock, loginNty->_packetLength, loginNty->_userName, loginNty->_pwd);
        }
        break;
        default:
        {
            printf("<socket=%llu>recv unkown message[%d], len£∫%d\n", _sock, header->_cmd, header->_packetLength);
        }
    }
}
#pragma endregion
