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
 * @file  : CheckStartupMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/06
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_CheckStartup_Impl_CheckStartupMgr_H__
#define __Service_LogicSvc_Modules_CheckStartup_Impl_CheckStartupMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/CheckStartup/Interface/ICheckStartupMgr.h"

class CheckStartupMgr : public ICheckStartupMgr
{
public:
    CheckStartupMgr();
    ~CheckStartupMgr();

public:
    virtual Int32 OnInitialize();

public:
    // 启动前注册未决事件(用于在startup前做的事情,未决完成后会调用global的startup)
    virtual bool RegisterPendingBeforeStartup(Int32 pendingType);
    // 启动前的事件完成后需要调用remove
    virtual void RemovePendingBeforeStartup(Int32 pendingType);
    // 是否可以startup
    virtual bool CanStartup() const;

private:
    void _RegisterEvents();
    void _UnRegisterEvents();
    void _OnWillStartup(fs::FS_Event *ev);
    void _OnLocalServerReady(fs::FS_Event *ev);

    // 检查若可以启动则抛启动事件
    void _CheckStartup();

private:
    std::set<Int32> _pendingEvs;

    fs::FS_ListenerStub _onWillStartup;
    fs::FS_ListenerStub _onLocalServerReady;
};

#endif
