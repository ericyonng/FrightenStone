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
#ifndef __Base_Common_Net_Defs_IocpDefs_H__
#define __Base_Common_Net_Defs_IocpDefs_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

#pragma region IOCP macro
#define IO_DATA_BUFF_SIZE 1024          // io数据缓冲大小
#pragma endregion

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
};

struct BASE_EXPORT IO_DATA_BASE
{
    // 重叠体
    OVERLAPPED _overlapped{0};          // 使用重叠体可以关联到iodatabase,在投递accept时候传入
    SOCKET _sock = INVALID_SOCKET;
    Int32 _ioType = 0;

    // 没必要每个客户端指定一个缓冲，太大了，因为iocp取数据 时候数据是从队列中先进先出的方式被拷贝出来，每个iocp中的线程是互斥的执行的
    WSABUF _wsaBuff{0};                 // 数据缓冲结构
    // char _buff[IO_DATA_BUFF_SIZE]{0};           
    // Int32 _length = 0;
};

struct BASE_EXPORT IO_EVENT
{
//     union
//     {
//         void *_ptr;                             // 传入自定义数据,绑定完成端口时
//         SOCKET _socket;                         // 传入socket
//     }data;
    IO_DATA_BASE *_ioData = NULL;               // 重叠体自定义的数据
    SOCKET _socket = INVALID_SOCKET;            // completionkey返回的socket socket在客户端断开后会被复用
    ULong _bytesTrans = 0;                      // 传输的字节数
};

FS_NAMESPACE_END

#endif

