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
 * @file  : FS_TcpClient.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/04
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include <FrightenStone/common/net/Impl/FS_TcpClient.h>
#include <FrightenStone/common/net/Impl/IFS_Session.h>
#include <FrightenStone/common/socket/socket.h>
#include <FrightenStone/common/status/status.h>
#include <FrightenStone/common/log/Log.h>
#include <FrightenStone/common/net/Impl/FS_SessionFactory.h>
#include <FrightenStone/common/net/Impl/FS_ClientCfgMgr.h>
#include <FrightenStone/common/net/Defs/FS_IocpBuffer.h>

FS_NAMESPACE_BEGIN

FS_TcpClient::FS_TcpClient()
    :_session(NULL)
    ,_isConnect(false)
    ,_maxSessionId(0)
    ,_sessionBufferAlloctor(NULL)
{

}

FS_TcpClient::~FS_TcpClient()
{
    _isConnect = false;
    Fs_SafeFree(_session);
    Fs_SafeFree(_sessionBufferAlloctor);
}

Int32 FS_TcpClient::Init()
{
    // 初始化配置
    const UInt64 sendBufferSize = g_ClientCfgMgr->GetSendBufferSize();
    const UInt64 recvBufferSize = g_ClientCfgMgr->GetRecvBufferSize();
    const UInt64 bufferSize = std::max<UInt64>(recvBufferSize, sendBufferSize);
    _sessionBufferAlloctor = new MemoryAlloctorWithLimit(bufferSize
                                                         , g_ClientCfgMgr->GetMemPoolBufferCntInit()
                                                         , g_ClientCfgMgr->GetMemPoolAllocMaxBytes());
    _sessionBufferAlloctor->InitMemory();
    return StatusDefs::Success;
}

SOCKET FS_TcpClient::InitSocket()
{
    auto ret = SocketUtil::InitSocketEnv();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_TcpClient>(_LOGFMT_("init socket env fail ret[%d]"), ret);
        return INVALID_SOCKET;
    }

    if(_session)
    {
        g_Log->w<FS_TcpClient>(_LOGFMT_("an old session will close sessionId[%llu] socket[%llu]")
                               , _session->GetSessionId(), _session->GetSocket());
        Close();
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == sock)
    {
        g_Log->e<FS_TcpClient>(_LOGFMT_("create socket failed"));
    }
    else {
        SocketUtil::MakeReUseAddr(sock);

        // _session = FS_SessionFactory::Create(++_maxSessionId, sock, NULL, _sessionBufferAlloctor, g_ClientCfgMgr->GetHeartbeatDeadTimeInterval());
        OnInitSocket();
    }
    return sock;
}

Int32 FS_TcpClient::Connect(const Byte8 *ip, UInt16 port)
{
    if(!_session)
    {
        if(INVALID_SOCKET == InitSocket())
        {
            g_Log->e<FS_TcpClient>(_LOGFMT_("InitSocket fail"));
            return StatusDefs::FS_TcpClient_ConnectFail;
        }
    }

    // 2 连接服务器 connect
    sockaddr_in  sin = {};
    if(!SocketUtil::FillTcpAddrInfo(ip, port, AF_INET, sin))
    {
        g_Log->e<FS_TcpClient>(_LOGFMT_("FillTcpAddrInfo fail ip[%s] port[%hu]"), ip, port);
        return StatusDefs::FS_TcpClient_ConnectFail;
    }

    Int32 ret = connect(_session->GetSocket(), (sockaddr*)&sin, sizeof(sockaddr_in));
    if(SOCKET_ERROR == ret)
    {
        g_Log->e<FS_TcpClient>(_LOGFMT_("<socket=%llu> connect <%s:%hu> failed...")
                               , _session->GetSocket(), ip, port);
    }
    else {
        _isConnect = true;
        OnConnect();
        g_Log->i<FS_TcpClient>(_LOGFMT_("sessionId[%llu] socket[%llu] target connect<%s:%hu> suc ret[%d]")
                               , _session->GetSessionId(), _session->GetSocket(), ip, port, ret);
        //CELLLog_Info("<socket=%d> connect <%s:%d> success...", (int)_pClient->sockfd(), ip, port);
    }

    return StatusDefs::Success;
}

void FS_TcpClient::Close()
{
    Fs_SafeFree(_session);
    _isConnect = false;
}

void FS_TcpClient::DoMsg()
{
    //循环 判断是否有消息需要处理
#ifdef _WIN32
    auto recvBuffer = _session->GetRecvBuffer()->CastToBuffer<FS_IocpBuffer>();
    while(recvBuffer->HasMsg())
    {
        // 处理网络消息
        auto frontMsg = recvBuffer->CastToData<NetMsg_DataHeader>();
        OnNetMsg(frontMsg);
        // 移除消息队列（缓冲区）最前的一条数据       
        recvBuffer->PopFront(frontMsg->_packetLength);
    }
#else
    // TODO:linux
#endif
}

Int32 FS_TcpClient::SendData(NetMsg_DataHeader *header)
{
    if(IsRun() && _session->PushMsgToSend(header))
        return static_cast<Int32>(header->_packetLength);

    return StatusDefs::Error;
}

Int32 FS_TcpClient::SendData(const Byte8 *data, UInt64 len)
{
    if(IsRun() && _session->PushMsgToSend(data, len))
        return static_cast<Int32>(len);

    return StatusDefs::Error;
}

FS_NAMESPACE_END

