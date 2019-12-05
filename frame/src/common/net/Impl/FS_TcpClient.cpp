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
    Fs_SafeFree(_sessionBufferAlloctor);
}

Int32 FS_TcpClient::Init()
{
    // ³õÊ¼»¯ÅäÖÃ
    const UInt64 sendBufferSize = g_ClientCfgMgr->GetSendBufferSize();
    const UInt64 recvBufferSize = g_ClientCfgMgr->GetRecvBufferSize();
    const UInt64 bufferSize = std::max<UInt64>(recvBufferSize, sendBufferSize);
    _sessionBufferAlloctor = new MemoryAlloctorWithLimit(bufferSize
                                                         , g_ClientCfgMgr->GetMemPoolBufferCntInit()
                                                         , g_ClientCfgMgr->GetMemPoolAllocMaxBytes());
    _sessionBufferAlloctor->InitMemory();
    return StatusDefs::Success;
}

SOCKET FS_TcpClient::InitSocket(Int32 sendSize /*= SEND_BUFF_SZIE*/, Int32 recvSize /*= RECV_BUFF_SZIE*/)
{
    auto ret = SocketUtil::InitSocketEnv();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_TcpClient>(_LOGFMT_("init socket env fail ret[%d]"), ret);
        return ret;
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

        _session = FS_SessionFactory::Create(++_maxSessionId, sock, NULL, _sessionBufferAlloctor);
        OnInitSocket();
    }
    return sock;
}

Int32 FS_TcpClient::Connect(const Byte8 *ip, UInt16 port)
{
    return StatusDefs::Success;
}

void FS_TcpClient::Close()
{

}


FS_NAMESPACE_END

