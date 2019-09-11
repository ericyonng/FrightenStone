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
 * @file  : Sender.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/6
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "base/common/net/Defs/Sender.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/log/Log.h"
#include "base/common/net/Defs/FS_Packet.h"

FS_NAMESPACE_BEGIN

void Sender::Start()
{
    _pool = new FS_ThreadPool(0, 1);
    auto task = DelegatePlusFactory::Create(this, &Sender::_OnWork);
    if(!_pool->AddTask(task))
    {
        g_Log->e<Sender>(_LOGFMT_("add task error"));
    }
}

void Sender::SendPacket(FS_Packet *packet)
{
    // 添加到队列
    PacketQueueNode *node = new PacketQueueNode;
    node->_packet = packet;
    _queue.push_back(node);
    node->_iterNode = --_queue.end();
    const auto clientId = packet->GetOwnerId();
    auto iterPacketNodes = _clientIdRefPacketNodes.find(clientId);
    if(iterPacketNodes == _clientIdRefPacketNodes.end())
        iterPacketNodes = _clientIdRefPacketNodes.insert(std::make_pair(clientId, std::set<PacketQueueNode *>())).first;

    iterPacketNodes->second.insert(node);
}

void Sender::_OnWork(const FS_ThreadPool *pool)
{
    // 注册postquit事件
    while(!pool->IsClearingPool())
    {
        // 监听发送成功
        // 
    }
}

void Sender::_OnSendSuc(PacketQueueNode *node, Int32 transferBytes)
{
}

FS_NAMESPACE_END
