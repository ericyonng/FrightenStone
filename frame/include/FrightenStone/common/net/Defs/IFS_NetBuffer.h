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
 * @file  : IFS_NetBuffer.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/4/16
 * @brief :
 */

#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_IFS_NetBuffer_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_IFS_NetBuffer_H__

#pragma once

#include "FrightenStone/common/net/Defs/IFS_Buffer.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/memorypool/memorypool.h"

#ifdef _WIN32
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#else
#include "FrightenStone/common/net/Defs/EpollDefs.h"
#endif

FS_NAMESPACE_BEGIN

class BASE_EXPORT IFS_NetBuffer : public IFS_Buffer
{
    OBJ_POOL_CREATE_DERIVE(IFS_NetBuffer, IFS_Buffer);
public:
    IFS_NetBuffer(size_t bufferSize, IMemoryAlloctor *memAlloctor);
    ~IFS_NetBuffer();

public:
    bool BindTo(UInt64 sessionId, SOCKET sock);
    UInt64 GetSessionId() const;
    SOCKET GetSocket() const;
    // bool HasMsg() const;
    bool NeedWrite() const;

    /* 网络io接口 */ 
#ifdef _WIN32
public:
    IoDataBase *MakeRecvIoData();
    IoDataBase *MakeSendIoData();
#else
public:
    EpollIoData * MakeRecvIoData();
    EpollIoData *MakeSendIoData();
#endif

public:
    FS_String ToString() const;

private:
    void _OnSendSucCallback(Int64 transferBytes);
    void _OnRecvSucCallback(Int64 transferBytes);

protected:
    UInt64 _sessionId;      // FS_Packet生命周期内不变更
    SOCKET _socket;         // FS_Packet生命周期内不变更

    // 网络io对象
#ifdef _WIN32
    IoDataBase _ioData;
#else
     EpollIoData _ioData;
#endif
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Defs/IFS_NetBufferImpl.h"

#endif
