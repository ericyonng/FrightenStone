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
 * @file  : IGlobalSysMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/14
 * @brief :
 */
#pragma once

#include "FrightenStone/exportbase.h"

class IGlobalSysMgr : public fs::IBaseLogicSysMgr
{
public:
    // 注册GlobalSys
    template <typename GlobalSysFactoryType>
    Int32 RegisterGlobalSys();
    virtual Int32 RegisterGlobalSys(fs::IFS_LogicSysFactory *sysFactory) = 0;

    // SysInfo获取
    virtual const fs::ILogicSysInfo *GetSysInfo(const fs::FS_String &globalSysName) const = 0;
    virtual const std::vector<fs::ILogicSysInfo *> &GetSysInfosList() const = 0;
    virtual const std::map<fs::FS_String, fs::ILogicSysInfo *> &GetSysInfosDict() const = 0;

public:
    // GlobalSys获取
    template <typename GlobalSysType>
    GlobalSysType *GetSys();
    virtual fs::IGlobalSys *GetSys(const fs::FS_String &sysName) = 0;
};

#include "TestInst/TestServer/GlobalSys/Impl/IGlobalSysMgrImpl.h"
