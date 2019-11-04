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
 * @file  : FS_Addr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_Addr.h"
#include "base/common/net/Impl/IFS_Session.h"

#include "base/common/socket/socket.h"
#include "base/common/status/status.h"
#include "base/common/crashhandle/CrashHandle.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_Addr, __DEF_OBJ_POOL_OBJ_NUM__, __DEF_OBJ_POOL_MAX_ALLOW_BYTES__);

FS_Addr::FS_Addr(IFS_Session *session, const sockaddr_in *addrInfo)
    :_addrInfo{}
    ,_session(session)
    ,_ip{0}
    ,_port(0)
    ,_ipPtr(_ip)
{
    if(!addrInfo)
    {
        const UInt64 sessionSock = _session->GetSocket();
        const UInt64 sessionId = _session->GetSessionId();
        g_Log->w<FS_Addr>(_LOGFMT_("addrInfo input nill value sessionId[%llu], socketId[%llu]")
                          , _session->GetSessionId(), sessionSock);

        Int32 lastError = 0;
        Int32 st = SocketUtil::GetPeerAddr(sessionSock, sizeof(_ip), _ipPtr, _port, lastError);
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_Addr>(_LOGFMT_("SocketUtil::GetPeerAddr fail st[%d] sessionId[%llu], socketId[%llu] lastError[%d]\n stack trace back:%s")
                              , st, sessionId, sessionSock, lastError, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
            return;
        }

        // todo 若要支持ipv6请修改为ipv6的family
        if(!SocketUtil::FillTcpAddrInfo(_ipPtr, _port, AF_INET, _addrInfo))
        {
            g_Log->e<FS_Addr>(_LOGFMT_("SocketUtil::FillTcpAddrInfo fail, sessionId[%llu], socket[%llu]\n stack trace back:%s")
                              , sessionId, sessionSock, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
            return;
        }

        _strIp = _ip;
    }
    else
    {
        _addrInfo = *addrInfo;
        UpdateAddrInfo();
    }
}

bool FS_Addr::UpdateAddrInfo()
{
    if(!SocketUtil::GetAddrInfoFromNetInfo(_addrInfo, sizeof(_ip), _ipPtr, _port))
    {
        g_Log->e<FS_Addr>(_LOGFMT_("GetAddrInfoFromNetInfo fail sessionId[%llu] socket[%llu]")
                          , _session->GetSessionId(), _session->GetSocket());
        return false;
    }

    _strIp = _ip;
    return true;
}

FS_NAMESPACE_END
