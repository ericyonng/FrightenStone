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
 * @file  : IFS_LogicSys.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifndef __Service_Common_LogicBase_Impl_IFS_LogicSys_H__
#define __Service_Common_LogicBase_Impl_IFS_LogicSys_H__

#pragma once

#include "FrightenStone/exportbase.h"

class ILogicSysInfo;

// 注意:不可再构造中做相关的业务事情因为还没初始化完成请在OnInitialize做初始化
class IFS_LogicSys
{
public:
    IFS_LogicSys();
    virtual ~IFS_LogicSys();

public:
    void BindDispatcher(fs::IFS_MsgDispatcher *dispatcher);
    // 在各个系统创建时候执行初始化，不保证一定在业务线程执行（global在主线程调用注册global系统时候执行，usersys则在创建新user对象时候执行）
    virtual void OnInitialize() {}

public:
    const fs::FS_String &GetSysName() const;
    const ILogicSysInfo *GetSysInfo() const;
    void SetSysInfo(const ILogicSysInfo *sysInfo);

protected:
    fs::IFS_MsgDispatcher *_dispatcher;
    const ILogicSysInfo *_sysInfo;
};

#include "Service/Common/LogicBase/Impl/IFS_LogicSysImpl.h"

#endif
