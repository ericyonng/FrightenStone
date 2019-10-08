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
 * @file  : IFS_Connector.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_IFS_Connector_H__
#define __Base_Common_Net_Impl_IFS_Connector_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;

class BASE_EXPORT IFS_Connector
{
public:
    IFS_Connector();
    virtual ~IFS_Connector();

public:
    virtual Int32 Start() = 0;
    virtual void Close() = 0;

//     /* TCP 常规操作 */
//     #pragma region tcp normal operate
//     /*
//     * brief:
//     *       1. - InitSocket 初始化Socket等
//     *       2. - Bind 绑定IP和端口号
//     *       3. - Listen 监听端口号
//     *       4. - Accept 接受客户端连接
//     *       5. - Start 根据跨平台的不同会有不同的server实例 这也是设计成使用泛型的一个原因
//     *       6. - BeforeClose 在关服前做些处理比如退出iocp
//     *       7. - Close 关闭服务器
//     */
// protected:
//     SOCKET _InitSocket();
//     Int32 _Bind(const Byte8 *ip, UInt16 port);
//     Int32 _Listen(Int32 unconnectQueueLen = SOMAXCONN);
//     SOCKET _Accept();
//     virtual void _BeforeClose();
//     void _Lock();
//     void _Unlock();
//     #pragma endregion
// 
//     /* 杂项 */
//     #pragma region misc
//     /*
//     * brief:
//     *       1. - _GetSocket 获取socket
//     *       2. - _AddClientToFSServer 将客户端加入管理
//     *       3. - _StatisticsMsgPerSecond 统计并输出每秒收到的网络消息
//     */
// protected:
//     SOCKET _GetSocket();
// 
//     #pragma endregion

    /* 数据成员 */
//    #pragma region data member
// private:
//     // 线程 
//     FS_ThreadPool *_threadPool;
//     // 消息处理对象，内部会创建线程
//     // 每秒消息计时
//     Time _msgCountTime;
//     SOCKET _sock;
// 
// protected:
//     // 客户端发送缓冲区大小
//     Int32 _sendBuffSize;
//     // 客户端接收缓冲区大小
//     Int32 _recvBuffSize;
//     // 客户端连接上限
//     Int32 _maxClient;
//     // SOCKET recv计数
//     std::atomic<Int32> _recvCount;
//     // 收到消息计数
//     std::atomic<Int32> _recvMsgCount;
//     // 客户端总共连入计数
//     std::atomic<Int32> _clientAcceptCnt;
//     // 当前保持连接已分配客户端计数
//     std::atomic<Int32> _clientJoinedCnt;
// 
//     Locker _locker;
//     UInt64 _clientMaxId;
//     const UInt64 _maxClientIdLimit;
//     Int64 _leaveClientCnt;
//     Int64 _joinClientCnt;
//     #pragma endregion
};

FS_NAMESPACE_END

#include "base/common/net/Impl/IFS_ConnectorImpl.h"

#endif
