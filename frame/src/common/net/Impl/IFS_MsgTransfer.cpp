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
 * @file  : IFS_MsgTransfer.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Impl/IFS_BasePoller.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Defs/FS_ConnectInfo.h"
#include "FrightenStone/common/net/Impl/FS_EpollTransferPoller.h"

#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"

FS_NAMESPACE_BEGIN

IFS_MsgTransfer::~IFS_MsgTransfer()
{
    Fs_SafeFree(_cfgs);
}

bool IFS_MsgTransfer::OnWillConnect(BriefSessionInfo *newSessionInfo)
{
    auto poller = GetPoller();

#ifndef _WIN32
    // TODO:OnWillConnect
    g_Log->i<IFS_MsgTransfer>(_LOGFMT_(" on will connect new brief session info:\n[%s]"), newSessionInfo->ToString().c_str());
    auto epollTransferPoller = reinterpret_cast<FS_EpollTransferPoller *>(poller);
    if (!epollTransferPoller->OnWillConnect(newSessionInfo->_sock, newSessionInfo->_sessionId))
    {
        g_Log->e<IFS_MsgTransfer>(_LOGFMT_("compId[%u] compType[%d] sock[%d] sessionId[%llu] has already connect or it is not free before")
            , _compId, _compType, newSessionInfo->_sock, newSessionInfo->_sessionId);
        return false;
    }
#endif

    auto newMsgBlock = MessageBlockUtil::BuildSessionConnectedNetMsgBlock(_compId, _generatorId, poller, newSessionInfo);
    ++_sessionCnt;
    if (!_concurrentMq->Push(_generatorId, newMsgBlock))
    {
        g_Log->e<IFS_MsgTransfer>(_LOGFMT_("concurrentmq push fail generatorId[%u] compid[%u] newSessionId[%llu] sock[%llu]")
            , _generatorId, _compId, newSessionInfo->_sessionId, newSessionInfo->_sock);
        Fs_SafeFree(newMsgBlock);
    }

    return true;
}

void IFS_MsgTransfer::OnConnectFailure(UInt32 dispatcherConsumerMsgQueueId, const FS_ConnectInfo &connectInfo, Int32 resultCode)
{
    auto newMsgBlock = MessageBlockUtil::BuildPostConnectionFailureToDispatcherMsgBlock(connectInfo, resultCode);
    if (!_concurrentMq->PushByConsumerId(_generatorId, dispatcherConsumerMsgQueueId, newMsgBlock))
    {
        g_Log->e<IFS_MsgTransfer>(_LOGFMT_("concurrentmq push fail generatorId[%u] compid[%u] connectInfo[%s:%hu] stub[%llu] resultCode[%d]")
            , _generatorId, _compId, connectInfo._ip.c_str(), connectInfo._port, connectInfo._stub, resultCode);
        Fs_SafeFree(newMsgBlock);
    }
}
FS_NAMESPACE_END

