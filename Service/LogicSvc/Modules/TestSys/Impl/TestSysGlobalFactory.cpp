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
 * @file  : TestSysGlobalFactory.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/15
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/TestSys/Impl/TestSysGlobalFactory.h"
#include "Service/LogicSvc/Modules/TestSys/Impl/TestSysGlobalFacade.h"
#include "Service/LogicSvc/Modules/TestSys/Impl/TestSysGlobal.h"
#include "Service/LogicSvc/Modules/Common/ModuleCommon.h"
#include "Service/LogicSvc/Modules/GlobalSys/GlobalSys.h"

fs::IFS_Facade * TestSysGlobalFactory::Create()
{
    return new TestSysGlobalFacade;
}

IFS_LogicSys * TestSysGlobalFactory::CreateSys() const
{
    return new TestSysGlobal;
}

ILogicSysInfo * TestSysGlobalFactory::CreateSysInfo() const
{
    return new LogicSysInfo<TestSysGlobal>();
}
