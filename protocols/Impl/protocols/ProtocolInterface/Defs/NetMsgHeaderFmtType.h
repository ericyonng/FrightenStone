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
 * @file  : NetMsgHeaderFmtType.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#ifndef __Protocols_Impl_ProtocolInterface_Defs_NetMsgHeaderFmtType_H__
#define __Protocols_Impl_ProtocolInterface_Defs_NetMsgHeaderFmtType_H__

#pragma once

#include "FrightenStone/exportbase.h"

FS_NAMESPACE_BEGIN

class NetMsgHeaderFmtType
{
public:
    /* 类型定义 */
    typedef UInt32 PacketLenDataType;       // 包头长度类型 32位64位以及跨平台占用的字节数都是固定的
    typedef UInt32 CmdDataType;             // 包头协议号类型 32位64位以及跨平台占用的字节数都是固定的

    // 消息头格式: [4字节长度][4字节cmd]
    static const UInt32 _msgHeaderSize;
    static const UInt32 _msgPacketLenSize;
    static const UInt32 _msgCmdSize;
};

FS_NAMESPACE_END

#endif
