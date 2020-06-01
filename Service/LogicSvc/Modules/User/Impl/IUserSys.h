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
 * @file  : IUserSys.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_User_Impl_IUserSys_H__
#define __Service_LogicSvc_Modules_User_Impl_IUserSys_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "Service/Common/LogicBase/Impl/IFS_LogicSys.h"
#include "Service/LogicSvc/Modules/User/Impl/User.h"

class User;

class IUserSys : public IFS_LogicSys
{
public:
    IUserSys();
    ~IUserSys() {}
    virtual void Release() { delete this; }

public:
    UInt64 GetSessionId() const;
    UInt64 GetUserId() const;

    virtual Int32 OnCreate();
    virtual void OnAfterCreate();

    // 取得用户系统对象
    template <typename UserSysType>
    UserSysType *GetSys();
    template <typename UserSysType>
    const UserSysType *GetSys() const;

    // 取得用户系统对象
    IUserSys *GetSys(const fs::FS_String &sysName);
    const IUserSys *GetSys(const fs::FS_String &sysName) const;

    // 取得所属玩家对象
    User * GetUser() { return _user; }
    const User *GetUser() const { return _user; }

    // 事件
public:
    virtual void OnConnect() {}
    
private:
    Int32 _Create(User *user);
    void _AfterCreate();

private:
    friend class User;

    User *_user;        // TODO:业务逻辑层需要移出引擎框架
};

#include "Service/LogicSvc/Modules/User/Impl/IUserSysImpl.h"

#endif
