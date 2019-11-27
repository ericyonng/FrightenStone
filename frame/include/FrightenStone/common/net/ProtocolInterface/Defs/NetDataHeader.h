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

FS_NAMESPACE_BEGIN

struct BASE_EXPORT NetMsg_DataHeader
{
    NetMsg_DataHeader();

    // 提供序列化反序列化接口
    // 序列化反序列化（ObjType 必须有SerialzieTo(FS_Stream *), DeserializeFrom(FS_Stream *)接口）
    // 序列化反序列化（ObjType 必须有bool SerializeTo(FS_Stream *) const, bool DeserializeFrom(FS_Stream *)接口）
    // bool SerializeTo(FS_Stream *str);
    UInt16 _packetLength;           // 包长度
    UInt16 _cmd;                    // 命令
};

inline NetMsg_DataHeader::NetMsg_DataHeader()
{
    _packetLength = 0;
    _cmd = 0;
}

FS_NAMESPACE_END

#endif

