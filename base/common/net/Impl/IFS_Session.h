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
 * @file  : IFS_Session.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_IFS_Session_H__
#define __Base_Common_Net_Impl_IFS_Session_H__
#pragma once
#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/objpool/objpool.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/net/Defs/FS_NetDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_SessionMgr;
class BASE_EXPORT IFS_Packet;
class BASE_EXPORT FS_Addr;
struct BASE_EXPORT NetMsg_DataHeader;
class BASE_EXPORT IFS_Buffer;

// TODO: IFS_Session, FS_IocpSession, FS_EpollSession

class BASE_EXPORT IFS_Session
{
    OBJ_POOL_CREATE_DEF(IFS_Session);
public:
    explicit IFS_Session(UInt64 sessionId, SOCKET sock);
    virtual ~IFS_Session();

public:
    UInt64 GetSessionId() const;
    SOCKET GetSocket() const;
    const FS_Addr *GetAddr() const;
    bool HasMsgToRead() const;      // 要等待消息接收完才能销毁
    bool HasMsgToSend() const;      // 要等待消息发送完才能消耗
    template<typename ObjType>
    ObjType *CastTo();
    template<typename ObjType>
    const ObjType *CastTo() const;
    void Lock();
    void Unlock();
    void UpdateHeartBeatExpiredTime();
    bool IsDestroy() const;
    const Time &GetHeartBeatExpiredTime() const;

    /* 事件 */
public:
    // 客户端销毁
    void OnDestroy();
    // 客户端连入
    void OnConnect();
    // 心跳连接超时
    void OnHeartBeatTimeOut();
    void OnMsgArrived();
    
public:
    bool Send(NetMsg_DataHeader *header);   // 请外部调用的时候务必加锁

protected:
    virtual bool _OnSend(IFS_Buffer *newBuffer) = 0;

private:
    void _Destroy();
    
protected:
    bool _isDestroy;
    Locker _lock;
    UInt64 _sessionId;
    FS_Addr *_addr;
    SOCKET _sock;
    Time _heartBeatExpiredTime; // 心跳过期时间
    IFS_Buffer *_recvBuffer;
    std::list<IFS_Buffer *> _toSend;
    FS_SessionMgr *_sessionMgr;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/IFS_SessionImpl.h"

#endif
