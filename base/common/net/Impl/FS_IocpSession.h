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
 * @file  : FS_IocpSession.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/14
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_IocpSession_H__
#define __Base_Common_Net_Impl_FS_IocpSession_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/net/Impl/IFS_Session.h"
#include "base/common/net/Defs/IocpDefs.h"
#include "base/common/net/protocol/protocol.h"
#include "base/common/net/Defs/IFS_Buffer.h"
#include "base/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_IocpBuffer;

class BASE_EXPORT FS_IocpSession : public IFS_Session
{
    OBJ_POOL_CREATE_DEF(FS_IocpSession);
public:
    FS_IocpSession(UInt64 sessionId, SOCKET sock, const sockaddr_in *addrInfo, IMemoryAlloctor *memAlloctor);
    virtual ~FS_IocpSession();

    // 操作
public:
    NetMsg_DataHeader *FrontRecvMsg();
    void PopFrontRecvMsg();

    IoDataBase *MakeRecvIoData();
    IoDataBase *MakeSendIoData();
    void ResetPostRecvMask();
    void ResetPostSendMask();
    void ResetAllIoMask();
    void EnableDisconnect();

    FS_IocpBuffer *CastToRecvBuffer();

    // 状态
public:
    bool IsPostIoChange() const;
    bool IsPostSend() const;
    bool IsPostRecv() const;
    bool CanPost() const;
    // 若要销毁session需要放到延迟队列中，且需要先closesocket然后等到所有io完成
    virtual bool CanDestroy() const;
    virtual bool CanDisconnect() const;

    // 事件
public:
    void OnSendSuc(size_t transferBytes, IoDataBase *ioData);
    void OnRecvSuc(size_t transferBytes, IoDataBase *ioData);

private:
    bool _isPostRecv;
    bool _isPostSend;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_IocpSessionImpl.h"

#endif
