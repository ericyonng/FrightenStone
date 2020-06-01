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
 * @file  : IGlobalSys.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_GlobalSys_Impl_IGlobalSys_H__
#define __Service_LogicSvc_Modules_GlobalSys_Impl_IGlobalSys_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "Service/Common/LogicBase/Impl/IFS_LogicSys.h"

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

    // 本地服务组件准备就绪,服务预热
    virtual void OnLocalServerReady() {}
    virtual void OnAfterLocalServerReady() {}
    virtual void OnLocalServerReadyFinish() {}

    // 服务完全启动成功,协议可以正常接入处理
    virtual void OnLocalServerStartup() {}
    virtual void OnAfterLocalServerStartup() {}
    virtual void OnLocalServerStartupFinish() {}

public:
    GlobalSysMgr *_globalMgr;
};

#include "Service/LogicSvc/Modules/GlobalSys/Impl/IGlobalSysImpl.h"

#endif
