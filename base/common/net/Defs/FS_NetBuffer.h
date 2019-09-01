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
 * @file  : FS_NetBuffer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Defs_FS_NetBuffer_H__
#define __Base_Common_Net_Defs_FS_NetBuffer_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/objpool/objpool.h"
#include "base/common/net/Defs/FS_NetDefs.h"
#include "base/common/net/Defs/IocpDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_NetBufferArray;
class BASE_EXPORT FS_NetBuffer
{
    OBJ_POOL_CREATE(FS_NetBuffer, _objPoolHelper);
public:
    FS_NetBuffer(FS_NetBufferArray *owner, Int32 sizeBuffer = FS_BUFF_SIZE_DEF);
    ~FS_NetBuffer();
    void Release();

    char *GetData();
    std::list<FS_NetBuffer *>::iterator GetNode();
    FS_NetBufferArray *GetOwner();
    void SetNode(const std::list<FS_NetBuffer *>::iterator &iterNode);

    bool Push(const char *data, Int32 len);
    void Pop(Int32 len);
    Int32 Write2socket(SOCKET sockfd);
    Int32 ReadFromSocket(SOCKET sockfd);

    bool HasMsg() const;
    bool NeedWrite() const;
    bool IsFull() const;
    bool IsEmpty() const;

#pragma region iocp
public:
#ifdef FS_USE_IOCP
    IO_DATA_BASE *MakeRecvIoData(SOCKET sockfd);
    IO_DATA_BASE *MakeSendIoData(SOCKET sockfd);

    // 从iocp读入或写入iocp时 buffer相应调整
    bool OnReadFromIocp(int recvBytes);
    bool OnWrite2Iocp(int sendBytes);
#endif
#pragma endregion

private:
    // 第二缓冲区 发送缓冲区
    char *_buff = NULL;
    // 可以用链表或队列来管理缓冲数据块
    // list<char*> _buffList;
    // 缓冲区的数据尾部位置，已有数据长度
    Int32 _lastPos = 0;
    // 缓冲区总的空间大小，字节长度
    Int32 _buffSize = 0;
    // 缓冲区写满次数计数
    int _fullCount = 0;
    FS_NetBufferArray *_owner = NULL;
    std::list<FS_NetBuffer *>::iterator _arrayNode;
#ifdef FS_USE_IOCP
    IO_DATA_BASE _ioData = {};
#endif // CELL_USE_IOCP
};

FS_NAMESPACE_END

#include "base/common/net/Defs/FS_NetBufferImpl.h"

#endif
