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
 * @file  : NetMethodUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/12/29
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/NetMethodUtil.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/log/Log.h"

FS_NAMESPACE_BEGIN
#ifdef _WIN32

void NetMethodUtil::PreparePostAccept(FS_Iocp *acceptorIocp, SOCKET acceptorSock, char **&bufArray, IoDataBase **&ioDataArray, Int32 maxSessionQuantityLimit)
{
    // 预先创建n个缓冲加速连接过程
    g_MemoryPool->Lock();
    bufArray = g_MemoryPool->Alloc<char *>(maxSessionQuantityLimit * sizeof(char *));
    g_MemoryPool->Unlock();

    for(Int32 i = 0; i < maxSessionQuantityLimit; ++i)
    {
        g_MemoryPool->Lock();
        bufArray[i] = g_MemoryPool->Alloc<char>(IOCP_ACCEPTOR_BUFFER);
        g_MemoryPool->Unlock();
    }

    // 预先创建n个iodata
    g_MemoryPool->Lock();
    ioDataArray = g_MemoryPool->Alloc<IoDataBase *>(sizeof(IoDataBase *)* maxSessionQuantityLimit);
    g_MemoryPool->Unlock();

    Int32 st = StatusDefs::Success;
    for(Int32 i = 0; i < maxSessionQuantityLimit; ++i)
    {
        g_MemoryPool->Lock();
        ioDataArray[i] = g_MemoryPool->Alloc<IoDataBase>(sizeof(IoDataBase));
        g_MemoryPool->Unlock();

        memset(ioDataArray[i], 0, sizeof(IoDataBase));
        ioDataArray[i]->_wsaBuff.buf = bufArray[i];
        ioDataArray[i]->_wsaBuff.len = IOCP_ACCEPTOR_BUFFER;

        st = acceptorIocp->PostAccept(acceptorSock, ioDataArray[i]);
        if(st != StatusDefs::Success)
        {
            g_Log->e<NetMethodUtil>(_LOGFMT_("prepare post accept fail sock[%llu] st[%d]")
                                      , acceptorSock, st);
        }
    }
}

void NetMethodUtil::FreePrepareAcceptBuffers(char **&bufArray, IoDataBase **&ioDataArray, Int32 maxSessionQuantityLimit)
{
    for(Int32 i = 0; i < maxSessionQuantityLimit; ++i)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(bufArray[i]);
        g_MemoryPool->Free(ioDataArray[i]);
        g_MemoryPool->Unlock();
    }

    g_MemoryPool->Lock();
    g_MemoryPool->Free(bufArray);
    g_MemoryPool->Free(ioDataArray);
    g_MemoryPool->Unlock();
}

#endif

FS_NAMESPACE_END
