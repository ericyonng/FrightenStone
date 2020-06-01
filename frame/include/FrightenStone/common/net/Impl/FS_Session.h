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
 * @file  : FS_Session.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/17
 * @brief : 会话对象,用于维护连接等
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_Session_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_Session_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN

class IFS_NetBuffer;

#ifdef _WIN32
class FS_Iocp;
struct IoDataBase;
#else
class FS_EpollTransferPoller;
struct EpollIoData;
class FS_Epoll;
#endif

class BASE_EXPORT FS_Session : public IFS_Session
{
    OBJ_POOL_CREATE_DEF(FS_Session);
public:
    FS_Session(BuildSessionInfo &buildData);
    virtual ~FS_Session();

    // 操作
public:
    void ResetPostRecvMask();
    void ResetPostSendMask();
    void EnableDisconnect();
    bool IsPostIoChange() const;
    bool IsPostSend() const;
    bool IsPostRecv() const;
    bool CanPost() const;
    // 若要销毁session需要放到延迟队列中，且需要先closesocket然后等到所有io完成
    virtual bool CanDestroy() const;
    virtual bool CanDisconnect() const;
    virtual FS_String ToString() const;

    // 网络操作
public:
    virtual void Close();
    Int32 PostRecv();   // 需要判断canpost,以及ispostrecv
    Int32 PostSend();
    virtual void OnConnect();
    void BindMsgHandler(fs::IDelegate<void, fs::FS_Session *> *msgHandler);
    void InvokeMsgHandler();

    #pragma region platform relation
public:
#ifdef _WIN32
    void Bind(FS_Iocp *iocp);
    void CancelPostedEventsAndMaskClose();

    void OnSendSuc(Int64 transferBytes, IoDataBase *ioData);
    void OnRecvSuc(Int64 transferBytes, IoDataBase *ioData);
#else
    void Bind(FS_EpollTransferPoller *transferPoller);
    bool IsRecvIoCanceled() const;
    bool IsSendIoCaceled() const;

    void SetRecvIoCanceled();
    void SetSendIoCanceled();

    void CancelRecvIoAndMaskClose(Int32 reason, bool giveRes = false);
    void CancelSendIoAndMaskClose(Int32 reason, bool giveRes = false);

    void OnSendSuc(Int64 transferBytes, EpollIoData *ioData);
    void OnRecvSuc(Int64 transferBytes, EpollIoData *ioData);
#endif
    #pragma endregion

private:
    bool _isPostRecv;
    bool _isPostSend;
    fs::IDelegate<void, fs::FS_Session *> *_msgHandler; // 消息处理回调
    UInt16 _protocolPort;
    #pragma region platform relation
#ifdef _WIN32
    FS_Iocp *_iocp;
#else
    bool _isRecvIoCanceled;
    bool _isSendIoCanceled;
    FS_EpollTransferPoller *_transferPoller;
    FS_Epoll *_epoll;
#endif
    #pragma endregion
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_SessionImpl.h"

#endif
