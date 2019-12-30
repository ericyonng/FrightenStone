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
 * @file  : MessageBlockUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/29
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Defs/FS_NetMessageBlock.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"

#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

FS_MessageBlock *MessageBlockUtil::BuildTransferMonitorArrivedMessageBlock(UInt32 compId, UInt32 generatorId, IoEvent *ev, Int32 errorCode)
{
    FS_NetArrivedMsg *block = new FS_NetArrivedMsg();
    block->_compId = compId;
    block->_generatorId = generatorId;
    g_MemoryPool->Lock();
    block->_ioEv = g_MemoryPool->Alloc<IoEvent>(sizeof(IoEvent));
    g_MemoryPool->Unlock();
    *block->_ioEv = *ev;
    block->_errorCode = errorCode;
    return block;
}

FS_MessageBlock *MessageBlockUtil::BuildTransferWillConnectMessageBlock(Int32 generatorId, UInt64 sessionId, SOCKET sock, const sockaddr_in *addrInfo)
{
    FS_NetSessionWillConnectMsg *block = new FS_NetSessionWillConnectMsg;
    block->_generatorId = generatorId;
    block->_sessionId = sessionId;
    block->_sock = sock;
    block->_addrInfo = *addrInfo;
    return block;
}

FS_NAMESPACE_END