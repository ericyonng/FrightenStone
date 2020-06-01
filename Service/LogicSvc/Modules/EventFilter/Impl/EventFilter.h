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
 * @file  : EventFilter.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief : 事件过滤器,事件转发器(添加在全局,其实也是监听某一事件,但这一事件会根据某个条件转发到另一个事件管理器中进行派发)
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
    void _OnSessionWillConnected(fs::FS_Event *ev);
    void _OnSessionConnecting(fs::FS_Event *ev);
    void _OnSessionConnectFail(fs::FS_Event *ev);
    void _OnSessionConnected(fs::FS_Event *ev);
    void _OnAfterSessionConnected(fs::FS_Event *ev);
    void _OnSessionWillDisconnect(fs::FS_Event *ev);
    void _OnSessionDisconnect(fs::FS_Event *ev);
    void _OnAfterSessionDisconnect(fs::FS_Event *ev);
    void _OnLogicBeforeMsgDispatch(fs::FS_Event *ev);
    void _OnUserLogin(fs::FS_Event *ev);
    void _OnUserCreated(fs::FS_Event *ev);
    void _OnUserLoginRes(fs::FS_Event *ev);
    void _OnUserWillClose(fs::FS_Event *ev);
    void _OnUserClose(fs::FS_Event *ev);
    void _OnUserCloseFinish(fs::FS_Event *ev);

private:
    fs::FS_ListenerStub _onSessionWillConnected;            // 即将连入
    fs::FS_ListenerStub _onSessionConnecting;               // 正在连入 用于处理黑名单等连入规则
    fs::FS_ListenerStub _onSessionConnectFail;              // 会话连入失败
    fs::FS_ListenerStub _onSessionConnected;                // 会话连入
    fs::FS_ListenerStub _afterSessionConnected;             // 会话连入
    fs::FS_ListenerStub _onSessionWillDisconnect;           // 会话断开
    fs::FS_ListenerStub _onSessionDisconnect;               // 会话断开
    fs::FS_ListenerStub _onAfterSessionDisconnect;          // 会话断开
    fs::FS_ListenerStub _onLogicBeforeMsgDispatch;          // 在消息调度之前
    fs::FS_ListenerStub _onUserLogin;                       // 用户登陆
    fs::FS_ListenerStub _onUserCreated;                     // 用户创建完成
    fs::FS_ListenerStub _onUserLoginRes;                    // 用户登陆反馈
    fs::FS_ListenerStub _onUserWillClose;                   // 用户即将关闭会话
    fs::FS_ListenerStub _onUserClose;                       // 用户会话关闭
    fs::FS_ListenerStub _onUserCloseFinish;                 // 用户会话关闭结束
};
#endif
