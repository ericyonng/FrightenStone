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
 * @file  : FS_Session.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_Session_H__
#define __Base_Common_Net_Impl_FS_Session_H__
#pragma once
#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_SessionMgr;
class BASE_EXPORT FS_Packet;
class BASE_EXPORT FS_Addr;
struct BASE_EXPORT NetMsg_DataHeader;

class BASE_EXPORT FS_Session
{
    OBJ_POOL_CREATE_DEF(FS_Session);
public:
    explicit FS_Session(UInt64 sessionId, SOCKET sock, const FS_SessionMgr *sessionMgr);
    ~FS_Session();

public:
    UInt64 GetSessionId() const;
    SOCKET GetSocket() const;
    const FS_Addr *GetAddr() const;
    bool HasMsgToRead() const;
    bool HasMsgToSend() const;

public:
    // 客户端销毁
    void OnDestroy();
    // 客户端连入
    void OnConnect();
    // 心跳连接超时
    void OnHeartBeatTimeOut();
    void OnMsgArrived();
    void PushMsgToSend(NetMsg_DataHeader *header);

private:
    UInt64 _sessionId;
    FS_Addr *_addr;
    SOCKET _sock;
    std::list<FS_Packet *> _recvPackets;
    std::list<FS_Packet *> _toSend;
    const FS_SessionMgr *_sessionMgr;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_SessionImpl.h"

#endif
