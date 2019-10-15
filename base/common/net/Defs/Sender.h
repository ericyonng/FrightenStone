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
 * @file  : Sender.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/6
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Defs_Sender_H__
#define __Base_Common_Net_Defs_Sender_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/assist/utils/Impl/STLUtil.h"
#include "base/common/component/Impl/FS_ThreadPool.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_Packet2;
struct BASE_EXPORT BufferQueueNode;
class BASE_EXPORT FS_Iocp;
struct BASE_EXPORT IO_EVENT;

class BASE_EXPORT Sender
{
    OBJ_POOL_CREATE_DEF(Sender);
public:
    Sender(const IDelegate<bool, UInt64> *isClientDestroy);
    ~Sender();

    void Start();
    void Close();
    void SendPacket(FS_Packet2 *packet);

private:
    void _OnWork(const FS_ThreadPool *pool);
    void _OnSendSuc(BufferQueueNode *node, Int32 transferBytes);
    Int32 _WaitForNetResponse();

    void _Add2SendQueue(FS_Packet2 *packet);
    void _RemoveFromQueue(UInt64 clientId);
    void _ClearData();

public:
    FS_ThreadPool *_pool;
    FS_Iocp *_sender;
    IO_EVENT *_ioEvent;
    ConditionLocker _locker;
    bool _isDestroy;
    std::list<FS_Packet2 *> _packetCacheQueue;   // 待转移到发送队列的缓冲队列
    std::map<UInt64, std::list<BufferQueueNode *>> _sendingQueue; // key:clientId
    std::set<UInt64> _needPostClientIds;
    IDelegate<void> *_quitIocp;
    const IDelegate<bool, UInt64> *_isClientDestroy;
};

FS_NAMESPACE_END

#include "base/common/net/Defs/SenderImpl.h"

#endif
