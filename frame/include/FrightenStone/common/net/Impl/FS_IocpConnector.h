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
 * @file  : FS_IocpConnector.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpConnector_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpConnector_H__
#pragma once

#ifdef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_Connector.h"
#include <FrightenStone/common/component/Impl/FS_String.h>
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class User;
struct ConnectorCfgs;

class BASE_EXPORT FS_IocpConnector : public IFS_Connector
{
    OBJ_POOL_CREATE_DEF(FS_IocpConnector);

public:
    FS_IocpConnector(IFS_NetEngine *netEngine
                     , UInt32 compId
                     , Locker &locker
                     , Int32 &curSessionCnt
                     , Int32 &maxSessionQuantityLimit
                     ,UInt64 &curMaxSessionId);
    virtual ~FS_IocpConnector();

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &cfgs);
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose() {}

public:
    // 连接
    virtual Int32 Connect(const FS_ConnectInfo &connectInfo);
    // 投递消息通知连接操作完成
    virtual void PostConnectOpFinish();

private:
    // 断开 从dispatcher调用委托回调
    void _OnUserDisconnected(UInt64 sessionId);

    Int32 _CheckConnect(const FS_ConnectInfo &connectInfo, FS_String &addrInfoOut) const;
    void _MakeAddrInfo(const FS_String &ip, UInt64 port, FS_String &addrInfo) const;
    Int32 _Connect(SOCKET sock, const sockaddr_in &sin, const FS_ConnectInfo &connectInfo);

private:
    // 连接器配置
    ConnectorCfgs * _cfgs;
    // 客户端连接上限
    Locker &_locker;
    Int32 &_curSessionCnt;
    Int32 &_maxSessionQuantityLimit;
    UInt64 &_curMaxSessionId;
    std::set<UInt64> _sucConnectedSessionIds;
};

#pragma region inline
inline void FS_IocpConnector::_MakeAddrInfo(const FS_String &ip, UInt64 port, FS_String &addrInfo) const
{
    addrInfo.AppendFormat("%s:%hu", ip.c_str(), port);
}
#pragma endregion
FS_NAMESPACE_END

#endif

#endif
