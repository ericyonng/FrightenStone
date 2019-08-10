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

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_NetBuffer;

class BASE_EXPORT FS_Client
{
    OBJ_POOL_CREATE(FS_Client, _objPoolHelper);
public:
    FS_Client(Int64 clientId
              , SOCKET sockfd = INVALID_SOCKET
              , int sendSize = SEND_BUFF_SZIE
              , int recvSize = RECV_BUFF_SZIE);
    ~FS_Client();

public:
    Int32 RecvData();
    // 立即将发送缓冲区的数据发送给客户端
    Int32 SendDataReal();
    // 发送数据 缓冲区的控制根据业务需求的差异而调整
    Int32 SendData(NetMsg_DataHeader *header);
    // 返回值 len 或 SOCKET_ERROR
    Int32 SendData(const char *data, Int32 len);
    NetMsg_DataHeader *FrontMsg();
    void PopFrontMsg();

    void ResetDTHeart();
    void ResetDTSend();
    // 心跳检测 心跳检测需要一个set序列
    bool CheckHeart(const TimeSlice &slice);
    // 定时发送消息检测
    bool CheckSend(const TimeSlice &dt);

    #ifdef FS_USE_IOCP
    IO_DATA_BASE *MakeRecvIoData();
    void OnRecvFromIocp(Int32 rcvBytes);

    IO_DATA_BASE *MakeSendIoData();
    void OnSend2iocp(Int32 snd);

    bool IsPostIoChange() const;
    #endif // FS_USE_IOCP

    /* 杂项 */
    #pragma region misc
public:
    void Destroy();
    SOCKET GetSocket() const;
    bool HasRecvMsg() const;
    bool NeedWrite() const;
    bool IsDestroy() const;
    #pragma endregion

    //////////用于调试的成员变量
public:
    Int64 _id;
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
    // socket fd_set  file desc set
    SOCKET _sockfd;
    // 第二缓冲区 接收消息缓冲区
    FS_NetBuffer *_recvBuff;
    // 发送缓冲区
    FS_NetBuffer *_sendBuff;
    // 心跳死亡计时
    TimeSlice _heartDeadSlice; // 心跳序列优化使用时间戳TODO
    // 上次发送消息数据的时间 
    TimeSlice _lastSendSlice; // 用于定时发送
#ifdef FS_USE_IOCP
    bool _isPostRecv = false;
    bool _isPostSend = false;
#endif
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_ClientImpl.h"

#endif
