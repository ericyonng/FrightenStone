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
 * @file  : IUserMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/13
 * @brief :
 */

#ifndef __Logic_Modules_User_Interface_H__
#define __Logic_Modules_User_Interface_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "Logic/Common/LogicBase/Impl/IBaseLogicSysMgr.h"

class ILogicSysInfo;
class IFS_LogicSysFactory;
class User;

class IUserMgr : public IBaseLogicSysMgr
{
public:
    // 注册User系统(模板方法)
    template <typename IUserSysFactory>
    Int32 RegisterUserSys();
    // 注册User系统
    virtual Int32 RegisterUserSys(IFS_LogicSysFactory *sysFactory) = 0;

    // 取得User系统信息
    virtual const ILogicSysInfo *GetSysInfo(const fs::FS_String &sysName) const = 0;
    // 取得User系统工厂
    virtual const IFS_LogicSysFactory *GetSysFactory(const fs::FS_String &sysName) const = 0;
    // 取得User系统工厂字典
    virtual const std::map<fs::FS_String, IFS_LogicSysFactory *> &GetSysFactoriesDict() const = 0;
    // 取得User系统信息列表
    virtual const std::vector<ILogicSysInfo *> &GetSysInfosList() const = 0;

    // 创建用户
    virtual Int32 CreateUser(UInt64 sessionId) = 0;

    // 获取用户
    virtual User *GetUserBySessionId(UInt64 sessionId) = 0;
    // 移除用户
    virtual void RemoveUser(UInt64 sessionId) = 0;
};

#include "Logic/Modules/User/Impl/IUserMgrImpl.h"

#endif
