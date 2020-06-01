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
 * @file  : EpollDefs.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/18
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_EpollDefs_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_EpollDefs_H__
#pragma once

#ifndef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Defs/BufferQueueNode.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"

#undef __FS_MAX_EVENT_NUM_TO_HANDLE_ONCE_DEF__
#define __FS_MAX_EVENT_NUM_TO_HANDLE_ONCE_DEF__     128                         // 每次默认处理的最大事件上限

 // epoll能监听的事件最大上限 linux 2.6.8之后的版本这个参数没有作用,现在linux最大上限只跟内存以及文件描述符上限有关系,是动态的
#undef __FS_MAX_EPOLL_EVENT_NUM_LIMIT__
#define __FS_MAX_EPOLL_EVENT_NUM_LIMIT__            10000

#undef __ADD_FS_EPOLL_EVENT_FLAGS_DEF__
#define __ADD_FS_EPOLL_EVENT_FLAGS_DEF__    (EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR | EPOLLOUT)   // 关注epoll可读,断开,错误以及使用et模式

#undef __ADD_FS_EPOLL_ACCEPT_EVENT_FLAGS_DEF__
#define __ADD_FS_EPOLL_ACCEPT_EVENT_FLAGS_DEF__     (EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR)

#undef __DEL_FS_EPOLL_EVENT_FLAGS_DEF__
#define __DEL_FS_EPOLL_EVENT_FLAGS_DEF__    (EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR | EPOLLOUT)    // 从所有关注的事件中移除fd

FS_NAMESPACE_BEGIN

class BASE_EXPORT EpollWaitTimeOutType
{
public:
    enum
    {
        BlockNoTimeOut = -1,                // 没有超时的阻塞等待
        WakeupImmediately = 0,              // 立即被唤醒
    };
};

class BASE_EXPORT EpollDefs
{
public:
    enum
    {
        InvalidFd = -1,         // 无效文件描述符
        EpollError = -1,        // epoll错误
    };
};

// class BASE_EXPORT EpollIoType
// {
// public:
//     enum
//     {
//         IO_Accept = 10,
//         IO_Recv,
//         IO_Send,
//     };
// };

struct BASE_EXPORT EpollIoData
{
    OBJ_POOL_CREATE_DEF(EpollIoData);

    EpollIoData():_node(NULL), _data(NULL), _dateLen(0) {}
    ~EpollIoData();

    UInt64 _sessionId = 0;                   // 会话唯一id
    SOCKET _sock = INVALID_SOCKET;
    union
    {
        BufferQueueNode *_node;         // packet所在的队列节点
    };
    IDelegate<void, Int64> *_callback = NULL; // 完成时的回调

    // 要处理的数据
    Byte8 *_data;
    Int64 _dateLen;
};

FS_NAMESPACE_END

#endif

#endif
