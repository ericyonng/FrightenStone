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
 * @file  : FS_MessageBlock.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/1
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_MessageQueue_Defs_FS_MessageBlock_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_MessageQueue_Defs_FS_MessageBlock_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/objpool/objpool.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/component/Impl/FS_Stream.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"

FS_NAMESPACE_BEGIN

class IUser;
struct BASE_EXPORT FS_MessageBlock
{
    FS_MessageBlock();
    virtual ~FS_MessageBlock();

    template<typename DerivedObjType>
    DerivedObjType *CastTo();

    FS_Stream *_data;                            // 序列化的数据字节流
};

class BASE_EXPORT MessageBlockType
{
public:
    enum
    {
        MB_None = 0,                    // 无效
        MB_NetMsgArrived = 1,           // 收到网络包
        MB_NetMsgSended = 2,            // 发送网络包
        MB_NetSessionDisconnect = 3,    // 会话断开
        MB_NetSessionConnected = 4,     // 会话连入
    };
};

struct BASE_EXPORT FS_NetMsgBufferBlock : public FS_MessageBlock
{
    FS_NetMsgBufferBlock();
    ~FS_NetMsgBufferBlock();

    template<typename NetMsgObjType>
    NetMsgObjType *CastBufferTo();

    // 内存池创建
    Int32 _mbType;      // MessageBlockType
    Byte8 *_buffer; // NetMsg_DataHeader *
    Int32 _generatorId; // 生产者id
    UInt64 _sessionId;

    /* 连入时候的额外回调参数 _mbType==MB_NetSessionConnected */
    IDelegate<void, IUser *> *_newUserRes;
    IDelegate<void, IUser *> *_userDisconnected;
};

FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/MessageQueue/Defs/FS_MessageBlockImpl.h"

#endif
