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
 * @file  : IGlobalSys.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifndef __Logic_Common_LogicBase_Impl_IGlobalSys_H__
#define __Logic_Common_LogicBase_Impl_IGlobalSys_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "Logic/Common/LogicBase/Impl/IFS_LogicSys.h"

class GlobalSysMgr;

class IGlobalSys :public IFS_LogicSys
{
public:
    IGlobalSys() {}
    ~IGlobalSys() {}
    
public:
    // GlobalSys管理器设置/获取
    void SetGlobalMgr(GlobalSysMgr *mgr);
    GlobalSysMgr *GetGlobalMgr();

public:
    GlobalSysMgr *_globalMgr;
};

#include "Logic/Common/LogicBase/Impl/IGlobalSysImpl.h"

#endif
