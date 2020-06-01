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
 * @file  : MessageBlockUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/12/29
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Defs/NetMessageBlock.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Impl/IFS_BasePoller.h"

#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/memorypool/memorypool.h"


FS_NAMESPACE_BEGIN

#ifdef _WIN32
FS_MessageBlock * MessageBlockUtil::BuildIocpMsgArrivedMsgBlock(UInt32 compId, UInt32 generatorId, IoEvent *ev, Int32 errorCode)
{
    IocpMsgArrivedMsgBlock *block = new IocpMsgArrivedMsgBlock();
    block->_compId = compId;
    block->_generatorId = generatorId;
    block->_ioEv = new IoEvent;
    *block->_ioEv = *ev;
    block->_errorCode = errorCode;
    return block;
}
#endif

FS_MessageBlock * MessageBlockUtil::BuildSessionConnectedNetMsgBlock(
    UInt32 compId
    , UInt32 generatorId
    , IFS_BasePoller *poller
    , BriefSessionInfo *newSessionInfo)
{
    auto newMsgBlock = new SessionConnectedNetMsgBlock(compId, generatorId);
    newMsgBlock->_acceptorCompId = newSessionInfo->_acceptorCompId;
    newMsgBlock->_connectorCompId = newSessionInfo->_connectorCompId;
    newMsgBlock->_protocolPort = newSessionInfo->_protocolPort;
    newMsgBlock->_srcCompType = newSessionInfo->_srcCompType;
    newMsgBlock->_sessionId = newSessionInfo->_sessionId;
    newMsgBlock->_sock = newSessionInfo->_sock;
    newMsgBlock->_addrInfo = *newSessionInfo->_addrInfo;
    newMsgBlock->_onUserDisconnectedRes = newSessionInfo->_userDisconnectedRes;
    newMsgBlock->_stub = newSessionInfo->_stub;
    newSessionInfo->_userDisconnectedRes = NULL;

#ifdef _WIN32
    newMsgBlock->_iocp = poller->GetNetModuleObj();
#else
    newMsgBlock->_poller = poller;
#endif

    return newMsgBlock;
}

FS_MessageBlock * MessageBlockUtil::BuildPostConnectionFailureToDispatcherMsgBlock(const FS_ConnectInfo &connectInfo, Int32 resultCode)
{
    return new PostConnectionFailureToDispatcherMsgBlock(connectInfo, resultCode);
}

FS_MessageBlock * MessageBlockUtil::BuildPostAsynConnectionToConnectorMsgBlock(UInt32 dispatcherMsgQueueConsumerId, const FS_ConnectInfo &connectInfo)
{
    auto newMsgBlock = new PostAsynConnectionToConnectorMsgBlock(connectInfo);
    newMsgBlock->_dispatcherMsgQueueConsumerId = dispatcherMsgQueueConsumerId;
    return newMsgBlock;
}

#ifndef _WIN32

FS_MessageBlock * MessageBlockUtil::BuildEpollEvEpollInEvMsgBlock(UInt64 sessionId)
{
    EpollEvEpollInEvMsgBlock *block = new EpollEvEpollInEvMsgBlock;
    block->_sessionId = sessionId;
    return block;
}

FS_MessageBlock *MessageBlockUtil::BuildEpollEvEpollOutEvMsgBlock(UInt64 sessionId)
{
    EpollEvEpollOutEvMsgBlock *block = new EpollEvEpollOutEvMsgBlock;
    block->_sessionId = sessionId;
    return block;
}
#endif

FS_NAMESPACE_END

