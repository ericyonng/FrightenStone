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
 * @file  : FS_IocpConnector.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_IocpConnector.h"
#include <FrightenStone/common/net/Defs/FS_ConnectInfo.h>
#include <FrightenStone/common/net/Defs/BriefSessionInfo.h>
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/log/Log.h"
#include <FrightenStone/common/socket/socket.h>

#ifdef _WIN32

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_DEF_IMPL(FS_IocpConnector, __DEF_OBJ_POOL_OBJ_NUM__);

FS_IocpConnector::FS_IocpConnector(IFS_NetEngine *netEngine, UInt32 compId
                                   , Locker &locker
                                   , Int32 &curSessionCnt
                                   , Int32 &maxSessionQuantityLimit
                                   , UInt64 &curMaxSessionId)
    :IFS_Connector(netEngine, compId)
    ,  _locker(locker)
    ,_curSessionCnt(curSessionCnt)
    ,_maxSessionQuantityLimit(maxSessionQuantityLimit)
    ,_curMaxSessionId(curMaxSessionId)
    ,_cfgs(NULL)
{
/*     _CrtMemCheckpoint(&s1);*/
}

FS_IocpConnector::~FS_IocpConnector()
{
    Fs_SafeFree(_cfgs);
}

Int32 FS_IocpConnector::BeforeStart(const NetEngineTotalCfgs &cfgs)
{
    _cfgs = new ConnectorCfgs;
    *_cfgs = cfgs._connectorCfgs;
    return StatusDefs::Success;
}

Int32 FS_IocpConnector::Start()
{
    return StatusDefs::Success;
}

void FS_IocpConnector::AfterStart()
{
    MaskReady(true);
}

void FS_IocpConnector::WillClose()
{
    MaskReady(false);
}

void FS_IocpConnector::BeforeClose()
{
}

void FS_IocpConnector::Close()
{
}

Int32 FS_IocpConnector::Connect(const FS_ConnectInfo &connectInfo)
{
    // 校验目标连接是否已经在pending队列中
    FS_String addrInfo;
    auto ret = _CheckConnect(connectInfo, addrInfo);
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_IocpConnector>(_LOGFMT_("check connect fail ret[%d]"), ret);
        return ret;
    }

    // 校验目标连接是否已经成功连接
    // 1.初始化socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == sock)
    {
        g_Log->e<FS_IocpConnector>(_LOGFMT_("create socket failed"));
        return StatusDefs::FS_IocpConnector_CreateSocketFail;
    }

    // socket重用
    SocketUtil::MakeReUseAddr(sock);

    sockaddr_in  sin = {};
    if(!SocketUtil::FillTcpAddrInfo(connectInfo._ip.c_str(), connectInfo._port, AF_INET, sin))
    {
        g_Log->e<FS_TcpClient>(_LOGFMT_("FillTcpAddrInfo fail ip[%s] port[%hu]"), connectInfo._ip.c_str(), connectInfo._port);
        return StatusDefs::FS_TcpClient_ConnectFail;
    }

    ret = _Connect(sock, sin, connectInfo);
    if(ret == StatusDefs::Success)
    {
        _locker.Lock();
        ++_curMaxSessionId;
        ++_curSessionCnt;
        UInt64 curMaxSessionId = _curMaxSessionId;
        _sucConnectedSessionIds.insert(curMaxSessionId);
        _locker.Unlock();

        sockaddr_in addr_in;
        SocketUtil::FillTcpAddrInfo(connectInfo._ip.c_str(), connectInfo._port, AF_INET, addr_in);

        // 新会话
        auto netEngine = GetEngine();
        BriefSessionInfo newSessionInfo;
        newSessionInfo._addrInfo = &addr_in;
        newSessionInfo._sessionId = curMaxSessionId;
        newSessionInfo._sock = sock;
        newSessionInfo._userDisconnectedRes = DelegatePlusFactory::Create(this, &FS_IocpConnector::_OnUserDisconnected);
        netEngine->_HandleCompEv_WillConnect(&newSessionInfo);
    }
    else
    {
        SocketUtil::DestroySocket(sock);
        g_Log->w<FS_IocpConnector>(_LOGFMT_("connect fail ret[%d]"), ret);
    }

    return ret;
}

void FS_IocpConnector::_OnUserDisconnected(UInt64 sessionId)
{
    _locker.Lock();
    _sucConnectedSessionIds.erase(sessionId);
    --_curSessionCnt;
    _locker.Unlock();
}

Int32 FS_IocpConnector::_CheckConnect(const FS_ConnectInfo &connectInfo, FS_String &addrInfoOut) const
{
    _locker.Lock();
    Int32 ret = StatusDefs::Success;
    do {
        _MakeAddrInfo(connectInfo._ip, connectInfo._port, addrInfoOut);
        // 可以同时链接同一个断开
//         if(_sucAddrInfos.find(addrInfoOut) != _sucAddrInfos.end())
//         {
//             ret = StatusDefs::FS_IocpConnector_ConnectRepeated;
//             break;
//         }

        if(_curSessionCnt >= _maxSessionQuantityLimit)
        {
            ret = StatusDefs::FS_IocpConnector_ConnectOverLimit;
            break;
        }
    } while(0);
    _locker.Unlock();

    return ret;
}


Int32 FS_IocpConnector::_Connect(SOCKET sock, const sockaddr_in &sin, const FS_ConnectInfo &connectInfo)
{
    // 设置非阻塞模式
    auto ret = SocketUtil::SetNoBlock(sock);
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_IocpConnector>(_LOGFMT_("SetNoBlock fail sock[%llu] ret[%d]"), sock, ret);
        return ret;
    }

    // 链接
    bool isTimeOut = false;
    timeval timeout = {};
    auto conRet = ::connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in));

    Int32 detectRet = 0;
    timeout.tv_usec = static_cast<Long>(_cfgs->_connectTimeOutMs * Time::_microSecondPerMilliSecond);
    isTimeOut = SocketUtil::IsDetectTimeOut(sock, timeout, false, true, &detectRet);

    // 判断是否链接成功
    int optval = -1;
    int optlen = sizeof(optval);
    int retget = getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)(&optval), &optlen);
    if(retget != 0 || optval != 0)
        isTimeOut = true;
    else
        isTimeOut = false;

    ret = SocketUtil::SetBlock(sock);
    if(ret != StatusDefs::Success)
    {
        g_Log->w<FS_IocpConnector>(_LOGFMT_("SetBlock fail sock[%llu] ret[%d]")
                                   , sock, ret);
    }

    if(isTimeOut)
    {
        g_Log->e<FS_TcpClient>(_LOGFMT_("<socket=%llu> connect <%s:%hu> timeout failed...")
                               , sock, connectInfo._ip.c_str(), connectInfo._port);
        return StatusDefs::FS_IocpConnector_ConnectTimeOut;
    }

    return StatusDefs::Success;
}
FS_NAMESPACE_END

#endif
