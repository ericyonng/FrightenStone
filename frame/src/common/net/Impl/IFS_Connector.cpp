/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : IFS_Connector.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_Connector.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"

#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/socket/socket.h"

FS_NAMESPACE_BEGIN

IFS_Connector::MessageQueueHandler IFS_Connector::_msgBlockHandler[NetMessageType::End] = { NULL };

IFS_Connector::~IFS_Connector()
{
    Fs_SafeFree(_pool);
    Fs_SafeFree(_cfgs);
}


Int32 IFS_Connector::BeforeStart(const NetEngineTotalCfgs &cfgs)
{
    if (_isInit)
        return StatusDefs::Success;

    const Int32 st = IFS_EngineComp::BeforeStart(cfgs);
    if (st != StatusDefs::Success)
    {
        g_Log->e<IFS_Connector>(_LOGFMT_("engine comp before start fail st[%d]"), st);
        return st;
    }

    _cfgs = new ConnectorCfgs;
    *_cfgs = cfgs._connectorCfgs;
    _pool = new FS_ThreadPool(0, 1);

    // 订阅消息
    _msgBlockHandler[NetMessageType::PostAsynConnectionToConnector] = &IFS_Connector::_OnAsynConnection;
    _msgBlockHandler[NetMessageType::SessionDisconnected] = &IFS_Connector::_OnSessionDisconnected;
    return StatusDefs::Success;
}

Int32 IFS_Connector::Start()
{
    if (_isInit)
        return StatusDefs::Success;

    return IFS_EngineComp::Start();
}

void IFS_Connector::AfterStart()
{
    if (_isInit)
        return;

    auto delgateTask = DelegatePlusFactory::Create(this, &IFS_Connector::_OnMonitor);
    if (!_pool->AddTask(delgateTask, true))
    {
        g_Log->e<IFS_Connector>(_LOGFMT_("thread poll add task fail"));
        return;
    }
    g_Log->sys<IFS_Connector>(_LOGFMT_("connector start suc..."));
    _isInit = true;
}

void IFS_Connector::WillClose()
{
    if (!_isInit)
        return;
}

void IFS_Connector::BeforeClose()
{
    if (!_isInit)
        return;

    IFS_EngineComp::BeforeClose();
}

void IFS_Connector::Close()
{
    if (!_isInit)
        return;

    IFS_EngineComp::Close();
}

void IFS_Connector::AfterClose()
{
    if (!_isInit)
        return;

    _pool->Close();
    _isInit = false;
    Fs_SafeFree(_pool);
    Fs_SafeFree(_cfgs);
}

Int32 IFS_Connector::Connect(UInt32 dispatcherConsumerId, const FS_ConnectInfo &connectInfo)
{
    Int32 ret = StatusDefs::Success;
    do
    {
        // 校验目标连接是否已经在pending队列中
        FS_String addrInfo;
        ret = _CheckConnect(connectInfo, addrInfo);
        if (ret != StatusDefs::Success)
        {
            g_Log->e<IFS_Connector>(_LOGFMT_("check connect fail ret[%d]"), ret);
            break;
        }

        // 校验目标连接是否已经成功连接
        // 1.初始化socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == sock)
        {
            g_Log->e<IFS_Connector>(_LOGFMT_("create socket failed"));
            ret = StatusDefs::Socket_CreateFailure;
            break;
        }

        // 设置非阻塞模式
        ret = SocketUtil::SetNoBlock(sock);
        if (ret != StatusDefs::Success)
        {
            g_Log->e<IFS_Connector>(_LOGFMT_("SetNoBlock fail sock[%llu] ret[%d]"), sock, ret);
            ret = StatusDefs::Socket_SetNoBlockParamError;
            break;
        }

        // socket重用
        SocketUtil::MakeReUseAddr(sock);

        sockaddr_in  sin = {};
        if (!SocketUtil::FillTcpAddrInfo(connectInfo._ip.c_str(), connectInfo._port, AF_INET, sin))
        {
            g_Log->e<IFS_Connector>(_LOGFMT_("FillTcpAddrInfo fail ip[%s] port[%hu]"), connectInfo._ip.c_str(), connectInfo._port);
            ret = StatusDefs::Socket_FillTcpAddrInfoFailure;
            break;
        }

        ret = _Connect(sock, sin, connectInfo);
        if (ret == StatusDefs::Success)
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
            newSessionInfo._userDisconnectedRes = DelegatePlusFactory::Create(this, &IFS_Connector::_OnUserDisconnected);
            newSessionInfo._stub = connectInfo._stub;
            newSessionInfo._connectorCompId = _compId;
            newSessionInfo._srcCompType = _compType;
            newSessionInfo._protocolPort = connectInfo._port;
            if (!netEngine->HandleCompEv_WillConnect(&newSessionInfo))
            {
                _locker.Lock();
                --_curSessionCnt;
                _sucConnectedSessionIds.erase(curMaxSessionId);
                _locker.Unlock();
            }
        }
        else
        {
            SocketUtil::DestroySocket2(sock);
            g_Log->w<IFS_Connector>(_LOGFMT_("connect fail ret[%d]"), ret);
            break;
        }

    } while (0);

    // 不成功且需要回执的需要post失败
    if (ret != StatusDefs::Success && connectInfo._stub)
        _OnConnectFailureRes(dispatcherConsumerId, connectInfo, ret);

    return ret;
}

Int32 IFS_Connector::AsynConnect(UInt32 dispatcherMsgQueueConsumerId, const FS_ConnectInfo &connectInfo)
{
    if (!_isInit)
        return StatusDefs::NotInit;

    PostAsynConnectionToConnectorMsgBlock *newMsgBlock = new PostAsynConnectionToConnectorMsgBlock(connectInfo);
    newMsgBlock->_dispatcherMsgQueueConsumerId = dispatcherMsgQueueConsumerId;
    if (!_myCompMq->Push(newMsgBlock))
    {
        g_Log->w<IFS_Connector>(_LOGFMT_("comp mq push msg block fail"));
        Fs_SafeFree(newMsgBlock);
        return StatusDefs::Error;
    }

    return StatusDefs::Success;
}

void IFS_Connector::_OnUserDisconnected(UInt64 sessionId)
{
    if (!_isInit)
        return;

    SessionDisconnectedMsgBlock *newMsgBlock = new SessionDisconnectedMsgBlock(sessionId);
    if (!_myCompMq->Push(newMsgBlock))
    {
        g_Log->w<IFS_Connector>(_LOGFMT_("comp mq push msg block fail"));
        Fs_SafeFree(newMsgBlock);
    }
}

Int32 IFS_Connector::_CheckConnect(const FS_ConnectInfo &connectInfo, FS_String &addrInfoOut) const
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

        if (_curSessionCnt >= _maxSessionQuantityLimit)
        {
            ret = StatusDefs::IFS_Connector_ConnectOverLimit;
            break;
        }
    } while (0);
    _locker.Unlock();

    return ret;
}

Int32 IFS_Connector::_Connect(SOCKET sock, const sockaddr_in &sin, const FS_ConnectInfo &connectInfo)
{
    // 链接
    bool isTimeOut = false;
    timeval timeout = {};
    auto conRet = ::connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in));
    if (conRet == 0)
        return StatusDefs::Success;

#ifndef _WIN32
    Int32 err = errno;
    if (err != EINPROGRESS)
    {
        g_Log->w<IFS_Connector>(_LOGFMT_("fail connect sock[%d] err[%d]"), sock, err);
        return StatusDefs::Failed;
    }
#endif

    Int32 detectRet = 0;
    timeout.tv_usec = static_cast<Long>(_cfgs->_connectTimeOutMs * Time::_microSecondPerMilliSecond);
    isTimeOut = SocketUtil::IsDetectTimeOut(sock, timeout, false, true, &detectRet);

    // 判断是否链接成功
    int optval = -1;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
    int retget = getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)(&optval), &optlen);
    if (retget != 0 || optval != 0)
        isTimeOut = true;
    else
        isTimeOut = false;

    if (isTimeOut)
    {
        g_Log->e<IFS_Connector>(_LOGFMT_("<socket=%llu> connect <%s:%hu> timeout failed...")
            , sock, connectInfo._ip.c_str(), connectInfo._port);
        return StatusDefs::IFS_Connector_ConnectTimeOut;
    }

    return StatusDefs::Success;
}

void IFS_Connector::_OnConnectFailureRes(UInt32 dispatcherConsumerId, const FS_ConnectInfo &connectInfo, Int32 resultCode)
{
    GetEngine()->HandleCompEv_ConnectFailure(dispatcherConsumerId, connectInfo, resultCode);
}

void IFS_Connector::_OnMonitor(FS_ThreadPool *pool)
{
    FS_NetMsgBlock *evMsgBlock = NULL;
    std::list<FS_MessageBlock *> *messageBlockList = new std::list<FS_MessageBlock *>;
    MaskReady(true);
    g_Log->sys<IFS_Connector>(_LOGFMT_("connector monitor is working..."));
    EngineCompsMethods::WaitForAllCompsReady(_engine);

    while (pool->IsPoolWorking() && _myCompMq->IsWorking())
    {
        _myCompMq->WaitForPoping(messageBlockList);
        if (!pool->IsPoolWorking() && messageBlockList->empty())
            break;

        for (auto iterBlock = messageBlockList->begin(); iterBlock != messageBlockList->end();)
        {
            evMsgBlock = reinterpret_cast<FS_NetMsgBlock *>(*iterBlock);
            (this->*_msgBlockHandler[evMsgBlock->_messageType])(evMsgBlock);
            Fs_SafeFree(evMsgBlock);
            iterBlock = messageBlockList->erase(iterBlock);
        }
    }

    STLUtil::DelListContainer(*messageBlockList);
    Fs_SafeFree(messageBlockList);
    MaskReady(false);

    g_Log->sys<IFS_Connector>(_LOGFMT_("connector monitor is finished..."));
}

void IFS_Connector::_OnAsynConnection(FS_NetMsgBlock *msgBlock)
{
    auto asynConnectionMessageBlock = msgBlock->CastTo<PostAsynConnectionToConnectorMsgBlock>();
    auto st = Connect(asynConnectionMessageBlock->_dispatcherMsgQueueConsumerId
        , asynConnectionMessageBlock->_connectInfo);
    if (st != StatusDefs::Success)
    {
        const FS_ConnectInfo &connectInfo = asynConnectionMessageBlock->_connectInfo;
        g_Log->w<IFS_Connector>(_LOGFMT_("connect target[%s:%hu] fail st[%d] stub[%llu]")
            , connectInfo._ip.c_str(), connectInfo._port, st, connectInfo._stub);
    }
}

void IFS_Connector::_OnSessionDisconnected(FS_NetMsgBlock *msgBlock)
{
    auto disconnectedMsgBlock = msgBlock->CastTo<SessionDisconnectedMsgBlock>();
    _locker.Lock();
    _sucConnectedSessionIds.erase(disconnectedMsgBlock->_sessionId);
    --_curSessionCnt;
    _locker.Unlock();

}
FS_NAMESPACE_END


