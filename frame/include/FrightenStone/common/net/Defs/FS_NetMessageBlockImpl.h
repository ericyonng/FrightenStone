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
 * @file  : FS_NetMessageBlockImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/29
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Defs_FS_NetMessageBlock_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_NetMsgBufferBlock::FS_NetMsgBufferBlock()
    : _compId(0)
    , _generatorId(0)
    , _netMessageBlockType(NetMessageBlockType::Net_None)
{
}

inline FS_NetMsgBufferBlock::~FS_NetMsgBufferBlock()
{

}

inline FS_NetArrivedMsg::FS_NetArrivedMsg()
    :_ioEv(NULL)
    ,_errorCode(StatusDefs::Success)
{
    _netMessageBlockType = NetMessageBlockType::Net_NetMsgArrived;
}

inline FS_NetArrivedMsg::~FS_NetArrivedMsg()
{
    if(_ioEv)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(_ioEv);
        g_MemoryPool->Unlock();
    }
}

inline FS_NetSessionWillConnectMsg::FS_NetSessionWillConnectMsg()
    :_sessionId(0)
    ,_sock(INVALID_SOCKET)
    ,_addrInfo{0}
{
    _netMessageBlockType = NetMessageBlockType::Met_NetSessionConnected;
}

inline FS_NetSessionWillConnectMsg::~FS_NetSessionWillConnectMsg()
{
}

FS_NAMESPACE_END
#endif
