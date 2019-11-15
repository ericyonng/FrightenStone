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
 * @file  : FS_MsgWriteStream.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/10
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_MsgWriteStream_H__
#define __Base_Common_Net_Impl_FS_MsgWriteStream_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/FS_Stream.h"
#include "base/common/net/protocol/protocol.h"
#include "base/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

// 消息数据字节流 字节流结构：buff = [长度] + [字节流区]
class BASE_EXPORT FS_MsgWriteStream :public FS_Stream
{
    OBJ_POOL_CREATE_DEF(FS_MsgWriteStream);
public:
    FS_MsgWriteStream(char *data, int size, bool isDelete = false, bool isPoolCreate = false);
    FS_MsgWriteStream(int size = 1024);

    /* 写入字节流 */
    #pragma region write bytes
    /*
    *   brief:
    *       1. - SetNetMsgCmd 写入消息号
    *       2. - WriteString(const char *str, Int32 len) 写入字节流
    *       3. - WriteString(const char *str) 写入字符串
    *       4. - Finish 字节流结束，请在结束时务必调用Finish以便发送消息正确！！！
    */
public:
    void SetNetMsgCmd(UInt16 cmd);
    bool WriteString(const char *str, Int32 len);
    bool WriteString(const char *str);
    void Finish();
    #pragma endregion
};


FS_NAMESPACE_END

#include "base/common/net/Impl/FS_MsgWriteStreamImpl.h"

#endif
