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
 * @file  : FS_MsgReadStream.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/10
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_MsgReadStream_H__
#define __Base_Common_Net_Impl_FS_MsgReadStream_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/FS_Stream.h"
#include "base/common/net/protocol/protocol.h"
#include "base/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

// 消息数据字节流
class BASE_EXPORT FS_MsgReadStream :public FS_Stream
{
    OBJ_POOL_CREATE(FS_MsgReadStream, _objPoolHelper);
public:
    FS_MsgReadStream(NetMsg_DataHeader *header);
    FS_MsgReadStream(char *data, Int32 size, bool isDelete = false, bool isPoolCreate = false);
    virtual ~FS_MsgReadStream();

public:
    UInt16 GetNetMsgCmd();
};


FS_NAMESPACE_END

#include "base/common/net/Impl/FS_MsgReadStreamImpl.h"

#endif
