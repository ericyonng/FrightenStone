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
 * @file  : IFS_MsgTransfer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgTransfer_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgTransfer_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"

FS_NAMESPACE_BEGIN

class  IFS_Session;
struct NetMsg_DataHeader;
class ConcurrentMessageQueueNoThread;
class MessageQueueNoThread;
struct BriefSessionInfo;
struct TransferCfgs;

class BASE_EXPORT IFS_MsgTransfer : public IFS_EngineComp
{
public:
    IFS_MsgTransfer(IFS_NetEngine *engine, Int32 compId);
    virtual ~IFS_MsgTransfer();

    /* 网络事件 */
public:
    virtual void OnWillConnect(UInt64 sessionId, SOCKET sock, const sockaddr_in *addrInfo) = 0;

    /* 属性方法 */
public:
    virtual Int32 GetSessionCnt() = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void OnDestroy() = 0;
    virtual void OnHeartBeatTimeOut(IFS_Session *session) = 0;

    // 消息队列
    virtual void AttachMsgQueue(ConcurrentMessageQueueNoThread *messageQueue, Int32 generatorId) = 0;
    virtual void AttachSenderMsgQueue(MessageQueueNoThread *messageQueue) = 0;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_MsgTransferImpl.h"

#endif

