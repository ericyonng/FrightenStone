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
 * @file  : FS_Client.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_Client_H__
#define __Base_Common_Net_Impl_FS_Client_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/net/Defs/FS_NetDefs.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/net/Defs/FS_NetBuffer.h"
#include "base/common/net/protocol/protocol.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/net/Defs/FS_NetBufferArray.h"
#include "base/common/assist/utils/Impl/STLUtil.h"
#include "base/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN
class BASE_EXPORT  FS_Packet;
class BASE_EXPORT FS_Client
{
    OBJ_POOL_CREATE(FS_Client, _objPoolHelper);
public:
    FS_Client(UInt64 clientId
              , SOCKET sockfd = INVALID_SOCKET
              , int sendSize = SEND_BUFF_SZIE
              , int recvSize = RECV_BUFF_SZIE);
    ~FS_Client();

public:
    // 发送数据 缓冲区的控制根据业务需求的差异而调整
    void SendData(NetMsg_DataHeader *header);
    // 返回值 len 或 SOCKET_ERROR
    void SendData(const char *data, Int32 len);
    void SetSender(IDelegatePlus<void, FS_Packet *> *sendFunc);

    NetMsg_DataHeader *FrontRecvMsg();
    void PopRecvFrontMsg();

    void UpdateHeartBeatExpiredTime();

    #ifdef FS_USE_IOCP
    IO_DATA_BASE *MakeRecvIoData();

    bool IsPostIoChange() const;
    bool IsPostSend() const;
    bool IsPostRecv() const;
    void ResetPostRecv();
    #endif // FS_USE_IOCP

private:
    void _SendData(const char *data, Int32 len);

    /* 杂项 */
    #pragma region misc
public:
    SOCKET GetSocket() const;
    UInt64 GetId() const;
    bool HasRecvMsg() const;
    bool IsDestroy() const;
    const Time &GetHeartBeatExpiredTime() const;
    void Close();

    #pragma endregion

    //////////用于调试的成员变量
public:
    // 所属serverid
    int _serverId = -1;
    // 测试接收发逻辑用
    // 用于server检测接收到的消息ID是否连续 每收到一个客户端消息会自增1以便与客户端的msgid校验，不匹配则报错处理（说明丢包等）
    int _recvMsgId = 1;
    // 测试接收发逻辑用
    // 用于client检测接收到的消息ID是否连续 每发送一个消息会自增1以便与客户端的sendmsgid校验，不匹配则客户端报错（说明丢包等）
    int _sendMsgId = 1;
    ///////////////////////////////////

private:
    // 每个FS_Server 唯一id
    UInt64 _id;
    // socket fd_set  file desc set
    SOCKET _sockfd;
    // 第二缓冲区 接收消息缓冲区
    FS_Packet *_recvBuff;
    // 发送缓冲区
    IDelegatePlus<void, FS_Packet *> *_sendFunc;
    // 心跳过期时间
    Time _heartBeatExpiredTime; // 心跳过期时间

#ifdef FS_USE_IOCP
    bool _isPostRecv = false;
    bool _isPostSend = false;
#endif
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_ClientImpl.h"

#endif
