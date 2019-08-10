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
 * @file  : FS_TcpServer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/03
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_TcpServer.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/socket/socket.h"
#include "base/common/log/Log.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/net/Impl/FS_Client.h"
#include "base/common/net/Impl/FS_Server.h"
#include "base/common/assist/utils/Impl/STLUtil.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_IMPL(FS_TcpServer, _objPoolHelper, __DEF_OBJ_POOL_OBJ_NUM__)

FS_TcpServer::FS_TcpServer()
    :_threadPool(new FS_ThreadPool(0, 1))
    ,_sock(INVALID_SOCKET)
    ,_sendBuffSize(0)
    ,_recvBuffSize(0)
    ,_maxClient(0)
    ,_recvCount{0}
    ,_recvMsgCount{0}
    ,_clientAcceptCnt{0}
    ,_clientJoinedCnt{0}
    ,_clientMaxId{0}
{
    // TODO:读取配置
    _sendBuffSize = SEND_BUFF_SZIE;
    // _sendBuffSize = CELLConfig::Instance().getInt("nSendBuffSize", SEND_BUFF_SZIE);
    _recvBuffSize = RECV_BUFF_SZIE;
    // _nRecvBuffSize = CELLConfig::Instance().getInt("nRecvBuffSize", RECV_BUFF_SZIE);
    // _nMaxClient = CELLConfig::Instance().getInt("nMaxClient", FD_SETSIZE);
    _maxClient = FD_SETSIZE;
}

FS_TcpServer::~FS_TcpServer()
{
    Close();
    FS_Release(_threadPool);
}

void FS_TcpServer::Release()
{
    delete this;
}

#pragma region tcp normal operate
SOCKET FS_TcpServer::InitSocket()
{
    auto st = SocketUtil::InitSocketEnv();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_TcpServer>(_LOGFMT_("Init socket environment error st[%d]"), st);
        return INVALID_SOCKET;
    }

    if(INVALID_SOCKET != _sock)
    {
        g_Log->w<FS_TcpServer>(_LOGFMT_("initSocket close old socket<%d>..."), static_cast<Int32>(_sock));
        // CELLLog_Warring("initSocket close old socket<%d>...", (int)_sock);
        Close();
    }

    // ipv4 的流数据
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == _sock)
    {
        g_Log->e<FS_TcpServer>(_LOGFMT_("create socket failed..."));
        return _sock;
        // CELLLog_PError("create socket failed...");
    }

    // 套接字重用
    st = SocketUtil::MakeReUseAddr(_sock);
    if(st != StatusDefs::Success)
    {
        g_Log->w<FS_TcpServer>(_LOGFMT_("make reuse of socket failed sock[%d]"), static_cast<Int32>(_sock));
    }

    g_Log->net("create socket<%d> success...", static_cast<Int32>(_sock));
    g_Log->sys(_LOGFMT_("create socket<%d> success..."), static_cast<Int32>(_sock));
    return _sock;
}

Int32 FS_TcpServer::Bind(const Byte8 *ip, UInt16 port)
{
    // 2 bind 绑定用于接受客户端连接的网络端口
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    // host to net unsigned short
    sin.sin_port = htons(port);

#ifdef _WIN32
    if(ip) {
        inet_pton(sin.sin_family, ip, &(sin.sin_addr));// 比较新的函数对比inet_addr
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

    int ret = bind(_sock, (sockaddr *)&sin, sizeof(sin));
    if(SOCKET_ERROR == ret)
    {
        g_Log->e<FS_TcpServer>(_LOGFMT_("bind port<%hu> failed..."), port);
        return StatusDefs::Socket_Error;
    }

    g_Log->net("bind port<%hu> success...", port);
    g_Log->sys(_LOGFMT_("bind port<%hu> success..."), port);
    return StatusDefs::Success;
}

Int32 FS_TcpServer::Listen(Int32 unconnectQueueLen)
{
    // 3 listen 监听网络端口
    int ret = listen(_sock, unconnectQueueLen);
    if(SOCKET_ERROR == ret)
    {
        g_Log->e<FS_TcpServer>(_LOGFMT_("listen socket<%d> failed..."), static_cast<Int32>(_sock));
        return StatusDefs::Socket_Error;
    }

    g_Log->net("listen port socket<%d> success...", static_cast<Int32>(_sock));
    g_Log->sys(_LOGFMT_("listen port socket<%d> success..."), static_cast<Int32>(_sock));
    return StatusDefs::Success;
}

SOCKET FS_TcpServer::Accept()
{
    // 4 accept 等待接受客户端连接
    sockaddr_in clientAddr = {};
    Int32 addrLen = sizeof(sockaddr_in);
    SOCKET sock = INVALID_SOCKET;

#ifdef _WIN32
    sock = accept(_sock, (sockaddr*)&clientAddr, &addrLen);
#else
    sock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&addrLen);
#endif

    if(INVALID_SOCKET == sock)
    {
        g_Log->e<FS_TcpServer>(_LOGFMT_("accept INVALID_SOCKET..."));
        return INVALID_SOCKET;
    }

    if(_clientAcceptCnt >= _maxClient)
    {// 是否超过最大连接数
        // 获取IP地址 inet_ntoa(clientAddr.sin_addr)
        SocketUtil::DestroySocket(sock);
        g_Log->w<FS_TcpServer>(_LOGFMT_("Accept to maxClient[%d]"), _maxClient);
        return INVALID_SOCKET;
    }

    ++_clientAcceptCnt;
    SocketUtil::MakeReUseAddr(sock);
    // 将新客户端分配给客户数量最少的cellServer
    _locker.Lock();
    Int64 clientId = ++_clientMaxId;
    _locker.Unlock();
    _AddClientToFSServer(new FS_Client(clientId, sock, _sendBuffSize, _recvBuffSize));
    // 获取IP地址 inet_ntoa(clientAddr.sin_addr)

    return sock;
}

void FS_TcpServer::BeforeClose()
{

}

void FS_TcpServer::Close()
{
    g_Log->net("FS_TcpServer.BeforeClose begin");
    g_Log->sys(_LOGFMT_("FS_TcpServer.BeforeClose begin"));

    BeforeClose();

    g_Log->net("FS_TcpServer.Close begin");
    g_Log->sys(_LOGFMT_("FS_TcpServer.Close begin"));

    _threadPool->Clear();
    if(_sock != INVALID_SOCKET)
    {
        STLUtil::DelVectorContainer(_fsServers);

        // 关闭套节字socket
        SocketUtil::DestroySocket(_sock);
        _sock = INVALID_SOCKET;
    }

    g_Log->net("FS_TcpServer.Close end");
    g_Log->sys(_LOGFMT_("FS_TcpServer.Close end"));
}

#pragma endregion

#pragma region net event
void FS_TcpServer::OnNetJoin(FS_Client *client)
{
    ++_clientJoinedCnt;
    g_Log->net("client<%d> join", client->GetSocket());
    g_Log->sys(_LOGFMT_("client<%d> join"), client->GetSocket());
}

void FS_TcpServer::OnNetLeave(FS_Client *client)
{
    --_clientAcceptCnt;
    --_clientJoinedCnt;
    g_Log->net("client<%d> leave", static_cast<Int32>(client->GetSocket()));
    g_Log->sys(_LOGFMT_("client<%d> leave"), static_cast<Int32>(client->GetSocket()));
}

Int32 FS_TcpServer::OnNetMsg(FS_Server *server, FS_Client *client, NetMsg_DataHeader *header)
{
    ++_recvMsgCount;
    return StatusDefs::Success;
}

void FS_TcpServer::OnPrepareNetRecv(FS_Client *client)
{
    ++_recvCount;
}
#pragma endregion

#pragma region misc
void FS_TcpServer::_AddClientToFSServer(FS_Client *client)
{
    // 查找客户数量最少的FS_Server消息处理对象
    auto minServer = _fsServers[0];
    for(auto svr : _fsServers)
    {
        if(minServer->GetClientCount() > svr->GetClientCount())
            minServer = svr;
    }
    minServer->AddClient(client);
}

void FS_TcpServer::_StatisticsMsgPerSecond()
{
    const auto &nowTime = Time::Now();
    const auto slice = nowTime - _msgCountTime;
    if(slice.GetTotalSeconds() >= 1)
    {
        g_Log->net("thread<%d>,timeSlice<%d>,socket<%d>,Accept<%d>,Join<%d>,recv<%d>,msg<%d>"
                   , static_cast<Int32>(_fsServers.size())
                   , slice.GetTotalSeconds()
                   , static_cast<Int32>(_sock)
                   , (Int32)_clientAcceptCnt
                   , (Int32)_clientJoinedCnt
                   , (Int32)_recvCount
                   , (Int32)_recvMsgCount);

        _recvCount = 0;
        _recvMsgCount = 0;
        _msgCountTime.FlushTime();
    }
}
#pragma endregion

FS_NAMESPACE_END
