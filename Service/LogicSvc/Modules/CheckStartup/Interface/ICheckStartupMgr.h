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
 * @file  : ICheckStartupMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/06
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_CheckStartup_Interface_ICheckStartupMgr_H__
#define __Service_LogicSvc_Modules_CheckStartup_Interface_ICheckStartupMgr_H__

#pragma once

#include "Service/LogicSvc/Common.h"

class ICheckStartupMgr : public IBaseLogicSysMgr
{
public:
    // 启动前注册未决事件(用于在startup前做的事情,未决完成后会调用global的startup)
    virtual bool RegisterPendingBeforeStartup(Int32 pendingType) = 0;
    // 启动前的事件完成后需要调用remove
    virtual void RemovePendingBeforeStartup(Int32 pendingType) = 0;
    // 是否可以startup
    virtual bool CanStartup() const = 0;
};

#endif
