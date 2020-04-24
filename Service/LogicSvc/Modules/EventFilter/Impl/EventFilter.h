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
 * @file  : EventFilter.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_EventFilter_Impl_EventFilter_H__
#define __Service_LogicSvc_Modules_EventFilter_Impl_EventFilter_H__

#pragma once

#include "Service/LogicSvc/Common.h"

class EventFilter : public IFS_EventFilter
{
public:
    EventFilter();
    ~EventFilter();

public:
    virtual void Register(fs::EventManager &evMgr);
    virtual void UnRegister(fs::EventManager &evMgr);

private:
    void _OnSessionConnected(fs::FS_Event *ev);
    void _OnAfterSessionConnected(fs::FS_Event *ev);
    void _OnSessionWillDisconnect(fs::FS_Event *ev);
    void _OnAfterSessionDisconnect(fs::FS_Event *ev);
    void _OnLogicBeforeMsgDispatch(fs::FS_Event *ev);

private:
    fs::FS_ListenerStub _onSessionConnected;                // 会话连入
    fs::FS_ListenerStub _afterSessionConnected;             // 会话连入
    fs::FS_ListenerStub _onSessionWillDisconnect;           // 会话断开
    fs::FS_ListenerStub _onAfterSessionDisconnect;          // 会话断开
    fs::FS_ListenerStub _onLogicBeforeMsgDispatch;          // 在消息调度之前
};
#endif
