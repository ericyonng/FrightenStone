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
 * @file  : FS_MsgReadStreamImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/08/10
 * @brief :
 * 
 *
 * 
 */
#ifdef __Protocols_Impl_ProtocolInterface_Impl_FS_MsgReadStream_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_MsgReadStream::FS_MsgReadStream(char *data, Int32 size, bool isDelete, bool isPoolCreate)
    :FS_Stream(data, size, isDelete, isPoolCreate)
{
    OffsetWrLenOnWrChange(size);

    // 预先读取消息长度
    // ReadInt16();
    // 预先读取消息命令
    // getNetCmd();
}

inline FS_MsgReadStream::~FS_MsgReadStream()
{
}

FS_NAMESPACE_END

#endif
