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
 * @file  : FS_TcpClient.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/04
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_TcpClient_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_TcpClient_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include <FrightenStone/common/net/Defs/FS_NetDefs.h>
#include <FrightenStone/common/component/Impl/FS_Delegate.h>

FS_NAMESPACE_BEGIN

class IFS_Session;
struct NetMsg_DataHeader;
class IMemoryAlloctor;

class BASE_EXPORT FS_TcpClient
{
public:
    FS_TcpClient();
    virtual ~FS_TcpClient();

public:
    // 初始化socket
    SOCKET InitSocket(Int32 sendSize = SEND_BUFF_SZIE, Int32 recvSize = RECV_BUFF_SZIE);
    // 连接服务器
    Int32 Connect(const Byte8 *ip, UInt16 port);
    // 关闭套接字
    virtual void Close();
    
    // 是否工作中
    bool IsRun();

    // 接收数据 处理粘包 拆分包
    Int32 RecvData();
    void DoMsg();
    // 发送数据
    int SendData(NetMsg_DataHeader *header);
    int SendData(const Byte8 *data, Int32 len);

    // 处理网络消息
    virtual bool OnRun(Int32 microseconds = 1) = 0;
    // 响应网络消息
    virtual void OnNetMsg(NetMsg_DataHeader *header) = 0;

protected:
    virtual void OnInitSocket() {}
    virtual void OnConnect() {}

private:
    inline void _UpdateCanCreateNewNodeForAlloctor(size_t addOccupiedBytes);

private:
    std::atomic<UInt64> _maxSessionId;
    IFS_Session *_session;
    bool _isConnect;

    /* 内存池用于缓冲 */ // TODO:可以抽象一个有上限的内存池方便使用
    IMemoryAlloctor *_sessionBufferAlloctor;
    UInt64 _maxAlloctorBytes;
    std::atomic<size_t> _curAlloctorOccupiedBytes;
    std::atomic_bool _canCreateNewNodeForAlloctor;
    IDelegate<void, size_t> *_updateAlloctorOccupied;
};

FS_NAMESPACE_END

#include <FrightenStone/common/net/Impl/FS_TcpClientImpl.h>

#endif
