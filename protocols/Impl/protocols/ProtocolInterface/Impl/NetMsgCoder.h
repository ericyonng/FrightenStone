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
 * @file  : NetMsgCoder.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#ifndef __Protocols_Impl_ProtocolInterface_Impl_NetMsgCoder_H__
#define __Protocols_Impl_ProtocolInterface_Impl_NetMsgCoder_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "protocols/ProtocolInterface/Defs/NetMsgHeaderFmtType.h"

FS_NAMESPACE_BEGIN

// 编码器会在start时预留长度位,真正的数据只从start之后开始,finish时候会在长度位写入真实流长度
class NetMsgCoder
{
    OBJ_POOL_CREATE_DEF(NetMsgCoder);
public:
    NetMsgCoder();
    virtual ~NetMsgCoder() {}
    virtual void Release();     // 库与执行程序的堆不同释放请使用release方法

    // 缓冲区不够的事情应由外部检测可写再写入,编码器只是作为组件,不做安全检测
    // 编码:start->write->finish,...(若有多个消息,可以循环start->write->finish过程)
public:
    void BindBuffer(Byte8 *msgBuffer, UInt64 bufferSize);
    void BindBuffer(FS_String *msgBuffer);
    template<typename BufferType>
    bool Start();
    template<typename BufferType>
    bool SetCmd(UInt32 cmd);
    template<typename DataType>
    bool WriteByte(DataType &data);     // 请保证DataType为基本数据类型写入,保证内存是紧凑的,避免出现内存对齐的问题导致远程解包失败
    bool WriteByte(const Byte8 *data, UInt64 dataLen);
    template<typename DataType>
    void WriteToString(DataType &data);     // 请保证DataType为基本数据类型写入,保证内存是紧凑的,避免出现内存对齐的问题导致远程解包失败
    void WriteToString(const Byte8 *data, UInt64 dataLen);
    // 结束处理
    template<typename BufferType>
    void Finish();
    bool IsBufferNotEnough() const;

    // 写入的总长度
    UInt64 GetTotalWriteBytes() const;
    // 获取buffer剩余的空间

protected:
    bool _CheckSizeAndMaskNotEnough(size_t appendSize);
    bool _CheckSizeNoMaskNotEnough(size_t appendSize) const;

protected:
    void *_buffer;             // 接管的缓冲区,不检查缓冲区大小请外部自行检测安全后再写入缓冲
    UInt64 _bufferSize;          // 若缓冲是byte8类型的则有效
    UInt64 _curHeaderPos;       // 起始位置
    UInt64 _wrLen;              // 当前消息写入长度
    bool _isBufferNotEnough;    // 缓冲是否不足缓冲是byte8类型有效
};

FS_NAMESPACE_END

#include "protocols/ProtocolInterface/Impl/NetMsgCoderImpl.h"

#endif
