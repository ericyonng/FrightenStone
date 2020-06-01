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
 * @file  : EventFilterMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief : 事件过滤器管理,事件转发器管理,可管理多个类型的过滤器
 */
#ifndef __Service_LogicSvc_Modules_EventFilter_Impl_EventFilterMgr_H__
#define __Service_LogicSvc_Modules_EventFilter_Impl_EventFilterMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/EventFilter/Interface/IEventFilterMgr.h"

class EventFilterMgr : public IEventFilterMgr
{
public:
    EventFilterMgr();
    ~EventFilterMgr();

public:
    virtual Int32 OnInitialize();
    virtual void WillClose();

private:
    Int64 _miscEventFilterId;
};
#endif
