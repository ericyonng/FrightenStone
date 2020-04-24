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
 * @file  : FS_EpollBuffer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/02/01
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_FS_EpollBuffer_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_FS_EpollBuffer_H__

#pragma once

#ifndef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Defs/IFS_Buffer.h"
#include "FrightenStone/common/net/Defs/EpollDefs.h"

#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_EpollBuffer : public IFS_Buffer
{
    OBJ_POOL_CREATE_DEF(FS_EpollBuffer);
public:
    explicit FS_EpollBuffer(size_t bufferSize, IMemoryAlloctor *memAlloctor);
    virtual ~FS_EpollBuffer();

public:
    EpollIoData *MakeRecvIoData();
    EpollIoData *MakeSendIoData();
    bool HasMsg() const;
    bool NeedWrite() const;

    FS_String ToString() const;

private:
    void _OnSendSucCallback(Int64 transferBytes);
    void _OnRecvSucCallback(Int64 transferBytes);

private:
    EpollIoData _ioData;
};
FS_NAMESPACE_END

#include "FrightenStone/common/net/Defs/FS_EpollBufferImpl.h"

#endif

#endif
