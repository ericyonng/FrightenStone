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

class BASE_EXPORT FS_Packet;
class BASE_EXPORT PacketQueueNode;
class BASE_EXPORT FS_Iocp;

class BASE_EXPORT Sender
{
    OBJ_POOL_CREATE_DEF(Sender);
public:
    Sender();
    ~Sender();

    void Start();
    void SendPacket(FS_Packet *packet);

private:
    void _OnWork(const FS_ThreadPool *pool);
    void _OnSendSuc(PacketQueueNode *node, Int32 transferBytes);

public:
    FS_ThreadPool *_pool;
    FS_Iocp *_sender;
    Locker _locker;
    std::list<PacketQueueNode *> _queue;
    std::map<UInt64, std::set<PacketQueueNode *>> _clientIdRefPacketNodes;
};

FS_NAMESPACE_END

#include "base/common/net/Defs/SenderImpl.h"

#endif
