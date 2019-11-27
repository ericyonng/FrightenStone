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
 * @file  : IFS_BusinessLogic.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/20
 * @brief :
 * 
 *
 * 
 */

#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_BusinessLogic_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_BusinessLogic_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/net/Impl/FS_ServerCore.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/net/Impl/FS_MsgReadStream.h"
#include "FrightenStone/common/net/Impl/FS_MsgWriteStream.h"

FS_NAMESPACE_BEGIN

struct BASE_EXPORT NetMsg_DataHeader;
class BASE_EXPORT IFS_MsgDispatcher;

class BASE_EXPORT IFS_BusinessLogic
{
public:
    IFS_BusinessLogic();
    virtual ~IFS_BusinessLogic();
    virtual void Release() = 0;

public:
    virtual Int32 BeforeStart() { return StatusDefs::Success; }
    virtual Int32 Start() = 0;
    virtual Int32 AfterStart() { return StatusDefs::Success; }
    virtual void WillClose() {} // 断开与模块之间的依赖
    virtual void BeforeClose() {}
    virtual void Close() = 0;
    virtual void AfterClose() {}

public:
    virtual void OnMsgDispatch(UInt64 sessionId, UInt64 generatorId, NetMsg_DataHeader *msgData) = 0;
    virtual void OnSessionDisconnected(UInt64 sessionId) = 0;
    virtual void OnSessionConnected(UInt64 sessionId) = 0;

    void SetDispatcher(IFS_MsgDispatcher *dispatcher);

protected:
    IFS_MsgDispatcher *_dispatcher;
};


FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_BusinessLogicImpl.h"

#endif
