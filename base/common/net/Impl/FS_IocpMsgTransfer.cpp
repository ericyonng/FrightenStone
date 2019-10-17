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
 * @file  : FS_IocpMsgTransfer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "base/common/net/Impl/FS_IocpMsgTransfer.h"

#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/net/Impl/FS_Iocp.h"
#include "base/common/net/Defs/IocpDefs.h"
#include "base/common/log/Log.h"

FS_NAMESPACE_BEGIN
FS_IocpMsgTransfer::FS_IocpMsgTransfer()
    :_threadPool(NULL)
    ,_iocp(NULL)
    ,_ioEvent(NULL)
    ,_disconnectedCallback(NULL)
{
}

FS_IocpMsgTransfer::~FS_IocpMsgTransfer()
{
    Fs_SafeFree(_disconnectedCallback);
    Fs_SafeFree(_threadPool);
    Fs_SafeFree(_iocp);
    Fs_SafeFree(_ioEvent);
}

Int32 FS_IocpMsgTransfer::BeforeStart()
{
    _threadPool = new FS_ThreadPool(0, 1);
    _iocp = new FS_Iocp;
    _ioEvent = new IO_EVENT;
    const Int32 st = _iocp->Create();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("create iocp fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpMsgTransfer::Start()
{
    return StatusDefs::Success;
}

void FS_IocpMsgTransfer::BeforeClose()
{
}

void FS_IocpMsgTransfer::Close()
{
}

void FS_IocpMsgTransfer::AfterClose()
{
    Fs_SafeFree(_disconnectedCallback);
}

void FS_IocpMsgTransfer::OnConnect(IFS_Session *session)
{
    _locker.Lock();
    _willAddSessions.push_back(session);
    ++_sessionCnt;
    _locker.Unlock();
}

void FS_IocpMsgTransfer::OnDestroy()
{

}

void FS_IocpMsgTransfer::OnHeartBeatTimeOut()
{
}

void FS_IocpMsgTransfer::RegisterDisconnected(IDelegate<void, IFS_Session *> *callback)
{
    Fs_SafeFree(_disconnectedCallback);
    _disconnectedCallback = callback;
}

Int32 FS_IocpMsgTransfer::GetSessionCnt()
{
    return _sessionCnt;
}

void FS_IocpMsgTransfer::_OnMoniterMsg(const FS_ThreadPool *pool)
{
    while(!pool->IsClearingPool())
    {
        // 1.将待连入的session转移到容器中
        // 2.投递接收数据事件
        // 3.
    }
}

Int32 FS_IocpMsgTransfer::_ListenEvent()
{
    auto ret = _iocpClientMsgTransfer->WaitForCompletion(*_ioEvent, 1);
    if(ret != StatusDefs::Success)
    {
        if(ret != StatusDefs::IOCP_WaitTimeOut)
            g_Log->net<FS_IocpMsgTransferServer>("FS_IOCPServer%d.DoIocpNetEvents.wait nothing but ret[%d]", _id, ret);
        // g_Log->any<FS_IocpMsgTransferServer>("FS_IOCPServer%d.DoIocpNetEvents.wait nothing but ret[%d]", _id, ret);
        return ret;
    }

    // 处理iocp退出
    if(_ioEvent->_data._code == IocpDefs::IO_QUIT)
    {
        g_Log->sys<FS_IocpMsgTransferServer>(_LOGFMT_("iocp退出 code=%lld"), _ioEvent->_data._code);
        return StatusDefs::IOCP_Quit;
    }

    // 判断client是否已被移除
    const UInt64 clientId = _ioEvent->_data._clientId;
    auto client = _GetClient(clientId);
    if(!client)
    {
        g_Log->net<FS_IocpMsgTransferServer>("clientId[%llu] is removed before", clientId);
        return ret;
    }

    if(IocpDefs::IO_RECV == _ioEvent->_ioData->_ioType)
    {// 接收数据 完成 Completion

        if(_ioEvent->_bytesTrans <= 0)
        {// 客户端断开处理
            g_Log->any<FS_IocpMsgTransferServer>("client sock[%llu] clientId[%llu] IO_TYPE::RECV bytesTrans[%lu]"
                                                 , client->GetSocket()
                                                 , clientId,
                                                 _ioEvent->_bytesTrans);
            g_Log->net<FS_IocpMsgTransferServer>("client sock[%llu] clientId[%llu] will remove IO_TYPE::IO_RECV bytesTrans[%lu]"
                                                 , client->GetSocket()
                                                 , clientId
                                                 , _ioEvent->_bytesTrans);
            //CELLLog_Info("rmClient sockfd=%d, IO_TYPE::RECV bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
            // _RmClient(*_ioEvent);
            _RmClient(client);
            return ret;
        }

        // 判断是否断开 已断开的有可能是上次postsend只后1ms内系统没有完成send，同时客户端被移除导致
        if(client->IsDestroy())
        {
            g_Log->e<FS_IocpMsgTransferServer>(_LOGFMT_("IO_RECV clientId[%llu] is destroy")
                                               , client->GetId());
        }

        // 完成回调
        _ioEvent->_ioData->_callback->Invoke(_ioEvent->_bytesTrans);
        client->ResetPostRecv();
        _OnPrepareNetRecv(client);
        _msgArrivedClientIds.insert(clientId);
        _needToPostClientIds.insert(clientId);
    }
    else
    {
        g_Log->e<FS_IocpMsgTransferServer>(_LOGFMT_("undefine io type[%d]."), _ioEvent->_ioData->_ioType);
    }

    return ret;
}

void FS_IocpMsgTransfer::_OnDisconnected(IFS_Session *session)
{
    --_sessionCnt;
    _disconnectedCallback->Invoke(session);
}

IFS_Session *FS_IocpMsgTransfer::_GetSession(UInt64 sessionId)
{
    auto iterSession = _sessions.find(sessionId);
    if(iterSession == _sessions.end())
        return NULL;

    return iterSession->second;
}

FS_NAMESPACE_END

