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
 * @file  : IFS_Acceptor.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/12/08
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Acceptor_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Acceptor_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/socket/socket.h"

FS_NAMESPACE_BEGIN

class IFS_Session;
class IFS_NetEngine;
struct BriefListenAddrInfo;
struct AcceptorCfgs;

class BASE_EXPORT IFS_Acceptor : public IFS_EngineComp
{
public:
    IFS_Acceptor(UInt32 compId
                 , IFS_NetEngine *netEngine
                 , Locker &locker
                 , Int32 &curSessionCnt
                 , Int32 &maxSessionQuantityLimit
                 , UInt64 &curMaxSessionId
                 , const AcceptorCfgs &cfg);
    virtual ~IFS_Acceptor();

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &totalCfgs);
    UInt16 GetListenPort() const;

protected:
    SOCKET _InitSocket();
    Int32 _Bind(const BriefListenAddrInfo &addrInfo);
    Int32 _Listen(Int32 unconnectQueueLen = SOMAXCONN);
    void _GetRealIp(const FS_String &cfgIp, const Byte8 *&realIp);
    virtual IFS_AcceptorPoller *_CreatePoller() = 0;

protected:
    AcceptorCfgs *_cfgs;
    SOCKET _sock;

    // 客户端连接上限
    Locker &_locker;
    Int32 &_curSessionCnt;
    Int32 &_maxSessionQuantityLimit;
    UInt64 &_curMaxSessionId;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_AcceptorImpl.h"
#endif
