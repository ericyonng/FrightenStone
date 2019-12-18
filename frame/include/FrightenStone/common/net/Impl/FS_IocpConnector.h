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

FS_NAMESPACE_BEGIN

class User;
struct ConnectorCfgs;

class BASE_EXPORT FS_IocpConnector : public IFS_Connector
{
public:
    FS_IocpConnector(Locker &locker
                     , Int32 &curSessionCnt
                     , Int32 &maxSessionQuantityLimit
                     ,UInt64 &curMaxSessionId
                     , const UInt64 &maxSessionIdLimit);
    virtual ~FS_IocpConnector();

public:
    virtual Int32 BeforeStart(const ConnectorCfgs &cfgs);
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();

public:
    // 连接
    virtual Int32 Connect(const FS_ConnectInfo &connectInfo);
    virtual std::map<UInt64, IUser *> &GetUsers();  // 可以在user中启动定时器

    // 成功连接时的回调
    virtual void RegOnSucConnect(IDelegate<void, BriefSessionInfo *> *sucCallback);

private:
    // 连入成功 从dispatcher调用委托回调
    void _OnNewUserRes(IUser *user);
    // 断开 从dispatcher调用委托回调
    void _OnUserDisconnected(IUser *user);

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
    const UInt64 &_maxSessionIdLimit;
    std::set<UInt64> _sucConnectedSessionIds;
    std::map<UInt64, IUser *> _sessionIdRefUser;

    // 成功连接后
    IDelegate<void, BriefSessionInfo *> *_onConnectSuc;
};

#pragma region inline
inline void FS_IocpConnector::_MakeAddrInfo(const FS_String &ip, UInt64 port, FS_String &addrInfo) const
{
    addrInfo.AppendFormat("%s%hu", ip.c_str(), port);
}

inline std::map<UInt64, IUser *> &FS_IocpConnector::GetUsers()
{
    return _sessionIdRefUser;
}

inline void FS_IocpConnector::RegOnSucConnect(IDelegate<void, BriefSessionInfo *> *sucCallback)
{
    FS_Release(_onConnectSuc);
    _onConnectSuc = sucCallback;
}
#pragma endregion
FS_NAMESPACE_END

#endif

#endif
