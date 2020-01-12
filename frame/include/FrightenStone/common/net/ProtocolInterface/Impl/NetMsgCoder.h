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
 * @file  : NetMsgCoder.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_ProtocolInterface_Impl_NetMsgCoder_H__
#define __Frame_Include_FrightenStone_Common_Net_ProtocolInterface_Impl_NetMsgCoder_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/ProtocolInterface/Defs/NetMsgHeaderFmtType.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

// 编码器会在start时预留长度位,真正的数据只从start之后开始,finish时候会在长度位写入真实流长度
class BASE_EXPORT NetMsgCoder
{
    OBJ_POOL_CREATE_DEF(NetMsgCoder);
public:
    NetMsgCoder();
    virtual ~NetMsgCoder() {}
    virtual void Release();     // 库与执行程序的堆不同释放请使用release方法

    // 缓冲区不够的事情应由外部检测可写再写入,编码器只是作为组件,不做安全检测
    // 编码:start->write->finish,...(若有多个消息,可以循环start->write->finish过程)
public:
    void BindBuffer(Byte8 *msgBuffer);
    void Start();
    void SetCmd(UInt32 cmd);
    template<typename DataType>
    void Write(DataType &data);     // 请保证DataType为基本数据类型写入,保证内存是紧凑的,避免出现内存对齐的问题导致远程解包失败
    void Write(const Byte8 *data, UInt64 dataLen);
    // 结束处理
    void Finish();

    // 写入的总长度
    UInt64 GetTotalWriteBytes() const;
    // 获取buffer剩余的空间

protected:
    Byte8 *_buffer;             // 接管的缓冲区,不检查缓冲区大小请外部自行检测安全后再写入缓冲
    UInt64 _curHeaderPos;       // 起始位置
    UInt64 _wrLen;              // 当前消息写入长度
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/ProtocolInterface/Impl/NetMsgCoderImpl.h"

#endif
