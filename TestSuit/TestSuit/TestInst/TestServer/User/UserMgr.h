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
 * @file  : UserMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "TestInst/TestServer/User/IUserMgr.h"

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
    virtual Int32 RegisterUserSys(fs::IFS_LogicSysFactory *sysFactory);

    // 取得User系统信息
    virtual const fs::ILogicSysInfo *GetSysInfo(const fs::FS_String &sysName) const;
    // 取得User系统工厂
    virtual const fs::IFS_LogicSysFactory *GetSysFactory(const fs::FS_String &sysName) const;
    // 取得User系统工厂字典
    virtual const std::map<fs::FS_String, fs::IFS_LogicSysFactory *> &GetSysFactoriesDict() const;
    // 取得User系统信息列表
    virtual const std::vector<fs::ILogicSysInfo *> &GetSysInfosList() const;

public:
    // 创建用户
    virtual Int32 CreateUser(UInt64 sessionId);

public:
    User *GetUserBySessionId(UInt64 sessionId);
    User *GetUserByUserId(UInt64 userId);
    void RemoveUser(UInt64 sessionId);
    
private:
    User *_NewUser(UInt64 sessionId);

private:
    void _ClearUsers();

private:
    UInt64 _curMaxUserId;
    std::map<UInt64, User *> _sessionIdRefUsers;
    std::map<UInt64, User *> _userIdRefUsers;

    UserFacade *_userFacade;

    std::vector<fs::ILogicSysInfo *> _sysInfoList;                                  // 系统信息列表
    std::map<fs::FS_String, fs::ILogicSysInfo *> _sysNameRefSysInfoDict;            // 用户系统信息
    std::map<fs::FS_String, fs::IFS_LogicSysFactory *> _sysNameRefUserSysFactory;   // 用户系统工厂
};
