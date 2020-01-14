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
 * @file  : IUserSys.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifndef __Logic_Common_LogicBase_Impl_IUserSys_H__
#define __Logic_Common_LogicBase_Impl_IUserSys_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "Logic/Common/LogicBase/Impl/IFS_LogicSys.h"

class User;

class IUserSys : public IFS_LogicSys
{
public:
    IUserSys();
    ~IUserSys() {}
    virtual void Release() { delete this; }

public:
    virtual UInt64 GetSessionId() const = 0;
    virtual void Close() = 0;

    virtual Int32 OnCreate();
    virtual void OnAfterCreate();
    
private:
    Int32 _Create(User *user);
    void _AfterCreate();

private:
    friend class User;

    User *_user;        // TODO:业务逻辑层需要移出引擎框架
};

#include "Logic/Common/LogicBase/Impl/IUserSysImpl.h"

#endif
