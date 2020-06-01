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
 * @file  : TestSysGlobalFactory.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/15
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_TestSys_Impl_TestSysGlobalFactory_H__
#define __Service_LogicSvc_Modules_TestSys_Impl_TestSysGlobalFactory_H__
#pragma once

#include "Service/Common/LogicCommon.h"

class TestSysGlobalFactory : public fs::IFS_FacadeFactory, public IFS_LogicSysFactory
{
public:
    virtual fs::IFS_Facade *Create();

    // facade注册到globalsys时候一并创建globalsys
    virtual IFS_LogicSys *CreateSys() const;
    virtual ILogicSysInfo *CreateSysInfo() const;
};

#endif
