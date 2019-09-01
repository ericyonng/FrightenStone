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
 * @file  : FS_NetBufferArray.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/23
 * @brief :
 *          FS_NetBufferArray是有序的buffer队列，写入数据空间不足时会自动扩展，
 *          当弹出数据时也是有序的从头节点弹出，
 *
 * 
 */
#ifndef __Base_Common_Net_Defs_FS_NetBufferArray_H__
#define __Base_Common_Net_Defs_FS_NetBufferArray_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/assist/utils/Impl/STLUtil.h"
#include "base/common/net/Defs/FS_NetBuffer.h"
#include "base/common/assist/utils/Impl/SystemUtil.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/crashhandle/CrashHandle.h"
#include "base/common/log/Log.h"

FS_NAMESPACE_BEGIN

// 数据出去的方向从front开始
// 数据进来的方向从back开始
class BASE_EXPORT FS_NetBufferArray
{
    OBJ_POOL_CREATE_DEF(FS_NetBufferArray);
public:
    FS_NetBufferArray(Int32 sizeBuffer = FS_BUFF_SIZE_DEF);
    ~FS_NetBufferArray();
    void Release();
    
public:
    char *GetData();
    std::list<FS_NetBuffer *>::iterator GetFrontNode();
    bool Push(const char *data, Int32 len);
    void Pop(std::list<FS_NetBuffer *>::iterator &iterNode, Int32 len);

    Int32 Write2socket(SOCKET sockfd);
    Int32 ReadFromSocket(SOCKET sockfd);

    bool HasMsg() const;
    bool NeedWrite() const;
    bool IsFirstNodeNull() const;

#pragma region iocp
public:
#ifdef FS_USE_IOCP
    IO_DATA_BASE *MakeRecvIoData(SOCKET sockfd);
    IO_DATA_BASE *MakeSendIoData(SOCKET sockfd);

    // 从iocp读入或写入iocp时 buffer相应调整
    bool OnReadFromIocp(std::list<FS_NetBuffer *>::iterator &iterNode, int recvBytes);
    bool OnWrite2Iocp(std::list<FS_NetBuffer *>::iterator &iterNode, int sendBytes);
#endif
#pragma endregion

private:
    Int32 _bufferSize;
    std::list<FS_NetBuffer *> _buffers;
};

FS_NAMESPACE_END

#include "base/common/net/Defs/FS_NetBufferArrayImpl.h"

#endif
