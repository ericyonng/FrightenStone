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
 * @file  : FS_EpollMsgTransfer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/20
 * @brief :
 */
#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollMsgTransfer.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Impl/FS_EpollPoller.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_DEF_IMPL(FS_EpollMsgTransfer, 32)

FS_EpollMsgTransfer::FS_EpollMsgTransfer(UInt32 compId, IFS_NetEngine *engine)
    :IFS_MsgTransfer(compId, engine)
    , _isInit{false}
    , _cfgs(NULL)
    , _sessionCnt{0}
    , _poller(NULL)
{
}

FS_EpollMsgTransfer::~FS_EpollMsgTransfer()
{
    Fs_SafeFree(_poller);
    Fs_SafeFree(_cfgs);
}

Int32 FS_EpollMsgTransfer::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    if(_isInit)
        return StatusDefs::Success;

    _cfgs = new TransferCfgs;
    *_cfgs = totalCfgs._transferCfgs;

    _poller = new FS_EpollPoller(this, PollerDefs::MonitorType_Transfer);
    _poller->AttachMessageQueue(_concurrentMq);

    Int32 st = _poller->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollMsgTransfer>(_LOGFMT_("_poller->BeforeStart fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_EpollMsgTransfer::Start()
{
    if(_isInit)
        return StatusDefs::Success;

    Int32 st = _poller->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollMsgTransfer>(_LOGFMT_("_poller->Start fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

void FS_EpollMsgTransfer::AfterStart()
{
    if(_isInit)
        return;

    _poller->AfterStart();
    _isInit = true;
    g_Log->sys<FS_EpollMsgTransfer>(_LOGFMT_("transfer start suc..."));
}

void FS_EpollMsgTransfer::WillClose()
{
    if(!_isInit)
        return;

    _poller->WillClose();
}

void FS_EpollMsgTransfer::BeforeClose()
{
    if(!_isInit)
        return;

    _poller->BeforeClose();
}

void FS_EpollMsgTransfer::Close()
{
    if(!_isInit)
        return;

    _poller->Close();
}

void FS_EpollMsgTransfer::AfterClose()
{
    if(!_isInit)
        return;

    _poller->AfterClose();
    _isInit = false;
}

bool FS_EpollMsgTransfer::OnWillConnect(BriefSessionInfo *newSessionInfo)
{
    g_Log->i<FS_EpollMsgTransfer>(_LOGFMT_(" on will connect new brief session info:\n[%s]"), newSessionInfo->ToString().c_str());
    if(!_poller->OnWillConnect(newSessionInfo->_sock, newSessionInfo->_sessionId))
    {
        g_Log->e<FS_EpollMsgTransfer>(_LOGFMT_("compId[%u] compType[%d] sock[%d] sessionId[%llu] has already connect or it is not free before")
                                      , _compId, _compType, newSessionInfo->_sock, newSessionInfo->_sessionId);
        return false;
    }

    // TODO:连接消息消息
    auto newMessageBlock = MessageBlockUtil::BuildTransferEpollEvSessionWillConnectMsg(_compId
                                                                                       , _generatorId
                                                                                       , _poller
                                                                                       , newSessionInfo);
    ++_sessionCnt;
#ifdef _DEBUG
    if(!_concurrentMq->Push(_generatorId, newMessageBlock))
    {
        g_Log->e<FS_EpollMsgTransfer>(_LOGFMT_("concurrentmq push fail generatorId[%u] compid[%u] newSessionId[%llu] sock[%llu]")
                                      , _generatorId, _compId, newSessionInfo->_sessionId, newSessionInfo->_sock);
    }
#else
    _concurrentMq->Push(_generatorId, newMessageBlock);
#endif

    return true;
}

void FS_EpollMsgTransfer::OnPostConnectOpFinish()
{
    // TODO:连接消息消息
    auto newMessageBlock = MessageBlockUtil::BuildConnectorPostConnectOpFinish(_compId, _generatorId);
#ifdef _DEBUG
    if(!_concurrentMq->Push(_generatorId, newMessageBlock))
    {
        g_Log->e<FS_EpollMsgTransfer>(_LOGFMT_("concurrentmq push fail generatorId[%u] compid[%u] ")
                                      , _generatorId, _compId);
    }
#else
    _concurrentMq->Push(_generatorId, newMessageBlock);
#endif
}

bool FS_EpollMsgTransfer::IsIoFinish() const
{
    return _poller->IsIoFinish();
}
FS_NAMESPACE_END


#endif

