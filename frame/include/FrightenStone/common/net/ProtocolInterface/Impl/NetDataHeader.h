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
 * @file  : NetDataHeader.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/27
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_ProtocolInterface_Defs_NetDataHeader_H__
#define __Frame_Include_FrightenStone_Common_Net_ProtocolInterface_Defs_NetDataHeader_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class NetMsgCoder;
class NetMsgDecoder;

/*
*
* 网络消息请遵循:
*               1.读取网络消息务必DeserializeFrom,
*               2.发送网络消息需要SerializeTo,
*               3.避免因为内存对齐问题导致数据错乱
*/

struct BASE_EXPORT NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DEF(NetMsg_DataHeader);

    NetMsg_DataHeader();
    virtual ~NetMsg_DataHeader();

    // 序列化将数据写入缓冲区
    Int64 SerializeTo(char *buffer);
    // 从网络缓冲区反序列化得到消息
    bool DeserializeFrom(const char *buffer);
    virtual void SerializeTo(NetMsgCoder *buffer) = 0;
    virtual bool DeserializeFrom(NetMsgDecoder *buffer) = 0;

    UInt32 _packetLength;           // 包长度 发送时候这个值是不准的,以实际拷贝到缓冲区的长度为准
    UInt32 _cmd;                    // 命令

    NetMsgCoder *_coder;            // 编码序列化成字节流 RAII规则(资源使用即初始化)
    NetMsgDecoder *_decoder;        // 解码从字节流中读取消息  
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/ProtocolInterface/Impl/NetDataHeaderImpl.h"
#endif

