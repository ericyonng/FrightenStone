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

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_IocpSession : public IFS_Session
{
public:
    FS_IocpSession(UInt64 sessionId, SOCKET sock);
    virtual ~FS_IocpSession();

public:
    void BindToSender(IDelegate<void, IFS_Buffer *> *sender);

    NetMsg_DataHeader *FrontRecvMsg();
    void PopFrontRecvMsg();

    IoDataBase *MakeRecvIoData();
    IoDataBase *MakeSendIoData();

    bool IsPostIoChange() const;
    bool IsPostSend() const;
    bool IsPostRecv() const;
    void ResetPostSend();
    void ResetPostRecv();

protected:
    virtual bool _OnSend(IFS_Buffer *newBuffer);

private:
    IDelegate<void, IFS_Buffer *> *_sender;

#ifdef FS_USE_IOCP
    bool _isPostRecv;
    bool _isPostSend;
#endif
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_IocpSessionImpl.h"

#endif
