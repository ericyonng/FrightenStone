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
 * @file  : IFS_Session.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Session_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Session_H__
#pragma once
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/objpool/objpool.h"
#include "FrightenStone/common/net/Defs/FS_BufferFactory.h"

FS_NAMESPACE_BEGIN

class  FS_SessionMgr;
class  IFS_Packet;
class  FS_Addr;
struct NetMsg_DataHeader;
class  IFS_NetBuffer;

struct BASE_EXPORT BuildSessionInfo
{
    OBJ_POOL_CREATE_ANCESTOR(BuildSessionInfo);

    UInt64 _sessionId = 0;
    UInt32 _transferCompId = 0;
    UInt32 _acceptorCompId = 0;
    UInt32 _connectorCompId = 0;
    UInt16 _protocolPort = 0;
    Int32 _srcCompType = 0;
    SOCKET _sock = INVALID_SOCKET;
    const sockaddr_in *_addrInfo = NULL;
    IMemoryAlloctor *_memAlloctor = NULL;
    Int64 _heartbeatIntervalMicroSeconds = 0;
};

// TODO: IFS_Session, FS_Session
// 只支持单线程
class BASE_EXPORT IFS_Session
{
    OBJ_POOL_CREATE_ANCESTOR(IFS_Session);

public:
    explicit IFS_Session(BuildSessionInfo &buildInfo);
    virtual ~IFS_Session();

    // 获取属性与状态
public:
    UInt64 GetSessionId() const;
    UInt32 GetTransferCompId() const;
    UInt32 GetSrcAcceptorCompId() const;
    UInt32 GetSrcConnectorCompId() const;
    UInt16 GetProtocolPort() const;
    Int32 GetSrcCompType() const;
    SOCKET GetSocket() const;
    const FS_Addr *GetAddr() const;
    bool HasMsgToRead() const;      // 要等待消息接收完才能销毁
    bool HasMsgToSend() const;      // 要等待消息发送完才能消耗
    bool IsListen() const;          // 是否监听套接字
    void SetListener(bool isListen);
    template<typename ObjType>
    ObjType *CastTo();
    template<typename ObjType>
    const ObjType *CastTo() const;
    bool IsClose() const;
    const Time &GetHeartBeatExpiredTime() const;
    virtual bool CanDestroy() const;
    virtual bool CanDisconnect() const;
    void MaskClose(); // 若还有post操作则先maskdestroy，再在最后的时候真正的destroy掉
    bool IsDelayClose() const;
    IFS_NetBuffer *GetRecvBuffer();
    virtual bool IsValid() const;
    bool IsDestroy() const;
    virtual FS_String ToString() const;

    // 操作
public:
    virtual void Close();
    void UpdateHeartBeatExpiredTime();
    void ResetAddr();

    // 一个session只投递一个send，发完再继续发下一个，务必从队列头开始投递
    bool PushMsgToSend(const Byte8 *data, UInt64 len);   // 请外部调用的时候务必加锁
    bool IsSendBufferListEmpty() const;
    IFS_NetBuffer *GetSendBuffer();
    IFS_NetBuffer *NewSendBuffer();

    /* 事件 */
public:
    // 客户端销毁
    void OnDestroy();
    // 客户端连入
    virtual void OnConnect();
    void OnDisconnect();
    // 心跳连接超时
    void OnHeartBeatTimeOut();
    void OnMsgArrived();

    // 内部
private:
    void _Destroy();
    
protected:
    bool _isDestroy;
    bool _maskClose;
    // Locker _lock;
    UInt64 _sessionId;
    UInt32 _transferCompId;
    UInt32 _acceptorCompId;
    UInt32 _connectorCompId;
    UInt16 _protocolPort;       // 若会话来自监视器则是监听端口,若是连接器则是连接目标的端口
    Int32 _srcCompType;         // 来源的组件类型
    FS_Addr *_addr;
    SOCKET _sock;
    std::atomic<Int32> _lastErrorReason;
    bool _isListen;
    Time _heartBeatExpiredTime; // 心跳过期时间
    Int64 _heartbeatIntervalMicroSeconds;   // ms
    // IFS_NetBuffer *_recvBuffer;                 // 指定接收缓冲区只有一个,若有单包超过接收缓冲区的将不作处理,所以应该避免发送一个单包大于缓冲区的网络包,应该切割成几份发送
    IFS_NetBuffer *_recvBuffer; // 接收缓冲区大小有限，一个网络包大小不可大过缓冲区大小,否则将不处理并有可能kill掉session
    std::list<IFS_NetBuffer *> _toSend;     // 必须保留至少一个缓冲区不能全部释放掉,不可发送单包大于缓冲区的消息
    IMemoryAlloctor *_alloctor;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_SessionImpl.h"

#endif
