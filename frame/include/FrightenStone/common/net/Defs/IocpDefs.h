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
 * @file  : IocpDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/18
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_IocpDefs_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_IocpDefs_H__
#pragma once

#ifdef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

/* iocp若干定义类型 */
class BASE_EXPORT IocpDefs
{
public:
    /* IO操作类型 */
    enum IO_TYPE:Int32
    {
        IO_ACCEPT = 10,
        IO_RECV,
        IO_SEND,
    };

    enum 
    {
        IO_QUIT = -1,           // 退出信号
    };
};

/* packet节点 */
class BASE_EXPORT IFS_Buffer;
struct BASE_EXPORT BufferQueueNode
{
    OBJ_POOL_CREATE_DEF(BufferQueueNode);
    BufferQueueNode();
    ~BufferQueueNode();

    std::list<IFS_Buffer *>::iterator _iterNode;
    bool _isPost;
};

struct BASE_EXPORT IoDataBase
{
    OBJ_POOL_CREATE_DEF(IoDataBase);
public:
    IoDataBase():_node(NULL){}
    ~IoDataBase();

    // 重叠体
    OVERLAPPED _overlapped{0};          // 使用重叠体可以关联到iodatabase,在投递accept时候传入
    Int32 _ioType = 0;
    UInt64 _sessionId = 0;                   // 会话唯一id
    SOCKET _sock = INVALID_SOCKET;
    union
    {
        BufferQueueNode *_node;         // packet所在的队列节点
    };
    IDelegate<void, Int64> *_callback = NULL; // 完成时的回调

    // 没必要每个客户端指定一个缓冲，太大了，
    // 因为iocp取数据 时候数据是从队列中先进先出的方式被拷贝出来，
    // 每个iocp中的线程是互斥的执行的
    WSABUF _wsaBuff{0};                 // 数据缓冲结构
};

// struct BASE_EXPORT IO_EVENT
// {
//     IO_EVENT();
// //     union
// //     {
// //         void *_ptr;                             // 传入自定义数据,绑定完成端口时
// //         UInt64 _sessionId;                      // 传入session唯一id
// //         SOCKET _socket;                         // 套接字
// //         Int64 _code;                            // 状态码
// //     }_data;   // 绑定完成端口时，绑定的完成键，在完成时会回传，建议回传sessionId,用于判断客户端是否存在
//     UInt64 _sessionId;                          // 传入session唯一id 绑定完成端口完成键回传
//     IoDataBase *_ioData = NULL;                 // Post时候传入的重叠体自定义的数据
//     // SOCKET _socket = INVALID_SOCKET;         // completionkey返回的socket socket在客户端断开后会被复用
//     ULong _bytesTrans = 0;                      // 传输的字节数
// };


FS_NAMESPACE_END

#endif

#endif

