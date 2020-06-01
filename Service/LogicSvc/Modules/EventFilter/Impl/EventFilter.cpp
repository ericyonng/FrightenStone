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
 * @file  : EventFilter.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/EventFilter/Impl/EventFilter.h"
#include "Service/LogicSvc/Modules/EventFilter/Defs/EventFilterDefs.h"

EventFilter::EventFilter()
    :_onSessionWillConnected(0)
    , _onSessionConnecting(0)
    , _onSessionConnectFail(0)
    , _onSessionConnected(0)
    ,_afterSessionConnected(0)
    ,_onSessionWillDisconnect(0)
    , _onSessionDisconnect(0)
    ,_onAfterSessionDisconnect(0)
    ,_onLogicBeforeMsgDispatch(0)
    ,_onUserLogin(0)
    ,_onUserCreated(0)
    , _onUserLoginRes(0)
    , _onUserWillClose(0)
    , _onUserClose(0)
    , _onUserCloseFinish(0)
{

}

EventFilter::~EventFilter()
{

}

void EventFilter::Register(fs::EventManager &evMgr)
{
    if(_afterSessionConnected)
        return;

    _onSessionWillConnected = evMgr.AddListener(EventIds::Logic_OnSessionWillConnected, this, &EventFilter::_OnSessionWillConnected);
    _onSessionConnecting = evMgr.AddListener(EventIds::Logic_OnSessionConnecting, this, &EventFilter::_OnSessionConnecting);
    _onSessionConnectFail = evMgr.AddListener(EventIds::Logic_OnSessionConnectFail, this, &EventFilter::_OnSessionConnectFail);
    _onSessionConnected = evMgr.AddListener(EventIds::Logic_OnSessionConnected, this, &EventFilter::_OnSessionConnected);
    _afterSessionConnected = evMgr.AddListener(EventIds::Logic_OnAfterSessionConnected, this, &EventFilter::_OnAfterSessionConnected);
    _onSessionWillDisconnect = evMgr.AddListener(EventIds::Logic_OnSessionWillDisconnect, this, &EventFilter::_OnSessionWillDisconnect);
    _onSessionDisconnect = evMgr.AddListener(EventIds::Logic_OnSessionDisconnect, this, &EventFilter::_OnSessionDisconnect);
    _onAfterSessionDisconnect = evMgr.AddListener(EventIds::Logic_OnAfterSessionDisconnected, this, &EventFilter::_OnAfterSessionDisconnect);
    _onUserLogin = evMgr.AddListener(EventIds::User_Login, this, &EventFilter::_OnUserLogin);
    _onUserCreated = evMgr.AddListener(EventIds::User_Created, this, &EventFilter::_OnUserCreated);
    _onUserLoginRes = evMgr.AddListener(EventIds::User_LoginRes, this, &EventFilter::_OnUserLoginRes);
    _onUserWillClose = evMgr.AddListener(EventIds::User_WillClose, this, &EventFilter::_OnUserWillClose);
    _onUserClose = evMgr.AddListener(EventIds::User_Close, this, &EventFilter::_OnUserClose);
    _onUserCloseFinish = evMgr.AddListener(EventIds::User_CloseFinish, this, &EventFilter::_OnUserCloseFinish);
}

void EventFilter::UnRegister(fs::EventManager &evMgr)
{
    if(!_afterSessionConnected)
        return;

    evMgr.RemoveListenerX(_onSessionWillConnected);
    evMgr.RemoveListenerX(_onSessionConnecting);
    evMgr.RemoveListenerX(_onSessionConnectFail);
    evMgr.RemoveListenerX(_onSessionConnected);
    evMgr.RemoveListenerX(_afterSessionConnected);
    evMgr.RemoveListenerX(_onSessionWillDisconnect);
    evMgr.RemoveListenerX(_onSessionDisconnect);
    evMgr.RemoveListenerX(_onAfterSessionDisconnect);
    evMgr.RemoveListenerX(_onUserLogin);
    evMgr.RemoveListenerX(_onUserCreated);
    evMgr.RemoveListenerX(_onUserLoginRes);
    evMgr.RemoveListenerX(_onUserWillClose);
    evMgr.RemoveListenerX(_onUserClose);
    evMgr.RemoveListenerX(_onUserCloseFinish);
}

void EventFilter::_OnSessionWillConnected(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnSessionConnecting(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnSessionConnectFail(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnSessionConnected(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnAfterSessionConnected(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnSessionWillDisconnect(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnSessionDisconnect(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnAfterSessionDisconnect(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnLogicBeforeMsgDispatch(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnUserLogin(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnUserCreated(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnUserLoginRes(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnUserWillClose(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnUserClose(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnUserCloseFinish(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}
