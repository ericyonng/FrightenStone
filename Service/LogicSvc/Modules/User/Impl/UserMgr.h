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
 * @file  : UserMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */

#ifndef __Service_LogicSvc_Modules_User_Impl_UserMgr_H__
#define __Service_LogicSvc_Modules_User_Impl_UserMgr_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "Service/LogicSvc/Modules/User/Interface/IUserMgr.h"

class User;
class UserFacade;

class UserMgr : public IUserMgr
{
public:
    UserMgr();
    ~UserMgr();

public:
    virtual Int32 OnInitialize();
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();
    void WillClose();

public:
    // 注册User系统
    virtual Int32 RegisterUserSys(IFS_LogicSysFactory *sysFactory);

    // 取得User系统信息
    virtual const ILogicSysInfo *GetSysInfo(const fs::FS_String &sysName) const;
    // 取得User系统工厂
    virtual const IFS_LogicSysFactory *GetSysFactory(const fs::FS_String &sysName) const;
    // 取得User系统工厂字典
    virtual const std::map<fs::FS_String, IFS_LogicSysFactory *> &GetSysFactoriesDict() const;
    // 取得User系统信息列表
    virtual const std::vector<ILogicSysInfo *> &GetSysInfosList() const;

    // 创建用户
    virtual Int32 CreateUser(UInt64 sessionId, bool isShowLog = false);
    User *GetUserBySessionId(UInt64 sessionId);
    User *GetUserByUserId(UInt64 userId);
    std::set<User *> *GetUsersByIp(const fs::FS_String &ip);
    void RemoveUser(UInt64 sessionId);
    virtual const std::map<UInt64, User *> &GetAllUsers() const;

private:
    User *_NewUser(UInt64 sessionId);
    void _ClearUsers();

    /* 协议处理 */
    void _RegisterProtocols();
    void _OnUserLoginReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);
    void _OnUserLoginRes(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);
    void _OnUserLogoutReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);
    void _OnUserCheckHeartReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);

    /* 事件处理 */
    void _RegisterEvents();
    void _UnRegisterEvents();
    void _OnSessionWillConnected(fs::FS_Event *ev);
    void _OnAfterSessionDisconnected(fs::FS_Event *ev);

    void _FireCreated(UInt64 sessionId, UInt64 userId);
    void _FireLogin(UInt64 sessionId, UInt64 userId, Int32 recvMsgId);
    void _FireLoginRes(UInt64 sessionId, UInt64 userId, Int32 recvMsgId);

private:
    UInt64 _curMaxUserId;
    std::map<UInt64, User *> _sessionIdRefUsers;
    std::map<UInt64, User *> _userIdRefUsers;
    std::map <fs::FS_String, std::set<User *>> _ipRefUsers;                     // ip映射

    std::vector<ILogicSysInfo *> _sysInfoList;                                  // 系统信息列表
    std::map<fs::FS_String, ILogicSysInfo *> _sysNameRefSysInfoDict;            // 用户系统信息
    std::map<fs::FS_String, IFS_LogicSysFactory *> _sysNameRefUserSysFactory;   // 用户系统工厂

    fs::FS_ListenerStub _sessionWillConnectedStub;
    fs::FS_ListenerStub _afterSessionDisconnectedStub;
};

#pragma region inline
inline const std::map<UInt64, User *> &UserMgr::GetAllUsers() const
{
    return _sessionIdRefUsers;
}

#pragma endregion
#endif
