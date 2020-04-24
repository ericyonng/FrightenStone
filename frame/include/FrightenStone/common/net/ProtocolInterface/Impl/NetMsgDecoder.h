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
 * @file  : NetMsgDecoder.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_ProtocolInterface_Impl_NetMsgDecoder_H__
#define __Frame_Include_FrightenStone_Common_Net_ProtocolInterface_Impl_NetMsgDecoder_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/ProtocolInterface/Defs/NetMsgHeaderFmtType.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT NetMsgDecoder
{
    OBJ_POOL_CREATE_DEF(NetMsgDecoder);
public:
    NetMsgDecoder();
    virtual ~NetMsgDecoder() {}
    virtual void Release();

    // 请遵循:Decode->Read
    // 请先decode然后再read,注意datatype请保证传入的是基本数据类型,
    // 避免因为内存对齐而出现数据错误
public:
    virtual bool Decode(const char *msgBuffer);
    bool Read(Byte8 *data, UInt64 readLen);
    template<typename DataType>
    bool Read(DataType &dataOut);
    
    UInt64 GetTotalReadBytes() const;
    UInt32 GetMsgLen() const;
    UInt32 GetCmd() const;
    const Byte8 *GetMsgBuffer() const;
    
protected:
    UInt32 _cmd;                                    // 消息协议号
    UInt32 _len;                                    // 消息长度
    const Byte8 *_buffer;                            // 消息缓冲
    UInt64 _curPos;                                 // 当前读取的位置
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/ProtocolInterface/Impl/NetMsgDecoderImpl.h"

#endif
